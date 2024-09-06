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
    #include "cosa_moca_network_info.h"
}

using namespace testing;

extern MocaHalMock *g_mocaHALMock;
extern SafecLibMock* g_safecLibMock;
extern AnscMemoryMock * g_anscMemoryMock;

//Test for MocaIf_GetAssocDevices - Success
TEST_F(CcspMoCATestFixture, MocaIfGetAssocDevicesSuccess)
{
    ULONG ulInterfaceIndex = 0;
    ULONG pulCount = 1;

    PCOSA_DML_MOCA_ASSOC_DEVICE pConf = new COSA_DML_MOCA_ASSOC_DEVICE;

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

//For pnum_cpes == *pulCount
    EXPECT_CALL(*g_mocaHALMock, moca_GetMocaCPEs(_,_,_)).Times(1).WillOnce(DoAll(SetArgPointee<2>(1), Return(0)));
    EXPECT_CALL(*g_mocaHALMock, moca_GetNumAssociatedDevices(_,_)).Times(1).WillOnce(DoAll(SetArgPointee<1>(1), Return(0)));
    EXPECT_CALL(*g_mocaHALMock, moca_GetAssociatedDevices(_,_)).Times(1).WillOnce(DoAll(SetArgPointee<1>(pAssocDevices), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _memcpy_s_chk(_,_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_anscMemoryMock, AnscFreeMemory(_)).Times(3);
    EXPECT_EQ(MocaIf_GetAssocDevices( ulInterfaceIndex, &pulCount, &pConf), ANSC_STATUS_SUCCESS);

//For moca_GetAssociatedDevices failure
    EXPECT_CALL(*g_mocaHALMock, moca_GetMocaCPEs(_,_,_)).Times(1).WillOnce(DoAll(SetArgPointee<2>(1), Return(0)));
    EXPECT_CALL(*g_mocaHALMock, moca_GetNumAssociatedDevices(_,_)).Times(1).WillOnce(DoAll(SetArgPointee<1>(1), Return(0)));
    EXPECT_CALL(*g_mocaHALMock, moca_GetAssociatedDevices(_,_)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_EQ(MocaIf_GetAssocDevices( ulInterfaceIndex, &pulCount, &pConf), 1);

    free(pAssocDevices);
    pAssocDevices = NULL;

    delete pConf;
}