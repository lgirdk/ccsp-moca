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

    module: cosa_moca_apis.c

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

#include "cosa_moca_apis.h"
#include "cosa_moca_internal.h"
#include "safec_lib_common.h"
#include "syscfg/syscfg.h"
#include "msgpack.h"
#include "cosa_moca_param.h"
#include "cosa_moca_helpers.h"
#include "cosa_moca_webconfig_api.h"
#include "ccsp_trace.h"
#include <trower-base64/base64.h>

#ifndef CONFIG_SYSTEM_MOCA
#define _COSA_SIM_ 1 
#undef _COSA_HAL_
#endif

#if  _COSA_HAL_

#include "moca_hal.h"

//upstreamed ccsp_moca_diag.patch as part of RDKB-41505
#if defined (_XB6_PRODUCT_REQ_) && defined (_COSA_BCM_ARM_)
PCOSA_DML_MOCA_IF_SINFO       gpSInfo;
#endif
#if 0
#ifdef AnscTraceWarning
#undef AnscTraceWarning
#define AnscTraceWarning(a) printf("%s:%d> ", __FUNCTION__, __LINE__); printf a
#endif
#endif

static unsigned int moca_enable_db=0xFF;
extern ANSC_HANDLE g_MoCAObject ;

/* MoCA Reset US - RDKB-22615 use this MOCA_RESET_DELAY_IN_SECS macro */
#define MOCA_RESET_DELAY_IN_SECS		5

/* This Marco to check return status of moca_SetIfConfig HAL function */
#define  STATUS_SUCCESS     0

#ifdef STATUS_FAILURE
 #undef STATUS_FAILURE
 #define STATUS_FAILURE     0xFFFFFFFF
#endif

#define MOCA_INTEFACE_NUMBER    1
extern  ANSC_HANDLE                        bus_handle;
#if 0
/* Just fill in USGv2 required parameters */
COSA_DML_MOCA_IF_CFG   g_MoCAIfCfg1 = 
    {  
       .bEnabled = FALSE,
       .X_CISCO_COM_Reset = FALSE,
       .X_CISCO_COM_MixedMode = FALSE,
       .X_CISCO_COM_ChannelScanning = FALSE,
       .X_CISCO_COM_AutoPowerControlEnable = FALSE,
       .AutoPowerControlPhyRate = 235,
       .X_CISCO_COM_EnableTabooBit = FALSE,
       .PrivacyEnabledSetting = FALSE,
       .KeyPassphrase = "KeyPassphrase",
       .bPreferredNC = FALSE,
       .FreqCurrentMaskSetting = "0000000002000000",
       .X_CISCO_COM_ChannelScanMask = "0000000001400000",
       .TxPowerLimit = 7,
       .BeaconPowerLimit = 0
    };

/* Just fill in USGv2 required parameters */
COSA_DML_MOCA_IF_CFG   g_MoCAIfCfg2 = 
    {
       .bEnabled = FALSE,
       .X_CISCO_COM_Reset = FALSE,
       .X_CISCO_COM_MixedMode = FALSE,
       .X_CISCO_COM_ChannelScanning = FALSE,
       .X_CISCO_COM_AutoPowerControlEnable = FALSE,
       .AutoPowerControlPhyRate = 235,
       .X_CISCO_COM_EnableTabooBit = FALSE,
       .PrivacyEnabledSetting = FALSE,
       .KeyPassphrase = "KeyPassphrase",
       .bPreferredNC = FALSE,
       .FreqCurrentMaskSetting = "0000000002000000",
       .X_CISCO_COM_ChannelScanMask = "0000000001400000",
       .TxPowerLimit = 7,
       .BeaconPowerLimit = 0
    };
#endif

#ifdef SA_CUSTOM
static void write_log_console(char* msg)
{
        FILE *pFile = NULL;
        char buffer[128]={0};

        char timestring[26]={0};
        time_t timer;
        struct tm* tm_info;

        // Get local time and store it in timestring
        time(&timer);
        tm_info = localtime(&timer);
        strftime(timestring, 26, "%Y-%m-%d %H:%M:%S", tm_info);

        //combine time string and the input msg
        sprintf(buffer, "CiscoARM : %s %s", timestring, msg);
        pFile = fopen("/rdklogs/logs/Consolelog.txt.0","a+");
        if(pFile)
        {
                fprintf(pFile, buffer);
                fprintf(pFile, "\n");
                fclose(pFile);
        }
        return;
}
#endif

COSA_DML_MOCA_CFG     g_MoCACfg = 
    { 
        "X_CISCO_COM_ProvisioningFilename",
        "2031:0000:1F1F:0000:0000:0100:11A0:ADDF",
        false, 
        MOCA_PROVISIONING_SERVADDR_TYPE_IPV6
    };

COSA_DML_MOCA_ASSOC_DEVICE   g_MoCAAssocDevice[5] = 
    { 
        {   
            .MACAddress= "00:00:0c:9f:f0:01",
            .NodeID = 1, 
            .TxPackets=6601,
            .RxErroredAndMissedPackets = 6602,
            .PHYTxRate = 6603,
            .PHYRxRate = 6604,
            .TxBcastRate = 6605,
            .X_CISCO_COM_RxBcastRate = 6606, 
        },
        {   
            .MACAddress= "00:00:0c:9f:f0:02",
            .NodeID = 2, 
            .TxPackets=6607,
            .RxErroredAndMissedPackets = 6608,
            .PHYTxRate = 6609,
            .PHYRxRate = 6610,
            .TxBcastRate = 6611,
            .X_CISCO_COM_RxBcastRate = 6612, 
        },
        {   
            .MACAddress= "00:00:0c:9f:f0:03",
            .NodeID = 3, 
            .TxPackets=6613,
            .RxErroredAndMissedPackets = 6614,
            .PHYTxRate = 6615,
            .PHYRxRate = 6616,
            .TxBcastRate = 6617,
            .X_CISCO_COM_RxBcastRate = 6618, 
        },
        {   
            .MACAddress= "00:00:0c:9f:f0:04",
            .NodeID = 4, 
            .TxPackets=6619,
            .RxErroredAndMissedPackets = 6620,
            .PHYTxRate = 6621,
            .PHYRxRate = 6622,
            .TxBcastRate = 6623,
            .X_CISCO_COM_RxBcastRate = 6624, 
        },
        {   
            .MACAddress= "00:00:0c:9f:f0:05",
            .NodeID = 5, 
            .TxPackets=6625,
            .RxErroredAndMissedPackets = 6626,
            .PHYTxRate = 6627,
            .PHYRxRate = 6628,
            .TxBcastRate = 6629,
            .X_CISCO_COM_RxBcastRate = 6630, 
        }
    };


COSA_DML_MOCA_EXTCOUNTER g_MoCAExtCounter[5] = 
    {
        {
            .Type=1,
            .Map=6601,
            .Rsrv=6602,
            .Lc=6603,
            .Adm=6604,
            .Probe=6605,
            .Async=6606
        },
        {
            .Type=2,
            .Map=6607,
            .Rsrv=6608,
            .Lc=6609,
            .Adm=6610,
            .Probe=6611,
            .Async=6612
        },
        {
            .Type=3,
            .Map=6613,
            .Rsrv=6614,
            .Lc=6615,
            .Adm=6616,
            .Probe=6617,
            .Async=6618
        },
        {
            .Type=4,
            .Map=6619,
            .Rsrv=6620,
            .Lc=6621,
            .Adm=6622,
            .Probe=6623,
            .Async=6624
        },
        {
            .Type=5,
            .Map=6625,
            .Rsrv=6626,
            .Lc=6627,
            .Adm=6628,
            .Probe=6629,
            .Async=6630
        }
    };

COSA_DML_MOCA_EXTAGGRCOUNTER g_MoCAExtAggrCounter[2] = 
    {
        {
            .Tx=6601,
            .Rx=6602
        },
        {
            .Tx=6603,
            .Rx=6604
        }
    };


COSA_DML_MOCA_PEER  g_MoCAPeer[2] = 
    {
        {
            6601
        },
        {
            6602
        }
    };

COSA_DML_MOCA_MESH  g_MoCAMesh[2] = 
    {
        {
        .TxNodeID=6602,
        .RxNodeID=6601,
        .TxRate=6001
        },
        {
        .TxNodeID=6609,
        .RxNodeID=6608,
        .TxRate=6002
        }
    };

COSA_DML_MOCA_FLOW  g_MoCAFlow[2] = 
    {
        {
            .FlowID=1,
            .IngressNodeID=6601,
            .EgressNodeID=6602,
            .FlowTimeLeft=6603,
            .DestinationMACAddress="01:00:5e:00:00:fc",
            .PacketSize=6604,
            .PeakDataRate=6605,
            .BurstSize=6606,
            .FlowTag=6607
        },
        {
            .FlowID=2,
            .IngressNodeID=6608,
            .EgressNodeID=6609,
            .FlowTimeLeft=6610,
            .DestinationMACAddress="01:00:5e:00:00:fc",
            .PacketSize=6611,
            .PeakDataRate=6612,
            .BurstSize=6613,
            .FlowTag=6614
        }
    };

static int is_moca_available = 0;

#define JUDGE_MOCA_HARDWARE_AVAILABLE(RET) \
    if (!is_moca_available && ((moca_enable_db==0xFF || moca_enable_db==0) ? 1 : ((is_moca_available = moca_HardwareEquipped()) == FALSE))) { \
        CcspTraceWarning((" -- Moca hardware is not available.\n")); \
        return RET; \
    }; 

#define MOCA_LOGVALUE_FILE "/tmp/moca_telemetry_xOpsLogSettings.txt"


void CosaMocaTelemetryxOpsLogSettingsSync()
{
    FILE *fp = fopen(MOCA_LOGVALUE_FILE, "w");
    if (fp != NULL) {
        char log_period[32] = {0};
        char log_enable[32] = {0};
        if((syscfg_get( NULL, "moca_log_enabled", log_enable, sizeof(log_enable)) != 0) || (log_enable[0] == '\0'))
        {
                CcspTraceWarning(("moca_log_enabled syscfg failed\n"));
                sprintf(log_enable,"false");
        }

        if((syscfg_get( NULL, "moca_log_period", log_period, sizeof(log_period)) != 0) || (log_period[0] == '\0'))
        {
                CcspTraceWarning(("moca_log_period syscfg failed\n"));
                sprintf(log_period,"3600");
        }

        fprintf(fp,"%s,%s\n", log_period, log_enable);
        fclose(fp);
    }
}


ULONG
CosaDmlGetMocaHardwareStatus
    (
        ANSC_HANDLE                 hContext
    )
{
    UNREFERENCED_PARAMETER(hContext);
    static int ret = 0;
    int retries = 0;

    if (ret) {
        return ret;
    }

    do
    {
       ret = moca_HardwareEquipped();
       CcspTraceInfo(("-- Moca hardware status check#%d.\n", retries));
       if(ret)
          break;
       else
          sleep(1);
    }while(retries++ < 5);

    char buf[10]={0};
    if( (syscfg_get( NULL, "moca_enabled", buf, sizeof(buf)) == 0) && (strlen(buf) != 0) )
    {
	  moca_enable_db=atoi(buf);
	  CcspTraceWarning(("-- Moca enable flag %d(%s).\n", moca_enable_db, buf)); 
    } 
    return ret;
}

ANSC_STATUS
CosaDmlMocaInit
    (
        ANSC_HANDLE                 hDml,
        PANSC_HANDLE                phContext
)
{
    UNREFERENCED_PARAMETER(hDml);
    UNREFERENCED_PARAMETER(phContext);


    if (CosaDmlGetMocaHardwareStatus(NULL) != 1 ) { 
        CcspTraceWarning(("-- Moca hardware is not available.\n")); 
        is_moca_available = 0;
	return ANSC_STATUS_FAILURE; 
    } else {
        is_moca_available = 1;
    }

    AnscTraceWarning(("CosaDmlMocaInit -- \n"));


    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlMocaGetCfg
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_MOCA_CFG          pCfg
    )
{
    UNREFERENCED_PARAMETER(hContext);
    JUDGE_MOCA_HARDWARE_AVAILABLE(ANSC_STATUS_FAILURE)

    AnscTraceWarning(("CosaDmlMocaGetCfg -- .\n"));

    *pCfg = g_MoCACfg;
    
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlMocaSetCfg
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_MOCA_CFG          pCfg
    )
{
    UNREFERENCED_PARAMETER(hContext);
    JUDGE_MOCA_HARDWARE_AVAILABLE(ANSC_STATUS_FAILURE)

    AnscTraceWarning(("CosaDmlMocaSetCfg -- %s %s %s.\n", 
        pCfg->X_CISCO_COM_ProvisioningFilename, 
        pCfg->X_CISCO_COM_ProvisioningServerAddress, 
        (pCfg->X_CISCO_COM_ProvisioningServerAddressType==1)?"IPv4":"IPv6"));


    return ANSC_STATUS_SUCCESS;
}

ULONG
CosaDmlMocaGetNumberOfIfs
    (
        ANSC_HANDLE                 hContext
    )
{
    UNREFERENCED_PARAMETER(hContext);
    JUDGE_MOCA_HARDWARE_AVAILABLE(0)

    AnscTraceWarning(("CosaDmlMocaGetNumberOfIfs -- Number:%d.\n", MOCA_INTEFACE_NUMBER));

    return MOCA_INTEFACE_NUMBER;
}

ANSC_STATUS
CosaDmlMocaIfGetEntry
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulInterfaceIndex,
        PCOSA_DML_MOCA_IF_FULL      pEntry
    )
{
    JUDGE_MOCA_HARDWARE_AVAILABLE(ANSC_STATUS_FAILURE)
    errno_t rc = -1;

    AnscTraceWarning(("CosaDmlMocaIfGetEntry -- ulInterfaceIndex:%lu.\n", ulInterfaceIndex));

    if (pEntry)
    {
        rc = memset_s(pEntry, sizeof(COSA_DML_MOCA_IF_FULL), 0, sizeof(COSA_DML_MOCA_IF_FULL));
        ERR_CHK(rc);
    }
    else
    {
        return ANSC_STATUS_FAILURE;
    }
        
    if (ulInterfaceIndex == 0)
    {
        CosaDmlMocaIfGetCfg(hContext, ulInterfaceIndex, &pEntry->Cfg);
        CosaDmlMocaIfGetDinfo(hContext, ulInterfaceIndex, &pEntry->DynamicInfo);
       // moca_IfGetStaticInfo(ulInterfaceIndex, &pEntry->StaticInfo);
       CosaDmlMocaIfGetStaticInfo(hContext,ulInterfaceIndex,&pEntry->StaticInfo);				
		//AnscCopyString(pEntry->StaticInfo.Name, "sw_5");
    }
    else
    {
        return ANSC_STATUS_FAILURE;
    }

    return ANSC_STATUS_SUCCESS;
}

#define kMax_beaconPwrLen               3
#define kMax_TxPowerLimit               3
#define kMax_AutoPowerControlPhyRate    4
#define kMax_FreqCurrentMaskSetting     5
#define kMax_StringValue                20

ANSC_STATUS
CosaDmlMocaIfReset
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulInterfaceIndex,
	PCOSA_DML_MOCA_CFG	    MCfg,
        PCOSA_DML_MOCA_IF_CFG       pCfg,
	PCOSA_DML_MOCA_IF_DINFO     pInfo
    )
{
    UNREFERENCED_PARAMETER(hContext);
    JUDGE_MOCA_HARDWARE_AVAILABLE(ANSC_STATUS_FAILURE)
    moca_cfg_t mocaCfg;
    memset(&mocaCfg, 0, sizeof(moca_cfg_t));

    moca_dynamic_info_t mocaDInfo;
    memset(&mocaDInfo, 0, sizeof(moca_dynamic_info_t));

    int return_status = STATUS_FAILURE;

    if ( !pCfg )
    {
        return ANSC_STATUS_FAILURE;
    }

    if (!pInfo)
    {
        return ANSC_STATUS_FAILURE;
    }

    if ( ulInterfaceIndex != 0 )
    {
	return ANSC_STATUS_FAILURE;
    }

    if (pCfg->bEnabled == FALSE)
    {
	CcspTraceWarning(("No need to reset MoCA if MoCA is already disabled\n"));
	return ANSC_STATUS_FAILURE;
    }

		/* To obtain Operational Status info from moca_IfGetDynamicInfo HAL function */
		moca_IfGetDynamicInfo(ulInterfaceIndex, &mocaDInfo);
		pInfo->Status = mocaDInfo.Status;

		/* To check for Operational state of MoCA Before Reset */
		CcspTraceWarning(("Operational state of MoCA before Reset : %s\n", (pInfo->Status==1)?"Up":(pInfo->Status==2)?"Down":(pInfo->Status==3)?"Unknown":(pInfo->Status==4)?"Dormant":(pInfo->Status==5)?"NotPresent":(pInfo->Status==6)?"LowerLayerDown":"Error"));

		/* To Check for Provisioning state of MOCA Before Reset */
		CcspTraceWarning(("Provisioning state of MoCA before Reset. CosaDmlMocaIfReset -- ProvisioningFilename:%s, ProvisioningServerAddress:%s, ProvisioningServer ddressType:%s\n", MCfg->X_CISCO_COM_ProvisioningFilename, MCfg->X_CISCO_COM_ProvisioningServerAddress, (MCfg->X_CISCO_COM_ProvisioningServerAddressType==1)?"IPv4":"IPv6"));

	        /* Get default value of MoCA Interface*/
		moca_GetIfConfig(ulInterfaceIndex, &mocaCfg);

		/* MoCA Interface Setting to FALSE and syscfg commit it. That mean, we are disabled the MoCA interface here */
		CcspTraceWarning(("%s > Disabling MoCA Interface...\n", __func__));
		pCfg->bEnabled = FALSE;
		mocaCfg.bEnabled = pCfg->bEnabled;
		return_status = moca_SetIfConfig(ulInterfaceIndex, &mocaCfg);

		if(return_status == STATUS_SUCCESS)
		{
			if (syscfg_set_commit(NULL, "moca_enabled", "0") != 0)
			{
				AnscTraceWarning(("syscfg_set failed\n"));
			}
		}
		else
		{
			CcspTraceWarning(("MoCA Interface not set to FALSE\n"));
			return ANSC_STATUS_FAILURE;
		}

		/* 5 second delay given for MoCA Reset */
		CcspTraceWarning(("5s Delay starts\n"));
		sleep(MOCA_RESET_DELAY_IN_SECS);
		CcspTraceWarning(("5s Delay ends\n"));

		/* MoCA Interface Setting to TRUE and syscfg commit it. That mean, we are Enabled the MoCA interface here */
		CcspTraceWarning(("%s > Enabling MoCA Interface...\n", __func__));
		pCfg->bEnabled = TRUE;
		mocaCfg.bEnabled = pCfg->bEnabled;
		return_status = moca_SetIfConfig(ulInterfaceIndex, &mocaCfg);

		if(return_status == STATUS_SUCCESS)
		{
			if (syscfg_set_commit(NULL, "moca_enabled", "1") != 0)
			{
				AnscTraceWarning(("syscfg_set failed\n"));
			}
		}
		else
		{
			CcspTraceWarning(("MoCA Interface not set to TRUE\n"));
			return ANSC_STATUS_FAILURE;
		}

	        /* To check for Operational state of MoCA After Reset */
		CcspTraceWarning(("Operational state of MoCA After Reset : %s\n", (pInfo->Status==1)?"Up":(pInfo->Status==2)?"Down":(pInfo->Status==3)?"Unknown":(pInfo->Status==4)?"Dormant":(pInfo->Status==5)?"NotPresent":(pInfo->Status==6)?"LowerLayerDown":"Error"));

		/* To Check for Provisioning state of MOCA After Reset */
		CcspTraceWarning(("Provisioning state of MoCA After Reset. CosaDmlMocaIfReset -- ProvisioningFilename:%s, ProvisioningServerAddress:%s, ProvisioningServerAddressType:%s\n", MCfg->X_CISCO_COM_ProvisioningFilename, MCfg->X_CISCO_COM_ProvisioningServerAddress, (MCfg->X_CISCO_COM_ProvisioningServerAddressType==1)?"IPv4":"IPv6"));
    
return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlMocaIfSetCfg
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulInterfaceIndex,
        PCOSA_DML_MOCA_IF_CFG       pCfg
    )
{
    UNREFERENCED_PARAMETER(hContext);
    char str_value[kMax_StringValue];
    int status,mode=0;
    int freq;
    char bridgeMode[64] = {0};
    errno_t rc = -1;
    moca_cfg_t mocaCfg;

    JUDGE_MOCA_HARDWARE_AVAILABLE(ANSC_STATUS_FAILURE)

    rc = memset_s(&mocaCfg, sizeof(moca_cfg_t), 0, sizeof(moca_cfg_t));
    ERR_CHK(rc);

    AnscTraceWarning(("CosaDmlMocaIfSetCfg -- ulInterfaceIndex:%lu.\n", ulInterfaceIndex));
    
    if ( !pCfg )
    {
        return ANSC_STATUS_FAILURE;
    }
    syscfg_get(NULL,"bridge_mode",bridgeMode,sizeof(bridgeMode));
    mode=atoi(bridgeMode);
    
    if ( ulInterfaceIndex == 0 )
    {
#if defined (_CM_HIGHSPLIT_SUPPORTED_)
         unsigned char IsHighSplitEnabled = CosaMoCAIsCMHighSplitDiplexerMode();
#endif /* * _CM_HIGHSPLIT_SUPPORTED_ */

         if(pCfg->X_CISCO_COM_Reset == TRUE) {

             AnscTraceWarning(("Resetting MoCA to factory default settings\n"));

			/* Translate the data structures */
			mocaCfg.InstanceNumber 					= pCfg->InstanceNumber;
			rc = strcpy_s(mocaCfg.Alias, sizeof(mocaCfg.Alias), pCfg->Alias);
                        if(rc != EOK)
                        {
                             ERR_CHK(rc);
                             return ANSC_STATUS_FAILURE;
                        }
			rc = memcpy_s(mocaCfg.FreqCurrentMaskSetting, sizeof(mocaCfg.FreqCurrentMaskSetting), pCfg->FreqCurrentMaskSetting, sizeof(mocaCfg.FreqCurrentMaskSetting));
                        if(rc != EOK)
                        {
                           ERR_CHK(rc);
                           return ANSC_STATUS_FAILURE;
                        }
			rc = memcpy_s(mocaCfg.KeyPassphrase, sizeof(mocaCfg.KeyPassphrase), pCfg->KeyPassphrase, sizeof(mocaCfg.KeyPassphrase));
                        if(rc != EOK)
                        {
                           ERR_CHK(rc);
                           return ANSC_STATUS_FAILURE;
                        }
			mocaCfg.MaxIngressBWThreshold 			= pCfg->MaxIngressBWThreshold;
			mocaCfg.MaxEgressBWThreshold 			= pCfg->MaxEgressBWThreshold;
			mocaCfg.Reset 							= pCfg->X_CISCO_COM_Reset;
			mocaCfg.MixedMode 						= pCfg->X_CISCO_COM_MixedMode;
			mocaCfg.ChannelScanning 				= pCfg->X_CISCO_COM_ChannelScanning;
			rc = memcpy_s(mocaCfg.NodeTabooMask, sizeof(mocaCfg.NodeTabooMask), pCfg->NodeTabooMask, sizeof(mocaCfg.NodeTabooMask));
                        if(rc != EOK)
                        {
                           ERR_CHK(rc);
                           return ANSC_STATUS_FAILURE;
                        }
			/* Default Values Due to Reset Condition */
			mocaCfg.ChannelScanning 				= TRUE;
			mocaCfg.bPreferredNC 					= TRUE;
			mocaCfg.PrivacyEnabledSetting 			= FALSE;
			mocaCfg.bEnabled 						= TRUE;
			mocaCfg.AutoPowerControlEnable 			= TRUE;
			mocaCfg.EnableTabooBit 					= FALSE;
			mocaCfg.TxPowerLimit 					= 7;
			mocaCfg.BeaconPowerLimit 				= 9;
			mocaCfg.AutoPowerControlPhyRate 		= 235;
            mocaCfg.Reset 							= FALSE;
			//upstreamed 1007_moca_reset_count_arrisxb6-11165.patch as part of RDKB-41505
			/*ARRISXB6-11165: MOCA reset count is not incrementing. Setting the following to TRUE so in MOCA HAL
			 layer reset counter gets incremented during MOCA reset scenario */
			#if defined(INTEL_PUMA7)
	    		mocaCfg.Reset                                           = TRUE;
			#endif
	
         } else {
			/* Translate the data structures */
			mocaCfg.InstanceNumber 					= pCfg->InstanceNumber;
			rc = strcpy_s(mocaCfg.Alias, sizeof(mocaCfg.Alias), pCfg->Alias);
                        if(rc != EOK)
                        {
                             ERR_CHK(rc);
                             return ANSC_STATUS_FAILURE;
                        }
			/*RDKB-8493 Disable moca in bridge mode, not allow to enable back*/
			if( ( mode == 0 ) 
#if defined (_CM_HIGHSPLIT_SUPPORTED_)
                  && ( FALSE == IsHighSplitEnabled ) 
#endif /* * _CM_HIGHSPLIT_SUPPORTED_ */
              )
			{
				mocaCfg.bEnabled 						= pCfg->bEnabled;
			}
			else
			{
				pCfg->bEnabled=FALSE;
				mocaCfg.bEnabled 						= pCfg->bEnabled;
				AnscTraceWarning(("CosaDmlMocaIfSetCfg -- in Bridege mode cannot enable or disable MoCA\n"));
			}
			mocaCfg.bPreferredNC 					= pCfg->bPreferredNC;
			mocaCfg.PrivacyEnabledSetting 			= pCfg->PrivacyEnabledSetting;
			rc = memcpy_s(mocaCfg.FreqCurrentMaskSetting, sizeof(mocaCfg.FreqCurrentMaskSetting), pCfg->FreqCurrentMaskSetting, sizeof(mocaCfg.FreqCurrentMaskSetting));
                        if(rc != EOK)
                        {
                             ERR_CHK(rc);
                             return ANSC_STATUS_FAILURE;
                        }
			rc = memcpy_s(mocaCfg.KeyPassphrase, sizeof(mocaCfg.KeyPassphrase), pCfg->KeyPassphrase, sizeof(mocaCfg.KeyPassphrase));
                        if(rc != EOK)
                        {
                            ERR_CHK(rc);
                            return ANSC_STATUS_FAILURE;
                        }
			mocaCfg.TxPowerLimit 					= pCfg->TxPowerLimit;
			mocaCfg.BeaconPowerLimit 				= pCfg->BeaconPowerLimit;
			mocaCfg.MaxIngressBWThreshold 			= pCfg->MaxIngressBWThreshold;
			mocaCfg.MaxEgressBWThreshold 			= pCfg->MaxEgressBWThreshold;
			mocaCfg.Reset 							= pCfg->X_CISCO_COM_Reset;
			mocaCfg.MixedMode 						= pCfg->X_CISCO_COM_MixedMode;
			mocaCfg.ChannelScanning 				= pCfg->X_CISCO_COM_ChannelScanning;
			mocaCfg.AutoPowerControlEnable 			= pCfg->X_CISCO_COM_AutoPowerControlEnable;
			mocaCfg.EnableTabooBit 					= pCfg->X_CISCO_COM_EnableTabooBit;
			rc = memcpy_s(mocaCfg.NodeTabooMask, sizeof(mocaCfg.NodeTabooMask), pCfg->NodeTabooMask, sizeof(mocaCfg.NodeTabooMask));
                        if(rc != EOK)
                        {
                            ERR_CHK(rc);
                            return ANSC_STATUS_FAILURE;
                        }
			mocaCfg.AutoPowerControlPhyRate 		= pCfg->AutoPowerControlPhyRate;
			rc = memcpy_s(mocaCfg.ChannelScanMask, sizeof(mocaCfg.ChannelScanMask), pCfg->X_CISCO_COM_ChannelScanMask, sizeof(mocaCfg.ChannelScanMask));
                        if(rc != EOK)
                        {
                            ERR_CHK(rc);
                            return ANSC_STATUS_FAILURE;
                        }
		 }

         if ( moca_SetIfConfig(ulInterfaceIndex, &mocaCfg) != STATUS_SUCCESS)
         {
            AnscTraceWarning(("%s: moca_SetIfConfig returns error returning \n", __FUNCTION__));
            return ANSC_STATUS_FAILURE;            
         }

         AnscTraceWarning(("pCfg->bEnabled: %d\n", pCfg->bEnabled));

         if(pCfg->bEnabled == TRUE) {

             if (syscfg_set_commit(NULL, "moca_enabled", "1") != 0) {
                 AnscTraceWarning(("syscfg_set failed\n"));
             }

         } else {

             if (syscfg_set_commit(NULL, "moca_enabled", "0") != 0) {
                 AnscTraceWarning(("syscfg_set failed\n"));
             }
         }

         AnscTraceWarning(("pCfg->X_CISCO_COM_EnableTabooBit: %d\n", pCfg->X_CISCO_COM_EnableTabooBit));

         if(pCfg->X_CISCO_COM_EnableTabooBit == TRUE) {

             if (syscfg_set(NULL, "moca_node_taboo_enabled", "1") != 0) {
                     AnscTraceWarning(("syscfg_set failed\n"));
             } else {

             	if (syscfg_set(NULL, "moca_node_taboo_mask", (const char*)pCfg->NodeTabooMask) != 0) {
                     AnscTraceWarning(("syscfg_set failed\n"));
             	} else {

                    if (syscfg_commit() != 0) {
                            AnscTraceWarning(("syscfg_commit failed\n"));
                    }
		}
             }

         } else {

             if (syscfg_set_commit(NULL, "moca_node_taboo_enabled", "0") != 0) {
                 AnscTraceWarning(("syscfg_set failed\n"));
             }
         }

         AnscTraceWarning(("pCfg->X_CISCO_COM_ChannelScanning: %d\n", pCfg->X_CISCO_COM_ChannelScanning));

         if(pCfg->X_CISCO_COM_ChannelScanning == TRUE) {
             if (syscfg_set_commit(NULL, "moca_scan_enabled", "1") != 0) {
                 AnscTraceWarning(("syscfg_set failed\n"));
             }
         } else {
             if (syscfg_set_commit(NULL, "moca_scan_enabled", "0") != 0) {
                 AnscTraceWarning(("syscfg_set failed\n"));
             }
         }

         AnscTraceWarning(("pCfg->bPreferredNC: %d\n", pCfg->bPreferredNC));

         if(pCfg->bPreferredNC == TRUE) {
             if (syscfg_set_commit(NULL, "moca_preferred_nc", "1") != 0) {
                 AnscTraceWarning(("syscfg_set failed\n"));
             }
         } else {
             if (syscfg_set_commit(NULL, "moca_preferred_nc", "0") != 0) {
                 AnscTraceWarning(("syscfg_set failed\n"));
             }
         }

         AnscTraceWarning(("pCfg->X_CISCO_COM_MixedMode: %d\n", pCfg->X_CISCO_COM_MixedMode));

         if(pCfg->X_CISCO_COM_MixedMode == TRUE) {
             if (syscfg_set_commit(NULL, "moca_mixed_mode", "1") != 0) {
                 AnscTraceWarning(("syscfg_set failed\n"));
             }
         } else {
             if (syscfg_set_commit(NULL, "moca_mixed_mode", "0") != 0) {
                 AnscTraceWarning(("syscfg_set failed\n"));
             }
         }

         AnscTraceWarning(("pCfg->PrivacyEnabledSetting: %d\n", pCfg->PrivacyEnabledSetting));

         if(pCfg->PrivacyEnabledSetting == TRUE) {
             if (syscfg_set_commit(NULL, "moca_enable_privacy", "1") != 0) {
                 AnscTraceWarning(("syscfg_set failed\n"));
             }

         } else {
             if (syscfg_set_commit(NULL, "moca_enable_privacy", "0") != 0) {
                 AnscTraceWarning(("syscfg_set failed\n"));
             }
         }

         AnscTraceWarning(("pCfg->KeyPassphrase\n"));

         if(strlen(pCfg->KeyPassphrase) != 0) {
             if (syscfg_set_commit(NULL, "moca_password_seed", pCfg->KeyPassphrase) != 0) {
                 AnscTraceWarning(("syscfg_set failed\n"));
             }
         }     

         AnscTraceWarning(("pCfg->BeaconPowerLimit: %lu\n", pCfg->BeaconPowerLimit));
         status = snprintf(str_value, kMax_beaconPwrLen, "%lu", pCfg->BeaconPowerLimit);

         if(status > 0) {
             if (syscfg_set_commit(NULL, "moca_beacon_pwr_level", str_value) != 0) {
                 AnscTraceWarning(("syscfg_set failed\n"));
             }
         } else {

             AnscTraceWarning(("snprintf failed\n"));
         }

         AnscTraceWarning(("pCfg->TxPowerLimit: %d\n", pCfg->TxPowerLimit));
         status = snprintf(str_value, kMax_TxPowerLimit, "%d", pCfg->TxPowerLimit);

         if(status > 0) {
             if (syscfg_set_commit(NULL, "moca_maximum_tx_power", str_value) != 0) {
                 AnscTraceWarning(("syscfg_set failed\n"));
             }
         } else {

             AnscTraceWarning(("snprintf failed\n"));
         }

         AnscTraceWarning(("pCfg->AutoPowerControlPhyRate: %lu\n", pCfg->AutoPowerControlPhyRate));
         status = snprintf(str_value, kMax_AutoPowerControlPhyRate, "%lu", pCfg->AutoPowerControlPhyRate);
         
         if(status > 0) {
    
             if (syscfg_set_commit(NULL, "moca_phy_rate", str_value) != 0) {
                 AnscTraceWarning(("syscfg_set failed\n"));
             }

         } else {

             AnscTraceWarning(("snprintf failed\n"));
         }


         AnscTraceWarning(("pCfg->X_CISCO_COM_AutoPowerControlEnable: %d\n", pCfg->X_CISCO_COM_AutoPowerControlEnable));

         if(pCfg->X_CISCO_COM_AutoPowerControlEnable == TRUE) {

             if (syscfg_set_commit(NULL, "moca_pwr_apc_enable", "1") != 0) {
                 AnscTraceWarning(("syscfg_set failed\n"));
             }

         } else {

             if (syscfg_set_commit(NULL, "moca_pwr_apc_enable", "0") != 0) {
                 AnscTraceWarning(("syscfg_set failed\n"));
             }
         }

         if(pCfg->X_CISCO_COM_ChannelScanning == FALSE) {

             AnscTraceWarning(("pCfg->FreqCurrentMaskSetting: %s\n", pCfg->FreqCurrentMaskSetting));
    
             //sscanf(pCfg->FreqCurrentMaskSetting, "%016x", &mask);
             //freq = moca_FreqMaskToValue(mask); 
             freq = moca_FreqMaskToValue(pCfg->FreqCurrentMaskSetting);
             status = snprintf(str_value, kMax_FreqCurrentMaskSetting, "%d", freq);
             
             AnscTraceWarning(("freq: %s\n", str_value));
    
             if(status > 0) {

#ifdef SA_CUSTOM
                  errno_t rc = -1;
                  int ind = -1;
                 rc = strcmp_s("1150", strlen("1150"), str_value, &ind);
                 ERR_CHK(rc);
                 if((rc == EOK) && (ind != 0))
                 { 
                     rc = strcmp_s("1175", strlen("1175"), str_value, &ind);
                     ERR_CHK(rc);
                     if(( rc == EOK) && (ind != 0))
                     {
                          rc = strcpy_s(str_value, sizeof(str_value), "1175");
                          if(rc != EOK)
                          {
                              ERR_CHK(rc);
                              return ANSC_STATUS_FAILURE;
                          }
                          write_log_console ("updating the current moca_freq_plan/str_value to 1175");
                     }
                 }    
#endif

                 if (syscfg_set_commit(NULL, "moca_freq_plan", str_value) != 0) {
                     AnscTraceWarning(("syscfg_set failed\n"));
                 }
    
             } else {
    
                 AnscTraceWarning(("snprintf failed\n"));
             }
         }
         //upstreamed ccsp_moca_diag.patch as part of RDKB-41505
	 #if defined (_XB6_PRODUCT_REQ_) && defined (_COSA_BCM_ARM_)
	 CosaDmlMocaIfGetStaticInfo(hContext,ulInterfaceIndex,gpSInfo);
	 #endif
    }
    else
    {
        return ANSC_STATUS_FAILURE;
    }

    return ANSC_STATUS_SUCCESS;
}


ANSC_STATUS
CosaDmlMocaIfGetCfg
    (
        ANSC_HANDLE                 hContext,
        ULONG                       uIndex,
        PCOSA_DML_MOCA_IF_CFG       pCfg
    )
{
    UNREFERENCED_PARAMETER(hContext);
    JUDGE_MOCA_HARDWARE_AVAILABLE(ANSC_STATUS_FAILURE)
	moca_cfg_t mocaCfg;

//    unsigned int moca_enable_db=0;
    char buf[10]={0};
    errno_t rc = -1;
    if ( !pCfg )
    {
        return ANSC_STATUS_FAILURE;
    }

    if( (syscfg_get( NULL, "moca_enabled", buf, sizeof(buf)) == 0) && (strlen(buf) != 0) )
   {
	moca_enable_db=atoi(buf);
	
   }
    if ( uIndex == 0 )
    {
        rc = memset_s(&mocaCfg, sizeof(moca_cfg_t), 0, sizeof(moca_cfg_t));
        ERR_CHK(rc);
        if(STATUS_SUCCESS != moca_GetIfConfig(uIndex, &mocaCfg))
        {
           CcspTraceError(("-- moca_GetIfConfig failure\n"));
           return ANSC_STATUS_FAILURE;
        }
		

        /* XF3-5279 - PCOSA_DML_MOCA_IF_CFG instancenumber starts from 1.
         * but moca_cfg_t instance number starts from 0.
         * cosa instance number already updated in CosaMoCAInitialize()
         * Hence disabled it. 
         */
// 		pCfg->InstanceNumber 						= mocaCfg.InstanceNumber;
		rc = STRCPY_S_NOCLOBBER(pCfg->Alias, sizeof(pCfg->Alias), mocaCfg.Alias);
                if(rc != EOK)
                {
                    ERR_CHK(rc);
                    return ANSC_STATUS_FAILURE;
                }
		pCfg->bEnabled 								= mocaCfg.bEnabled;

		if ( ( 1 != pCfg->bSnmpUpdate ) && ( moca_enable_db != pCfg->bEnabled ) )
		{
			AnscTraceWarning(("syscfg db and moca driver value are not in sync, setting db value to driver\n"));
			mocaCfg.bEnabled=moca_enable_db;
	              if ( moca_SetIfConfig(uIndex, &mocaCfg) != STATUS_SUCCESS)
                        {
                         AnscTraceWarning(("moca_SetIfConfig returns error\n"));
                         return ANSC_STATUS_FAILURE;
                        }
			pCfg->bEnabled 								= mocaCfg.bEnabled;
		}
		else if ( ( 1 == pCfg->bSnmpUpdate ) && ( moca_enable_db != pCfg->bEnabled ) )
 		{
			AnscTraceWarning(("SNMP set for MoCA is received, setting new MoCA enable status to db\n"));
			 if(pCfg->bEnabled == TRUE) {

			     if (syscfg_set_commit(NULL, "moca_enabled", "1") != 0) {
				     AnscTraceWarning(("syscfg_set failed\n"));
			     }

			 } else {

			     if (syscfg_set_commit(NULL, "moca_enabled", "0") != 0) {
				     AnscTraceWarning(("syscfg_set failed\n"));
			     }
			 }
			 pCfg->bSnmpUpdate = 0;

		}

		pCfg->bPreferredNC 							= mocaCfg.bPreferredNC;
		pCfg->PrivacyEnabledSetting 				= mocaCfg.PrivacyEnabledSetting;
		rc = MEMCPY_S_NOCLOBBER(pCfg->FreqCurrentMaskSetting, sizeof(pCfg->FreqCurrentMaskSetting), mocaCfg.FreqCurrentMaskSetting, sizeof(mocaCfg.FreqCurrentMaskSetting));
                if(rc != EOK)
                {
                    ERR_CHK(rc);
                    return ANSC_STATUS_FAILURE;
                }
		rc = MEMCPY_S_NOCLOBBER(pCfg->KeyPassphrase, sizeof(pCfg->KeyPassphrase), mocaCfg.KeyPassphrase, sizeof(mocaCfg.KeyPassphrase));
                if(rc != EOK)
                {
                    ERR_CHK(rc);
                    return ANSC_STATUS_FAILURE;
                }
		pCfg->TxPowerLimit 							= mocaCfg.TxPowerLimit;
		pCfg->PowerCntlPhyTarget 					= mocaCfg.AutoPowerControlPhyRate;
		pCfg->BeaconPowerLimit 						= mocaCfg.BeaconPowerLimit;
		pCfg->MaxIngressBWThreshold 				= mocaCfg.MaxIngressBWThreshold;
		pCfg->MaxEgressBWThreshold 					= mocaCfg.MaxEgressBWThreshold;
		pCfg->X_CISCO_COM_Reset 					= mocaCfg.Reset;
		pCfg->X_CISCO_COM_MixedMode 				= mocaCfg.MixedMode;
		pCfg->X_CISCO_COM_ChannelScanning 			= mocaCfg.ChannelScanning;
		pCfg->X_CISCO_COM_AutoPowerControlEnable 	= mocaCfg.AutoPowerControlEnable;
		pCfg->X_CISCO_COM_EnableTabooBit 			= mocaCfg.EnableTabooBit;
		rc = MEMCPY_S_NOCLOBBER(pCfg->NodeTabooMask, sizeof(pCfg->NodeTabooMask), mocaCfg.NodeTabooMask, sizeof(mocaCfg.NodeTabooMask));
                if(rc != EOK)
                {
                    ERR_CHK(rc);
                    return ANSC_STATUS_FAILURE;
                }
		pCfg->AutoPowerControlPhyRate 				= mocaCfg.AutoPowerControlPhyRate;
		rc = MEMCPY_S_NOCLOBBER(pCfg->X_CISCO_COM_ChannelScanMask, sizeof(pCfg->X_CISCO_COM_ChannelScanMask), mocaCfg.ChannelScanMask, sizeof(mocaCfg.ChannelScanMask));
                if(rc != EOK)
                {
                    ERR_CHK(rc);
                    return ANSC_STATUS_FAILURE;
                }
    }
    else
    {
        return ANSC_STATUS_FAILURE;
    }
    
    //upstreamed ccsp_moca_taboo_2870.patch as part of RDKB-41505
    #if defined (_XB6_PRODUCT_REQ_) && defined (_COSA_BCM_ARM_)
    CosaDmlMocaIfGetStaticInfo(hContext,uIndex,gpSInfo);
    #endif
    return ANSC_STATUS_SUCCESS;
    
}


ANSC_STATUS
CosaDmlMocaIfGetDinfo
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulInterfaceIndex,
        PCOSA_DML_MOCA_IF_DINFO     pInfo
    )
{
    UNREFERENCED_PARAMETER(hContext);
    JUDGE_MOCA_HARDWARE_AVAILABLE(ANSC_STATUS_FAILURE)
	errno_t rc = -1;
        moca_dynamic_info_t mocaDInfo;

	rc = memset_s(&mocaDInfo, sizeof(moca_dynamic_info_t), 0, sizeof(moca_dynamic_info_t));
        ERR_CHK(rc);

    if (!pInfo)
    {
        return ANSC_STATUS_FAILURE;
    }

    rc = memset_s(pInfo, sizeof(COSA_DML_MOCA_IF_DINFO), 0, sizeof(COSA_DML_MOCA_IF_DINFO));
    ERR_CHK(rc);
        
    if (ulInterfaceIndex == 0)
    {
        moca_IfGetDynamicInfo(ulInterfaceIndex, &mocaDInfo);

		/* Translate the Data Structures */
		pInfo->Status 											= mocaDInfo.Status;
		pInfo->LastChange 										= mocaDInfo.LastChange;
		pInfo->MaxIngressBW 									= mocaDInfo.MaxIngressBW;
		pInfo->MaxEgressBW 										= mocaDInfo.MaxEgressBW;
		rc = memcpy_s(pInfo->CurrentVersion, sizeof(pInfo->CurrentVersion), mocaDInfo.CurrentVersion,  sizeof(pInfo->CurrentVersion));
                if (rc != EOK)
                {
                     ERR_CHK(rc);
                     return ANSC_STATUS_FAILURE;
                }
		pInfo->NetworkCoordinator 								= mocaDInfo.NetworkCoordinator;
		pInfo->NodeID 											= mocaDInfo.NodeID;
		pInfo->MaxNodes 										= 16;
		pInfo->BackupNC 										= mocaDInfo.BackupNC;
		pInfo->PrivacyEnabled 									= mocaDInfo.PrivacyEnabled;
		rc = memcpy_s(pInfo->FreqCurrentMask, sizeof(pInfo->FreqCurrentMask), mocaDInfo.FreqCurrentMask, sizeof(pInfo->FreqCurrentMask));
                if (rc != EOK)
                {
                     ERR_CHK(rc);
                     return ANSC_STATUS_FAILURE;
                }
		pInfo->CurrentOperFreq 									= mocaDInfo.CurrentOperFreq;
		pInfo->LastOperFreq 									= mocaDInfo.LastOperFreq;
		pInfo->TxBcastRate 										= mocaDInfo.TxBcastRate;
		pInfo->MaxIngressBWThresholdReached 					= mocaDInfo.MaxIngressBWThresholdReached;
		pInfo->MaxEgressBWThresholdReached 						= mocaDInfo.MaxEgressBWThresholdReached;
		pInfo->X_CISCO_COM_NumberOfConnectedClients 			= mocaDInfo.NumberOfConnectedClients;
#ifdef MOCA_DIAGONISTIC
		pInfo->LinkUpTime 								= mocaDInfo.LinkUpTime;
#endif
		rc = memcpy_s(pInfo->X_CISCO_NetworkCoordinatorMACAddress, sizeof(pInfo->X_CISCO_NetworkCoordinatorMACAddress), mocaDInfo.NetworkCoordinatorMACAddress, sizeof(pInfo->X_CISCO_NetworkCoordinatorMACAddress));
                if (rc != EOK)
                {
                     ERR_CHK(rc);
                     return ANSC_STATUS_FAILURE;
                }
		pInfo->X_CISCO_COM_BestNetworkCoordinatorID 			= mocaDInfo.NetworkCoordinator;
    }
    else
    {
        return ANSC_STATUS_FAILURE;
    }
        
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlMocaIfGetStaticInfo
    (
        ANSC_HANDLE                 hContext,
        ULONG                       uIndex,
        PCOSA_DML_MOCA_IF_SINFO       pSInfo
    )
{
    UNREFERENCED_PARAMETER(hContext);
    JUDGE_MOCA_HARDWARE_AVAILABLE(ANSC_STATUS_FAILURE)
        errno_t rc = -1;
	moca_static_info_t mocaStaticCfg;

    AnscTraceWarning(("CosaDmlMocaIfGetStaticInfo -- ulInterfaceIndex:%lu.\n", uIndex));
    
    if ( !pSInfo )
    {
        return ANSC_STATUS_FAILURE;
    }

    //upstreamed ccsp_moca_diag.patch as part of RDKB-41505
    #if defined (_XB6_PRODUCT_REQ_) && defined (_COSA_BCM_ARM_)
    gpSInfo = pSInfo;
    #endif

    if ( uIndex == 0 )
    {
	rc = memset_s(&mocaStaticCfg, sizeof(moca_static_info_t), 0, sizeof(moca_static_info_t));
        ERR_CHK(rc);
        moca_IfGetStaticInfo(uIndex, &mocaStaticCfg);
		
		/* Translate the data structures */
#ifdef MULTILAN_FEATURE
#if defined(INTEL_PUMA7)
                rc = STRCPY_S_NOCLOBBER(pSInfo->Name, sizeof(pSInfo->Name), "nmoca0");
                if (rc != EOK)
                {
                    ERR_CHK(rc);
                    return ANSC_STATUS_FAILURE;
                }
#else
                rc = STRCPY_S_NOCLOBBER(pSInfo->Name, sizeof(pSInfo->Name), "sw_5");
                if (rc != EOK)
                {
                    ERR_CHK(rc);
                    return ANSC_STATUS_FAILURE;
                }
#endif
#else
                rc = STRCPY_S_NOCLOBBER(pSInfo->Name, sizeof(pSInfo->Name), "sw_5");
                if (rc != EOK)
                {
                    ERR_CHK(rc);
                    return ANSC_STATUS_FAILURE;
                }
#endif
		rc = MEMCPY_S_NOCLOBBER(pSInfo->MacAddress, sizeof(pSInfo->MacAddress), mocaStaticCfg.MacAddress, sizeof(mocaStaticCfg.MacAddress));
		if (rc != EOK)
                {
                    ERR_CHK(rc);
                    return ANSC_STATUS_FAILURE;
                }
                rc = MEMCPY_S_NOCLOBBER(pSInfo->FirmwareVersion, sizeof(pSInfo->FirmwareVersion), mocaStaticCfg.FirmwareVersion, sizeof(mocaStaticCfg.FirmwareVersion));
                if (rc != EOK)
                {
                    ERR_CHK(rc);
                    return ANSC_STATUS_FAILURE;
                }
		pSInfo->MaxBitRate  = mocaStaticCfg.MaxBitRate;
		rc = MEMCPY_S_NOCLOBBER(pSInfo->HighestVersion, sizeof(pSInfo->HighestVersion), mocaStaticCfg.HighestVersion, sizeof(mocaStaticCfg.HighestVersion));
                if (rc != EOK)
                {
                    ERR_CHK(rc);
                    return ANSC_STATUS_FAILURE;
                }
		rc = MEMCPY_S_NOCLOBBER(pSInfo->FreqCapabilityMask, sizeof(pSInfo->FreqCapabilityMask), mocaStaticCfg.FreqCapabilityMask, sizeof(mocaStaticCfg.FreqCapabilityMask));
                if (rc != EOK)
                {
                    ERR_CHK(rc);
                    return ANSC_STATUS_FAILURE;
                }
		rc = MEMCPY_S_NOCLOBBER(pSInfo->NetworkTabooMask, sizeof(pSInfo->NetworkTabooMask), mocaStaticCfg.NetworkTabooMask, sizeof(mocaStaticCfg.NetworkTabooMask));
                if (rc != EOK)
                {
                    ERR_CHK(rc);
                    return ANSC_STATUS_FAILURE;
                }
		pSInfo->TxBcastPowerReduction = mocaStaticCfg.TxBcastPowerReduction;
		pSInfo->QAM256Capable = mocaStaticCfg.QAM256Capable;
		pSInfo->PacketAggregationCapability = mocaStaticCfg.PacketAggregationCapability;
		
    }
    else
    {
        return ANSC_STATUS_FAILURE;
    }

    return ANSC_STATUS_SUCCESS;
    
}


ANSC_STATUS
CosaDmlMocaIfGetStats
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulInterfaceIndex,
        PCOSA_DML_MOCA_STATS        pStats
    )
{
    UNREFERENCED_PARAMETER(hContext);
    JUDGE_MOCA_HARDWARE_AVAILABLE(ANSC_STATUS_FAILURE)
        errno_t rc = -1;
	moca_stats_t mocaStats;

	rc = memset_s(&mocaStats, sizeof(moca_stats_t), 0, sizeof(moca_stats_t));
        ERR_CHK(rc);

    AnscTraceWarning(("CosaDmlMocaIfGetStats -- ulInterfaceIndex:%lu.\n", ulInterfaceIndex));

    if ( !pStats )
    {
        return ANSC_STATUS_FAILURE;
    }

    rc = memset_s(pStats, sizeof(COSA_DML_MOCA_STATS), 0, sizeof(COSA_DML_MOCA_STATS));
    ERR_CHK(rc);

    if ( ulInterfaceIndex == 1 )
    { 
        moca_IfGetStats(ulInterfaceIndex, &mocaStats);

		/* Translate the Data Structures */
		pStats->BytesSent 						= mocaStats.BytesSent;
		pStats->BytesReceived 					= mocaStats.BytesReceived;
		pStats->PacketsSent						= mocaStats.PacketsSent;
		pStats->PacketsReceived 				= mocaStats.PacketsReceived;
		pStats->ErrorsSent						= mocaStats.ErrorsSent;
		pStats->ErrorsReceived					= mocaStats.ErrorsReceived;
		pStats->UnicastPacketsSent				= mocaStats.UnicastPacketsSent;
		pStats->UnicastPacketsReceived  		= mocaStats.UnicastPacketsReceived;
		pStats->DiscardPacketsSent				= mocaStats.DiscardPacketsSent;
		pStats->DiscardPacketsReceived  		= mocaStats.DiscardPacketsReceived;
		pStats->MulticastPacketsSent			= mocaStats.MulticastPacketsSent;
		pStats->MulticastPacketsReceived 		= mocaStats.MulticastPacketsReceived;
		pStats->BroadcastPacketsSent			= mocaStats.BroadcastPacketsSent;
		pStats->BroadcastPacketsReceived    	= mocaStats.BroadcastPacketsReceived;
		pStats->UnknownProtoPacketsReceived		= mocaStats.UnknownProtoPacketsReceived;
		pStats->X_CISCO_COM_ExtAggrAverageTx 	= mocaStats.ExtAggrAverageTx;
		pStats->X_CISCO_COM_ExtAggrAverageRx 	= mocaStats.ExtAggrAverageRx;
    }
    else if ( ulInterfaceIndex == 0 )
    {
               moca_IfGetStats(ulInterfaceIndex, &mocaStats);

                /* Translate the Data Structures */
                pStats->BytesSent                                               = mocaStats.BytesSent;
                pStats->BytesReceived                                   = mocaStats.BytesReceived;
                pStats->PacketsSent                                             = mocaStats.PacketsSent;
                pStats->PacketsReceived                                 = mocaStats.PacketsReceived;
                pStats->ErrorsSent                                              = mocaStats.ErrorsSent;
                pStats->ErrorsReceived                                  = mocaStats.ErrorsReceived;
                pStats->UnicastPacketsSent                              = mocaStats.UnicastPacketsSent;
                pStats->UnicastPacketsReceived                  = mocaStats.UnicastPacketsReceived;
                pStats->DiscardPacketsSent                              = mocaStats.DiscardPacketsSent;
                pStats->DiscardPacketsReceived                  = mocaStats.DiscardPacketsReceived;
                pStats->MulticastPacketsSent                    = mocaStats.MulticastPacketsSent;
                pStats->MulticastPacketsReceived                = mocaStats.MulticastPacketsReceived;
                pStats->BroadcastPacketsSent                    = mocaStats.BroadcastPacketsSent;
                pStats->BroadcastPacketsReceived        = mocaStats.BroadcastPacketsReceived;
                pStats->UnknownProtoPacketsReceived             = mocaStats.UnknownProtoPacketsReceived;
                pStats->X_CISCO_COM_ExtAggrAverageTx    = mocaStats.ExtAggrAverageTx;
                pStats->X_CISCO_COM_ExtAggrAverageRx    = mocaStats.ExtAggrAverageRx;
    }
    else
    {
        return ANSC_STATUS_FAILURE;
    }

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlMocaIfResetStats
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulInterfaceIndex
    )
{
    UNREFERENCED_PARAMETER(hContext);
    JUDGE_MOCA_HARDWARE_AVAILABLE(ANSC_STATUS_FAILURE)

    AnscTraceWarning(("CosaDmlMocaIfResetStats -- ulInterfaceIndex:%lu\n", ulInterfaceIndex));


    return ANSC_STATUS_SUCCESS;
}


ULONG
CosaDmlMocaIfExtCounterGetNumber
    (
        ANSC_HANDLE                      hContext,
        ULONG                            ulInterfaceIndex
    )
{
    UNREFERENCED_PARAMETER(hContext);
    ULONG ulCount = 0;

    JUDGE_MOCA_HARDWARE_AVAILABLE(0)

    AnscTraceWarning(("CosaDmlMocaIfExtCounterGetNumber -- ulInterfaceIndex:%lu.\n", ulInterfaceIndex));

    if ( ulInterfaceIndex == 0 )
    {
		moca_GetNumAssociatedDevices(ulInterfaceIndex, &ulCount);
        return ulCount;
    }
    else
    {
        AnscTraceWarning(("CosaDmlMocaIfExtCounterGetNumber -- Error, not such Entry\n"));
        return 0;
    }

}

/*
    ANSC_HANDLE   :  Hardcoded to NULL currently.
    ulInterfaceIndex : it's the index of Interface.
    ulIndex              : it's the index of this ExtCounter.
    PCOSA_CONTEXT_MOCA_LINK_OBJECT

    return value: if successful, return ANSC_STATUS_SUCCESS.
                        if fail,           return ANSC_STATUS_FAILURE
*/
ANSC_STATUS
CosaDmlMocaIfExtCounterGetEntry
    (
        ANSC_HANDLE                      hContext,
        ULONG                            ulInterfaceIndex,
        ULONG                            ulIndex,
        PCOSA_DML_MOCA_EXTCOUNTER        pConf
    )
{
    UNREFERENCED_PARAMETER(hContext);
    ULONG ulCount = 0;
    errno_t rc = -1;
	moca_mac_counters_t mocaMacStats;

    JUDGE_MOCA_HARDWARE_AVAILABLE(ANSC_STATUS_FAILURE)

    rc = memset_s(&mocaMacStats, sizeof(moca_mac_counters_t), 0, sizeof(moca_mac_counters_t));
    ERR_CHK(rc);

    AnscTraceWarning(("CosaDmlMocaIfExtCounterGetEntry -- ulInterfaceIndex:%lu, ulIndex:%lu\n", ulInterfaceIndex, ulIndex));

    if ( ulInterfaceIndex == 0 )
    {
		moca_GetNumAssociatedDevices(ulInterfaceIndex, &ulCount);

        if (ulIndex >=ulCount )
        {
            AnscTraceWarning(("CosaDmlMocaIfExtCounterGetEntry -- Error, not such Entry\n"));
            return ANSC_STATUS_FAILURE;
        }
        
        moca_IfGetExtCounter(ulIndex, &mocaMacStats);

		/* Translate the Data Structures */
		pConf->Map 			= mocaMacStats.Map;
		pConf->Rsrv			= mocaMacStats.Rsrv;
		pConf->Lc			= mocaMacStats.Lc;
		pConf->Adm			= mocaMacStats.Adm;
		pConf->Probe		= mocaMacStats.Probe;
		pConf->Async		= mocaMacStats.Async;

        //*pConf = g_MoCAExtCounter[ulIndex];
    }
    else
    {
        AnscTraceWarning(("CosaDmlMocaIfExtCounterGetEntry -- Error, not such Entry\n"));
        return ANSC_STATUS_FAILURE;
    }

    return ANSC_STATUS_SUCCESS;
}


ULONG
CosaDmlMocaIfExtAggrCounterGetNumber
    (
        ANSC_HANDLE                      hContext,
        ULONG                            ulInterfaceIndex
    )
{
    UNREFERENCED_PARAMETER(hContext);
    JUDGE_MOCA_HARDWARE_AVAILABLE(ANSC_STATUS_FAILURE)

    AnscTraceWarning(("CosaDmlMocaIfExtAggrCounterGetNumber -- ulInterfaceIndex:%lu.\n", ulInterfaceIndex));

    if ( ulInterfaceIndex == 0 ) {

        // Default to 1 since Intel's MoCA API does not support PCOSA_DML_MOCA_EXTAGGRCOUNTER for other
        // nodes in the MoCA network
        return 1;
    }
    else
    {
        AnscTraceWarning(("CosaDmlMocaIfExtAggrCounterGetNumber -- Error, not such Entry\n"));
        return 0;
    }

}

/*
    ANSC_HANDLE   :  Hardcoded to NULL currently.
    ulInterfaceIndex : it's the index of Interface.
    ulIndex              : it's the index of this ExtCounter.
    PCOSA_CONTEXT_MOCA_LINK_OBJECT

    return value: if successful, return ANSC_STATUS_SUCCESS.
                        if fail,           return ANSC_STATUS_FAILURE
*/
ANSC_STATUS
CosaDmlMocaIfExtAggrCounterGetEntry
    (
        ANSC_HANDLE                      hContext,
        ULONG                            ulInterfaceIndex,
        ULONG                            ulIndex,
        PCOSA_DML_MOCA_EXTAGGRCOUNTER    pConf
    )
{
    UNREFERENCED_PARAMETER(hContext);
    moca_aggregate_counters_t mocaCounters;
    errno_t rc = -1;

    JUDGE_MOCA_HARDWARE_AVAILABLE(ANSC_STATUS_FAILURE)

    rc = memset_s(&mocaCounters, sizeof(moca_aggregate_counters_t), 0, sizeof(moca_aggregate_counters_t));
    ERR_CHK(rc);

    AnscTraceWarning(("CosaDmlMocaIfExtAggrCounterGetEntry -- ulInterfaceIndex:%lu, ulIndex:%lu\n", ulInterfaceIndex, ulIndex));

    if ( ulInterfaceIndex == 0 )
    {
        if (ulIndex >=1 )
        {
            AnscTraceWarning(("CosaDmlMocaIfExtAggrCounterGetEntry -- Error, not such Entry\n"));
            return ANSC_STATUS_FAILURE;
        }

        moca_IfGetExtAggrCounter(ulIndex, &mocaCounters);

		pConf->Tx		= mocaCounters.Tx;
		pConf->Rx		= mocaCounters.Rx;

        //*pConf = g_MoCAExtAggrCounter[0];
    }
    else
    {
        AnscTraceWarning(("CosaDmlMocaIfExtAggrCounterGetEntry -- Error, not such Entry\n"));
        return ANSC_STATUS_FAILURE;
    }
    
    return ANSC_STATUS_SUCCESS;
}

/*
    This function is used to get total Peer tables.
    The returned memory should be allocated by AnscAllocateMemory. Or else there is leaking.
*/
ANSC_STATUS
CosaDmlMocaIfPeerTableGetTable
    (
        ANSC_HANDLE                      hContext,
        ULONG                            ulInterfaceIndex,
        PCOSA_DML_MOCA_PEER             *ppConf,
        PULONG                           pCount
    )
{
    UNREFERENCED_PARAMETER(hContext);
    JUDGE_MOCA_HARDWARE_AVAILABLE(ANSC_STATUS_FAILURE)

    AnscTraceWarning(("CosaDmlMocaIfPeerTableGetTable -- ulInterfaceIndex:%lu, ppConf:%lx\n", ulInterfaceIndex, (long unsigned int)ppConf));

    if ( !ppConf || !pCount )
    {
        return ANSC_STATUS_FAILURE;
    }

    *ppConf = (PCOSA_DML_MOCA_PEER)AnscAllocateMemory(sizeof(g_MoCAPeer));
    if ( !(*ppConf) )
    {
        *ppConf = NULL;
        *pCount = 0;
        return ANSC_STATUS_FAILURE;
    }
    
    AnscCopyMemory( *ppConf, &g_MoCAPeer, sizeof(g_MoCAPeer) );
    *pCount = sizeof(g_MoCAPeer)/sizeof(COSA_DML_MOCA_PEER);

    return ANSC_STATUS_SUCCESS;
}

/*
    This function is used to get total Mesh tables.
    The returned memory should be allocated by AnscAllocateMemory. Or else there is leaking.
*/
#if 0
ANSC_STATUS
CosaDmlMocaIfMeshTableGetTable
    (
        ANSC_HANDLE                      hContext,
        ULONG                            ulInterfaceIndex,
        PCOSA_DML_MOCA_MESH             *ppConf,
        PULONG                           pCount
    )
{
    JUDGE_MOCA_HARDWARE_AVAILABLE(ANSC_STATUS_FAILURE)

    AnscTraceWarning(("CosaDmlMocaIfFlowTableGetTable -- ulInterfaceIndex:%lu, ppConf:%x\n", ulInterfaceIndex, (UINT)ppConf));

    if ( !pCount || !ppConf || ulInterfaceIndex != 0)
    {
        return ANSC_STATUS_FAILURE;
    }

    *pCount = ( kMoca_MaxMocaNodes * ( kMoca_MaxMocaNodes - 1 ) );

    *ppConf = (PCOSA_DML_MOCA_MESH)AnscAllocateMemory(sizeof(COSA_DML_MOCA_MESH) * ( kMoca_MaxMocaNodes * ( kMoca_MaxMocaNodes - 1 ) ) );
    if ( !*ppConf )
    {
        *pCount = 0;
        return ANSC_STATUS_FAILURE;
    }
    
    if (moca_GetFullMeshRates(ulInterfaceIndex, *ppConf, pCount) != STATUS_SUCCESS)
    {
        AnscFreeMemory(*ppConf);
        *ppConf = NULL;
        *pCount = 0;
        AnscTraceError(("%s: fail to get MoCA associated device\n", __FUNCTION__));
        return ANSC_STATUS_FAILURE;
    }

    return ANSC_STATUS_SUCCESS;
}
#endif

ANSC_STATUS
CosaDmlMocaIfMeshTableGetTable
    (
        ANSC_HANDLE                      hContext,
        ULONG                            ulInterfaceIndex,
        PCOSA_DML_MOCA_MESH             *ppMeshTable,
        PULONG                           pCount
    )
{
    UNREFERENCED_PARAMETER(hContext);
    JUDGE_MOCA_HARDWARE_AVAILABLE(ANSC_STATUS_FAILURE)

    AnscTraceWarning(("CosaDmlMocaIfMeshTableGetTable -- ulInterfaceIndex:%lu, ppMeshTable:%lx\n", ulInterfaceIndex, (long unsigned int)ppMeshTable));

    if ( !pCount || !ppMeshTable || ulInterfaceIndex != 0)
    {
        return ANSC_STATUS_FAILURE;
    }

    moca_mesh_table_t *pmesh_table = (moca_mesh_table_t *)AnscAllocateMemory
				(sizeof(moca_mesh_table_t) * (kMoca_MaxMocaNodes * ( kMoca_MaxMocaNodes - 1 )));

    *pCount = kMoca_MaxMocaNodes * ( kMoca_MaxMocaNodes - 1 );

    if ( !pmesh_table )
    {
        *pCount = 0;
        return ANSC_STATUS_FAILURE;
    }

    if (moca_GetFullMeshRates(ulInterfaceIndex, pmesh_table, pCount) != STATUS_SUCCESS)
    {
        AnscFreeMemory(pmesh_table);
        *ppMeshTable = NULL;
        *pCount = 0;
        AnscTraceError(("%s: fail to get MoCA Mesh Table\n", __FUNCTION__));
        return ANSC_STATUS_FAILURE;
    }

    if ( *pCount )
    {
	unsigned int i;
	PCOSA_DML_MOCA_MESH pMeshTable;

	*ppMeshTable = (PCOSA_DML_MOCA_MESH)AnscAllocateMemory(sizeof(COSA_DML_MOCA_MESH) * (*pCount) );

	if ( !*ppMeshTable )
	{
	    AnscFreeMemory(pmesh_table);
	    *pCount = 0;
	    return ANSC_STATUS_FAILURE;
	}

	pMeshTable = *ppMeshTable;

	for (i = 0; i < *pCount; i++)
	{
	    pMeshTable->TxNodeID = pmesh_table[i].TxNodeID;
	    pMeshTable->RxNodeID = pmesh_table[i].RxNodeID;
	    pMeshTable->TxRate   = pmesh_table[i].TxRate;
	    pMeshTable++;
	}
    }

    AnscFreeMemory(pmesh_table);

    return ANSC_STATUS_SUCCESS;
}

/*
    This function is used to get total Flow tables.
    The returned memory should be allocated by AnscAllocateMemory. Or else there is leaking.
*/

ANSC_STATUS
CosaDmlMocaIfFlowTableGetTable
    (
        ANSC_HANDLE                      hContext,
        ULONG                            ulInterfaceIndex,
        PCOSA_DML_MOCA_FLOW             *ppConf,
        PULONG                           pCount
    )
{
    UNREFERENCED_PARAMETER(hContext);
    JUDGE_MOCA_HARDWARE_AVAILABLE(ANSC_STATUS_FAILURE)

    AnscTraceWarning(("CosaDmlMocaIfFlowTableGetTable -- ulInterfaceIndex:%lu, ppConf:%lx\n", ulInterfaceIndex, (long unsigned int)ppConf));

    if ( !pCount || !ppConf || ulInterfaceIndex != 0)
    {
        return ANSC_STATUS_FAILURE;
    }

    *pCount = kMoca_MaxMocaNodes;
    *ppConf = (PCOSA_DML_MOCA_FLOW)AnscAllocateMemory(sizeof(COSA_DML_MOCA_FLOW) * kMoca_MaxMocaNodes);

    if ( !*ppConf )
    {
        *pCount = 0;
        return ANSC_STATUS_FAILURE;
    }
    
    if (moca_GetFlowStatistics(ulInterfaceIndex, (moca_flow_table_t*)*ppConf, pCount) != STATUS_SUCCESS)
    {
        AnscFreeMemory(*ppConf);
        *ppConf = NULL;
        *pCount = 0;
        AnscTraceError(("%s: fail to get MoCA flow table\n", __FUNCTION__));
        return ANSC_STATUS_FAILURE;
    }
    
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlMocaIfGetQos
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulInterfaceIndex,
        PCOSA_DML_MOCA_QOS          pConf
    )
{
    UNREFERENCED_PARAMETER(hContext);
    JUDGE_MOCA_HARDWARE_AVAILABLE(ANSC_STATUS_FAILURE)

    pConf->Enabled = FALSE;
    
    AnscTraceWarning(("CosaDmlMocaIfGetQos -- ulInterfaceIndex:%lu, Enabled:%s\n", ulInterfaceIndex, (pConf->Enabled==TRUE)?"TRUE":"FALSE" ));

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlMocaIfSetQos
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulInterfaceIndex,
        PCOSA_DML_MOCA_QOS          pConf
    )
{
    UNREFERENCED_PARAMETER(hContext);
    JUDGE_MOCA_HARDWARE_AVAILABLE(ANSC_STATUS_FAILURE)

    AnscTraceWarning(("CosaDmlMocaIfSetQos -- ulInterfaceIndex:%lu, Enabled:%s\n", ulInterfaceIndex, (pConf->Enabled==TRUE)?"TRUE":"FALSE" ));



    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlMocaIfGetAssocDevices
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulInterfaceIndex,
        PULONG                      pulCount,
        PCOSA_DML_MOCA_ASSOC_DEVICE *ppDeviceArray,
        PULONG                      *ppMeshRateArray    /* Not used now */
    )
{
    UNREFERENCED_PARAMETER(hContext);
    UNREFERENCED_PARAMETER(ppMeshRateArray);
    JUDGE_MOCA_HARDWARE_AVAILABLE(ANSC_STATUS_FAILURE)

    ULONG ulSize = 0;

    if ( !pulCount || !ppDeviceArray )
    {
        return ANSC_STATUS_FAILURE;
    }

    if ( ulInterfaceIndex == 0 )
    {
	moca_cpe_t cpes[kMoca_MaxCpeList];
        int        pnum_cpes     = 0,
                   iReturnStatus = STATUS_SUCCESS;

       iReturnStatus =  moca_GetMocaCPEs(ulInterfaceIndex, cpes, &pnum_cpes);

       AnscTraceWarning(("pnum_cpes: %u\n", pnum_cpes));

       if( ( iReturnStatus == STATUS_SUCCESS ) && \
           ( 0 < pnum_cpes )
         )
        {
		moca_GetNumAssociatedDevices(ulInterfaceIndex, pulCount);

        AnscTraceWarning(("*pulCount: %lu\n", *pulCount));

        if ( *pulCount )
        {
			moca_associated_device_t*       pdevice_array  = NULL;
			unsigned int                    i;

            ulSize = sizeof(COSA_DML_MOCA_ASSOC_DEVICE) * (*pulCount);
                
            *ppDeviceArray = (PCOSA_DML_MOCA_ASSOC_DEVICE)AnscAllocateMemory(ulSize);

		    pdevice_array = (moca_associated_device_t *)
                AnscAllocateMemory
                    (
                        sizeof(moca_associated_device_t) * (*pulCount + COSA_DML_MOCA_AssocDeviceSafeguard)
                    );
			    
            if ( *ppDeviceArray && pdevice_array )
            {
                INT                 iReturnStatus   = STATUS_SUCCESS;
                errno_t             rc              = -1;
                PCOSA_DML_MOCA_ASSOC_DEVICE pDeviceArray = *ppDeviceArray;
		rc = memset_s(pdevice_array , sizeof(moca_associated_device_t) * (*pulCount + COSA_DML_MOCA_AssocDeviceSafeguard), 0 , sizeof(moca_associated_device_t) * (*pulCount + COSA_DML_MOCA_AssocDeviceSafeguard));
                ERR_CHK(rc);

                iReturnStatus = moca_GetAssociatedDevices(ulInterfaceIndex, &pdevice_array);

                if ( iReturnStatus == STATUS_SUCCESS )
                {
        			/* Translate the Data Structures */
    				for (i = 0; i < *pulCount; i++)
    				{
                                        rc = memcpy_s(pDeviceArray->MACAddress, sizeof(pDeviceArray->MACAddress), pdevice_array[i].MACAddress, sizeof(pDeviceArray->MACAddress));
                                        if(rc != EOK)
                                        {
                                              ERR_CHK(rc);
                                              return ANSC_STATUS_FAILURE;
                                        }
    					pDeviceArray->NodeID 							= pdevice_array[i].NodeID;
    					pDeviceArray->PreferredNC 						= pdevice_array[i].PreferredNC;
                                        rc = memcpy_s(pDeviceArray->HighestVersion, sizeof(pDeviceArray->HighestVersion), pdevice_array[i].HighestVersion, sizeof(pDeviceArray->HighestVersion));
                                        if(rc != EOK)
                                        {
                                              ERR_CHK(rc);
                                              return ANSC_STATUS_FAILURE;
                                        }
    					pDeviceArray->PHYTxRate 						= pdevice_array[i].PHYTxRate;
    					pDeviceArray->PHYRxRate 						= pdevice_array[i].PHYRxRate;
    					pDeviceArray->TxPowerControlReduction 			= pdevice_array[i].TxPowerControlReduction;
    					pDeviceArray->RxPowerLevel 						= pdevice_array[i].RxPowerLevel;
    					pDeviceArray->TxBcastRate 						= pdevice_array[i].TxBcastRate;
    					pDeviceArray->RxBcastPowerLevel					= pdevice_array[i].RxBcastPowerLevel;
    					pDeviceArray->TxPackets							= pdevice_array[i].TxPackets;
    					pDeviceArray->RxPackets							= pdevice_array[i].RxPackets;
    					pDeviceArray->RxErroredAndMissedPackets			= pdevice_array[i].RxErroredAndMissedPackets;
    					pDeviceArray->QAM256Capable						= pdevice_array[i].QAM256Capable;
    					pDeviceArray->PacketAggregationCapability 		= pdevice_array[i].PacketAggregationCapability;
    					pDeviceArray->RxSNR								= pdevice_array[i].RxSNR;
    					pDeviceArray->Active							= pdevice_array[i].Active;
    					pDeviceArray->X_CISCO_COM_RxBcastRate			= pdevice_array[i].RxBcastRate;
    					pDeviceArray->X_CISCO_COM_NumberOfClients		= pdevice_array[i].NumberOfClients;
                        ++pDeviceArray;  
    				}
                }

                AnscFreeMemory(pdevice_array);

                return  ANSC_STATUS_SUCCESS;
            }
			else
			{
    			AnscTraceWarning(("CosaDmlMocaIfGetAssocDevices -- Memory Allocation Failure "
									"ulInterfaceIndex:%lu, pulCount:%lu\n", ulInterfaceIndex, *pulCount));

                if ( pdevice_array )
                {
                    AnscFreeMemory(pdevice_array);
                }
                if ( *ppDeviceArray )
                {
                    AnscFreeMemory(*ppDeviceArray);
                    *ppDeviceArray = NULL;
                }

			return  ANSC_STATUS_RESOURCES;
		    }
		}
        }
    }

    AnscTraceWarning(("CosaDmlMocaIfGetAssocDevices -- ulInterfaceIndex:%lu, pulCount:%lu\n", ulInterfaceIndex, *pulCount));

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlMocaGetResetCount
    (
        ANSC_HANDLE                 hContext,
        ULONG                       *pValue
    )
{
    UNREFERENCED_PARAMETER(hContext);
    moca_GetResetCount(pValue);	
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlMocaGetLogStatus
	(
		PCOSA_DML_MOCA_LOG_STATUS  pMyObject
	)
{
	char buf[16]={0};
        errno_t rc = -1;
        int ind = -1;
	
	pMyObject->Log_Enable = FALSE;
	pMyObject->Log_Period = 3600;
	
	if(syscfg_get( NULL, "moca_log_enabled", buf, sizeof(buf)) == 0)
	{
		if( strlen(buf) != 0)
		{
                    rc = strcmp_s(buf, sizeof(buf), "true", &ind);
                    ERR_CHK(rc);
                    if(rc == EOK)
                    {
		        pMyObject->Log_Enable =  (ind ? FALSE : TRUE);
                    }
		}
	}
	rc = memset_s(buf, sizeof(buf), 0,sizeof(buf));
        ERR_CHK(rc);
	
	if(syscfg_get( NULL, "moca_log_period", buf, sizeof(buf)) == 0)
	{
		if( strlen(buf) != 0)
		{
		    pMyObject->Log_Period =  atoi(buf);
		}
	}
       /*Coverity Fix CID :60622 MISSING_RETURN */
       return ANSC_STATUS_SUCCESS;

}

ANSC_STATUS is_usg_in_bridge_mode(BOOL *pBridgeMode)
{
    ULONG ulEntryNameLen;
    char ucEntryNameValue[256] = {0};
    parameterValStruct_t varStruct;
    errno_t rc = -1;
    int ind = -1;

    varStruct.parameterName = "Device.X_CISCO_COM_DeviceControl.LanManagementEntry.1.LanMode";
    varStruct.parameterValue = ucEntryNameValue;

    ulEntryNameLen = sizeof(ucEntryNameValue);
    if (ANSC_STATUS_SUCCESS == COSAGetParamValueByPathName(bus_handle,&varStruct,&ulEntryNameLen))
    {
         rc = strcmp_s("bridge-static", strlen("bridge-static"), varStruct.parameterValue, &ind);
         ERR_CHK(rc);
         if((rc == EOK) && (!ind))
         {
             *pBridgeMode = TRUE;
         }
         else
         {
            rc = strcmp_s("full-bridge-static", strlen("full-bridge-static"), varStruct.parameterValue, &ind);
            if((rc == EOK) && (!ind))
            {
                 *pBridgeMode = TRUE;
            }
            else
            {
                 *pBridgeMode = FALSE;
            }
         }

        return ANSC_STATUS_SUCCESS;
    }
    else{
        return ANSC_STATUS_FAILURE;
    }

}

ANSC_STATUS CosaMoCAGetForceEnable(PCOSA_DML_MOCA_CFG pCfg)
{
	char buf[8] = {0};
	pCfg->bForceEnabled = FALSE;
        errno_t rc = -1;
        int ind = -1;

	if(syscfg_get( NULL, "X_RDKCENTRAL-COM_ForceEnable", buf, sizeof(buf)) == 0){
		if( strlen(buf) != 0)
		{
                        rc = strcmp_s(buf, sizeof(buf), "true", &ind);
                        ERR_CHK(rc);
			if(rc == EOK)
                        {
			    pCfg->bForceEnabled =  (ind ? FALSE : TRUE);
			    AnscTraceInfo(("X_RDKCENTRAL-COM_ForceEnable is %d\n",pCfg->bForceEnabled));
			    return ANSC_STATUS_SUCCESS;
                        }
		}
	}
	return ANSC_STATUS_FAILURE;  
}

void* MoCA_Interface_Reset(void *arg)
{
	UNREFERENCED_PARAMETER(arg);
        pthread_detach(pthread_self());
	PCOSA_DATAMODEL_MOCA            pMyObject     = (PCOSA_DATAMODEL_MOCA    )g_MoCAObject;
	PCOSA_DML_MOCA_CFG              pCfg          = &pMyObject->MoCACfg;
	PCOSA_DML_MOCA_IF_CFG		pFCfg	      =	&pMyObject->MoCAIfFullTable[0].MoCAIfFull.Cfg;
	PCOSA_DML_MOCA_IF_DINFO		pDCfg	      = &pMyObject->MoCAIfFullTable[0].MoCAIfFull.DynamicInfo;
	ANSC_STATUS         ReturnStatus  = ANSC_STATUS_FAILURE;

                CcspTraceWarning(("MoCA Interface going to Reset\n"));
                ReturnStatus = CosaDmlMocaIfReset((ANSC_HANDLE)NULL, pFCfg->InstanceNumber-1, pCfg, pFCfg, pDCfg);
                if(ReturnStatus == ANSC_STATUS_SUCCESS)
                {
                        CcspTraceWarning(("Enabling MoCA Reset succesful\n"));
                }
                else
                {
                        CcspTraceWarning(("Enabling MoCA Reset Failed\n"));
                }
   /*Coverity Fix CID: 140394 MISSING_RETURN */
  return NULL;
 
}

BOOL MoCA_SetForceEnable(PCOSA_DML_MOCA_IF_CFG pCfg, PCOSA_DML_MOCA_CFG  pFCfg, BOOL bValue)
{
	ANSC_STATUS         ReturnStatus  = ANSC_STATUS_FAILURE;
	char buff[16] = {0};

	if(pFCfg->bForceEnabled == bValue)
	{
		AnscTraceInfo(("MoCA ForceEnable already = %d\n",bValue));
	}
	else
	{
                 /*Coverity  Fix CID:58757 DC.STRING_BUFFER */
		snprintf(buff,sizeof(buff),"%s",( bValue ) ? "true" :"false" );
		if(syscfg_set_commit(NULL, "X_RDKCENTRAL-COM_ForceEnable", buff) != 0)
		{
			AnscTraceWarning(("syscfg_set failed\n"));
			return FALSE;
		}
		else
		{
			pFCfg->bForceEnabled = bValue;
			AnscTraceInfo(("X_RDKCENTRAL-COM_ForceEnable = %d\n",bValue));

			if (bValue)
			{
				if(!(pCfg->bEnabled))
				{
					pCfg->bEnabled = TRUE;
					ReturnStatus = CosaDmlMocaIfSetCfg((ANSC_HANDLE)NULL, pCfg->InstanceNumber-1, pCfg);
					if(ReturnStatus == ANSC_STATUS_SUCCESS)
					{
						AnscTraceInfo(("Enabling MoCA due to Force Enable Flag TRUE\n"));
					}
					else
					{
						AnscTraceWarning(("CosaDmlMocaIfSetCfg Failed\n"));
					}
				}
				else
				{
					AnscTraceInfo(("MoCA already Enabled\n"));
				}
			}
		}
	}
	return TRUE;
}

BOOL CosaDmlmocaUnpack(char* blob)
{
    AnscTraceWarning(("CosaDmlmocaUnpack_FUNCTION\n"))
    mocadoc_t *md = NULL;
	int err;

	if(blob != NULL)	{

		CcspTraceWarning(("---------------start of b64 decode--------------\n"));

		char * decodeMsg =NULL;
		int decodeMsgSize =0;
		int size =0;

		msgpack_zone mempool;
		msgpack_object deserialized;
		msgpack_unpack_return unpack_ret;

		decodeMsgSize = b64_get_decoded_buffer_size(strlen(blob));
		decodeMsgSize = decodeMsgSize +1;
		CcspTraceWarning(("Bob decodeMsgSize is %d\n", decodeMsgSize ));
		decodeMsg = (char *) malloc(sizeof(char) * decodeMsgSize);
		if( NULL == decodeMsg)
		{
			CcspTraceWarning(("decodeMsg allocation failed\n"));
			return FALSE;
		}
		size = b64_decode((uint8_t *) blob, strlen(blob),(uint8_t *) decodeMsg );
		CcspTraceWarning(("base64 decoded data contains %d bytes\n",size));
	
		msgpack_zone_init(&mempool, 2048);
		unpack_ret = msgpack_unpack(decodeMsg, size, NULL, &mempool, &deserialized);
		switch(unpack_ret)
		{
			case MSGPACK_UNPACK_SUCCESS:
			    CcspTraceWarning(("MSGPACK_UNPACK_SUCCESS :%d\n",unpack_ret));
			break;
			case MSGPACK_UNPACK_EXTRA_BYTES:
			    CcspTraceWarning(("MSGPACK_UNPACK_EXTRA_BYTES :%d\n",unpack_ret));
			break;
			case MSGPACK_UNPACK_CONTINUE:
				CcspTraceWarning(("MSGPACK_UNPACK_CONTINUE :%d\n",unpack_ret));
			break;
			case MSGPACK_UNPACK_PARSE_ERROR:
				CcspTraceWarning(("MSGPACK_UNPACK_PARSE_ERROR :%d\n",unpack_ret));
			break;
			case MSGPACK_UNPACK_NOMEM_ERROR:
				CcspTraceWarning(("MSGPACK_UNPACK_NOMEM_ERROR :%d\n",unpack_ret));
			break;
			default:
				CcspTraceWarning(("Message Pack decode failed with error: %d\n", unpack_ret));
		}
		msgpack_zone_destroy(&mempool);

		CcspTraceWarning(("---------------End of b64 decode--------------\n"));

		if(unpack_ret == MSGPACK_UNPACK_SUCCESS)
		{
			md = mocadoc_convert(decodeMsg, size);//used to process the incoming msgobject
			err = errno;
			CcspTraceWarning(( "errno: %s\n", mocadoc_strerror(err) ));
			if( decodeMsg )
			{
				free(decodeMsg);
				decodeMsg = NULL;
			}

			if(md != NULL)
			{
				CcspTraceWarning(("md->subdoc_name is %s\n", md->subdoc_name));
				CcspTraceWarning(("md->version is %lu\n", (long)md->version));
				CcspTraceWarning(("md->transaction_id %lu\n",(long) md->transaction_id));
				CcspTraceWarning(("md->enable %s\n", (1 == md->param->enable)?"true":"false"));

				execData *execDataPf = NULL ;
				execDataPf = (execData*) malloc (sizeof(execData));
				if ( execDataPf != NULL )
				{
					memset(execDataPf, 0, sizeof(execData));
					execDataPf->txid = md->transaction_id; 
					execDataPf->version = md->version; 
					execDataPf->numOfEntries = 0; 

					strncpy(execDataPf->subdoc_name,"moca",sizeof(execDataPf->subdoc_name)-1);

					execDataPf->user_data = (void*) md ;
					execDataPf->calcTimeout = NULL ;
					execDataPf->executeBlobRequest = Process_Moca_WebConfigRequest;
					execDataPf->rollbackFunc = rollback_moca_conf ;
					execDataPf->freeResources = freeResources_moca ;
					PushBlobRequest(execDataPf);
					CcspTraceWarning(("PushBlobRequest complete\n"));
					return TRUE;
				}
				else 
				{
					CcspTraceWarning(("execData memory allocation failed\n"));
					mocadoc_destroy(md);
					return FALSE;
				}

			}
			return TRUE;                    
		}
		else
		{
			if ( decodeMsg )
			{
				free(decodeMsg);
				decodeMsg = NULL;
			}
			CcspTraceWarning(("Corrupted moca enable msgpack value\n"));
			return FALSE;
		}
			return TRUE;	
	}
	return TRUE;
}

#elif ( _COSA_SIM_ )


ANSC_STATUS
CosaDmlMocaInit
    (
        ANSC_HANDLE                 hDml,
        PANSC_HANDLE                phContext
)
{
    PCOSA_DATAMODEL_MOCA  pMyObject    = (PCOSA_DATAMODEL_MOCA)phContext;

    AnscTraceWarning(("CosaDmlMocaInit -- \n"));



    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlMocaGetCfg
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_MOCA_CFG          pCfg
    )
{
    AnscTraceWarning(("CosaDmlMocaIfGetCfg -- .\n"));



    
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlMocaSetCfg
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_MOCA_CFG          pCfg
    )
{
    AnscTraceWarning(("CosaDmlMocaSetCfg -- %s %s %s.\n", 
        pCfg->X_CISCO_COM_ProvisioningFilename, 
        pCfg->X_CISCO_COM_ProvisioningServerAddress, 
        (pCfg->X_CISCO_COM_ProvisioningServerAddressType==1)?"IPv4":"IPv6"));




    return ANSC_STATUS_SUCCESS;
}

ULONG
CosaDmlMocaGetNumberOfIfs
    (
        ANSC_HANDLE                 hContext
    )
{
    AnscTraceWarning(("CosaDmlMocaGetNumberOfIfs -- Number:%d.\n", MOCA_INTEFACE_NUMBER));




    return 0;
}

ANSC_STATUS
CosaDmlMocaIfGetEntry
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulInterfaceIndex,
        PCOSA_DML_MOCA_IF_FULL      pEntry
    )
{
    AnscTraceWarning(("CosaDmlMocaIfGetEntry -- ulInterfaceIndex:%lu.\n", ulInterfaceIndex));

    if ( !pEntry)
    {
        return ANSC_STATUS_FAILURE;
    }



    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlMocaIfSetCfg
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulInterfaceIndex,
        PCOSA_DML_MOCA_IF_CFG       pCfg
    )
{
    AnscTraceWarning(("CosaDmlMocaIfSetCfg -- ulInterfaceIndex:%lu.\n", ulInterfaceIndex));
    
    if ( !pCfg )
    {
        return ANSC_STATUS_FAILURE;
    }




    return ANSC_STATUS_SUCCESS;
}


ANSC_STATUS
CosaDmlMocaIfGetCfg
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulInterfaceIndex,
        PCOSA_DML_MOCA_IF_CFG       pCfg
    )
{

    if ( !pCfg )
    {
        return ANSC_STATUS_FAILURE;
    }

    return ANSC_STATUS_SUCCESS;
}


ANSC_STATUS
CosaDmlMocaIfGetDinfo
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulInterfaceIndex,
        PCOSA_DML_MOCA_IF_DINFO     pInfo
    )
{

    if (!pInfo)
    {
        return ANSC_STATUS_FAILURE;
    }


        
    return ANSC_STATUS_SUCCESS;
}


ANSC_STATUS
CosaDmlMocaIfGetStats
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulInterfaceIndex,
        PCOSA_DML_MOCA_STATS        pStats
    )
{
    errno_t rc = -1;

    AnscTraceWarning(("CosaDmlMocaIfGetDinfo -- ulInterfaceIndex:%lu.\n", ulInterfaceIndex));

    if ( !pStats )
    {
        return ANSC_STATUS_FAILURE;
    }

    rc = memset_s(pStats, sizeof(COSA_DML_MOCA_STATS), 0, sizeof(COSA_DML_MOCA_STATS));
    ERR_CHK(rc);

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlMocaIfResetStats
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulInterfaceIndex
    )
{
    AnscTraceWarning(("CosaDmlMocaIfResetStats -- ulInterfaceIndex:%lu\n", ulInterfaceIndex));



    return ANSC_STATUS_SUCCESS;
}


ULONG
CosaDmlMocaIfExtCounterGetNumber
    (
        ANSC_HANDLE                      hContext,
        ULONG                            ulInterfaceIndex
    )
{
    AnscTraceWarning(("CosaDmlMocaIfExtCounterGetNumber -- ulInterfaceIndex:%lu.\n", ulInterfaceIndex));


    return 0;
}

/*
    ANSC_HANDLE   :  Hardcoded to NULL currently.
    ulInterfaceIndex : it's the index of Interface.
    ulIndex              : it's the index of this ExtCounter.
    PCOSA_CONTEXT_MOCA_LINK_OBJECT

    return value: if successful, return ANSC_STATUS_SUCCESS.
                        if fail,           return ANSC_STATUS_FAILURE
*/
ANSC_STATUS
CosaDmlMocaIfExtCounterGetEntry
    (
        ANSC_HANDLE                      hContext,
        ULONG                            ulInterfaceIndex,
        ULONG                            ulIndex,
        PCOSA_DML_MOCA_EXTCOUNTER        pConf
    )
{
    AnscTraceWarning(("CosaDmlMocaIfExtCounterGetEntry -- ulInterfaceIndex:%lu, ulIndex:%lu\n", ulInterfaceIndex, ulIndex));



    return ANSC_STATUS_SUCCESS;
}


ULONG
CosaDmlMocaIfExtAggrCounterGetNumber
    (
        ANSC_HANDLE                      hContext,
        ULONG                            ulInterfaceIndex
    )
{
    AnscTraceWarning(("CosaDmlMocaIfExtAggrCounterGetNumber -- ulInterfaceIndex:%lu.\n", ulInterfaceIndex));


    return 0;
}

/*
    ANSC_HANDLE   :  Hardcoded to NULL currently.
    ulInterfaceIndex : it's the index of Interface.
    ulIndex              : it's the index of this ExtCounter.
    PCOSA_CONTEXT_MOCA_LINK_OBJECT

    return value: if successful, return ANSC_STATUS_SUCCESS.
                        if fail,           return ANSC_STATUS_FAILURE
*/
ANSC_STATUS
CosaDmlMocaIfExtAggrCounterGetEntry
    (
        ANSC_HANDLE                      hContext,
        ULONG                            ulInterfaceIndex,
        ULONG                            ulIndex,
        PCOSA_DML_MOCA_EXTAGGRCOUNTER    pConf
    )
{
    AnscTraceWarning(("CosaDmlMocaIfExtAggrCounterGetEntry -- ulInterfaceIndex:%lu, ulIndex:%lu\n", ulInterfaceIndex, ulIndex));


    
    return ANSC_STATUS_SUCCESS;
}

/*
    This function is used to get total Peer tables.
    The returned memory should be allocated by AnscAllocateMemory. Or else there is leaking.
*/
ANSC_STATUS
CosaDmlMocaIfPeerTableGetTable
    (
        ANSC_HANDLE                      hContext,
        ULONG                            ulInterfaceIndex,
        PCOSA_DML_MOCA_PEER             *ppConf,
        PULONG                           pCount
    )
{
    AnscTraceWarning(("CosaDmlMocaIfPeerTableGetTable -- ulInterfaceIndex:%lu, ppConf:%x\n", ulInterfaceIndex, (UINT)ppConf));




    *ppConf = NULL;
    *pCount = 0;

    return ANSC_STATUS_SUCCESS;
}

/*
    This function is used to get total Flow tables.
    The returned memory should be allocated by AnscAllocateMemory. Or else there is leaking.
*/
ANSC_STATUS
CosaDmlMocaIfFlowTableGetTable
    (
        ANSC_HANDLE                      hContext,
        ULONG                            ulInterfaceIndex,
        PCOSA_DML_MOCA_FLOW             *ppConf,
        PULONG                           pCount
    )
{
    AnscTraceWarning(("CosaDmlMocaIfFlowTableGetTable -- ulInterfaceIndex:%lu, ppConf:%x\n", ulInterfaceIndex, (UINT)ppConf));


    
    *ppConf = NULL;
    *pCount = 0;

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlMocaIfGetQos
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulInterfaceIndex,
        PCOSA_DML_MOCA_QOS          pConf
    )
{



    AnscTraceWarning(("CosaDmlMocaIfGetQos -- ulInterfaceIndex:%lu, Enabled:%s\n", ulInterfaceIndex, (pConf->Enabled==TRUE)?"TRUE":"FALSE" ));

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlMocaIfSetQos
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulInterfaceIndex,
        PCOSA_DML_MOCA_QOS          pConf
    )
{
    AnscTraceWarning(("CosaDmlMocaIfSetQos -- ulInterfaceIndex:%lu, Enabled:%s\n", ulInterfaceIndex, (pConf->Enabled==TRUE)?"TRUE":"FALSE" ));



    return ANSC_STATUS_SUCCESS;
}

/*
    ppMeshRateArray is hardcoded to be NULL currently.
    
*/
ANSC_STATUS
CosaDmlMocaIfGetAssocDevices
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulInterfaceIndex,
        PULONG                      pulCount,
        PCOSA_DML_MOCA_ASSOC_DEVICE *ppDeviceArray,
        PULONG                      *ppMeshRateArray    /* Not used now */
    )
{
    if ( !pulCount || !ppDeviceArray )
    {
        return ANSC_STATUS_FAILURE;
    }


    *pulCount      = 0;
    *ppDeviceArray = NULL;

    AnscTraceWarning(("CosaDmlMocaIfGetAssocDevices -- ulInterfaceIndex:%lu, pulCount:%lu\n", ulInterfaceIndex, *pulCount));

    return ANSC_STATUS_SUCCESS;
}
ANSC_STATUS is_usg_in_bridge_mode(BOOL *pBridgeMode)
{
        AnscTraceWarning(("_COSA_SIM-- \n"));
        return 0;
}

ANSC_STATUS CosaMoCAGetForceEnable(PCOSA_DML_MOCA_CFG pCfg)
{
	AnscTraceWarning(("CosaMoCAForceEnable_FUNCTION-- \n"));
	return 0;
}

void* MoCA_Interface_Reset(void *arg)
{
	CcspTraceWarning(("MoCARest_SetEnable_FUNCTION\n"));
	return 0;
}

BOOL MoCA_SetForceEnable(PCOSA_DML_MOCA_IF_CFG pCfg, PCOSA_DML_MOCA_CFG  pFCfg)
{
	AnscTraceWarning(("MoCA_SetForceEnable_FUNCTION\n"));
	return 0;
}

BOOL CosaDmlmocaUnpack(char* blob)
{
    AnscTraceWarning(("CosaDmlmocaUnpack_FUNCTION\n"))
    return 0;
}

#endif


