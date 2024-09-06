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
    #include "cosa_moca_apis.h"
    #include "cosa_moca_internal.h"

    int consoleDebugEnable;
    FILE* debugLogFile = nullptr;
    char g_Subsystem[32] = {0};
    void* bus_handle;
}

using namespace testing;

extern MocaHalMock *g_mocaHALMock;
extern SyscfgMock *g_syscfgMock;
extern SafecLibMock* g_safecLibMock;
extern AnscMemoryMock * g_anscMemoryMock;
extern DslhDmagntExportedMock* g_dslhDmagntExportedMock;
extern base64Mock *g_base64Mock;
extern msgpackMock *g_msgpackMock;
extern webconfigFwMock *g_webconfigFwMock;

// Test for CosaDmlMocaInit - Failure
TEST_F(CcspMoCATestFixture, CosaDmlMocaInitFailure)
{
    EXPECT_CALL(*g_mocaHALMock, moca_HardwareEquipped()).Times(6).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_syscfgMock, syscfg_get(_,_,_,_)).Times(1).WillOnce(Return(0));
    ANSC_STATUS status = CosaDmlMocaInit(NULL, NULL);
    EXPECT_EQ(ANSC_STATUS_FAILURE, status);
}

//Test for CosaDmlMocaGetNumberOfIfs - Failure
TEST_F(CcspMoCATestFixture, CosaDmlMocaGetNumberOfIfsFailure)
{
    ULONG status = CosaDmlMocaGetNumberOfIfs(NULL);
    EXPECT_EQ(0, status);
}

// Test for CosaDmlMocaInit - Success
TEST_F(CcspMoCATestFixture, CosaDmlMocaInitSuccess)
{
    EXPECT_CALL(*g_mocaHALMock, moca_HardwareEquipped()).Times(1).WillOnce(Return(1));
    EXPECT_CALL(*g_syscfgMock, syscfg_get(_,_,_,_)).Times(1).WillOnce(Return(0));
    ANSC_STATUS status = CosaDmlMocaInit(NULL, NULL);
    EXPECT_EQ(ANSC_STATUS_SUCCESS, status);
}

//Test for CosaDmlMocaGetNumberOfIfs - Success
TEST(CosaMocaApisTest, CosaDmlMocaGetNumberOfIfsSuccess)
{
    ULONG status = CosaDmlMocaGetNumberOfIfs(NULL);
    EXPECT_EQ(1, status);
}

// Test for CosaDmlMocaGetNumberOfIfs
TEST(CosaMocaApisTest, CosaDmlMocaGetNumberOfIfs)
{
    ULONG numIfs = CosaDmlMocaGetNumberOfIfs(NULL);
    // Add assertions for numIfs value if needed
}
/*
// Test for CosaDmlMocaIfGetEntry
TEST(CosaMocaApisTest, CosaDmlMocaIfGetEntry)
{
    ULONG ulInterfaceIndex = 0; // Set the desired interface index
    PCOSA_DATAMODEL_MOCA  entry;
    ANSC_STATUS status = CosaDmlMocaIfGetEntry(NULL, ulInterfaceIndex, &entry->MoCAIfFullTable[1].MoCAIfFull);
    EXPECT_EQ(ANSC_STATUS_SUCCESS, status);
    // Add additional assertions for entry values if needed
}
*/
// Test for CosaDmlMocaIfReset - Sucess
TEST_F(CcspMoCATestFixture, CosaDmlMocaIfResetSuccess)
{
    ULONG ulInterfaceIndex = 0; // Set the desired interface index
    ANSC_HANDLE g_TestMoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));
    
    PCOSA_DATAMODEL_MOCA        pTestObject   = (PCOSA_DATAMODEL_MOCA    )g_TestMoCAObject;
	PCOSA_DML_MOCA_CFG          mCfg          = &pTestObject->MoCACfg;
	PCOSA_DML_MOCA_IF_CFG		pCfg	      =	&pTestObject->MoCAIfFullTable[0].MoCAIfFull.Cfg;
	PCOSA_DML_MOCA_IF_DINFO		pInfo	      = &pTestObject->MoCAIfFullTable[0].MoCAIfFull.DynamicInfo;
    // Initialize mCfg, pCfg, and pInfo with desired values
    pCfg->bEnabled = TRUE;

    EXPECT_CALL(*g_mocaHALMock, moca_IfGetDynamicInfo(_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_mocaHALMock, moca_GetIfConfig(_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_mocaHALMock, moca_SetIfConfig(_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_syscfgMock, syscfg_set_nns_commit(_,_)).Times(2).WillRepeatedly(Return(0));

    ANSC_STATUS status = CosaDmlMocaIfReset(NULL, ulInterfaceIndex, mCfg, pCfg, pInfo);
    EXPECT_EQ(ANSC_STATUS_SUCCESS, status);

    free(g_TestMoCAObject);
    g_TestMoCAObject = NULL;
}

// Test for CosaDmlMocaIfReset - MocaDisabledFailure Failure
TEST_F(CcspMoCATestFixture, CosaDmlMocaIfResetMocaDisabledFailure)
{
    ULONG ulInterfaceIndex = 0; // Set the desired interface index
    ANSC_HANDLE g_TestMoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));
    
    PCOSA_DATAMODEL_MOCA        pTestObject   = (PCOSA_DATAMODEL_MOCA    )g_TestMoCAObject;
	PCOSA_DML_MOCA_CFG          mCfg          = &pTestObject->MoCACfg;
	PCOSA_DML_MOCA_IF_CFG		pCfg	      =	&pTestObject->MoCAIfFullTable[0].MoCAIfFull.Cfg;
	PCOSA_DML_MOCA_IF_DINFO		pInfo	      = &pTestObject->MoCAIfFullTable[0].MoCAIfFull.DynamicInfo;
    // Initialize mCfg, pCfg, and pInfo with desired values
    pCfg->bEnabled = FALSE;

    ANSC_STATUS status = CosaDmlMocaIfReset(NULL, ulInterfaceIndex, mCfg, pCfg, pInfo);
    EXPECT_EQ(ANSC_STATUS_FAILURE, status);

    free(g_TestMoCAObject);
    g_TestMoCAObject = NULL;
}

// Test for CosaDmlMocaIfReset - pCfg NULL Failure
TEST_F(CcspMoCATestFixture, CosaDmlMocaIfResetPcfgNullFailure)
{
    ULONG ulInterfaceIndex = 0; // Set the desired interface index
    ANSC_HANDLE g_TestMoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));
    
    PCOSA_DATAMODEL_MOCA        pTestObject   = (PCOSA_DATAMODEL_MOCA    )g_TestMoCAObject;
	PCOSA_DML_MOCA_CFG          mCfg          = &pTestObject->MoCACfg;
	PCOSA_DML_MOCA_IF_CFG		pCfg	      =	&pTestObject->MoCAIfFullTable[0].MoCAIfFull.Cfg;
	PCOSA_DML_MOCA_IF_DINFO		pInfo	      = &pTestObject->MoCAIfFullTable[0].MoCAIfFull.DynamicInfo;
    // Initialize mCfg, pCfg, and pInfo with desired values
    pCfg->bEnabled = FALSE;

    ANSC_STATUS status = CosaDmlMocaIfReset(NULL, ulInterfaceIndex, mCfg, NULL, pInfo);
    EXPECT_EQ(ANSC_STATUS_FAILURE, status);

    free(g_TestMoCAObject);
    g_TestMoCAObject = NULL;
}

// Test for CosaDmlMocaIfReset - pInfo NULL Failure
TEST_F(CcspMoCATestFixture, CosaDmlMocaIfResetPinfoNullFailure)
{
    ULONG ulInterfaceIndex = 0; // Set the desired interface index
    ANSC_HANDLE g_TestMoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));
    
    PCOSA_DATAMODEL_MOCA        pTestObject   = (PCOSA_DATAMODEL_MOCA    )g_TestMoCAObject;
	PCOSA_DML_MOCA_CFG          mCfg          = &pTestObject->MoCACfg;
	PCOSA_DML_MOCA_IF_CFG		pCfg	      =	&pTestObject->MoCAIfFullTable[0].MoCAIfFull.Cfg;
	PCOSA_DML_MOCA_IF_DINFO		pInfo	      = &pTestObject->MoCAIfFullTable[0].MoCAIfFull.DynamicInfo;
    // Initialize mCfg, pCfg, and pInfo with desired values
    pCfg->bEnabled = FALSE;

    ANSC_STATUS status = CosaDmlMocaIfReset(NULL, ulInterfaceIndex, mCfg, pCfg, NULL);
    EXPECT_EQ(ANSC_STATUS_FAILURE, status);

    free(g_TestMoCAObject);
    g_TestMoCAObject = NULL;
}

// Test for CosaDmlMocaIfReset - ulInterfaceIndex Zero Failure
TEST_F(CcspMoCATestFixture, CosaDmlMocaIfResetUlInterfaceIndexZeroFailure)
{
    ULONG ulInterfaceIndex = 0; // Set the desired interface index
    ANSC_HANDLE g_TestMoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));
    
    PCOSA_DATAMODEL_MOCA        pTestObject   = (PCOSA_DATAMODEL_MOCA    )g_TestMoCAObject;
	PCOSA_DML_MOCA_CFG          mCfg          = &pTestObject->MoCACfg;
	PCOSA_DML_MOCA_IF_CFG		pCfg	      =	&pTestObject->MoCAIfFullTable[0].MoCAIfFull.Cfg;
	PCOSA_DML_MOCA_IF_DINFO		pInfo	      = &pTestObject->MoCAIfFullTable[0].MoCAIfFull.DynamicInfo;
    // Initialize mCfg, pCfg, and pInfo with desired values
    pCfg->bEnabled = FALSE;

    ANSC_STATUS status = CosaDmlMocaIfReset(NULL, 0, mCfg, pCfg, pInfo);
    EXPECT_EQ(ANSC_STATUS_FAILURE, status);

    free(g_TestMoCAObject);
    g_TestMoCAObject = NULL;
}

// Test for CosaDmlMocaIfSetCfg - Success
TEST_F(CcspMoCATestFixture, CosaDmlMocaIfSetCfgSuccess)
{
    ULONG ulInterfaceIndex = 0; // Set the desired interface index
    ANSC_HANDLE g_TestMoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));
    
    PCOSA_DATAMODEL_MOCA        pTestObject   = (PCOSA_DATAMODEL_MOCA    )g_TestMoCAObject;
	PCOSA_DML_MOCA_IF_CFG		pCfg	      =	&pTestObject->MoCAIfFullTable[0].MoCAIfFull.Cfg;

    pCfg->X_CISCO_COM_Reset = TRUE;

    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_syscfgMock, syscfg_get(_,_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_safecLibMock, _memcpy_s_chk(_,_,_,_,_,_)).Times(3).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_mocaHALMock, moca_SetIfConfig(_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_syscfgMock, syscfg_set_nns_commit(_,_)).Times(11).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_mocaHALMock, moca_FreqMaskToValue(_)).Times(1).WillOnce(Return(0));

    ANSC_STATUS status = CosaDmlMocaIfSetCfg(NULL, ulInterfaceIndex, pCfg);
    EXPECT_EQ(ANSC_STATUS_SUCCESS, status);

    free(g_TestMoCAObject);
    g_TestMoCAObject = NULL;
}

// Test for CosaDmlMocaIfSetCfg - Failure
TEST_F(CcspMoCATestFixture, CosaDmlMocaIfSetCfgFailure)
{
    ULONG ulInterfaceIndex = 0; // Set the desired interface index
    ANSC_HANDLE g_TestMoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));
    
    PCOSA_DATAMODEL_MOCA        pTestObject   = (PCOSA_DATAMODEL_MOCA    )g_TestMoCAObject;
	PCOSA_DML_MOCA_IF_CFG		pCfg	      =	&pTestObject->MoCAIfFullTable[0].MoCAIfFull.Cfg;

    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_syscfgMock, syscfg_get(_,_,_,_)).Times(1).WillOnce(Return(0));

    ANSC_STATUS status = CosaDmlMocaIfSetCfg(NULL, 1, pCfg);
    EXPECT_EQ(ANSC_STATUS_FAILURE, status);

    free(g_TestMoCAObject);
    g_TestMoCAObject = NULL;
}

// Test for CosaDmlMocaIfGetCfg - Success
TEST_F(CcspMoCATestFixture, CosaDmlMocaIfGetCfgSuccess)
{
    ULONG uIndex = 0; // Set the desired interface index
    ANSC_HANDLE g_TestMoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));
    
    PCOSA_DATAMODEL_MOCA        pTestObject   = (PCOSA_DATAMODEL_MOCA    )g_TestMoCAObject;
	PCOSA_DML_MOCA_IF_CFG		pCfg	      =	&pTestObject->MoCAIfFullTable[0].MoCAIfFull.Cfg;

    EXPECT_CALL(*g_syscfgMock, syscfg_get(_,_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_mocaHALMock, moca_GetIfConfig(_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_mocaHALMock, moca_SetIfConfig(_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_safecLibMock, _memcpy_s_chk(_,_,_,_,_,_)).Times(4).WillRepeatedly(Return(0));

    ANSC_STATUS status = CosaDmlMocaIfGetCfg(NULL, uIndex, pCfg);
    EXPECT_EQ(ANSC_STATUS_SUCCESS, status);

    free(g_TestMoCAObject);
    g_TestMoCAObject = NULL;
}

// Test for CosaDmlMocaIfGetCfg - Failure
TEST_F(CcspMoCATestFixture, CosaDmlMocaIfGetCfgFailure)
{
    ULONG uIndex = 1; // Set the desired interface index
    ANSC_HANDLE g_TestMoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));
    
    PCOSA_DATAMODEL_MOCA        pTestObject   = (PCOSA_DATAMODEL_MOCA    )g_TestMoCAObject;
	PCOSA_DML_MOCA_IF_CFG		pCfg	      =	&pTestObject->MoCAIfFullTable[0].MoCAIfFull.Cfg;

    EXPECT_CALL(*g_syscfgMock, syscfg_get(_,_,_,_)).Times(1).WillOnce(Return(0));

    ANSC_STATUS status = CosaDmlMocaIfGetCfg(NULL, uIndex, pCfg);
    EXPECT_EQ(ANSC_STATUS_FAILURE, status);

    free(g_TestMoCAObject);
    g_TestMoCAObject = NULL;
}

// Test for CosaDmlMocaIfGetDinfo - Success
TEST_F(CcspMoCATestFixture, CosaDmlMocaIfGetDinfoSuccess)
{
    ULONG uIndex = 0; // Set the desired interface index
    ANSC_HANDLE g_TestMoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));
    
    PCOSA_DATAMODEL_MOCA        pTestObject   = (PCOSA_DATAMODEL_MOCA    )g_TestMoCAObject;
	PCOSA_DML_MOCA_IF_DINFO		pInfo	      = &pTestObject->MoCAIfFullTable[0].MoCAIfFull.DynamicInfo;

    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_mocaHALMock, moca_IfGetDynamicInfo(_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_safecLibMock, _memcpy_s_chk(_,_,_,_,_,_)).Times(3).WillRepeatedly(Return(0));

    ANSC_STATUS status = CosaDmlMocaIfGetDinfo(NULL, uIndex, pInfo);
    EXPECT_EQ(ANSC_STATUS_SUCCESS, status);

    free(g_TestMoCAObject);
    g_TestMoCAObject = NULL;
}

// Test for CosaDmlMocaIfGetDinfo - Failure
TEST_F(CcspMoCATestFixture, CosaDmlMocaIfGetDinfoFailure)
{
    ULONG uIndex = 1; // Set the desired interface index
    ANSC_HANDLE g_TestMoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));
    
    PCOSA_DATAMODEL_MOCA        pTestObject   = (PCOSA_DATAMODEL_MOCA    )g_TestMoCAObject;
	PCOSA_DML_MOCA_IF_DINFO		pInfo	      = &pTestObject->MoCAIfFullTable[0].MoCAIfFull.DynamicInfo;

    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));

    ANSC_STATUS status = CosaDmlMocaIfGetDinfo(NULL, uIndex, pInfo);
    EXPECT_EQ(ANSC_STATUS_FAILURE, status);

    free(g_TestMoCAObject);
    g_TestMoCAObject = NULL;
}

// Test for CosaDmlMocaIfGetStaticInfo - Success
TEST_F(CcspMoCATestFixture, CosaDmlMocaIfGetStaticInfoSuccess)
{
    ULONG uIndex = 0; // Set the desired interface index
    ANSC_HANDLE g_TestMoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));
    
    PCOSA_DATAMODEL_MOCA        pTestObject   = (PCOSA_DATAMODEL_MOCA    )g_TestMoCAObject;
    PCOSA_DML_MOCA_IF_SINFO		sInfo	      = &pTestObject->MoCAIfFullTable[0].MoCAIfFull.StaticInfo;

    EXPECT_CALL(*g_safecLibMock, _memcpy_s_chk(_,_,_,_,_,_)).Times(5).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_mocaHALMock, moca_IfGetStaticInfo(_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).Times(1).WillOnce(Return(0));

    ANSC_STATUS status = CosaDmlMocaIfGetStaticInfo(NULL, uIndex, sInfo);
    EXPECT_EQ(ANSC_STATUS_SUCCESS, status);

    free(g_TestMoCAObject);
    g_TestMoCAObject = NULL;
}

// Test for CosaDmlMocaIfGetStaticInfo - Failure
TEST_F(CcspMoCATestFixture, CosaDmlMocaIfGetStaticInfoFailure)
{
    ULONG uIndex = 1; // Set the desired interface index
    ANSC_HANDLE g_TestMoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));
    
    PCOSA_DATAMODEL_MOCA        pTestObject   = (PCOSA_DATAMODEL_MOCA    )g_TestMoCAObject;
    PCOSA_DML_MOCA_IF_SINFO		sInfo	      = &pTestObject->MoCAIfFullTable[0].MoCAIfFull.StaticInfo;

    ANSC_STATUS status = CosaDmlMocaIfGetStaticInfo(NULL, uIndex, sInfo);
    EXPECT_EQ(ANSC_STATUS_FAILURE, status);

    free(g_TestMoCAObject);
    g_TestMoCAObject = NULL;
}

// Test for CosaDmlMocaIfGetStats - uIndex 0 Success
TEST_F(CcspMoCATestFixture, CosaDmlMocaIfGetStatsUIndexZeroSuccess)
{
    ULONG uIndex = 0; // Set the desired interface index    
    COSA_DML_MOCA_STATS             Stats;

    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_mocaHALMock, moca_IfGetStats(_,_)).Times(1).WillOnce(Return(0));

    ANSC_STATUS status = CosaDmlMocaIfGetStats(NULL, uIndex, &Stats);
    EXPECT_EQ(ANSC_STATUS_SUCCESS, status);
}

// Test for CosaDmlMocaIfGetStats - uIndex 1 Success
TEST_F(CcspMoCATestFixture, CosaDmlMocaIfGetStatsUIndexOneSuccess)
{
    ULONG uIndex = 1; // Set the desired interface index    
    COSA_DML_MOCA_STATS             Stats;

    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_mocaHALMock, moca_IfGetStats(_,_)).Times(1).WillOnce(Return(0));

    ANSC_STATUS status = CosaDmlMocaIfGetStats(NULL, uIndex, &Stats);
    EXPECT_EQ(ANSC_STATUS_SUCCESS, status);
}

// Test for CosaDmlMocaIfGetStats - Failure
TEST_F(CcspMoCATestFixture, CosaDmlMocaIfGetStatsFailure)
{
    ULONG uIndex = 2; // Set the desired interface index    
    COSA_DML_MOCA_STATS             Stats;

    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));

    ANSC_STATUS status = CosaDmlMocaIfGetStats(NULL, uIndex, &Stats);
    EXPECT_EQ(ANSC_STATUS_FAILURE, status);
}

// Test for CosaDmlMocaIfGetAssocDevices - Success
TEST_F(CcspMoCATestFixture, CosaDmlMocaIfGetAssocDevicesSuccess)
{
    ULONG uIndex = 0; // Set the desired interface index
    ULONG AssocDeviceCount = 0;
    PCOSA_DML_MOCA_ASSOC_DEVICE pMoCAAssocDevice = NULL;

    moca_associated_device_t *pAssocDevices = NULL;

    pAssocDevices = (moca_associated_device_t *)AnscAllocateMemory(sizeof(moca_associated_device_t));
    uint8_t macAddress[6] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
    memcpy(pAssocDevices->MACAddress, macAddress, 6);
    pAssocDevices->NodeID = 0;
    pAssocDevices->PreferredNC = false;
    strncpy(pAssocDevices->HighestVersion, "1.0", sizeof(pAssocDevices->HighestVersion) - 1);
    pAssocDevices->HighestVersion[sizeof(pAssocDevices->HighestVersion) - 1] = '\0';
    pAssocDevices->PHYTxRate = 0;
    pAssocDevices->PHYRxRate = 0;
    pAssocDevices->TxPowerControlReduction = 0;
    pAssocDevices->RxPowerLevel = 0;
    pAssocDevices->TxBcastRate = 0;
    pAssocDevices->RxBcastPowerLevel = 0;
    pAssocDevices->TxPackets = 0;
    pAssocDevices->RxPackets = 0;
    pAssocDevices->RxErroredAndMissedPackets = 0;
    pAssocDevices->QAM256Capable = false;
    pAssocDevices->PacketAggregationCapability = false;
    pAssocDevices->RxSNR = 0;
    pAssocDevices->Active = false;
    pAssocDevices->RxBcastRate = 0;
    pAssocDevices->NumberOfClients = 0;

    //used SetArgPointee<2>(1) to set the value of moca_GetMocaCPEs 3rd parameter to 1
    EXPECT_CALL(*g_mocaHALMock, moca_GetMocaCPEs(_,_,_)).Times(1).WillOnce(DoAll(SetArgPointee<2>(1), Return(0)));
    //used SetArgPointee<1>(1) to set the value of moca_GetNumAssociatedDevices 2nd parameter to 1
    EXPECT_CALL(*g_mocaHALMock, moca_GetNumAssociatedDevices(_,_)).Times(1).WillOnce(DoAll(SetArgPointee<1>(1), Return(0)));
    //used SetArgPointee<1>(pAssocDevices) to set the value of moca_GetAssociatedDevices 2nd parameter to use pAssocDevices struct
    EXPECT_CALL(*g_mocaHALMock, moca_GetAssociatedDevices(_,_)).Times(1).WillOnce(DoAll(SetArgPointee<1>(pAssocDevices), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_safecLibMock, _memcpy_s_chk(_,_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_anscMemoryMock, AnscFreeMemory(_)).Times(1);

    ANSC_STATUS status = CosaDmlMocaIfGetAssocDevices(NULL, uIndex, &AssocDeviceCount, &pMoCAAssocDevice, NULL);
    EXPECT_EQ(ANSC_STATUS_SUCCESS, status);

    free(pMoCAAssocDevice);
    pMoCAAssocDevice = NULL;

    free(pAssocDevices);
    pAssocDevices = NULL;
}

// Test for CosaDmlMocaIfGetAssocDevices - uIndex
TEST_F(CcspMoCATestFixture, CosaDmlMocaIfGetAssocDevicesUIndexOne)
{
    ULONG uIndex = 1; // Set the desired interface index
    ULONG AssocDeviceCount = 0;
    PCOSA_DML_MOCA_ASSOC_DEVICE pMoCAAssocDevice = NULL;

    ANSC_STATUS status = CosaDmlMocaIfGetAssocDevices(NULL, uIndex, &AssocDeviceCount, &pMoCAAssocDevice, NULL);
}

// Test for CosaDmlMocaSetCfg
TEST(CosaMocaApisTest, CosaDmlMocaSetCfg)
{
    COSA_DML_MOCA_CFG cfg;
    // Initialize cfg with desired values
    ANSC_STATUS status = CosaDmlMocaSetCfg(NULL, &cfg);
    EXPECT_EQ(ANSC_STATUS_SUCCESS, status);
}

// Test for CosaDmlMocaGetCfg
TEST(CosaMocaApisTest, CosaDmlMocaGetCfg)
{
    COSA_DML_MOCA_CFG cfg;
    ANSC_STATUS status = CosaDmlMocaGetCfg(NULL, &cfg);

    EXPECT_STREQ(reinterpret_cast<const char*>(cfg.X_CISCO_COM_ProvisioningFilename), "X_CISCO_COM_ProvisioningFilename");
    EXPECT_STREQ(reinterpret_cast<const char*>(cfg.X_CISCO_COM_ProvisioningServerAddress), "2031:0000:1F1F:0000:0000:0100:11A0:ADDF");
    EXPECT_EQ(cfg.bForceEnabled, 0);
    EXPECT_EQ(cfg.X_CISCO_COM_ProvisioningServerAddressType, MOCA_PROVISIONING_SERVADDR_TYPE_IPV6);
    EXPECT_EQ(ANSC_STATUS_SUCCESS, status);
}

// Test for CosaDmlMocaIfResetStats
TEST(CosaMocaApisTest, CosaDmlMocaIfResetStats)
{
    ULONG uIndex = 0; // Set the desired interface index
    ANSC_STATUS status = CosaDmlMocaIfResetStats(NULL, uIndex);

    EXPECT_EQ(ANSC_STATUS_SUCCESS, status);
}

//Test for CosaDmlMocaIfSetQos
TEST(CosaMocaApisTest, CosaDmlMocaIfSetQos)
{
    ULONG uIndex = 0; // Set the desired interface index
    PCOSA_DML_MOCA_QOS pConf = new COSA_DML_MOCA_QOS;

    pConf->Enabled= false;

    ANSC_STATUS status = CosaDmlMocaIfSetQos(NULL, uIndex, pConf);

    EXPECT_EQ(ANSC_STATUS_SUCCESS, status);

    delete pConf;
}

//Test for CosaDmlMocaIfGetQos
TEST(CosaMocaApisTest, CosaDmlMocaIfGetQos)
{
    ULONG uIndex = 0; // Set the desired interface index
    PCOSA_DML_MOCA_QOS pConf = new COSA_DML_MOCA_QOS;

    ANSC_STATUS status = CosaDmlMocaIfGetQos(NULL, uIndex, pConf);

    EXPECT_EQ(pConf->Enabled, 0);
    EXPECT_EQ(ANSC_STATUS_SUCCESS, status);

    delete pConf;
}

//Test for CosaDmlMocaIfExtCounterGetNumber - uIndex 0
TEST_F(CcspMoCATestFixture, CosaDmlMocaIfExtCounterGetNumberUIndexZero)
{
    ULONG uIndex = 0; // Set the desired interface index
    ULONG ulCount = 1;

    //used SetArgPointee<1>(ulCount) to set the value of moca_GetNumAssociatedDevices 2nd parameter to ulCount
    EXPECT_CALL(*g_mocaHALMock, moca_GetNumAssociatedDevices(_,_)).Times(1).WillOnce(DoAll(SetArgPointee<1>(ulCount), Return(0)));

    ULONG ret = CosaDmlMocaIfExtCounterGetNumber(NULL, uIndex);

    EXPECT_EQ(1, ret);
}

//Test for CosaDmlMocaIfExtCounterGetNumber - uIndex 1
TEST(CosaMocaApisTest, CosaDmlMocaIfExtCounterGetNumberUIndexOne)
{
    ULONG uIndex = 1; // Set the desired interface index

    ULONG ret = CosaDmlMocaIfExtCounterGetNumber(NULL, uIndex);

    EXPECT_EQ(0, ret);
}

//Test for CosaDmlMocaIfExtCounterGetEntry - Success
TEST_F(CcspMoCATestFixture, CosaDmlMocaIfExtCounterGetEntrySuccess)
{
    ULONG uInterfaceIndex = 0; // Set the desired interface index
    ULONG uIndex = 0; // Set the desired index
    PCOSA_DML_MOCA_EXTCOUNTER pConf = new COSA_DML_MOCA_EXTCOUNTER;

    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_mocaHALMock, moca_GetNumAssociatedDevices(_,_)).Times(1).WillOnce(DoAll(SetArgPointee<1>(1), Return(0)));
    EXPECT_CALL(*g_mocaHALMock, moca_IfGetExtCounter(_,_)).Times(1).WillOnce(Return(0));

    ANSC_STATUS status = CosaDmlMocaIfExtCounterGetEntry(NULL, uInterfaceIndex, uIndex, pConf);

    EXPECT_EQ(ANSC_STATUS_SUCCESS, status);

    delete pConf;
}

//Test for CosaDmlMocaIfExtCounterGetEntry - uIndex 1 Failure
TEST_F(CcspMoCATestFixture, CosaDmlMocaIfExtCounterGetEntryUIndexOneFailure)
{
    ULONG uInterfaceIndex = 0; // Set the desired interface index
    ULONG uIndex = 1; // Set the desired index
    PCOSA_DML_MOCA_EXTCOUNTER pConf = NULL;

    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_mocaHALMock, moca_GetNumAssociatedDevices(_,_)).Times(1).WillOnce(DoAll(SetArgPointee<1>(1), Return(0)));

    ANSC_STATUS status = CosaDmlMocaIfExtCounterGetEntry(NULL, uInterfaceIndex, uIndex, pConf);

    EXPECT_EQ(ANSC_STATUS_FAILURE, status);
}

//Test for CosaDmlMocaIfExtCounterGetEntry - uInterfaceIndex 1 Failure
TEST_F(CcspMoCATestFixture, CosaDmlMocaIfExtCounterGetEntryUInterfaceIndexOneFailure)
{
    ULONG uInterfaceIndex = 1; // Set the desired interface index
    ULONG uIndex = 0; // Set the desired index
    PCOSA_DML_MOCA_EXTCOUNTER pConf = NULL;

    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(1).WillOnce(Return(0));

    ANSC_STATUS status = CosaDmlMocaIfExtCounterGetEntry(NULL, uInterfaceIndex, uIndex, pConf);

    EXPECT_EQ(ANSC_STATUS_FAILURE, status);
}

//Test for CosaDmlMocaIfExtAggrCounterGetNumber - uIndex 0 Success
TEST(CosaMocaApisTest, CosaDmlMocaIfExtAggrCounterGetNumberUIndexZeroSuccess)
{
    ULONG uIndex = 0; // Set the desired interface index

    ULONG ret = CosaDmlMocaIfExtAggrCounterGetNumber(NULL, uIndex);

    EXPECT_EQ(1, ret);
}

//Test for CosaDmlMocaIfExtAggrCounterGetNumber - uIndex 1 Success
TEST(CosaMocaApisTest, CosaDmlMocaIfExtAggrCounterGetNumberUIndexOneSuccess)
{
    ULONG uIndex = 1; // Set the desired interface index

    ULONG ret = CosaDmlMocaIfExtAggrCounterGetNumber(NULL, uIndex);

    EXPECT_EQ(0, ret);
}

//Test for CosaDmlMocaIfExtAggrCounterGetEntry - Success
TEST_F(CcspMoCATestFixture, CosaDmlMocaIfExtAggrCounterGetEntrySuccess)
{
    ULONG uInterfaceIndex = 0; // Set the desired interface index
    ULONG uIndex = 0; // Set the desired index
    PCOSA_DML_MOCA_EXTAGGRCOUNTER pConf = new COSA_DML_MOCA_EXTAGGRCOUNTER;

    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_mocaHALMock, moca_IfGetExtAggrCounter(_,_)).Times(1).WillOnce(Return(0));

    ANSC_STATUS status = CosaDmlMocaIfExtAggrCounterGetEntry(NULL, uInterfaceIndex, uIndex, pConf);

    EXPECT_EQ(ANSC_STATUS_SUCCESS, status);

    delete pConf;
}

//Test for CosaDmlMocaIfExtAggrCounterGetEntry - uInterfaceIndex 1 Failure
TEST_F(CcspMoCATestFixture, CosaDmlMocaIfExtAggrCounterGetEntryUInterfaceOneFailure)
{
    ULONG uInterfaceIndex = 1; // Set the desired interface index
    ULONG uIndex = 0; // Set the desired index
    PCOSA_DML_MOCA_EXTAGGRCOUNTER pConf = new COSA_DML_MOCA_EXTAGGRCOUNTER;

    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(1).WillOnce(Return(0));

    ANSC_STATUS status = CosaDmlMocaIfExtAggrCounterGetEntry(NULL, uInterfaceIndex, uIndex, pConf);

    EXPECT_EQ(ANSC_STATUS_FAILURE, status);

    delete pConf;
}

//Test for CosaDmlMocaIfExtAggrCounterGetEntry - uIndex 1 Failure
TEST_F(CcspMoCATestFixture, CosaDmlMocaIfExtAggrCounterGetEntryUIndexOneFailure)
{
    ULONG uInterfaceIndex = 0; // Set the desired interface index
    ULONG uIndex = 1; // Set the desired index
    PCOSA_DML_MOCA_EXTAGGRCOUNTER pConf = new COSA_DML_MOCA_EXTAGGRCOUNTER;

    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(1).WillOnce(Return(0));

    ANSC_STATUS status = CosaDmlMocaIfExtAggrCounterGetEntry(NULL, uInterfaceIndex, uIndex, pConf);

    EXPECT_EQ(ANSC_STATUS_FAILURE, status);

    delete pConf;
}

//Test for CosaDmlMocaIfPeerTableGetTable - Success
TEST_F(CcspMoCATestFixture, CosaDmlMocaIfPeerTableGetTableSuccess)
{
    ULONG uIndex = 0; // Set the desired interface index
    ULONG count = 0;
    PCOSA_DML_MOCA_PEER pConf = NULL;

    ANSC_STATUS status = CosaDmlMocaIfPeerTableGetTable(NULL, uIndex, &pConf, &count);

    EXPECT_EQ(2, count);
    EXPECT_EQ(ANSC_STATUS_SUCCESS, status);

    free(pConf);
    pConf = NULL;
}

//Test for CosaDmlMocaIfMeshTableGetTable - Success
TEST_F(CcspMoCATestFixture, CosaDmlMocaIfMeshTableGetTableSuccess)
{
    ULONG uIndex = 0; // Set the desired interface index
    ULONG count = 0;
    PCOSA_DML_MOCA_MESH pConf = NULL;

    EXPECT_CALL(*g_mocaHALMock, moca_GetFullMeshRates(_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_anscMemoryMock, AnscFreeMemory(_)).Times(1);
    ANSC_STATUS status = CosaDmlMocaIfMeshTableGetTable(NULL, uIndex, &pConf, &count);

    EXPECT_EQ(ANSC_STATUS_SUCCESS, status);

    free(pConf);
    pConf = NULL;
}

//Test for CosaDmlMocaIfMeshTableGetTable - Failure
TEST_F(CcspMoCATestFixture, CosaDmlMocaIfMeshTableGetTableFailure)
{
    ULONG uIndex = 0; // Set the desired interface index
    ULONG count = 0;
    PCOSA_DML_MOCA_MESH pConf = NULL;

    EXPECT_CALL(*g_mocaHALMock, moca_GetFullMeshRates(_,_,_)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(*g_anscMemoryMock, AnscFreeMemory(_)).Times(1);

    ANSC_STATUS status = CosaDmlMocaIfMeshTableGetTable(NULL, uIndex, &pConf, &count);

    EXPECT_EQ(ANSC_STATUS_FAILURE, status);
}

//Test for CosaDmlMocaIfFlowTableGetTable - Success
TEST_F(CcspMoCATestFixture, CosaDmlMocaIfFlowTableGetTableSuccess)
{
    ULONG uIndex = 0; // Set the desired interface index
    ULONG count = 0;
    PCOSA_DML_MOCA_FLOW pConf = NULL;

    EXPECT_CALL(*g_mocaHALMock, moca_GetFlowStatistics(_,_,_)).Times(1).WillOnce(Return(0));

    ANSC_STATUS status = CosaDmlMocaIfFlowTableGetTable(NULL, uIndex, &pConf, &count);

    EXPECT_EQ(ANSC_STATUS_SUCCESS, status);

    free(pConf);
    pConf = NULL;
}

//Test for CosaDmlMocaIfFlowTableGetTable - Failure
TEST_F(CcspMoCATestFixture, CosaDmlMocaIfFlowTableGetTableFailure)
{
    ULONG uIndex = 0; // Set the desired interface index
    ULONG count = 0;
    PCOSA_DML_MOCA_FLOW pConf = NULL;

    EXPECT_CALL(*g_mocaHALMock, moca_GetFlowStatistics(_,_,_)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(*g_anscMemoryMock, AnscFreeMemory(_)).Times(1);

    ANSC_STATUS status = CosaDmlMocaIfFlowTableGetTable(NULL, uIndex, &pConf, &count);

    EXPECT_EQ(ANSC_STATUS_FAILURE, status);

    free(pConf);
    pConf = NULL;
}

//Test for CosaDmlMocaGetResetCount
TEST_F(CcspMoCATestFixture, CosaDmlMocaGetResetCount)
{
    ULONG count = 0;

    EXPECT_CALL(*g_mocaHALMock, moca_GetResetCount(_)).Times(1).WillOnce(Return(0));

    ANSC_STATUS status = CosaDmlMocaGetResetCount(NULL, &count);

    EXPECT_EQ(ANSC_STATUS_SUCCESS, status);
}

//Test for CosaDmlMocaGetLogStatus
TEST_F(CcspMoCATestFixture, CosaDmlMocaGetLogStatus)
{
    char pValueTrue[] = "true";
    char pValueNum[] = "10";

    ANSC_HANDLE g_TestMoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));
    
    PCOSA_DATAMODEL_MOCA        pTestObject   = (PCOSA_DATAMODEL_MOCA    )g_TestMoCAObject;
    PCOSA_DML_MOCA_LOG_STATUS   status        = &pTestObject->LogStatus;

    EXPECT_CALL(*g_syscfgMock, syscfg_get(_,_,_,_))
        .Times(2)
        .WillOnce(DoAll(SetArrayArgument<2>(pValueTrue, pValueTrue + sizeof(pValueTrue)), Return(0)))
        .WillOnce(DoAll(SetArrayArgument<2>(pValueNum, pValueNum + sizeof(pValueNum)), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(1).WillOnce(Return(0));

    ANSC_STATUS ret = CosaDmlMocaGetLogStatus(status);

    EXPECT_EQ(ANSC_STATUS_SUCCESS, ret);

    free(g_TestMoCAObject);
    g_TestMoCAObject = NULL;
}

//Test for is_usg_in_bridge_mode - bridge-static Success
TEST_F(CcspMoCATestFixture, isUsgInBridgeModeBridgeStaticSuccess)
{
    BOOL bridgeId = FALSE;

    EXPECT_CALL(*g_dslhDmagntExportedMock, COSAGetParamValueByPathName(_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).Times(1).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));

    ANSC_STATUS status = is_usg_in_bridge_mode(&bridgeId);

    EXPECT_EQ(TRUE, bridgeId);
    EXPECT_EQ(ANSC_STATUS_SUCCESS, status);
}

//Test for is_usg_in_bridge_mode - full-bridge-static True Success
TEST_F(CcspMoCATestFixture, isUsgInBridgeModeFullBridgeStaticTrueSuccess)
{
    BOOL bridgeId = FALSE;

    EXPECT_CALL(*g_dslhDmagntExportedMock, COSAGetParamValueByPathName(_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
        .Times(2)
        .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
        .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));

    ANSC_STATUS status = is_usg_in_bridge_mode(&bridgeId);

    EXPECT_EQ(TRUE, bridgeId);
    EXPECT_EQ(ANSC_STATUS_SUCCESS, status);
}

//Test for is_usg_in_bridge_mode - full-bridge-static False Success
TEST_F(CcspMoCATestFixture, isUsgInBridgeModeFullBridgeStaticFalseSuccess)
{
    BOOL bridgeId = FALSE;

    EXPECT_CALL(*g_dslhDmagntExportedMock, COSAGetParamValueByPathName(_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
        .Times(2)
        .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
        .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)));

    ANSC_STATUS status = is_usg_in_bridge_mode(&bridgeId);

    EXPECT_EQ(FALSE, bridgeId);
    EXPECT_EQ(ANSC_STATUS_SUCCESS, status);
}

//Test for is_usg_in_bridge_mode - bridge-static Success
TEST_F(CcspMoCATestFixture, isUsgInBridgeModeFailure)
{
    BOOL bridgeId = FALSE;

    EXPECT_CALL(*g_dslhDmagntExportedMock, COSAGetParamValueByPathName(_,_,_)).Times(1).WillOnce(Return(1));

    ANSC_STATUS status = is_usg_in_bridge_mode(&bridgeId);

    EXPECT_EQ(ANSC_STATUS_FAILURE, status);
}

//Test for CosaMoCAGetForceEnable - Success
TEST_F(CcspMoCATestFixture, CosaMoCAGetForceEnableSuccess)
{
    char pValueTrue[] = "true";

    ANSC_HANDLE g_TestMoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));
    
    PCOSA_DATAMODEL_MOCA        pTestObject   = (PCOSA_DATAMODEL_MOCA    )g_TestMoCAObject;
	PCOSA_DML_MOCA_CFG          mCfg          = &pTestObject->MoCACfg;

    EXPECT_CALL(*g_syscfgMock, syscfg_get(_,_,_,_)).Times(1).WillOnce(DoAll(SetArrayArgument<2>(pValueTrue, pValueTrue + sizeof(pValueTrue)), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).Times(1).WillOnce(DoAll(SetArgPointee<3>(1), Return(0)));

    ANSC_STATUS status = CosaMoCAGetForceEnable(mCfg);

    EXPECT_EQ(ANSC_STATUS_SUCCESS, status);

    free(g_TestMoCAObject);
    g_TestMoCAObject = NULL;
}

//Test for CosaMoCAGetForceEnable - Failure
TEST_F(CcspMoCATestFixture, CosaMoCAGetForceEnableFailure)
{
    char pValueTrue[] = "true";

    ANSC_HANDLE g_TestMoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));
    
    PCOSA_DATAMODEL_MOCA        pTestObject   = (PCOSA_DATAMODEL_MOCA    )g_TestMoCAObject;
	PCOSA_DML_MOCA_CFG          mCfg          = &pTestObject->MoCACfg;

    EXPECT_CALL(*g_syscfgMock, syscfg_get(_,_,_,_)).Times(1).WillOnce(Return(1));

    ANSC_STATUS status = CosaMoCAGetForceEnable(mCfg);

    EXPECT_EQ(ANSC_STATUS_FAILURE, status);

    free(g_TestMoCAObject);
    g_TestMoCAObject = NULL;
}

//Test for MoCA_SetForceEnable - Success
TEST_F(CcspMoCATestFixture, MoCASetForceEnableSuccess)
{
    ANSC_HANDLE g_TestMoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));
    
    PCOSA_DATAMODEL_MOCA        pTestObject   = (PCOSA_DATAMODEL_MOCA    )g_TestMoCAObject;
	PCOSA_DML_MOCA_CFG          mCfg          = &pTestObject->MoCACfg;
	PCOSA_DML_MOCA_IF_CFG		pCfg	      =	&pTestObject->MoCAIfFullTable[0].MoCAIfFull.Cfg;

    mCfg->bForceEnabled = 0;
    pCfg->InstanceNumber = 1;
    pCfg->X_CISCO_COM_Reset = TRUE;

    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_syscfgMock, syscfg_get(_,_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_safecLibMock, _memcpy_s_chk(_,_,_,_,_,_)).Times(3).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_mocaHALMock, moca_SetIfConfig(_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_syscfgMock, syscfg_set_nns_commit(_,_)).Times(12).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_mocaHALMock, moca_FreqMaskToValue(_)).Times(1).WillOnce(Return(0));

    BOOL status = MoCA_SetForceEnable(pCfg, mCfg, 1);

    EXPECT_EQ(TRUE, status);

    free(g_TestMoCAObject);
    g_TestMoCAObject = NULL;
}

//Test for MoCA_SetForceEnable - Failure
TEST_F(CcspMoCATestFixture, MoCASetForceEnableFailure)
{
    ANSC_HANDLE g_TestMoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));
    
    PCOSA_DATAMODEL_MOCA        pTestObject   = (PCOSA_DATAMODEL_MOCA    )g_TestMoCAObject;
	PCOSA_DML_MOCA_CFG          mCfg          = &pTestObject->MoCACfg;
	PCOSA_DML_MOCA_IF_CFG		pCfg	      =	&pTestObject->MoCAIfFullTable[0].MoCAIfFull.Cfg;

    mCfg->bForceEnabled = 0;

    EXPECT_CALL(*g_syscfgMock, syscfg_set_nns_commit(_,_)).Times(1).WillRepeatedly(Return(1));

    BOOL status = MoCA_SetForceEnable(pCfg, mCfg, 1);

    EXPECT_EQ(FALSE, status);

    free(g_TestMoCAObject);
    g_TestMoCAObject = NULL;
}

//Test for CosaDmlmocaUnpack - Success
TEST_F(CcspMoCATestFixture, CosaDmlmocaUnpackSuccess)
{
    char * blobData = "moca";

    /*unsigned char hexArray[] = {
        0x81, 0xA4, 0x6D, 0x6F, 0x63, 0x61, 0x81, 0xA6,
        0x45, 0x6E, 0x61, 0x62, 0x6C, 0x65, 0xC3
    };*/
    EXPECT_CALL(*g_base64Mock, b64_get_decoded_buffer_size(_)).Times(1).WillOnce(Return(5));
    EXPECT_CALL(*g_base64Mock, b64_decode(_,_,_)).Times(1).WillOnce(Return(5));
    EXPECT_CALL(*g_msgpackMock, msgpack_zone_init(_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_msgpackMock, msgpack_unpack(_,_,_,_,_)).Times(1).WillOnce(Return(MSGPACK_UNPACK_SUCCESS));
    EXPECT_CALL(*g_msgpackMock, msgpack_zone_destroy(_)).Times(1);
    EXPECT_CALL(*g_msgpackMock, msgpack_unpack_next(_,_,_,_)).Times(1).WillOnce(Return(MSGPACK_UNPACK_SUCCESS));
    EXPECT_CALL(*g_msgpackMock, msgpack_object_print(_,_)).Times(1);

    BOOL status = CosaDmlmocaUnpack(blobData);

    EXPECT_EQ(TRUE, status);
}