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

#include "cosa_moca_webconfig_api.h"
#include "cosa_moca_internal.h"
#include "moca_hal.h"
#include "ccsp_trace.h"
#include "syscfg/syscfg.h"

extern ANSC_HANDLE g_MoCAObject ;

bool g_bMocaEnable_bkup = false;

uint32_t getBlobVersion(char* subdoc)
{

	char subdoc_ver[64] = {0}, buf[72] = {0};
    	snprintf(buf,sizeof(buf),"%s_version",subdoc);
    	if ( syscfg_get( NULL, buf, subdoc_ver, sizeof(subdoc_ver)) == 0 )
    	{
        	int version = atoi(subdoc_ver);
      		//  uint32_t version = strtoul(subdoc_ver, NULL, 10) ; 

        	return (uint32_t)version;
    	}
    	return 0;
}

/* API to update the subdoc version */
int setBlobVersion(char* subdoc,uint32_t version)
{

	char buf[72] = {0};
  	snprintf(buf,sizeof(buf),"%s_version",subdoc);
 	if(syscfg_set_u_commit(NULL,buf,version) != 0)
 	{
        	CcspTraceError(("syscfg_set failed\n"));
        	return -1;
 	}
     	
	return 0;
     	 
}

/* API to register all the supported subdocs , versionGet and versionSet are callback functions to get and set the subdoc versions in db */

void webConfigFrameworkInit()
{
	char *sub_docs[SUBDOC_COUNT+1]= {"moca",(char *) 0 };
    
    	blobRegInfo *blobData;

    	blobData = (blobRegInfo*) malloc(SUBDOC_COUNT * sizeof(blobRegInfo));

    	int i;
    	memset(blobData, 0, SUBDOC_COUNT * sizeof(blobRegInfo));

    	blobRegInfo *blobDataPointer = blobData;

    	for (i=0 ; i < SUBDOC_COUNT ; i++ )
    	{
        	strncpy( blobDataPointer->subdoc_name, sub_docs[i], sizeof(blobDataPointer->subdoc_name)-1);

        	blobDataPointer++;
    	}

   	    blobDataPointer = blobData ;

    	getVersion versionGet = getBlobVersion;

    	setVersion versionSet = setBlobVersion;
    
    	register_sub_docs(blobData,SUBDOC_COUNT,versionGet,versionSet);
 
}

int UpdateToDB( bool bEnable)
{
    if(bEnable)
    {
        if (syscfg_set_commit(NULL, "moca_enabled", "1") != 0)
        {
            CcspTraceWarning(("UpdateToDB syscfg_set failed\n"));
            return SYSCFG_FAILURE;
        } 
    }
    else
    {
        if (syscfg_set_commit(NULL, "moca_enabled", "0") != 0)
        {
            CcspTraceWarning(("UpdateToDB syscfg_set failed\n"));
            return SYSCFG_FAILURE;
        }
    }
    return 0;
}

int backup_mocavalue()
{	
    CcspTraceWarning(("%s: backup_mocavalue called \n", __FUNCTION__));
    char buf[10]={0};
    if( (syscfg_get( NULL, "moca_enabled", buf, sizeof(buf)) == 0) && ( buf[0] != '\0') )
    {
        g_bMocaEnable_bkup=atoi(buf);
    }
    else
    {
        return SYSCFG_FAILURE;
    }    
    CcspTraceWarning(("backup_mocavalue -- moca enable value is %d before update", g_bMocaEnable_bkup ));
    return 0;
}

int set_moca_conf( mocadoc_t* mocadoc )
{
    CcspTraceWarning(("%s: set_moca_confg called \n", __FUNCTION__));
    PCOSA_DATAMODEL_MOCA    pMyObject = (PCOSA_DATAMODEL_MOCA )g_MoCAObject;
    PCOSA_DML_MOCA_CFG      pCFG      = &pMyObject->MoCACfg;
    PCOSA_DML_MOCA_IF_CFG   pCfg	  = &pMyObject->MoCAIfFullTable[0].MoCAIfFull.Cfg;
    char buf[5] = {0};
#if defined (_CM_HIGHSPLIT_SUPPORTED_)
    unsigned char IsHighSplitEnabled = CosaMoCAIsCMHighSplitDiplexerMode();

    //MoCA Enable not supported when CM HighSplit Mode
    if( ( TRUE == IsHighSplitEnabled ) && ( mocadoc->param->enable == true ) )
    {
        CcspTraceWarning(("Enabling MOCA is not supported when CM HighSplit Mode\n"));
        return VALIDATION_FALIED;
    }
#endif /* * _CM_HIGHSPLIT_SUPPORTED_ */

    syscfg_get( NULL, "X_RDKCENTRAL-COM_VIDEOSERVICE", buf, sizeof(buf));
    if(  buf[0] != '\0' )
    {
        if (strcmp(buf,"1") == 0)
        {
            if(mocadoc->param->enable == false)
            {
                CcspTraceWarning(("Disabling MOCA is not supported when VideoService is ENABLED\n"));
                return VALIDATION_FALIED;
            }
        }
    }
    BOOL bridgeId = FALSE;
    if((mocadoc->param->enable == FALSE) && (pCFG->bForceEnabled == TRUE) && ((ANSC_STATUS_SUCCESS == is_usg_in_bridge_mode(&bridgeId)) && (FALSE == bridgeId))){
        CcspTraceWarning(("MOCA cannot Disabled due to X_RDKCENTRAL-COM_ForceEnable flag\n"));
        return VALIDATION_FALIED;
    }
    
    char bridgeMode[64] = {0};
    moca_cfg_t mocaCfg;
    memset(&mocaCfg, 0, sizeof(moca_cfg_t));

    syscfg_get(NULL,"bridge_mode",bridgeMode,sizeof(bridgeMode));
    int mode=atoi(bridgeMode);
    bool bBridge = false;

    if(mode==0)
    {
        if( pCfg->bEnabled  == mocadoc->param->enable )
        {
            CcspTraceWarning(("%s: doc value : %d and pCfg->bEnabled :%d are same, no update needed\n", __FUNCTION__, mocadoc->param->enable, pCfg->bEnabled));
            return 0;
        }
        pCfg->bEnabled = mocadoc->param->enable;
        mocaCfg.bEnabled = pCfg->bEnabled;
        CcspTraceWarning(("%s: set_moca_confg doc value : %d and pCfg->bEnabled :%d\n", __FUNCTION__, mocadoc->param->enable, pCfg->bEnabled));
    }
    else
    {
        bBridge = true;
        CcspTraceWarning(("%s in bridge with current moca enable value: %d\n", __FUNCTION__, pCfg->bEnabled));
        if( pCfg->bEnabled )
        {
            pCfg->bEnabled=FALSE;
            mocaCfg.bEnabled = pCfg->bEnabled;
            CcspTraceWarning(("set_moca_confg -- in Bridge mode cannot enable or disable MoCA\n"));
        }
        else
        {
            CcspTraceWarning(("set_moca_confg -- Moca is disabled in bridge mode, not need to process\n"));
            return -1;
        }
    }  
    mocaCfg.InstanceNumber                      = pCfg->InstanceNumber;
    strncpy(mocaCfg.Alias, pCfg->Alias, sizeof(mocaCfg.Alias) -1);
    mocaCfg.bPreferredNC                        = pCfg->bPreferredNC;
    mocaCfg.PrivacyEnabledSetting               = pCfg->PrivacyEnabledSetting;
    memcpy(mocaCfg.FreqCurrentMaskSetting,      pCfg->FreqCurrentMaskSetting, 128);
    memcpy(mocaCfg.KeyPassphrase,               pCfg->KeyPassphrase, 18);
    mocaCfg.TxPowerLimit                        = pCfg->TxPowerLimit;
    mocaCfg.BeaconPowerLimit                    = pCfg->BeaconPowerLimit;
    mocaCfg.MaxIngressBWThreshold               = pCfg->MaxIngressBWThreshold;
    mocaCfg.MaxEgressBWThreshold                = pCfg->MaxEgressBWThreshold;
    mocaCfg.Reset                               = pCfg->X_CISCO_COM_Reset;
    mocaCfg.MixedMode                           = pCfg->X_CISCO_COM_MixedMode;
    mocaCfg.ChannelScanning                     = pCfg->X_CISCO_COM_ChannelScanning;
    mocaCfg.AutoPowerControlEnable              = pCfg->X_CISCO_COM_AutoPowerControlEnable;
    mocaCfg.EnableTabooBit                      = pCfg->X_CISCO_COM_EnableTabooBit;
    memcpy(mocaCfg.NodeTabooMask,               pCfg->NodeTabooMask, 128);
    mocaCfg.AutoPowerControlPhyRate             = pCfg->AutoPowerControlPhyRate;
    memcpy(mocaCfg.ChannelScanMask,             pCfg->X_CISCO_COM_ChannelScanMask, 128);
        
    CcspTraceWarning(("moca_SetIfConfig hal -- called.\n")); 
    if ( moca_SetIfConfig(0, &mocaCfg) != STATUS_SUCCESS)
    {
        CcspTraceWarning(("%s: moca_SetIfConfig returns error returning \n", __FUNCTION__));
        return MOCA_HAL_FAILURE;
    }
    CcspTraceWarning(("pCfg->bEnabled: %d\n", pCfg->bEnabled));
    if( SYSCFG_FAILURE == UpdateToDB( pCfg->bEnabled ))
    {
        return SYSCFG_FAILURE;
    }
    if( bBridge )
    {
        return -1;
    }
    return 0;
}

/* CallBack API to execute Moca Blob request */
pErr Process_Moca_WebConfigRequest(void *Data)
{
    pErr execRetVal = NULL;
    execRetVal = (pErr) malloc (sizeof(Err));
    if (execRetVal == NULL )
    {
        CcspTraceError(("%s : malloc failed\n",__FUNCTION__));
        return execRetVal;
    }
    memset(execRetVal,0,sizeof(Err));

    execRetVal->ErrorCode = BLOB_EXEC_SUCCESS;
    mocadoc_t *mocadoc = (mocadoc_t *) Data ;
    CcspTraceWarning(("moca configurartion recieved!!!!!!\n"));

    if( SYSCFG_FAILURE == backup_mocavalue())
    {
        execRetVal->ErrorCode = SYSCFG_FAILURE;
        strncpy(execRetVal->ErrorMsg,"sysconfig failure to get moca enable backup value",sizeof(execRetVal->ErrorMsg)-1);

        return execRetVal;
    }
    int ret = set_moca_conf( mocadoc );
    if( 0 != ret)
    {
        if( SYSCFG_FAILURE == ret)
        {
            execRetVal->ErrorCode = SYSCFG_FAILURE;
            strncpy(execRetVal->ErrorMsg,"sysconfig failure for moca enable",sizeof(execRetVal->ErrorMsg)-1);
        }
        else if( MOCA_HAL_FAILURE  == ret )
        {
            execRetVal->ErrorCode = MOCA_HAL_FAILURE;
            strncpy(execRetVal->ErrorMsg,"moca enable hal call failed",sizeof(execRetVal->ErrorMsg)-1);
        }
        else if( VALIDATION_FALIED == ret)
        {
            execRetVal->ErrorCode = VALIDATION_FALIED;
            strncpy(execRetVal->ErrorMsg,"moca cannot be disabled for video service and force enable",sizeof(execRetVal->ErrorMsg)-1);
        }
        else if( -1 == ret)
        {
            execRetVal->ErrorCode = VALIDATION_FALIED;
            strncpy(execRetVal->ErrorMsg,"moca cannot be enabled/disabled in bridge mode",sizeof(execRetVal->ErrorMsg)-1);
        }
    }
    return execRetVal;
}

/* Callback function to rollback when moca enable blob execution fails */
int rollback_moca_conf()
{
    CcspTraceWarning(("%s: called \n", __FUNCTION__));
    int ret = 0;
    PCOSA_DATAMODEL_MOCA	pMyObject = (PCOSA_DATAMODEL_MOCA )g_MoCAObject;
    PCOSA_DML_MOCA_IF_CFG	pCfg	  =	&pMyObject->MoCAIfFullTable[0].MoCAIfFull.Cfg;
    //pCfg->bEnabled = g_bMocaEnable_bkup;
    moca_cfg_t mocaCfg;
    memset(&mocaCfg, 0, sizeof(moca_cfg_t));
    if( STATUS_SUCCESS == moca_GetIfConfig(0, &mocaCfg))
    {
        if( g_bMocaEnable_bkup != mocaCfg.bEnabled )
        {
            CcspTraceWarning(("%s: updating hal with backup value %d \n", __FUNCTION__, g_bMocaEnable_bkup ));
            mocaCfg.bEnabled = g_bMocaEnable_bkup;
            if ( moca_SetIfConfig(0, &mocaCfg) != STATUS_SUCCESS)
            {
                CcspTraceWarning(("%s: rollback_moca_conf hal call failed \n", __FUNCTION__));
                return MOCA_HAL_FAILURE;
            }
            pCfg->bEnabled = g_bMocaEnable_bkup;
            ret = UpdateToDB( pCfg->bEnabled );
        }
    }
    //ret = UpdateToDB( pCfg->bEnabled );   
    return ret ;
}

void freeResources_moca(void *arg)
{
    CcspTraceInfo((" Entering %s \n",__FUNCTION__));
    execData *blob_exec_data  = (execData*) arg;

    if ( blob_exec_data != NULL )
    {
        mocadoc_t *pmoca = (mocadoc_t *) blob_exec_data->user_data ;
        if ( pmoca != NULL )
        {
            mocadoc_destroy( pmoca );
            pmoca = NULL;

        }
        free(blob_exec_data);
        blob_exec_data = NULL ;
    }
    CcspTraceInfo((" Exiting %s \n",__FUNCTION__));
}
