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

    module: cosa_moca_network_info.h

        For COSA Data Model Library Development

    -------------------------------------------------------------------

    description:

        This file defines the apis for objects to support Data Model Library.

    -------------------------------------------------------------------


    author:

        COSA XML TOOL CODE GENERATOR 1.0

    -------------------------------------------------------------------

    revision:

        01/11/2011    initial revision.

**************************************************************************/

#ifndef  _COSA_MOCA_NETWORK_INFO_H
#define  _COSA_MOCA_NETWORK_INFO_H

#include "cosa_apis.h"
#include "cosa_moca_apis.h"
#include "ansc_platform.h"
#include "ccsp_base_api.h"
#include "ccsp_trace.h"


#define MOCA_POLLINGINTERVAL        20 

typedef struct _MoCADeviceInfo {
    char* ssidType;
    char* AssociatedDevice;
    char* deviceMac;
    char* parentMac;
    char* deviceType;

    int RSSI;
    int Status;
    int Updated;
    int StatusChange;

    struct _MoCADeviceInfo* next;

} MoCADeviceInfo;

typedef struct _MoCADeviceList
{
    int num_devices;
    struct _MoCADeviceInfo* deviceList;

} MoCADeviceList;

/*
    Standard function declaration
*/

void* SynchronizeMoCADevices(void *arg);
void Send_Update_to_LMLite(BOOL defaultSend);

ANSC_STATUS
MocaIf_GetAssocDevices
    (
        int                       ulInterfaceIndex,
        PULONG                      pulCount,
        PCOSA_DML_MOCA_ASSOC_DEVICE *ppDeviceArray
    );

#endif
