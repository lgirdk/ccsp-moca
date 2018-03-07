/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2016 RDK Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

/**********************************************************************
   Copyright [2014] [Cisco Systems, Inc.]
 
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
 
       http://www.apache.org/licenses/LICENSE-2.0
 
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
**********************************************************************/

/**************************************************************************

    module: cosa_moca_internal.c

        For COSA Data Model Library Development

    -------------------------------------------------------------------

    copyright:

        Cisco Systems, Inc.
        All Rights Reserved.

    -------------------------------------------------------------------

    description:

        This file implementes back-end apis for the COSA Data Model Library

        *  CosaMoCACreate
        *  CosaMoCAInitialize
        *  CosaMoCARemove
    -------------------------------------------------------------------

    environment:

        platform independent

    -------------------------------------------------------------------

    author:

        COSA XML TOOL CODE GENERATOR 1.0

    -------------------------------------------------------------------

    revision:

        01/11/2011    initial revision.

**************************************************************************/

#include "cosa_moca_internal.h"
#include <sysevent/sysevent.h>
#include <time.h>
#include "cosa_moca_network_info.h"
#include "cosa_moca_apis.h"

extern void * g_pDslhDmlAgent;
extern ANSC_HANDLE g_MoCAObject ;


void* SynchronizeMoCADevices(void *arg);

/**********************************************************************

    caller:     owner of the object

    prototype:

        ANSC_HANDLE
        CosaMoCACreate
            (
            );

    description:

        This function constructs cosa MoCA object and return handle.

    argument:  

    return:     newly created MoCA object.

**********************************************************************/

ANSC_HANDLE
CosaMoCACreate
    (
        VOID
    )
{
    ANSC_STATUS           returnStatus = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_MOCA  pMyObject    = (PCOSA_DATAMODEL_MOCA)NULL;

    /*
     * We create object by first allocating memory for holding the variables and member functions.
     */
    pMyObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));

    if ( !pMyObject )
    {
        return  (ANSC_HANDLE)NULL;
    }

    /*
     * Initialize the common variables and functions for a container object.
     */
    pMyObject->Oid               = COSA_DATAMODEL_MOCA_OID;
    pMyObject->Create            = CosaMoCACreate;
    pMyObject->Remove            = CosaMoCARemove;
    pMyObject->Initialize        = CosaMoCAInitialize;

    pMyObject->Initialize   ((ANSC_HANDLE)pMyObject);

    return  (ANSC_HANDLE)pMyObject;
}

static int sysevent_fd;
static token_t sysevent_token;
static pthread_t sysevent_tid;
static void *Moca_sysevent_handler (void *data)
{
	async_id_t moca_update;
	sysevent_setnotification(sysevent_fd, sysevent_token, "moca_updated", &moca_update);
	PCOSA_DATAMODEL_MOCA            pMyObject     = (PCOSA_DATAMODEL_MOCA)g_MoCAObject;
	time_t time_now = { 0 }, time_before = { 0 };

	pthread_detach(pthread_self());

	for (;;)
    {
        char name[25]={0};
		char val[42]={0};
		char buf[10]={0};
        int namelen = sizeof(name);
        int vallen  = sizeof(val);
        int err;
        async_id_t getnotification_asyncid;
        err = sysevent_getnotification(sysevent_fd, sysevent_token, name, &namelen,  val, &vallen, &getnotification_asyncid);

        if (err)
        {
			/* 
			   * Log should come for every 1hour 
			   * - time_now = getting current time 
			   * - difference between time now and previous time is greater than 
			   *	3600 seconds
			   * - time_before = getting current time as for next iteration 
			   *	checking		   
			   */ 
			time(&time_now);
			
			if(LOGGING_INTERVAL_SECS <= ((unsigned int)difftime(time_now, time_before)))
			{
				printf("%s-**********ERR: %d\n", __func__, err);
				AnscTraceWarning(("%s-**********ERR: %d\n", __func__, err));
				time(&time_before);
			}

		   sleep(10);
        }
		else 
		{
			if (strcmp(name, "moca_updated")==0)
		    {
			  int isUpdated = atoi(val);
			  if(isUpdated) {
			 	AnscTraceWarning(("Received moca_updated event , setting bSnmpUpdate to 1\n"));
				pMyObject->MoCAIfFullTable[0].MoCAIfFull.Cfg.bSnmpUpdate = 1;
				CosaDmlMocaIfGetCfg(NULL, 0, &pMyObject->MoCAIfFullTable[0].MoCAIfFull.Cfg);
			  } 		   
			}
	   }

	}
	return 0;
}

void CosaMoCAUpdate()
{
	sysevent_fd = sysevent_open("127.0.0.1", SE_SERVER_WELL_KNOWN_PORT, SE_VERSION, "moca-update", &sysevent_token);

	if (sysevent_fd >= 0)
	{
		pthread_create(&sysevent_tid, NULL, Moca_sysevent_handler, NULL);
	}
	return;
}

void MoCA_Log()
{

	PCOSA_DATAMODEL_MOCA            pMyObject           = (PCOSA_DATAMODEL_MOCA)g_MoCAObject;
	ULONG Interface_count, ulIndex,  ulIndex1, AssocDeviceCount;
	PCOSA_DML_MOCA_ASSOC_DEVICE             pMoCAAssocDevice = NULL;
	COSA_DML_MOCA_IF_DINFO DynamicInfo={0};
    ANSC_STATUS            returnStatusAssocDevices = ANSC_STATUS_SUCCESS;
    ANSC_STATUS            returnStatusDinfo = ANSC_STATUS_SUCCESS;
    char                   X_CISCO_NetworkCoordinatorMACAddress[18];
	char					mac_buff[32] , mac_buff1[256] ;

	Interface_count = CosaDmlMocaGetNumberOfIfs((ANSC_HANDLE)NULL);

	for ( ulIndex = 0; ulIndex < Interface_count; ulIndex++ )
	{
		AssocDeviceCount = 0;
        pMoCAAssocDevice = NULL;
		returnStatusAssocDevices = CosaDmlMocaIfGetAssocDevices
		(
		    (ANSC_HANDLE)NULL, 
		    pMyObject->MoCAIfFullTable[ulIndex].MoCAIfFull.Cfg.InstanceNumber-1, 
		    &AssocDeviceCount,
		    &pMoCAAssocDevice,
		    NULL
		);
        _ansc_memset(&DynamicInfo, 0, sizeof(COSA_DML_MOCA_IF_DINFO));
		returnStatusDinfo = CosaDmlMocaIfGetDinfo(NULL, pMyObject->MoCAIfFullTable[ulIndex].MoCAIfFull.Cfg.InstanceNumber-1, &DynamicInfo);          
		AnscTraceWarning(("----------------------\n"));
        if (returnStatusDinfo == ANSC_STATUS_SUCCESS)
        {
            snprintf(X_CISCO_NetworkCoordinatorMACAddress, sizeof(X_CISCO_NetworkCoordinatorMACAddress), "%s", DynamicInfo.X_CISCO_NetworkCoordinatorMACAddress);
            AnscTraceWarning(("MOCA_HEALTH : NCMacAddress %s \n", X_CISCO_NetworkCoordinatorMACAddress));
        }
		AnscTraceWarning(("MOCA_HEALTH : Interface %d , Number of Associated Devices %d \n", ulIndex+1 , AssocDeviceCount));
		memset(mac_buff1,0,sizeof(mac_buff1));
        if ((returnStatusAssocDevices == ANSC_STATUS_SUCCESS) && (pMoCAAssocDevice !=NULL))
        {
   		    for ( ulIndex1 = 0; ulIndex1 < AssocDeviceCount; ulIndex1++ )
		    {
		    	AnscTraceWarning(("MOCA_HEALTH : Device %d \n", ulIndex1 + 1));
	    		AnscTraceWarning(("MOCA_HEALTH : PHYTxRate %lu \n", pMoCAAssocDevice[ulIndex1].PHYTxRate));
			    AnscTraceWarning(("MOCA_HEALTH : PHYRxRate %lu \n", pMoCAAssocDevice[ulIndex1].PHYRxRate));
			    AnscTraceWarning(("MOCA_HEALTH : TxPowerControlReduction %lu \n", pMoCAAssocDevice[ulIndex1].TxPowerControlReduction));
			    AnscTraceWarning(("MOCA_HEALTH : RxPowerLevel %d \n", pMoCAAssocDevice[ulIndex1].RxPowerLevel));			

				memset(mac_buff,0,sizeof(mac_buff));
#if defined(_COSA_BCM_MIPS_)
		        AnscCopyString(mac_buff,pMoCAAssocDevice[ulIndex1].MACAddress);
#else
		        /* collect value */
		        _ansc_sprintf
		            (
		                mac_buff,
		                "%02X:%02X:%02X:%02X:%02X:%02X",
		                pMoCAAssocDevice[ulIndex1].MACAddress[0],
		                pMoCAAssocDevice[ulIndex1].MACAddress[1],
		                pMoCAAssocDevice[ulIndex1].MACAddress[2],
		                pMoCAAssocDevice[ulIndex1].MACAddress[3],
		                pMoCAAssocDevice[ulIndex1].MACAddress[4],
		                pMoCAAssocDevice[ulIndex1].MACAddress[5]
		            );
#endif
				strcat(mac_buff1,mac_buff);
				if(ulIndex1 < (AssocDeviceCount-1))
					strcat(mac_buff1,",");
		    }
        }
		AnscTraceWarning(("----------------------\n"));

		AnscTraceWarning(("MOCA_MAC_%d_TOTAL_COUNT:%d\n", ulIndex+1 , AssocDeviceCount));
		AnscTraceWarning(("MOCA_MAC_%d:%s\n", ulIndex+1 , mac_buff1));
		if (pMoCAAssocDevice)
		{
			AnscFreeMemory(pMoCAAssocDevice);
			pMoCAAssocDevice= NULL;
		}   
	}
}
void * Logger_Thread(void *data)
{

	COSA_DML_MOCA_LOG_STATUS Log_Status={3600,FALSE};
	LONG timeleft;
	ULONG Log_Period_old;
	pthread_detach(pthread_self());
	sleep(60);
  
  	while(!g_MoCAObject)
          sleep(10);
  
	volatile PCOSA_DATAMODEL_MOCA            pMyObject     = (PCOSA_DATAMODEL_MOCA)g_MoCAObject;
	while(1)
	{
		if(pMyObject->LogStatus.Log_Enable)
			MoCA_Log();

		timeleft = pMyObject->LogStatus.Log_Period;
		while(timeleft > 0)
		{
			Log_Period_old = pMyObject->LogStatus.Log_Period;
			sleep(60);
			timeleft=timeleft-60+(pMyObject->LogStatus.Log_Period)-Log_Period_old;
		}
	}
}
void CosaMoCALogger()
{
	pthread_t logger_tid;
	int res;
	res = pthread_create(&logger_tid, NULL, Logger_Thread, NULL);

	if(res != 0) 
	{
		AnscTraceWarning(("CosaMoCAInitialize Create Logger_Thread error %d\n", res));
	}
	else
	{
		AnscTraceWarning(("CosaMoCAInitialize Logger Thread Created\n"));
	}

}

/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaMoCAInitialize
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function initiate  cosa MoCA object and return handle.

    argument: ANSC_HANDLE                 hThisObject
            This handle is actually the pointer of this object
            itself.

    return:     operation status.

**********************************************************************/

ANSC_STATUS
CosaMoCAInitialize
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS               returnStatus   = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_MOCA      pMyObject      = (PCOSA_DATAMODEL_MOCA    )hThisObject;
    COSAGetHandleProc         pProc          = (COSAGetHandleProc       )NULL;
    //PCOSA_PLUGIN_INFO         pPlugInfo      = (PCOSA_PLUGIN_INFO       )g_pCosaBEManager->hCosaPluginInfo;
    PSLAP_OBJECT_DESCRIPTOR   pObjDescriptor = (PSLAP_OBJECT_DESCRIPTOR )NULL;
    ANSC_HANDLE               pPoamMoCADm    = (ANSC_HANDLE )NULL;
    ANSC_HANDLE               pSlapMoCADm    = (ANSC_HANDLE )NULL;
    ULONG                     ulCount        = 0;
    ULONG                     ulIndex        = 0;
    ULONG                     ulRole         = 0;
    ULONG                     ulNextInsNum   = 0;
    pthread_t tid;

/*
    pProc = (COSAGetHandleProc)pPlugInfo->AcquireFunction("COSAGetLPCRole");

    if ( pProc )
    {
        ulRole = (ULONG)(*pProc)(g_pDslhDmlAgent);
        CcspTraceWarning(("CosaMoCAInitialize - LPC role is %lu...\n", ulRole));
    }
*/

    AnscTraceWarning(("CosaMoCAInitialize start. \n"));

    if( !pMyObject )
    {
        AnscTraceWarning(("CosaMoCAInitialize -- ERROR!!!!! NULL pMyObject.\n"));
        return ANSC_STATUS_FAILURE;
    }

    pMyObject->pSlapMoCADm = (ANSC_HANDLE)pSlapMoCADm;
    pMyObject->pPoamMoCADm = (ANSC_HANDLE)pPoamMoCADm;

    if ( CosaDmlMocaInit(NULL, (ANSC_HANDLE)&pMyObject) != ANSC_STATUS_SUCCESS )
    {
        AnscTraceWarning(("CosaMoCAInitialize -- ERROR!!!!! CosaDmlMocaInit.\n"));
    }
    else
    {
        AnscTraceWarning(("CosaMoCAInitialize -- Success CosaDmlMocaInit.\n"));
    }


    if ( CosaDmlMocaGetCfg(NULL, &pMyObject->MoCACfg) != ANSC_STATUS_SUCCESS )
    {
        AnscTraceWarning(("CosaMoCAInitialize -- ERROR!!!!! CosaDmlMocaGetCfg.\n"));
    }
    else
    {
        AnscTraceWarning(("CosaMoCAInitialize -- Success CosaDmlMocaGetCfg.\n"));
    }

    _ansc_memset(pMyObject->MoCAIfFullTable, 0, sizeof(COSA_DML_MOCA_IF_FULL_TABLE) * MOCA_INTEFACE_NUMBER);

    ulCount = CosaDmlMocaGetNumberOfIfs((ANSC_HANDLE)pPoamMoCADm);
    if ( ulCount > MOCA_INTEFACE_NUMBER )
    {
        AnscTraceWarning(("CosaMoCAInitialize -- ERROR!!!!! the real MoCA interface number(%lu) is bigger than predefined number(%d).\n", ulCount, MOCA_INTEFACE_NUMBER));
        assert(ulCount <= MOCA_INTEFACE_NUMBER );
    }
    else
    {
        AnscTraceWarning(("CosaMoCAInitialize -- Success CosaDmlMocaGetNumberOfIfs.\n"));
    }
    
    ulNextInsNum = 1;

    for ( ulIndex = 0; ulIndex < ulCount; ulIndex++ )
    {
        CosaDmlMocaIfGetEntry((ANSC_HANDLE)pPoamMoCADm, ulIndex, &pMyObject->MoCAIfFullTable[ulIndex].MoCAIfFull);
        
        AnscSListInitializeHeader( &pMyObject->MoCAIfFullTable[ulIndex].pMoCAExtCounterTable );
        AnscSListInitializeHeader( &pMyObject->MoCAIfFullTable[ulIndex].pMoCAExtAggrCounterTable );
        //AnscSListInitializeHeader( &pMyObject->MoCAIfFullTable[ulIndex].MoCAMeshTxNodeTable );

        if ( CosaDmlMocaIfGetQos(NULL, ulIndex, &pMyObject->MoCAIfFullTable[ulIndex].MoCAIfQos) != ANSC_STATUS_SUCCESS )
        {
            AnscTraceWarning(("CosaMoCAInitialize -- ERROR!!!!! CosaDmlMocaIfGetQos.\n"));
        }
        else
        {
            AnscTraceWarning(("CosaMoCAInitialize -- Success CosaDmlMocaIfGetQos.\n"));
        }

        pMyObject->MoCAIfFullTable[ulIndex].MoCAIfFull.Cfg.InstanceNumber = ulNextInsNum++;    
    }
    
    //If MoCA enabled check the bridge mode.
    if  (TRUE == pMyObject->MoCAIfFullTable[0].MoCAIfFull.Cfg.bEnabled) 
    {
        char bridgeMode[16] = {0};
        int mode = 0;

        syscfg_get(NULL, "bridge_mode", bridgeMode, sizeof(bridgeMode));
        mode = atoi(bridgeMode);

        //If bridge mode enabled then Disable MoCA.
        if (0 != mode) 
        {
            if ( CosaDmlMocaIfSetCfg( NULL, 0, &pMyObject->MoCAIfFullTable[0].MoCAIfFull.Cfg) != ANSC_STATUS_SUCCESS )
            {
                AnscTraceWarning(("CosaMoCAInitialize -- ERROR!!!!! CosaDmlMocaIfSetCfg.\n"));
            }
            else
            {
                AnscTraceWarning(("CosaMoCAInitialize -- Success CosaDmlMocaIfSetCfg.\n"));
            }
        }
    }
    if ( CosaMoCAGetForceEnable(&pMyObject->MoCACfg) != ANSC_STATUS_SUCCESS )
    {
        AnscTraceWarning(("CosaMoCAInitialize -- ERROR!!!!! CosaMoCAGetForceEnable.\n"));
    }
    else
    {
        AnscTraceWarning(("CosaMoCAInitialize -- Success CosaMoCAGetForceEnable.\n"));
    }

	CosaDmlMocaGetLogStatus(&pMyObject->LogStatus);

    CosaMoCAUpdate();
    CosaMoCALogger();

#ifdef _COSA_INTEL_XB3_ARM_  
    moca_associatedDevice_callback_register(CosaMoCA_AssocDeviceSync_cb);

    if  (TRUE == pMyObject->MoCAIfFullTable[0].MoCAIfFull.Cfg.bEnabled)
    {
        AnscTraceWarning(("CosaMoCAInitialize -- start Assoc Devices Thread\n"));
        moca_startAssocDevicesThread();
    }
#else
    if (pthread_create(&tid, NULL, SynchronizeMoCADevices, NULL))
    {
        CcspTraceError(("RDK_LOG_ERROR, CcspMoCA %s : Failed to Start Thread to start SynchronizeMoCADevices  \n", __FUNCTION__ ));
        return ANSC_STATUS_FAILURE;
    }
#endif
    AnscTraceWarning(("CosaMoCAInitialize end. \n"));

    return returnStatus;
}

/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaMoCARemove
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function initiate  cosa MoCA object and return handle.

    argument:   ANSC_HANDLE                 hThisObject
            This handle is actually the pointer of this object
            itself.

    return:     operation status.

**********************************************************************/

ANSC_STATUS
CosaMoCARemove
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS               returnStatus      = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_MOCA      pMyObject         = (PCOSA_DATAMODEL_MOCA)hThisObject;
    /*PPOAM_COSAMOCADM_OBJECT*/ANSC_HANDLE   pSlapMoCADm       = (/*PPOAM_COSAMOCADM_OBJECT*/ANSC_HANDLE )pMyObject->pSlapMoCADm;
    /*PSLAP_COSAMOCADM_OBJECT*/ANSC_HANDLE   pPoamMoCADm       = (/*PSLAP_COSAMOCADM_OBJECT*/ANSC_HANDLE )pMyObject->pPoamMoCADm;
    COSAGetHandleProc         pProc             = (COSAGetHandleProc       )NULL;
    //PCOSA_PLUGIN_INFO         pPlugInfo         = (PCOSA_PLUGIN_INFO       )g_pCosaBEManager->hCosaPluginInfo;
    PSLAP_OBJECT_DESCRIPTOR   pObjDescriptor    = (PSLAP_OBJECT_DESCRIPTOR )NULL;
    ULONG                     i                 = 0;    
#if 0
    /* Remove Poam or Slap resounce */
    if ( pSlapMoCADm )
    {    
        pProc = (COSAGetHandleProc)pPlugInfo->AcquireFunction("COSAReleaseSlapObject");

        if ( pProc )
        {
            pObjDescriptor = (PSLAP_OBJECT_DESCRIPTOR)SlapCOSAMoCADmGetSlapObjDescriptor((ANSC_HANDLE)NULL);
            (*pProc)(pSlapMoCADm, pObjDescriptor);
        }
    }

    if ( pPoamMoCADm )
    {
        /*pPoamMoCADm->Remove((ANSC_HANDLE)pPoamMoCADm);*/
    }
    
    /* Remove necessary resounce */
    for (i = 0; i<MOCA_INTEFACE_NUMBER; i++)
    {
        if (pMyObject->MoCAIfFullTable[i].pMoCAAssocDevice)
        {
            AnscFreeMemory(pMyObject->MoCAIfFullTable[i].pMoCAAssocDevice);
            pMyObject->MoCAIfFullTable[i].pMoCAAssocDevice = NULL;
        }
    }
#endif
    /* Remove self */
    if (pMyObject)
    {
        AnscFreeMemory((ANSC_HANDLE)pMyObject);
    }

    return returnStatus;
}
