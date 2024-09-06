/*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
 *
 * Copyright 2024 RDK Management
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

#include "moca_mock.h"

extern "C"
{
    #include "cosa_moca_dml.h"
    #include "cosa_plugin_api.h"
    #include "plugin_main.h"

    BOOL MyRegisterFunction(void* hContext, char* pApiName, void* hModuleProc)
    {
        UNREFERENCED_PARAMETER(hContext);
        UNREFERENCED_PARAMETER(pApiName);
        UNREFERENCED_PARAMETER(hModuleProc);
        return TRUE;
    }
}

using namespace testing;

extern MocaHalMock *g_mocaHALMock;
extern SyscfgMock *g_syscfgMock;
extern SafecLibMock* g_safecLibMock;
extern SyseventMock *g_syseventMock;
extern webconfigFwMock *g_webconfigFwMock;
extern AnscMemoryMock * g_anscMemoryMock;
extern BaseAPIMock * g_baseapiMock;

//Test for COSA_Init - Success
TEST_F(CcspMoCATestFixture, COSA_InitSuccess)
{
    ULONG uMaxVersionSupported = 1;

    moca_associated_device_t *pAssocDevices = NULL;
    pAssocDevices = (moca_associated_device_t *)AnscAllocateMemory(sizeof(moca_associated_device_t));
    memset(pAssocDevices, 0, sizeof(moca_associated_device_t));

    uint8_t macAddress1[6] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
    memcpy(pAssocDevices[0].MACAddress, macAddress1, 6);
    pAssocDevices[0].NodeID = 0;
    pAssocDevices[0].PreferredNC = false;
    strncpy(pAssocDevices[0].HighestVersion, "1.0", sizeof(pAssocDevices[0].HighestVersion) - 1);
    pAssocDevices[0].HighestVersion[sizeof(pAssocDevices[0].HighestVersion) - 1] = '\0';
    pAssocDevices[0].PHYTxRate = 0;
    pAssocDevices[0].PHYRxRate = 0;
    pAssocDevices[0].TxPowerControlReduction = 0;
    pAssocDevices[0].RxPowerLevel = 0;
    pAssocDevices[0].TxBcastRate = 0;
    pAssocDevices[0].RxBcastPowerLevel = 0;
    pAssocDevices[0].TxPackets = 0;
    pAssocDevices[0].RxPackets = 0;
    pAssocDevices[0].RxErroredAndMissedPackets = 0;
    pAssocDevices[0].QAM256Capable = false;
    pAssocDevices[0].PacketAggregationCapability = false;
    pAssocDevices[0].RxSNR = 0;
    pAssocDevices[0].Active = false;
    pAssocDevices[0].RxBcastRate = 0;
    pAssocDevices[0].NumberOfClients = 0;

    PCOSA_PLUGIN_INFO pPlugInfo  = (PCOSA_PLUGIN_INFO)AnscAllocateMemory(sizeof(COSA_PLUGIN_INFO));
    memset(pPlugInfo, 0, sizeof(COSA_PLUGIN_INFO));

    pPlugInfo->hContext = (ANSC_HANDLE)1;
    pPlugInfo->RegisterFunction = MyRegisterFunction;

    EXPECT_CALL(*g_syscfgMock, syscfg_get(_,_,_,_)).Times(6).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(11).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_syseventMock, sysevent_open(_, _, _, StrEq("moca-update"), _)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(*g_mocaHALMock, moca_GetIfConfig(_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).Times(3).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_mocaHALMock, moca_SetIfConfig(_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_safecLibMock, _memcpy_s_chk(_,_,_,_,_,_)).Times(14).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_mocaHALMock, moca_IfGetDynamicInfo(_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_mocaHALMock, moca_IfGetStaticInfo(_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_webconfigFwMock, register_sub_docs(_,_,_,_)).Times(1);
    EXPECT_CALL(*g_mocaHALMock, moca_GetMocaCPEs(_,_,_)).Times(1).WillOnce(DoAll(SetArgPointee<2>(1), Return(0)));
    EXPECT_CALL(*g_mocaHALMock, moca_GetNumAssociatedDevices(_,_)).Times(1).WillOnce(DoAll(SetArgPointee<1>(1), Return(0)));
    EXPECT_CALL(*g_mocaHALMock, moca_GetAssociatedDevices(_,_)).Times(1).WillOnce(DoAll(SetArgPointee<1>(pAssocDevices), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_safecLibMock, _strcat_s_chk(_,_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_baseapiMock, CcspBaseIf_discComponentSupportingNamespace(_,_,_,_,_,_)).Times(1).WillOnce(DoAll(SetArgPointee<5>(1), Return(0)));
    EXPECT_CALL(*g_syseventMock, sysevent_setnotification(_,_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_syseventMock, sysevent_getnotification(_,_,_,_,_,_,_)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(*g_anscMemoryMock, AnscFreeMemory(_));
    EXPECT_EQ(COSA_Init(uMaxVersionSupported, (void *)pPlugInfo), 0);

    sleep(5); //To avoid ABORT due to write to NULL pointer
    free(pPlugInfo);
    pPlugInfo = NULL;
}

//Test for COSA_Unload - Success
TEST_F(CcspMoCATestFixture, COSA_UnloadSuccess)
{
    COSA_Unload();
}