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
#include <sys/time.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include "safec_lib_common.h"
#include "syscfg/syscfg.h"
#include "webconfig_framework.h"

extern void * g_pDslhDmlAgent;
extern ANSC_HANDLE g_MoCAObject ;

 /* Coverity Issue Fix - CID:56550 : Buffer Over Run*/
#define MAX_GETFORMAT_TIME_SIZE 128

#if defined (_CM_HIGHSPLIT_SUPPORTED_)
static int sysevent_fd_cm;
static token_t sysevent_token_cm;
#endif /* * _CM_HIGHSPLIT_SUPPORTED_ */

void* SynchronizeMoCADevices(void *arg);
void webConfigFrameworkInit();

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

    AnscTraceWarning(("  Initializing WebConfig Framework!\n")); 
    webConfigFrameworkInit();
    AnscTraceWarning(("  Initializing WebConfig Framework done!\n"));

    return  (ANSC_HANDLE)pMyObject;
}

static int sysevent_fd;
static token_t sysevent_token;
static pthread_t sysevent_tid;
static void *Moca_sysevent_handler (void *data)
{
	UNREFERENCED_PARAMETER(data);
        async_id_t moca_update;
	sysevent_setnotification(sysevent_fd, sysevent_token, "moca_updated", &moca_update);
	time_t time_now = { 0 }, time_before = { 0 };

	pthread_detach(pthread_self());

        while(!g_MoCAObject)
          sleep(5);

        PCOSA_DATAMODEL_MOCA            pMyObject     = (PCOSA_DATAMODEL_MOCA)g_MoCAObject;

	for (;;)
    {
        char name[25]={0};
		char val[42]={0};
	int namelen = sizeof(name);
        int vallen  = sizeof(val);
        int err;
	errno_t rc = -1;
	int ind = -1;
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
		    rc = strcmp_s(name, sizeof(name), "moca_updated", &ind);
		    ERR_CHK(rc);
		    if((rc == EOK) && (ind == 0))
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
	char					mac_buff[32] , mac_buff1[256] ;
	errno_t rc = -1;

	Interface_count = CosaDmlMocaGetNumberOfIfs((ANSC_HANDLE)NULL);

	for ( ulIndex = 0; ulIndex < Interface_count; ulIndex++ )
	{
		AssocDeviceCount = 0;
		CosaDmlMocaIfGetAssocDevices
		(
		    (ANSC_HANDLE)NULL, 
		    pMyObject->MoCAIfFullTable[ulIndex].MoCAIfFull.Cfg.InstanceNumber-1, 
		    &AssocDeviceCount,
		    &pMoCAAssocDevice,
		    NULL
		);
		CosaDmlMocaIfGetDinfo(NULL, pMyObject->MoCAIfFullTable[ulIndex].MoCAIfFull.Cfg.InstanceNumber-1, &DynamicInfo);          
		AnscTraceWarning(("----------------------\n"));
                AnscTraceWarning(("MOCA_HEALTH : NCMacAddress %s \n",DynamicInfo.X_CISCO_NetworkCoordinatorMACAddress));
		AnscTraceWarning(("MOCA_HEALTH : Interface %lu , Number of Associated Devices %lu \n", ulIndex+1 , AssocDeviceCount));
		rc = memset_s(mac_buff1, sizeof(mac_buff1), 0, sizeof(mac_buff1));
		ERR_CHK(rc);
		for ( ulIndex1 = 0; ulIndex1 < AssocDeviceCount; ulIndex1++ )
		{
			AnscTraceWarning(("MOCA_HEALTH : Device %lu \n", ulIndex1 + 1));
			AnscTraceWarning(("MOCA_HEALTH : PHYTxRate %lu \n", pMoCAAssocDevice[ulIndex1].PHYTxRate));
			AnscTraceWarning(("MOCA_HEALTH : PHYRxRate %lu \n", pMoCAAssocDevice[ulIndex1].PHYRxRate));
			AnscTraceWarning(("MOCA_HEALTH : TxPowerControlReduction %lu \n", pMoCAAssocDevice[ulIndex1].TxPowerControlReduction));
			AnscTraceWarning(("MOCA_HEALTH : RxPowerLevel %d \n", pMoCAAssocDevice[ulIndex1].RxPowerLevel));			

			rc = memset_s(mac_buff, sizeof(mac_buff), 0, sizeof(mac_buff));
			ERR_CHK(rc);

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
                                char *pMacBuf = mac_buff;
				rc = strcat_s(mac_buff1, sizeof(mac_buff1), pMacBuf);
				ERR_CHK(rc);
				if(ulIndex1 < (AssocDeviceCount-1))
				{
					rc = strcat_s(mac_buff1, sizeof(mac_buff1), ",");
					ERR_CHK(rc);
				}
		}
		AnscTraceWarning(("----------------------\n"));

		AnscTraceWarning(("MOCA_MAC_%lu_TOTAL_COUNT:%lu\n", ulIndex+1 , AssocDeviceCount));
		AnscTraceWarning(("MOCA_MAC_%lu:%s\n", ulIndex+1 , mac_buff1));
		if (pMoCAAssocDevice)
		{
			AnscFreeMemory(pMoCAAssocDevice);
			pMoCAAssocDevice= NULL;
		}   
		
	}
}
void * Logger_Thread(void *data)
{

	UNREFERENCED_PARAMETER(data);
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
		AnscTraceWarning(("Create Logger_Thread error %d\n", res));
	}
	else
	{
		AnscTraceWarning(("Logger Thread Created\n"));
	}

}

#define MOCA_LOG_FILE "/tmp/moca_telemetry_xOpsLogSettings.txt"
#define BUF_LEN (10 * (sizeof(struct inotify_event) + 255 + 1))

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static const char *moca_telemetry_log = "/rdklogs/logs/moca_telemetry.txt";

static unsigned int gMoCALogInterval;
static char gMoCALogEnable[7] = {0};

static int IsFileExists(char *file_name)
{
    struct stat file;

    return (stat(file_name, &file));
}

static char *get_formatted_time(char *time)
{
    struct tm *tm_info;
    struct timeval tv_now;
    char tmp[MAX_GETFORMAT_TIME_SIZE] = {0};

    gettimeofday(&tv_now, NULL);
    tm_info = localtime(&tv_now.tv_sec);

    strftime(tmp, 128, "%y%m%d-%T", tm_info);

    if( snprintf(time, 128, "%s.%06lu", tmp, tv_now.tv_usec) >= 128) {
        CcspTraceWarning(( "%s Truncation while formatting time, expected_time:%s.%06d current_time:%s\n", __FUNCTION__ , tmp, (int)tv_now.tv_usec, time));
    }
    return time;
}

static void write_to_file(const char *file_name, char *fmt, ...)
{
    FILE *fp = NULL;
    va_list args;

    fp = fopen(file_name, "a+");
    if (fp == NULL) {
        return;
    }

    va_start(args, fmt);
    vfprintf(fp, fmt, args);
    va_end(args);

    fflush(fp);
    fclose(fp);
}

static void read_updated_log_interval()
{
    FILE *fp = NULL;
     /* Coverity Issue Fix - CID:56550 : Buffer Over Run*/
    char buff[256] = {0}, tmp[MAX_GETFORMAT_TIME_SIZE] = {0};
    errno_t rc = -1;
    fp = fopen(MOCA_LOG_FILE, "r");
    if (fp == NULL) {
        CcspTraceError(("%s  %s file open error!!!\n", __func__, MOCA_LOG_FILE));
	return;
    }
    if (fscanf(fp, "%d,%6s", &gMoCALogInterval, gMoCALogEnable) == EOF) {
        CcspTraceError(("%s Failed to read from file %s!!!\n", __func__, MOCA_LOG_FILE));
        fclose(fp);
        return;
    }
    fclose(fp);

    get_formatted_time(tmp);
    snprintf(buff, 256, "%s MOCA_TELEMETRY_LOG_PERIOD:%d\n", tmp, gMoCALogInterval);
    write_to_file(moca_telemetry_log, buff);
    
    rc = memset_s(tmp, sizeof(tmp), 0, sizeof(tmp));
    ERR_CHK(rc);
    get_formatted_time(tmp);  
    snprintf(buff, 256, "%s MOCA_TELEMETRY_LOG_ENABLED:%s\n", tmp, gMoCALogEnable);
    write_to_file(moca_telemetry_log, buff);
}

static void MocaTelemetryPush()
{
    PCOSA_DATAMODEL_MOCA            pMyObject           = (PCOSA_DATAMODEL_MOCA)g_MoCAObject;
    ULONG Interface_count, ulIndex,  ulIndex1, AssocDeviceCount;
    PCOSA_DML_MOCA_ASSOC_DEVICE             pMoCAAssocDevice = NULL;
    char                                    mac_buff[32] , mac_buff1[2048];
    char tmp[128] = {0}, buff[2256] = {0};
    errno_t rc = -1;    

    Interface_count = CosaDmlMocaGetNumberOfIfs((ANSC_HANDLE)NULL);

    for ( ulIndex = 0; ulIndex < Interface_count; ulIndex++ )
    {
	AssocDeviceCount = 0;
	CosaDmlMocaIfGetAssocDevices
	    (
	     (ANSC_HANDLE)NULL,
	     pMyObject->MoCAIfFullTable[ulIndex].MoCAIfFull.Cfg.InstanceNumber-1,
	     &AssocDeviceCount,
	     &pMoCAAssocDevice,
	     NULL
	    );

	rc = memset_s(mac_buff1, sizeof(mac_buff1), 0,sizeof(mac_buff1));
	ERR_CHK(rc);
	for ( ulIndex1 = 0; ulIndex1 < AssocDeviceCount; ulIndex1++ )
	{
	    rc = memset_s(mac_buff, sizeof(mac_buff), 0,sizeof(mac_buff));
	    ERR_CHK(rc);

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
            char *pMacBuf = mac_buff;
	    rc = strcat_s(mac_buff1, sizeof(mac_buff1), pMacBuf);
	    ERR_CHK(rc);
	    if(ulIndex1 < (AssocDeviceCount-1))
            {
		rc = strcat_s(mac_buff1, sizeof(mac_buff1), ",");
		ERR_CHK(rc);
	    }
	}

	rc = memset_s(tmp, sizeof(tmp), 0, sizeof(tmp));
	ERR_CHK(rc);
	get_formatted_time(tmp);
	rc = memset_s(buff, sizeof(buff), 0, sizeof(buff));
	ERR_CHK(rc);
	snprintf(buff, 2048, "%s MOCA_MAC_%lu_TOTAL_COUNT:%lu\n", tmp, ulIndex+1 , AssocDeviceCount);
	write_to_file(moca_telemetry_log, buff);
	rc = memset_s(tmp, sizeof(tmp), 0, sizeof(tmp));
	ERR_CHK(rc);
	get_formatted_time(tmp);
	snprintf(buff, 2256, "%s MOCA_MAC_%lu:%s\n", tmp, ulIndex+1 , mac_buff1);
	write_to_file(moca_telemetry_log, buff);
	if (pMoCAAssocDevice)
	{
	    AnscFreeMemory(pMoCAAssocDevice);
	    pMoCAAssocDevice= NULL;
	}
    }
}

void*  MocaTelemetryxOpsLogSettingsEventThread()
{
    struct inotify_event *event;
    int inotifyFd, numRead;
    char moca_log_buf[BUF_LEN] __attribute__ ((aligned(8))) = {0}, *ptr = NULL;

    while (!(IsFileExists(MOCA_LOG_FILE) == 0)) {
        CcspTraceError(("%s  %s file not found wait for 5 seconds and check \n", __func__, MOCA_LOG_FILE));
        sleep(5);
    }

    read_updated_log_interval();

    inotifyFd = inotify_init();
    inotify_add_watch(inotifyFd, MOCA_LOG_FILE, IN_MODIFY);

    for (;;)
    {
        numRead = read(inotifyFd, moca_log_buf, BUF_LEN);
        if (numRead < 0) {
            CcspTraceError(("%s Error returned is = numRead %d  %d\n", __func__, numRead, __LINE__));
        }

        /* Process all of the events in buffer returned by read() */
        for (ptr = moca_log_buf; ptr < moca_log_buf + numRead; )
        {
            event = (struct inotify_event *) ptr;
            if (event->mask & IN_MODIFY)
            {
		read_updated_log_interval();
		pthread_cond_signal(&cond);
            }
            ptr += sizeof(struct inotify_event) + event->len;
        }

    }
}

void* MocaTelemetryLoggingThread()
{
    struct timeval now;
    struct timespec t;
    int rc = 0;

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    while (1) { 
	gettimeofday(&now,NULL);
	t.tv_nsec = 0;
	t.tv_sec = now.tv_sec + gMoCALogInterval;
	pthread_mutex_lock(&mutex);
	rc = pthread_cond_timedwait(&cond, &mutex, &t);
	pthread_mutex_unlock(&mutex);
	if (rc == ETIMEDOUT) {
	    if (strncmp(gMoCALogEnable, "true", 5) == 0) {
		MocaTelemetryPush();
	    }
	}
    }
}

#if defined (_CM_HIGHSPLIT_SUPPORTED_)
/* * CosaMoCAIsCMHighSplitDiplexerMode() */
unsigned char CosaMoCAIsCMHighSplitDiplexerMode( void )
{
    char acDiplexerMode[16] = {0}; 

    if( ( 0 == sysevent_get( sysevent_fd_cm, sysevent_token_cm, "cm_diplexer_mode", acDiplexerMode,sizeof(acDiplexerMode) ) ) &&
        ( acDiplexerMode[0] != '\0' ) &&
        ( 0 == strcmp( acDiplexerMode, "high_split" ) ) )
    {
        return TRUE;
    } 

    return FALSE;
}
#endif /* * _CM_HIGHSPLIT_SUPPORTED_ */

int CosaMoCATelemetryInit()
{
    int i = 0;
    pthread_t tid[2];
    void *(*MocaTelemetryThread[2])() = {MocaTelemetryxOpsLogSettingsEventThread, MocaTelemetryLoggingThread};
    pthread_attr_t tattr;

    pthread_attr_init(&tattr);
    pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);

    for (i = 0; i < 2; i++) {
	int err = pthread_create(&tid[i], &tattr, MocaTelemetryThread[i], NULL);
	if (0 != err)
	{
	    CcspTraceError(("%s: Error creating the MoCA telemetry thread %d!\n", __FUNCTION__, __LINE__));
	    pthread_attr_destroy( &tattr );
	    return -1;
	}
    }
    pthread_attr_destroy( &tattr );

    return ANSC_STATUS_SUCCESS;
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
    //PCOSA_PLUGIN_INFO         pPlugInfo      = (PCOSA_PLUGIN_INFO       )g_pCosaBEManager->hCosaPluginInfo;
    ANSC_HANDLE               pPoamMoCADm    = (ANSC_HANDLE )NULL;
    ANSC_HANDLE               pSlapMoCADm    = (ANSC_HANDLE )NULL;
    ULONG                     ulCount        = 0;
    ULONG                     ulIndex        = 0;
    ULONG                     ulNextInsNum   = 0;
    pthread_t tid;
    errno_t rc = -1;

/*
    pProc = (COSAGetHandleProc)pPlugInfo->AcquireFunction("COSAGetLPCRole");

    if ( pProc )
    {
        ulRole = (ULONG)(*pProc)(g_pDslhDmlAgent);
        CcspTraceWarning(("CosaMoCAInitialize - LPC role is %lu...\n", ulRole));
    }
*/
    pMyObject->pSlapMoCADm = (ANSC_HANDLE)pSlapMoCADm;
    pMyObject->pPoamMoCADm = (ANSC_HANDLE)pPoamMoCADm;

#if defined (_CM_HIGHSPLIT_SUPPORTED_)
    //Placed sysevent initialization here for common usage 
    sysevent_fd_cm = sysevent_open("127.0.0.1", SE_SERVER_WELL_KNOWN_PORT, SE_VERSION, "moca_cm_event", &sysevent_token_cm);
#endif /* * _CM_HIGHSPLIT_SUPPORTED_ */

    CosaDmlMocaInit(NULL, (ANSC_HANDLE)&pMyObject);

    CosaDmlMocaGetCfg(NULL, &pMyObject->MoCACfg);

    rc = memset_s(pMyObject->MoCAIfFullTable, sizeof(pMyObject->MoCAIfFullTable), 0, sizeof(pMyObject->MoCAIfFullTable));
    ERR_CHK(rc);

    ulCount = CosaDmlMocaGetNumberOfIfs((ANSC_HANDLE)pPoamMoCADm);
    if ( ulCount > MOCA_INTEFACE_NUMBER )
    {
        AnscTraceWarning(("CosaMoCAInitialize -- ERROR!!!!! the real MoCA interface number(%lu) is bigger than predefined number(%d).\n", ulCount, MOCA_INTEFACE_NUMBER));
        assert(ulCount <= MOCA_INTEFACE_NUMBER );
    }
    
    ulNextInsNum = 1;
    pMyObject->MoCAIfFullTable[0].MoCAIfFull.Cfg.InstanceNumber = 1;
    for ( ulIndex = 0; ulIndex < ulCount; ulIndex++ )
    {
        CosaDmlMocaIfGetEntry((ANSC_HANDLE)pPoamMoCADm, ulIndex, &pMyObject->MoCAIfFullTable[ulIndex].MoCAIfFull);
        
        AnscSListInitializeHeader( &pMyObject->MoCAIfFullTable[ulIndex].pMoCAExtCounterTable );
        AnscSListInitializeHeader( &pMyObject->MoCAIfFullTable[ulIndex].pMoCAExtAggrCounterTable );
        //AnscSListInitializeHeader( &pMyObject->MoCAIfFullTable[ulIndex].MoCAMeshTxNodeTable );

        CosaDmlMocaIfGetQos(NULL, ulIndex, &pMyObject->MoCAIfFullTable[ulIndex].MoCAIfQos);
        
        pMyObject->MoCAIfFullTable[ulIndex].MoCAIfFull.Cfg.InstanceNumber = ulNextInsNum++;    
    }
        
    //If MoCA enabled check the bridge mode and cm diplexer mode.
    if  (TRUE == pMyObject->MoCAIfFullTable[0].MoCAIfFull.Cfg.bEnabled) 
    {
        char bridgeMode[16] = {0};
        int mode = 0;

        syscfg_get(NULL, "bridge_mode", bridgeMode, sizeof(bridgeMode));
        mode = atoi(bridgeMode);

        //If bridge mode enabled or high split mode enabled then Disable MoCA.
        if ( ( 0 != mode ) 
#if defined (_CM_HIGHSPLIT_SUPPORTED_)
             || ( TRUE == CosaMoCAIsCMHighSplitDiplexerMode() )
#endif /* * _CM_HIGHSPLIT_SUPPORTED_ */
           ) 
        {
            /*Coverity Fix CID:65666 CHECKED_RETURN */
            if( CosaDmlMocaIfSetCfg( NULL, 0, &pMyObject->MoCAIfFullTable[0].MoCAIfFull.Cfg) == ANSC_STATUS_FAILURE )
                  CcspTraceError(("RDK_LOG_ERROR, CcspMoCA %s : CosaDmlMocaIfSetCfg  create Error!!!\n", __FUNCTION__));
        }
    }
    CosaMoCAGetForceEnable(&pMyObject->MoCACfg);

    CosaDmlMocaGetLogStatus(&pMyObject->LogStatus);
    CosaMoCAUpdate();
    CosaMoCALogger();

    if (pthread_create(&tid, NULL, SynchronizeMoCADevices, NULL))
    {
        CcspTraceError(("RDK_LOG_ERROR, CcspMoCA %s : Failed to Start Thread to start SynchronizeMoCADevices  \n", __FUNCTION__ ));
        return ANSC_STATUS_FAILURE;
    }

    CosaMocaTelemetryxOpsLogSettingsSync();
    if (CosaMoCATelemetryInit() < 0) {
	CcspTraceError(("RDK_LOG_ERROR, CcspMoCA %s : CosaMoCATelemetryInit create Error!!!\n", __FUNCTION__));
	return ANSC_STATUS_FAILURE;
    }

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
    
    //PCOSA_PLUGIN_INFO         pPlugInfo         = (PCOSA_PLUGIN_INFO       )g_pCosaBEManager->hCosaPluginInfo;
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
    AnscFreeMemory((ANSC_HANDLE)pMyObject);

    return returnStatus;
}
