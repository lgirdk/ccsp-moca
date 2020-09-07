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

/*********************************************************************** 
  
    module: plugin_main.c

        Implement COSA Data Model Library Init and Unload apis.
 
    ---------------------------------------------------------------

    author:

        COSA XML TOOL CODE GENERATOR 1.0

    ---------------------------------------------------------------

    revision:

        09/28/2011    initial revision.

**********************************************************************/

#include "ansc_platform.h"
#include "ansc_load_library.h"
#include "cosa_plugin_api.h"
#include "plugin_main.h"
//#include "cosa_hosts_dml.h"
#include "cosa_moca_dml.h"

#define THIS_PLUGIN_VERSION                         1

ANSC_HANDLE g_MoCAObject ;

int ANSC_EXPORT_API
COSA_Init
    (
        ULONG                       uMaxVersionSupported, 
        void*                       hCosaPlugInfo         /* PCOSA_PLUGIN_INFO passed in by the caller */
    )
{
    PCOSA_PLUGIN_INFO               pPlugInfo  = (PCOSA_PLUGIN_INFO)hCosaPlugInfo;

    if ( uMaxVersionSupported < THIS_PLUGIN_VERSION )
    {
      /* this version is not supported */
        return -1;
    }   
    
    pPlugInfo->uPluginVersion       = THIS_PLUGIN_VERSION;
    /* register the back-end apis for the data model */
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "MoCA_GetParamBoolValue",  MoCA_GetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "MoCA_GetParamIntValue",  MoCA_GetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "MoCA_GetParamUlongValue",  MoCA_GetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "MoCA_GetParamStringValue",  MoCA_GetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "MoCA_SetParamBoolValue",  MoCA_SetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "MoCA_SetParamIntValue",  MoCA_SetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "MoCA_SetParamUlongValue",  MoCA_SetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "MoCA_SetParamStringValue",  MoCA_SetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "MoCA_Validate",  MoCA_Validate);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "MoCA_Commit",  MoCA_Commit);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "MoCA_Rollback",  MoCA_Rollback);

    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "MoCAReset_GetParamBoolValue",  MoCAReset_GetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "MoCAReset_SetParamBoolValue",  MoCAReset_SetParamBoolValue);

    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Interface1_GetEntryCount",  Interface1_GetEntryCount);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Interface1_GetEntry",  Interface1_GetEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Interface1_GetParamBoolValue",  Interface1_GetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Interface1_GetParamIntValue",  Interface1_GetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Interface1_GetParamUlongValue",  Interface1_GetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Interface1_GetParamStringValue",  Interface1_GetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Interface1_SetParamBoolValue",  Interface1_SetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Interface1_SetParamIntValue",  Interface1_SetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Interface1_SetParamUlongValue",  Interface1_SetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Interface1_SetParamStringValue",  Interface1_SetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Interface1_Validate",  Interface1_Validate);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Interface1_Commit",  Interface1_Commit);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Interface1_Rollback",  Interface1_Rollback);


    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Stats2_GetParamBoolValue",  Stats2_GetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Stats2_GetParamIntValue",  Stats2_GetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Stats2_GetParamUlongValue",  Stats2_GetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Stats2_GetParamStringValue",  Stats2_GetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Stats2_SetParamBoolValue",  Stats2_SetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Stats2_SetParamIntValue",  Stats2_SetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Stats2_SetParamUlongValue",  Stats2_SetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Stats2_SetParamStringValue",  Stats2_SetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Stats2_Validate",  Stats2_Validate);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Stats2_Commit",  Stats2_Commit);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Stats2_Rollback",  Stats2_Rollback);


    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "X_CISCO_COM_ExtCounterTable_GetEntryCount",  X_CISCO_COM_ExtCounterTable_GetEntryCount);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "X_CISCO_COM_ExtCounterTable_GetEntry",  X_CISCO_COM_ExtCounterTable_GetEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "X_CISCO_COM_ExtCounterTable_GetParamBoolValue",  X_CISCO_COM_ExtCounterTable_GetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "X_CISCO_COM_ExtCounterTable_GetParamIntValue",  X_CISCO_COM_ExtCounterTable_GetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "X_CISCO_COM_ExtCounterTable_GetParamUlongValue",  X_CISCO_COM_ExtCounterTable_GetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "X_CISCO_COM_ExtCounterTable_GetParamStringValue",  X_CISCO_COM_ExtCounterTable_GetParamStringValue);


    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "X_CISCO_COM_ExtAggrCounterTable_GetEntryCount",  X_CISCO_COM_ExtAggrCounterTable_GetEntryCount);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "X_CISCO_COM_ExtAggrCounterTable_GetEntry",  X_CISCO_COM_ExtAggrCounterTable_GetEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "X_CISCO_COM_ExtAggrCounterTable_GetParamBoolValue",  X_CISCO_COM_ExtAggrCounterTable_GetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "X_CISCO_COM_ExtAggrCounterTable_GetParamIntValue",  X_CISCO_COM_ExtAggrCounterTable_GetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "X_CISCO_COM_ExtAggrCounterTable_GetParamUlongValue",  X_CISCO_COM_ExtAggrCounterTable_GetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "X_CISCO_COM_ExtAggrCounterTable_GetParamStringValue",  X_CISCO_COM_ExtAggrCounterTable_GetParamStringValue);


    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "QoS_GetParamBoolValue",  QoS_GetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "QoS_GetParamIntValue",  QoS_GetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "QoS_GetParamUlongValue",  QoS_GetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "QoS_GetParamStringValue",  QoS_GetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Qos_SetParamBoolValue",  Qos_SetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Qos_SetParamIntValue",  Qos_SetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Qos_SetParamUlongValue",  Qos_SetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Qos_SetParamStringValue",  Qos_SetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Qos_Validate",  Qos_Validate);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Qos_Commit",  Qos_Commit);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Qos_Rollback",  Qos_Rollback);


    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "FlowStats_GetEntryCount",  FlowStats_GetEntryCount);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "FlowStats_GetEntry",  FlowStats_GetEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "FlowStats_IsUpdated",  FlowStats_IsUpdated);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "FlowStats_Synchronize",  FlowStats_Synchronize);
    //pPlugInfo->RegisterFunction(pPlugInfo->hContext, "FlowStats_GetParamBoolValue",  FlowStats_GetParamBoolValue);
    //pPlugInfo->RegisterFunction(pPlugInfo->hContext, "FlowStats_GetParamIntValue",  FlowStats_GetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "FlowStats_GetParamUlongValue",  FlowStats_GetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "FlowStats_GetParamStringValue",  FlowStats_GetParamStringValue);


    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "X_CISCO_COM_PeerTable_GetEntryCount",  X_CISCO_COM_PeerTable_GetEntryCount);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "X_CISCO_COM_PeerTable_GetEntry",  X_CISCO_COM_PeerTable_GetEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "X_CISCO_COM_PeerTable_IsUpdated",  X_CISCO_COM_PeerTable_IsUpdated);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "X_CISCO_COM_PeerTable_Synchronize",  X_CISCO_COM_PeerTable_Synchronize);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "X_CISCO_COM_PeerTable_GetParamBoolValue",  X_CISCO_COM_PeerTable_GetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "X_CISCO_COM_PeerTable_GetParamIntValue",  X_CISCO_COM_PeerTable_GetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "X_CISCO_COM_PeerTable_GetParamUlongValue",  X_CISCO_COM_PeerTable_GetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "X_CISCO_COM_PeerTable_GetParamStringValue",  X_CISCO_COM_PeerTable_GetParamStringValue);

    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "MeshTable_GetEntryCount",  MeshTable_GetEntryCount);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "MeshTable_GetEntry",  MeshTable_GetEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "MeshTable_IsUpdated",  MeshTable_IsUpdated);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "MeshTable_Synchronize",  MeshTable_Synchronize);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "MeshTable_GetParamUlongValue",  MeshTable_GetParamUlongValue);

    //pPlugInfo->RegisterFunction(pPlugInfo->hContext, "MeshTxNodeTable_GetEntryCount",  MeshTxNodeTable_GetEntryCount);
    //pPlugInfo->RegisterFunction(pPlugInfo->hContext, "MeshTxNodeTable_GetEntry",  MeshTxNodeTable_GetEntry);
    //pPlugInfo->RegisterFunction(pPlugInfo->hContext, "MeshTxNodeTable_IsUpdated",  MeshTxNodeTable_IsUpdated);
    //pPlugInfo->RegisterFunction(pPlugInfo->hContext, "MeshTxNodeTable_Synchronize",  MeshTxNodeTable_Synchronize);
    //pPlugInfo->RegisterFunction(pPlugInfo->hContext, "MeshTxNodeTable_GetParamUlongValue",  MeshTxNodeTable_GetParamUlongValue);


    //pPlugInfo->RegisterFunction(pPlugInfo->hContext, "MeshRxNodeTable_GetEntryCount",  MeshRxNodeTable_GetEntryCount);
    //pPlugInfo->RegisterFunction(pPlugInfo->hContext, "MeshRxNodeTable_GetEntry",  MeshRxNodeTable_GetEntry);
    //pPlugInfo->RegisterFunction(pPlugInfo->hContext, "MeshRxNodeTable_GetParamUlongValue",  MeshRxNodeTable_GetParamUlongValue);


    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "AssociatedDevice_GetEntryCount",  AssociatedDevice_GetEntryCount);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "AssociatedDevice_GetEntry",  AssociatedDevice_GetEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "AssociatedDevice_IsUpdated",  AssociatedDevice_IsUpdated);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "AssociatedDevice_Synchronize",  AssociatedDevice_Synchronize);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "AssociatedDevice_GetParamBoolValue",  AssociatedDevice_GetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "AssociatedDevice_GetParamIntValue",  AssociatedDevice_GetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "AssociatedDevice_GetParamUlongValue",  AssociatedDevice_GetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "AssociatedDevice_GetParamStringValue",  AssociatedDevice_GetParamStringValue);

     pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Logging_GetParamBoolValue",  Logging_GetParamBoolValue);
     pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Logging_GetParamUlongValue",  Logging_GetParamUlongValue);
     pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Logging_SetParamBoolValue",  Logging_SetParamBoolValue);
     pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Logging_SetParamUlongValue",  Logging_SetParamUlongValue);
     pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Logging_Validate",  Logging_Validate);
     pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Logging_Commit",  Logging_Commit);
     pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Logging_Rollback",  Logging_Rollback);

	g_MoCAObject          = (ANSC_HANDLE)CosaMoCACreate();

    return  0;
}

BOOL ANSC_EXPORT_API
COSA_IsObjectSupported
    (
        char*                        pObjName
    )
{
    
    UNREFERENCED_PARAMETER(pObjName);
    return TRUE;
}

void ANSC_EXPORT_API
COSA_Unload
    (
        void
    )
{
    /* unload the memory here */
}
