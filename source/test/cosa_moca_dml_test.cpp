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
    #include "cosa_moca_network_info.h"
    #include "dml_tr181_custom_cfg.h"
    #include "cosa_moca_apis.h"
}

using namespace testing;

extern MocaHalMock *g_mocaHALMock;
extern SyscfgMock *g_syscfgMock;
extern SafecLibMock* g_safecLibMock;
extern DslhDmagntExportedMock* g_dslhDmagntExportedMock;
extern base64Mock *g_base64Mock;
extern msgpackMock *g_msgpackMock;
extern UserTimeMock *g_usertimeMock;
extern AnscMemoryMock * g_anscMemoryMock;

// Test for MoCA_GetParamBoolValue - Success
TEST_F(CcspMoCATestFixture, MoCA_GetParamBoolValueSuccess)
{
    BOOL bValue = FALSE;
    char paramName1[64] = "X_RDKCENTRAL-COM_MoCAHost_Sync";
    char paramName2[64] = "X_RDKCENTRAL-COM_ForceEnable";

    extern ANSC_HANDLE g_MoCAObject;
    g_MoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));
    
    PCOSA_DATAMODEL_MOCA        pTestObject   = (PCOSA_DATAMODEL_MOCA    )g_MoCAObject;
	PCOSA_DML_MOCA_CFG          mCfg          = &pTestObject->MoCACfg;

    mCfg->bForceEnabled = TRUE;

//For X_RDKCENTRAL-COM_MoCAHost_Sync Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(MoCA_GetParamBoolValue(NULL, paramName1, &bValue), TRUE);
    EXPECT_EQ(bValue, FALSE);

//For X_RDKCENTRAL-COM_ForceEnable Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).Times(2).WillOnce(DoAll(SetArgPointee<3>(1), Return(0))).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(MoCA_GetParamBoolValue(NULL, paramName2, &bValue), TRUE);
    EXPECT_EQ(bValue, TRUE);

    free(g_MoCAObject);
    g_MoCAObject = NULL;
}

//Test for MoCA_GetParamBoolValue - Failure
TEST_F(CcspMoCATestFixture, MoCA_GetParamBoolValueFailure)
{
    BOOL bValue = FALSE;
    char paramName[64] = "stubValue";

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).Times(2).WillOnce(DoAll(SetArgPointee<3>(1), Return(0))).WillOnce(DoAll(SetArgPointee<3>(1), Return(0)));
    EXPECT_EQ(MoCA_GetParamBoolValue(NULL, paramName, &bValue), FALSE);
}

//Test for MoCA_GetParamIntValue
TEST_F(CcspMoCATestFixture, MoCA_GetParamIntValue)
{
    int iValue = 0;
    char paramName[16] = "stubValue";

    EXPECT_EQ(MoCA_GetParamIntValue(NULL, paramName, &iValue), FALSE);
}

//Test for MoCA_GetParamUlongValue - Success
TEST_F(CcspMoCATestFixture, MoCA_GetParamUlongValueSuccess)
{
    ULONG ulValue = 0;
    char paramName1[64] = "X_CISCO_COM_ProvisioningServerAddressType";
    char paramName2[16] = "MocaResetCount";

    extern ANSC_HANDLE g_MoCAObject;
    g_MoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));
    
    PCOSA_DATAMODEL_MOCA        pTestObject   = (PCOSA_DATAMODEL_MOCA    )g_MoCAObject;
	PCOSA_DML_MOCA_CFG          mCfg          = &pTestObject->MoCACfg;

    mCfg->X_CISCO_COM_ProvisioningServerAddressType = MOCA_PROVISIONING_SERVERADDRESS_TYPE(1);

//For X_CISCO_COM_ProvisioningServerAddressType Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(MoCA_GetParamUlongValue(NULL, paramName1, &ulValue), TRUE);
    EXPECT_EQ(ulValue, MOCA_PROVISIONING_SERVERADDRESS_TYPE(1));

//For MocaResetCount Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).Times(2).WillOnce(DoAll(SetArgPointee<3>(1), Return(0))).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_mocaHALMock, moca_GetResetCount(_)).Times(1).WillOnce(Return(0));
    EXPECT_EQ(MoCA_GetParamUlongValue(NULL, paramName2, &ulValue), TRUE);

    free(g_MoCAObject);
    g_MoCAObject = NULL;
}

//Test for MoCA_GetParamUlongValue - Failure
TEST_F(CcspMoCATestFixture, MoCA_GetParamUlongValueFailure)
{
    ULONG ulValue = 0;
    char paramName[16] = "stubValue";

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).Times(2).WillOnce(DoAll(SetArgPointee<3>(1), Return(0))).WillOnce(DoAll(SetArgPointee<3>(1), Return(0)));
    EXPECT_EQ(MoCA_GetParamUlongValue(NULL, paramName, &ulValue), FALSE);
}

//Test for MoCA_GetParamStringValue - Success
TEST_F(CcspMoCATestFixture, MoCA_GetParamStringValueSuccess)
{
    char pValue[64] = {0};
    char paramName1[64] = "X_CISCO_COM_ProvisioningFilename";
    char paramName2[64] = "X_CISCO_COM_ProvisioningServerAddress";
    char paramName3[8] = "Data";
    ULONG ulValue = 0;

    extern ANSC_HANDLE g_MoCAObject;
    g_MoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));
    
    PCOSA_DATAMODEL_MOCA        pTestObject   = (PCOSA_DATAMODEL_MOCA    )g_MoCAObject;
	PCOSA_DML_MOCA_CFG          mCfg          = &pTestObject->MoCACfg;

    strcpy((char*)mCfg->X_CISCO_COM_ProvisioningFilename, "stubValue");
    strcpy((char*)mCfg->X_CISCO_COM_ProvisioningServerAddress, "stubAddressValue");

//For X_CISCO_COM_ProvisioningFilename Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).WillOnce(Return(0));
    EXPECT_EQ(MoCA_GetParamStringValue(NULL, paramName1, pValue, &ulValue), 0);

//For X_CISCO_COM_ProvisioningServerAddress Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).Times(2).WillOnce(DoAll(SetArgPointee<3>(1), Return(0))).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).WillOnce(Return(0));
    EXPECT_EQ(MoCA_GetParamStringValue(NULL, paramName2, pValue, &ulValue), 0);

//For Data Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).Times(3).WillOnce(DoAll(SetArgPointee<3>(1), Return(0))).WillOnce(DoAll(SetArgPointee<3>(1), Return(0))).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(MoCA_GetParamStringValue(NULL, paramName3, pValue, &ulValue), 0);

    free(g_MoCAObject);
    g_MoCAObject = NULL;
}

//Test for MoCA_GetParamStringValue - Failure
TEST_F(CcspMoCATestFixture, MoCA_GetParamStringValueFailure)
{
    char pValue[64] = {0};
    char paramName1[64] = "X_CISCO_COM_ProvisioningFilename";
    char paramName2[64] = "X_CISCO_COM_ProvisioningServerAddress";
    char paramName3[8] = "Dummy";
    ULONG ulValue = 0;

    extern ANSC_HANDLE g_MoCAObject;
    g_MoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));
    
    PCOSA_DATAMODEL_MOCA        pTestObject   = (PCOSA_DATAMODEL_MOCA    )g_MoCAObject;
	PCOSA_DML_MOCA_CFG          mCfg          = &pTestObject->MoCACfg;

    strcpy((char*)mCfg->X_CISCO_COM_ProvisioningFilename, "stubValue");
    strcpy((char*)mCfg->X_CISCO_COM_ProvisioningServerAddress, "stubAddressValue");

//For X_CISCO_COM_ProvisioningFilename Failure
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).WillOnce(Return(1));
    EXPECT_NE(MoCA_GetParamStringValue(NULL, paramName1, pValue, &ulValue), 0);

//For X_CISCO_COM_ProvisioningServerAddress Failure
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).Times(2).WillOnce(DoAll(SetArgPointee<3>(1), Return(0))).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).WillOnce(Return(1));
    EXPECT_NE(MoCA_GetParamStringValue(NULL, paramName2, pValue, &ulValue), 0);

//For Unsupported Parameter Failure
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).Times(3).WillOnce(DoAll(SetArgPointee<3>(1), Return(0))).WillOnce(DoAll(SetArgPointee<3>(1), Return(0))).WillOnce(DoAll(SetArgPointee<3>(1), Return(0)));
    EXPECT_NE(MoCA_GetParamStringValue(NULL, paramName3, pValue, &ulValue), 0);

    free(g_MoCAObject);
    g_MoCAObject = NULL;
}

//Test for MoCA_SetParamBoolValue - Success
TEST_F(CcspMoCATestFixture, MoCA_SetParamBoolValueSuccess)
{
    char paramName1[64] = "X_RDKCENTRAL-COM_MoCAHost_Sync";
    char paramName2[64] = "X_RDKCENTRAL-COM_ForceEnable";

    extern ANSC_HANDLE g_MoCAObject;
    g_MoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));
    
    PCOSA_DATAMODEL_MOCA        pTestObject   = (PCOSA_DATAMODEL_MOCA    )g_MoCAObject;
    PCOSA_DML_MOCA_CFG          mCfg          = &pTestObject->MoCACfg;
	PCOSA_DML_MOCA_IF_CFG		pCfg	      =	&pTestObject->MoCAIfFullTable[0].MoCAIfFull.Cfg;

    mCfg->bForceEnabled = 0;
    pCfg->InstanceNumber = 1;
    pCfg->X_CISCO_COM_Reset = TRUE;

//For X_RDKCENTRAL-COM_MoCAHost_Sync Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(MoCA_SetParamBoolValue(NULL, paramName1, TRUE), 1);

//For X_RDKCENTRAL-COM_ForceEnable Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).Times(2).WillOnce(DoAll(SetArgPointee<3>(1), Return(0))).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_syscfgMock, syscfg_get(_,_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_safecLibMock, _memcpy_s_chk(_,_,_,_,_,_)).Times(3).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_mocaHALMock, moca_SetIfConfig(_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_syscfgMock, syscfg_set_nns_commit(_,_)).Times(12).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_mocaHALMock, moca_FreqMaskToValue(_)).Times(1).WillOnce(Return(0));
    EXPECT_EQ(MoCA_SetParamBoolValue(NULL, paramName2, TRUE), 1);

    free(g_MoCAObject);
    g_MoCAObject = NULL;
}

//Test for MoCA_SetParamBoolValue - Failure
TEST_F(CcspMoCATestFixture, MoCA_SetParamBoolValueFailure)
{
    char paramName[8] = "Dummy";

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).Times(2).WillOnce(DoAll(SetArgPointee<3>(1), Return(0))).WillOnce(DoAll(SetArgPointee<3>(1), Return(0)));
    EXPECT_EQ(MoCA_SetParamBoolValue(NULL, paramName, TRUE), 0);
}

//Test for MoCA_SetParamIntValue - Success
TEST_F(CcspMoCATestFixture, MoCA_SetParamIntValueSuccess)
{
    char paramName[8] = "Dummy";

    EXPECT_EQ(MoCA_SetParamIntValue(NULL, paramName, 1), 0);
}

//Test for MoCA_SetParamUlongValue - Success
TEST_F(CcspMoCATestFixture, MoCA_SetParamUlongValueSuccess)
{
    char paramName[64] = "X_CISCO_COM_ProvisioningServerAddressType";

    extern ANSC_HANDLE g_MoCAObject;
    g_MoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));
    
    PCOSA_DATAMODEL_MOCA        pTestObject   = (PCOSA_DATAMODEL_MOCA    )g_MoCAObject;
    PCOSA_DML_MOCA_CFG          mCfg          = &pTestObject->MoCACfg;

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(MoCA_SetParamUlongValue(NULL, paramName, 1), 1);

    free(g_MoCAObject);
    g_MoCAObject = NULL;
}

//Test for MoCA_SetParamUlongValue - Failure
TEST_F(CcspMoCATestFixture, MoCA_SetParamUlongValueFailure)
{
    char paramName[8] = "Dummy";

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).WillOnce(DoAll(SetArgPointee<3>(1), Return(0)));
    EXPECT_EQ(MoCA_SetParamUlongValue(NULL, paramName, 1), 0);
}

//Test for MoCA_SetParamStringValue - Success
TEST_F(CcspMoCATestFixture, MoCA_SetParamStringValueSuccess)
{
    char paramName1[64] = "X_CISCO_COM_ProvisioningFilename";
    char paramName2[64] = "X_CISCO_COM_ProvisioningServerAddress";
    char paramName3[8] = "Data";
    char pValue[64] = "stubValue";

    extern ANSC_HANDLE g_MoCAObject;
    g_MoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));
    
    PCOSA_DATAMODEL_MOCA        pTestObject   = (PCOSA_DATAMODEL_MOCA    )g_MoCAObject;
    PCOSA_DML_MOCA_CFG          mCfg          = &pTestObject->MoCACfg;

//For X_CISCO_COM_ProvisioningFilename Set Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).WillOnce(Return(0));
    EXPECT_EQ(MoCA_SetParamStringValue(NULL, paramName1, pValue), 1);

//For X_CISCO_COM_ProvisioningServerAddress Set Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).Times(2).WillOnce(DoAll(SetArgPointee<3>(1), Return(0))).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).WillOnce(Return(0));
    EXPECT_EQ(MoCA_SetParamStringValue(NULL, paramName2, pValue), 1);

//For Data Set Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).Times(3).WillOnce(DoAll(SetArgPointee<3>(1), Return(0))).WillOnce(DoAll(SetArgPointee<3>(1), Return(0))).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_base64Mock, b64_get_decoded_buffer_size(_)).Times(1).WillOnce(Return(5));
    EXPECT_CALL(*g_base64Mock, b64_decode(_,_,_)).Times(1).WillOnce(Return(5));
    EXPECT_CALL(*g_msgpackMock, msgpack_zone_init(_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_msgpackMock, msgpack_unpack(_,_,_,_,_)).Times(1).WillOnce(Return(MSGPACK_UNPACK_SUCCESS));
    EXPECT_CALL(*g_msgpackMock, msgpack_zone_destroy(_)).Times(1);
    EXPECT_CALL(*g_msgpackMock, msgpack_unpack_next(_,_,_,_)).Times(1).WillOnce(Return(MSGPACK_UNPACK_SUCCESS));
    EXPECT_CALL(*g_msgpackMock, msgpack_object_print(_,_)).Times(1);
    EXPECT_EQ(MoCA_SetParamStringValue(NULL, paramName3, pValue), 1);

    free(g_MoCAObject);
    g_MoCAObject = NULL;
}

//Test for MoCA_SetParamStringValue - Failure
TEST_F(CcspMoCATestFixture, MoCA_SetParamStringValueFailure)
{
    char paramName1[64] = "X_CISCO_COM_ProvisioningFilename";
    char paramName2[64] = "X_CISCO_COM_ProvisioningServerAddress";
    char paramName3[8] = "Dummy";
    char pValue[64] = "stubValue";

    extern ANSC_HANDLE g_MoCAObject;
    g_MoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));
    
    PCOSA_DATAMODEL_MOCA        pTestObject   = (PCOSA_DATAMODEL_MOCA    )g_MoCAObject;
    PCOSA_DML_MOCA_CFG          mCfg          = &pTestObject->MoCACfg;

//For X_CISCO_COM_ProvisioningFilename Set Failure
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).WillOnce(Return(1));
    EXPECT_EQ(MoCA_SetParamStringValue(NULL, paramName1, pValue), 0);

//For X_CISCO_COM_ProvisioningServerAddress Set Failure
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).Times(2).WillOnce(DoAll(SetArgPointee<3>(1), Return(0))).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).WillOnce(Return(1));
    EXPECT_EQ(MoCA_SetParamStringValue(NULL, paramName2, pValue), 0);

//For Unsupported Parameter Set Failure
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).Times(3).WillOnce(DoAll(SetArgPointee<3>(1), Return(0))).WillOnce(DoAll(SetArgPointee<3>(1), Return(0))).WillOnce(DoAll(SetArgPointee<3>(1), Return(0)));
    EXPECT_EQ(MoCA_SetParamStringValue(NULL, paramName3, pValue), 0);

    free(g_MoCAObject);
    g_MoCAObject = NULL;
}

//Test for MoCA_Validate - Success
TEST_F(CcspMoCATestFixture, MoCA_ValidateSuccess)
{
    EXPECT_EQ(MoCA_Validate(NULL, NULL, NULL), 1);
}

//Test for MoCA_Commit - Success
TEST_F(CcspMoCATestFixture, MoCA_CommitSuccess)
{
    extern ANSC_HANDLE g_MoCAObject;
    g_MoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));

    EXPECT_EQ(MoCA_Commit(NULL), 0);

    free(g_MoCAObject);
    g_MoCAObject = NULL;
}

//Test for MoCA_Rollback - Success
TEST_F(CcspMoCATestFixture, MoCA_RollbackSuccess)
{
    extern ANSC_HANDLE g_MoCAObject;
    g_MoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));

    EXPECT_EQ(MoCA_Rollback(NULL), 0);

    free(g_MoCAObject);
    g_MoCAObject = NULL;
}

//Test for MoCAReset_GetParamBoolValue - Success
TEST_F(CcspMoCATestFixture, MoCAReset_GetParamBoolValueSuccess)
{
    BOOL bValue = FALSE;
    char paramName[8] = "Reset";

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(MoCAReset_GetParamBoolValue(NULL, paramName, &bValue), TRUE);
    EXPECT_EQ(bValue, FALSE);
}

//Test for MoCAReset_GetParamBoolValue - Failure
TEST_F(CcspMoCATestFixture, MoCAReset_GetParamBoolValueFailure)
{
    BOOL bValue = FALSE;
    char paramName[8] = "Dummy";

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).WillOnce(DoAll(SetArgPointee<3>(1), Return(0)));
    EXPECT_EQ(MoCAReset_GetParamBoolValue(NULL, paramName, &bValue), FALSE);
}

//Test for MoCAReset_SetParamBoolValue - Success
TEST_F(CcspMoCATestFixture, MoCAReset_SetParamBoolValueSuccess)
{
    char paramName[8] = "Reset";
    extern ANSC_HANDLE g_MoCAObject;
    g_MoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(MoCAReset_SetParamBoolValue(NULL, paramName, TRUE), 1);

//Sleep is added to avoid crash due to free of g_MoCAObject when thread is still running
    sleep(3);
    free(g_MoCAObject);
    g_MoCAObject = NULL;
}

//Test for MoCAReset_SetParamBoolValue - Failure
TEST_F(CcspMoCATestFixture, MoCAReset_SetParamBoolValueFailure)
{
    char paramName[8] = "Dummy";

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).WillOnce(DoAll(SetArgPointee<3>(1), Return(0)));
    EXPECT_EQ(MoCAReset_SetParamBoolValue(NULL, paramName, TRUE), 0);
}

//Test for Interface1_GetEntryCount - Success
TEST_F(CcspMoCATestFixture, Interface1_GetEntryCountSuccess)
{
    ULONG ulCount = 0;
    EXPECT_EQ(Interface1_GetEntryCount(NULL), 1);
}

//Test for Interface1_GetEntry - Success
TEST_F(CcspMoCATestFixture, Interface1_GetEntrySuccess)
{
    ULONG ulIndex = 1;
    ULONG pEntry = 0;
    extern ANSC_HANDLE g_MoCAObject;
    g_MoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));

    PCOSA_DATAMODEL_MOCA        pTestObject   = (PCOSA_DATAMODEL_MOCA    )g_MoCAObject;
    pTestObject->MoCAIfFullTable[0].MoCAIfFull.Cfg.InstanceNumber = 1;

    EXPECT_NE(Interface1_GetEntry(NULL, ulIndex, &pEntry), nullptr);
}

//Test for Interface1_GetParamBoolValue - Success
TEST_F(CcspMoCATestFixture, Interface1_GetParamBoolValueSuccess)
{
    BOOL bValue = FALSE;
    char paramName1[64] = "Enable";
    char paramName2[64] = "Upstream";
    char paramName3[64] = "MaxNodes";
    char paramName4[64] = "PreferredNC";
    char paramName5[64] = "PrivacyEnabledSetting";
    char paramName6[64] = "PrivacyEnabled";
    char paramName7[64] = "QAM256Capable";
    char paramName8[64] = "X_CISCO_COM_Reset";
    char paramName9[64] = "X_CISCO_COM_MixedMode";
    char paramName10[64] = "X_CISCO_COM_ChannelScanning";
    char paramName11[64] = "X_CISCO_COM_AutoPowerControlEnable";
    char paramName12[64] = "X_CISCO_COM_EnableTabooBit";
    char paramName13[64] = "X_CISCO_COM_CycleMaster";

    ANSC_HANDLE hInsContext = (ANSC_HANDLE)1;
    extern ANSC_HANDLE g_MoCAObject;
    g_MoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));

    PCOSA_DATAMODEL_MOCA        pTestObject   = (PCOSA_DATAMODEL_MOCA    )g_MoCAObject;
    PCOSA_DML_MOCA_IF_CFG       pCfg          = &pTestObject->MoCAIfFullTable[0].MoCAIfFull.Cfg;

    PCOSA_DML_MOCA_IF_FULL pMoCAIfFull = (PCOSA_DML_MOCA_IF_FULL)malloc(sizeof(COSA_DML_MOCA_IF_FULL));
    memset(pMoCAIfFull, 0, sizeof(COSA_DML_MOCA_IF_FULL));

    pMoCAIfFull->Cfg.InstanceNumber = 0;
    pMoCAIfFull->Cfg.bEnabled = TRUE;
    pMoCAIfFull->DynamicInfo.MaxNodes = TRUE;
    pMoCAIfFull->Cfg.bPreferredNC = TRUE;
    pMoCAIfFull->Cfg.PrivacyEnabledSetting = TRUE;
    pMoCAIfFull->DynamicInfo.PrivacyEnabled = TRUE;
    pMoCAIfFull->StaticInfo.QAM256Capable = TRUE;
    pMoCAIfFull->Cfg.X_CISCO_COM_Reset = TRUE;
    pMoCAIfFull->Cfg.X_CISCO_COM_MixedMode = TRUE;
    pMoCAIfFull->Cfg.X_CISCO_COM_ChannelScanning = TRUE;
    pMoCAIfFull->Cfg.X_CISCO_COM_AutoPowerControlEnable = TRUE;
    pMoCAIfFull->Cfg.X_CISCO_COM_EnableTabooBit = TRUE;
    pMoCAIfFull->StaticInfo.X_CISCO_COM_CycleMaster = TRUE;

//For Enable Success
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Interface1_GetParamBoolValue((ANSC_HANDLE)pMoCAIfFull, paramName1, &bValue), TRUE);
    EXPECT_EQ(bValue, TRUE);

//For Upstream Success
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(2)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Interface1_GetParamBoolValue((ANSC_HANDLE)pMoCAIfFull, paramName2, &bValue), TRUE);
    EXPECT_EQ(bValue, FALSE);

//For MaxNodes Success
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(3)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Interface1_GetParamBoolValue((ANSC_HANDLE)pMoCAIfFull, paramName3, &bValue), TRUE);
    EXPECT_EQ(bValue, TRUE);

//For PreferredNC Success
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(4)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Interface1_GetParamBoolValue((ANSC_HANDLE)pMoCAIfFull, paramName4, &bValue), TRUE);
    EXPECT_EQ(bValue, TRUE);

//For PrivacyEnabledSetting Success
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(5)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Interface1_GetParamBoolValue((ANSC_HANDLE)pMoCAIfFull, paramName5, &bValue), TRUE);
    EXPECT_EQ(bValue, TRUE);

//For PrivacyEnabled Success
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(6)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Interface1_GetParamBoolValue((ANSC_HANDLE)pMoCAIfFull, paramName6, &bValue), TRUE);
    EXPECT_EQ(bValue, TRUE);

//For QAM256Capable Success
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(7)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Interface1_GetParamBoolValue((ANSC_HANDLE)pMoCAIfFull, paramName7, &bValue), TRUE);
    EXPECT_EQ(bValue, TRUE);

//For X_CISCO_COM_Reset Success
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(8)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Interface1_GetParamBoolValue((ANSC_HANDLE)pMoCAIfFull, paramName8, &bValue), TRUE);
    EXPECT_EQ(bValue, TRUE);

//For X_CISCO_COM_MixedMode Success
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(9)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Interface1_GetParamBoolValue((ANSC_HANDLE)pMoCAIfFull, paramName9, &bValue), TRUE);
    EXPECT_EQ(bValue, TRUE);

//For X_CISCO_COM_ChannelScanning Success
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(10)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Interface1_GetParamBoolValue((ANSC_HANDLE)pMoCAIfFull, paramName10, &bValue), TRUE);
    EXPECT_EQ(bValue, TRUE);

//For X_CISCO_COM_AutoPowerControlEnable Success
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(11)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Interface1_GetParamBoolValue((ANSC_HANDLE)pMoCAIfFull, paramName11, &bValue), TRUE);
    EXPECT_EQ(bValue, TRUE);

//For X_CISCO_COM_EnableTabooBit Success
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(12)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Interface1_GetParamBoolValue((ANSC_HANDLE)pMoCAIfFull, paramName12, &bValue), TRUE);
    EXPECT_EQ(bValue, TRUE);

//For X_CISCO_COM_CycleMaster Success
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(13)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Interface1_GetParamBoolValue((ANSC_HANDLE)pMoCAIfFull, paramName13, &bValue), TRUE);
    EXPECT_EQ(bValue, TRUE);

    free(pMoCAIfFull);
    pMoCAIfFull = NULL;

    free(g_MoCAObject);
    g_MoCAObject = NULL;

}

//Test for Interface1_GetParamIntValue - Success
TEST_F(CcspMoCATestFixture, Interface1_GetParamIntValueSuccess)
{
    char paramName[64] = "TxPowerLimit";
    int pValue = 0;

    ANSC_HANDLE hInsContext = (ANSC_HANDLE)1;
    extern ANSC_HANDLE g_MoCAObject;
    g_MoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));

    PCOSA_DATAMODEL_MOCA        pTestObject   = (PCOSA_DATAMODEL_MOCA    )g_MoCAObject;
    PCOSA_DML_MOCA_IF_CFG       pCfg          = &pTestObject->MoCAIfFullTable[0].MoCAIfFull.Cfg;

    PCOSA_DML_MOCA_IF_FULL pMoCAIfFull = (PCOSA_DML_MOCA_IF_FULL)malloc(sizeof(COSA_DML_MOCA_IF_FULL));
    memset(pMoCAIfFull, 0, sizeof(COSA_DML_MOCA_IF_FULL));

    pMoCAIfFull->Cfg.TxPowerLimit = 100;

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Interface1_GetParamIntValue((ANSC_HANDLE)pMoCAIfFull, paramName, &pValue), TRUE);
}

//Test for Interface1_GetParamUlongValue - Success
TEST_F(CcspMoCATestFixture, Interface1_GetParamUlongValueSuccess)
{
    ULONG pValue = 0;
    char paramName1[64] = "Status";
    char paramName2[64] = "LastChange";
    char paramName3[64] = "MaxBitRate";
    char paramName4[64] = "MaxIngressBW";
    char paramName5[64] = "MaxEgressBW";
    char paramName6[64] = "NetworkCoordinator";
    char paramName7[64] = "NodeID";
    char paramName8[64] = "BackupNC";
    char paramName9[64] = "CurrentOperFreq";
    char paramName10[64] = "LastOperFreq";
    char paramName11[64] = "PowerCntlPhyTarget";
    char paramName12[64] = "BeaconPowerLimit";
    char paramName13[64] = "TxBcastRate";
    char paramName14[64] = "TxBcastPowerReduction";
    char paramName15[64] = "PacketAggregationCapability";
    char paramName16[64] = "AutoPowerControlPhyRate";
    char paramName17[64] = "X_CISCO_COM_BestNetworkCoordinatorID";
    char paramName18[64] = "X_CISCO_COM_NumberOfConnectedClients";
    char paramName19[64] = "TxPowerLimit";

    ANSC_HANDLE hInsContext = (ANSC_HANDLE)1;
    extern ANSC_HANDLE g_MoCAObject;
    g_MoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));

    PCOSA_DATAMODEL_MOCA        pTestObject   = (PCOSA_DATAMODEL_MOCA    )g_MoCAObject;
    PCOSA_DML_MOCA_IF_CFG       pCfg          = &pTestObject->MoCAIfFullTable[0].MoCAIfFull.Cfg;

    PCOSA_DML_MOCA_IF_FULL pMoCAIfFull = (PCOSA_DML_MOCA_IF_FULL)malloc(sizeof(COSA_DML_MOCA_IF_FULL));
    memset(pMoCAIfFull, 0, sizeof(COSA_DML_MOCA_IF_FULL));

    pMoCAIfFull->Cfg.InstanceNumber = 0;
    pMoCAIfFull->DynamicInfo.Status = COSA_DML_IF_STATUS_Up;
    pMoCAIfFull->DynamicInfo.LastChange = 1;
    pMoCAIfFull->StaticInfo.MaxBitRate = 1;
    pMoCAIfFull->DynamicInfo.MaxIngressBW = 1;
    pMoCAIfFull->DynamicInfo.MaxEgressBW = 1;
    pMoCAIfFull->DynamicInfo.NetworkCoordinator = 1;
    pMoCAIfFull->DynamicInfo.NodeID = 1;
    pMoCAIfFull->DynamicInfo.BackupNC = 1;
    pMoCAIfFull->DynamicInfo.CurrentOperFreq = 1;
    pMoCAIfFull->DynamicInfo.LastOperFreq = 1;
    pMoCAIfFull->Cfg.PowerCntlPhyTarget = 1;
    pMoCAIfFull->Cfg.BeaconPowerLimit = 1;
    pMoCAIfFull->DynamicInfo.TxBcastRate = 1;
    pMoCAIfFull->StaticInfo.TxBcastPowerReduction = 1;
    pMoCAIfFull->StaticInfo.PacketAggregationCapability = 1;
    pMoCAIfFull->Cfg.AutoPowerControlPhyRate = 1;
    pMoCAIfFull->DynamicInfo.X_CISCO_COM_BestNetworkCoordinatorID = 1;
    pMoCAIfFull->DynamicInfo.X_CISCO_COM_NumberOfConnectedClients = 1;
    pMoCAIfFull->Cfg.TxPowerLimit = 1;

//For Status Success
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Interface1_GetParamUlongValue((ANSC_HANDLE)pMoCAIfFull, paramName1, &pValue), TRUE);
    EXPECT_EQ(pValue, 1);

//For LastChange Success
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(2)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Interface1_GetParamUlongValue((ANSC_HANDLE)pMoCAIfFull, paramName2, &pValue), TRUE);
    EXPECT_EQ(pValue, 1);

//For MaxBitRate Success
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(3)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Interface1_GetParamUlongValue((ANSC_HANDLE)pMoCAIfFull, paramName3, &pValue), TRUE);
    EXPECT_EQ(pValue, 1);

//For MaxIngressBW Success
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(4)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Interface1_GetParamUlongValue((ANSC_HANDLE)pMoCAIfFull, paramName4, &pValue), TRUE);
    EXPECT_EQ(pValue, 1);

//For MaxEgressBW Success
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(5)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Interface1_GetParamUlongValue((ANSC_HANDLE)pMoCAIfFull, paramName5, &pValue), TRUE);
    EXPECT_EQ(pValue, 1);

//For NetworkCoordinator Success
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(6)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Interface1_GetParamUlongValue((ANSC_HANDLE)pMoCAIfFull, paramName6, &pValue), TRUE);
    EXPECT_EQ(pValue, 1);

//For NodeID Success
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(7)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Interface1_GetParamUlongValue((ANSC_HANDLE)pMoCAIfFull, paramName7, &pValue), TRUE);
    EXPECT_EQ(pValue, 1);

//For BackupNC Success
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(8)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Interface1_GetParamUlongValue((ANSC_HANDLE)pMoCAIfFull, paramName8, &pValue), TRUE);
    EXPECT_EQ(pValue, 1);

//For CurrentOperFreq Success
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(9)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Interface1_GetParamUlongValue((ANSC_HANDLE)pMoCAIfFull, paramName9, &pValue), TRUE);
    EXPECT_EQ(pValue, 1);

//For LastOperFreq Success
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(10)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Interface1_GetParamUlongValue((ANSC_HANDLE)pMoCAIfFull, paramName10, &pValue), TRUE);
    EXPECT_EQ(pValue, 1);

//For PowerCntlPhyTarget Success
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(11)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Interface1_GetParamUlongValue((ANSC_HANDLE)pMoCAIfFull, paramName11, &pValue), TRUE);
    EXPECT_EQ(pValue, 1);

//For BeaconPowerLimit Success
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(12)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Interface1_GetParamUlongValue((ANSC_HANDLE)pMoCAIfFull, paramName12, &pValue), TRUE);
    EXPECT_EQ(pValue, 1);

//For TxBcastRate Success
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(13)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Interface1_GetParamUlongValue((ANSC_HANDLE)pMoCAIfFull, paramName13, &pValue), TRUE);
    EXPECT_EQ(pValue, 1);

//For TxBcastPowerReduction Success
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(14)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Interface1_GetParamUlongValue((ANSC_HANDLE)pMoCAIfFull, paramName14, &pValue), TRUE);
    EXPECT_EQ(pValue, 1);

//For PacketAggregationCapability Success
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(15)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Interface1_GetParamUlongValue((ANSC_HANDLE)pMoCAIfFull, paramName15, &pValue), TRUE);
    EXPECT_EQ(pValue, 1);

//For AutoPowerControlPhyRate Success
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(16)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Interface1_GetParamUlongValue((ANSC_HANDLE)pMoCAIfFull, paramName16, &pValue), TRUE);
    EXPECT_EQ(pValue, 1);

//For X_CISCO_COM_BestNetworkCoordinatorID Success
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(17)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Interface1_GetParamUlongValue((ANSC_HANDLE)pMoCAIfFull, paramName17, &pValue), TRUE);
    EXPECT_EQ(pValue, 1);

//For X_CISCO_COM_NumberOfConnectedClients Success
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(18)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Interface1_GetParamUlongValue((ANSC_HANDLE)pMoCAIfFull, paramName18, &pValue), TRUE);
    EXPECT_EQ(pValue, 1);;

//For TxPowerLimit Success
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(19)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Interface1_GetParamUlongValue((ANSC_HANDLE)pMoCAIfFull, paramName19, &pValue), TRUE);
    EXPECT_EQ(pValue, 1);

    free(pMoCAIfFull);
    pMoCAIfFull = NULL;

    free(g_MoCAObject);
    g_MoCAObject = NULL;
}

//Test for Interface1_GetParamStringValue - Success
TEST_F(CcspMoCATestFixture, Interface1_GetParamStringValueSuccess)
{
    char pValue[64] = {0};
    ULONG pSize = 0;

    char paramName1[64] = "Alias";
    char paramName2[64] = "Name";
    char paramName3[64] = "LowerLayers";
    char paramName4[64] = "MACAddress";
    char paramName5[64] = "FirmwareVersion";
    char paramName6[64] = "HighestVersion";
    char paramName7[64] = "CurrentVersion";
    char paramName8[64] = "FreqCapabilityMask";
    char paramName9[64] = "FreqCurrentMaskSetting";
    char paramName10[64] = "X_CISCO_COM_ChannelScanMask";
    char paramName11[64] = "FreqCurrentMask";
    char paramName12[64] = "KeyPassphrase";
    char paramName13[64] = "NetworkTabooMask";
    char paramName14[64] = "NodeTabooMask";
    char paramName15[64] = "X_CISCO_NetworkCoordinatorMACAddress";

    ANSC_HANDLE hInsContext = (ANSC_HANDLE)1;
    extern ANSC_HANDLE g_MoCAObject;
    g_MoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));

    PCOSA_DATAMODEL_MOCA        pTestObject   = (PCOSA_DATAMODEL_MOCA    )g_MoCAObject;
    PCOSA_DML_MOCA_IF_CFG       pCfg          = &pTestObject->MoCAIfFullTable[0].MoCAIfFull.Cfg;

    PCOSA_DML_MOCA_IF_FULL pMoCAIfFull = (PCOSA_DML_MOCA_IF_FULL)malloc(sizeof(COSA_DML_MOCA_IF_FULL));
    memset(pMoCAIfFull, 0, sizeof(COSA_DML_MOCA_IF_FULL));

    strcpy(pMoCAIfFull->Cfg.Alias, "Test");
    strcpy(pMoCAIfFull->StaticInfo.Name, "Test");
    strcpy((char*)pMoCAIfFull->StaticInfo.MacAddress, "123456789ABC");
    strcpy(pMoCAIfFull->StaticInfo.FirmwareVersion, "Test");
    strcpy(pMoCAIfFull->StaticInfo.HighestVersion, "Test");
    strcpy(pMoCAIfFull->DynamicInfo.CurrentVersion, "Test");
    strcpy((char*)pMoCAIfFull->StaticInfo.FreqCapabilityMask, "1234567");
    strcpy((char*)pMoCAIfFull->Cfg.FreqCurrentMaskSetting, "Test");
    strcpy((char*)pMoCAIfFull->Cfg.X_CISCO_COM_ChannelScanMask, "Test");
    strcpy((char*)pMoCAIfFull->DynamicInfo.FreqCurrentMask, "1234567");
    strcpy(pMoCAIfFull->Cfg.KeyPassphrase, "Test");
    strcpy((char*)pMoCAIfFull->StaticInfo.NetworkTabooMask, "12345678");
    strcpy((char*)pMoCAIfFull->Cfg.NodeTabooMask, "12345678");
    strcpy(pMoCAIfFull->DynamicInfo.X_CISCO_NetworkCoordinatorMACAddress, "123456789ABC");

//For Alias Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).WillOnce(Return(0));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).WillRepeatedly(Return(0));
    EXPECT_EQ(Interface1_GetParamStringValue((ANSC_HANDLE)pMoCAIfFull, paramName1, pValue, &pSize), 0);

//For Name Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(2)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).WillOnce(Return(0));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).WillRepeatedly(Return(0));
    EXPECT_EQ(Interface1_GetParamStringValue((ANSC_HANDLE)pMoCAIfFull, paramName2, pValue, &pSize), 0);

//For LowerLayers Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(3)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Interface1_GetParamStringValue((ANSC_HANDLE)pMoCAIfFull, paramName3, pValue, &pSize), 0);

//For MACAddress Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(4)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).WillRepeatedly(Return(0));
    EXPECT_EQ(Interface1_GetParamStringValue((ANSC_HANDLE)pMoCAIfFull, paramName4, pValue, &pSize), 0);

//For FirmwareVersion Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(5)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).WillOnce(Return(0));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).WillRepeatedly(Return(0));
    EXPECT_EQ(Interface1_GetParamStringValue((ANSC_HANDLE)pMoCAIfFull, paramName5, pValue, &pSize), 0);

//For HighestVersion Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(6)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).WillOnce(Return(0));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).WillRepeatedly(Return(0));
    EXPECT_EQ(Interface1_GetParamStringValue((ANSC_HANDLE)pMoCAIfFull, paramName6, pValue, &pSize), 0);

//For CurrentVersion Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(7)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).WillOnce(Return(0));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).WillRepeatedly(Return(0));
    EXPECT_EQ(Interface1_GetParamStringValue((ANSC_HANDLE)pMoCAIfFull, paramName7, pValue, &pSize), 0);

//For FreqCapabilityMask Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(8)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).WillRepeatedly(Return(0));
    EXPECT_EQ(Interface1_GetParamStringValue((ANSC_HANDLE)pMoCAIfFull, paramName8, pValue, &pSize), 0);

//For FreqCurrentMaskSetting Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(9)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).WillOnce(Return(0));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).WillRepeatedly(Return(0));
    EXPECT_EQ(Interface1_GetParamStringValue((ANSC_HANDLE)pMoCAIfFull, paramName9, pValue, &pSize), 0);

//For X_CISCO_COM_ChannelScanMask Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(10)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).WillOnce(Return(0));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).WillRepeatedly(Return(0));
    EXPECT_EQ(Interface1_GetParamStringValue((ANSC_HANDLE)pMoCAIfFull, paramName10, pValue, &pSize), 0);

//For FreqCurrentMask Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(11)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).WillRepeatedly(Return(0));
    EXPECT_EQ(Interface1_GetParamStringValue((ANSC_HANDLE)pMoCAIfFull, paramName11, pValue, &pSize), 0);

//For KeyPassphrase Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(12)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).WillOnce(Return(0));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).WillRepeatedly(Return(0));
    EXPECT_EQ(Interface1_GetParamStringValue((ANSC_HANDLE)pMoCAIfFull, paramName12, pValue, &pSize), 0);

//For NetworkTabooMask Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(13)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).WillOnce(Return(0));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).WillRepeatedly(Return(0));
    EXPECT_EQ(Interface1_GetParamStringValue((ANSC_HANDLE)pMoCAIfFull, paramName13, pValue, &pSize), 0);

//For NodeTabooMask Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(14)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).WillOnce(Return(0));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).WillRepeatedly(Return(0));
    EXPECT_EQ(Interface1_GetParamStringValue((ANSC_HANDLE)pMoCAIfFull, paramName14, pValue, &pSize), 0);

//For X_CISCO_NetworkCoordinatorMACAddress Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(15)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).WillOnce(Return(0));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).WillRepeatedly(Return(0));
    EXPECT_EQ(Interface1_GetParamStringValue((ANSC_HANDLE)pMoCAIfFull, paramName15, pValue, &pSize), 0);

    free(pMoCAIfFull);
    pMoCAIfFull = NULL;

    free(g_MoCAObject);
    g_MoCAObject = NULL;
}

//Test for Interface1_GetParamStringValue - Failure
TEST_F(CcspMoCATestFixture, Interface1_GetParamStringValueFailure)
{
    char pValue[64] = {0};
    ULONG pSize = 0;

    char paramName1[64] = "Alias";
    char paramName2[64] = "Name";
    char paramName3[64] = "LowerLayers";
    char paramName4[64] = "MACAddress";
    char paramName5[64] = "FirmwareVersion";
    char paramName6[64] = "HighestVersion";
    char paramName7[64] = "CurrentVersion";
    char paramName8[64] = "FreqCapabilityMask";
    char paramName9[64] = "FreqCurrentMaskSetting";
    char paramName10[64] = "X_CISCO_COM_ChannelScanMask";
    char paramName11[64] = "FreqCurrentMask";
    char paramName12[64] = "KeyPassphrase";
    char paramName13[64] = "NetworkTabooMask";
    char paramName14[64] = "NodeTabooMask";
    char paramName15[64] = "X_CISCO_NetworkCoordinatorMACAddress";

    ANSC_HANDLE hInsContext = (ANSC_HANDLE)1;
    extern ANSC_HANDLE g_MoCAObject;
    g_MoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));

    PCOSA_DATAMODEL_MOCA        pTestObject   = (PCOSA_DATAMODEL_MOCA    )g_MoCAObject;
    PCOSA_DML_MOCA_IF_CFG       pCfg          = &pTestObject->MoCAIfFullTable[0].MoCAIfFull.Cfg;

    PCOSA_DML_MOCA_IF_FULL pMoCAIfFull = (PCOSA_DML_MOCA_IF_FULL)malloc(sizeof(COSA_DML_MOCA_IF_FULL));
    memset(pMoCAIfFull, 0, sizeof(COSA_DML_MOCA_IF_FULL));

    strcpy(pMoCAIfFull->Cfg.Alias, "Test");
    strcpy(pMoCAIfFull->StaticInfo.Name, "Test");
    strcpy((char*)pMoCAIfFull->StaticInfo.MacAddress, "123456789ABC");
    strcpy(pMoCAIfFull->StaticInfo.FirmwareVersion, "Test");
    strcpy(pMoCAIfFull->StaticInfo.HighestVersion, "Test");
    strcpy(pMoCAIfFull->DynamicInfo.CurrentVersion, "Test");
    strcpy((char*)pMoCAIfFull->StaticInfo.FreqCapabilityMask, "1234567");
    strcpy((char*)pMoCAIfFull->Cfg.FreqCurrentMaskSetting, "Test");
    strcpy((char*)pMoCAIfFull->Cfg.X_CISCO_COM_ChannelScanMask, "Test");
    strcpy((char*)pMoCAIfFull->DynamicInfo.FreqCurrentMask, "1234567");
    strcpy(pMoCAIfFull->Cfg.KeyPassphrase, "Test");
    strcpy((char*)pMoCAIfFull->StaticInfo.NetworkTabooMask, "12345678");
    strcpy((char*)pMoCAIfFull->Cfg.NodeTabooMask, "12345678");
    strcpy(pMoCAIfFull->DynamicInfo.X_CISCO_NetworkCoordinatorMACAddress, "123456789ABC");

//For Alias Failure
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).WillOnce(Return(1));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).WillRepeatedly(Return(0));
    EXPECT_NE(Interface1_GetParamStringValue((ANSC_HANDLE)pMoCAIfFull, paramName1, pValue, &pSize), 0);

//For Name Failure
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(2)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).WillOnce(Return(1));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).WillRepeatedly(Return(0));
    EXPECT_NE(Interface1_GetParamStringValue((ANSC_HANDLE)pMoCAIfFull, paramName2, pValue, &pSize), 0);

//For FirmwareVersion Failure
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(5)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).WillOnce(Return(1));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).WillRepeatedly(Return(0));
    EXPECT_NE(Interface1_GetParamStringValue((ANSC_HANDLE)pMoCAIfFull, paramName5, pValue, &pSize), 0);

//For HighestVersion Failure
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(6)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).WillOnce(Return(1));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).WillRepeatedly(Return(0));
    EXPECT_NE(Interface1_GetParamStringValue((ANSC_HANDLE)pMoCAIfFull, paramName6, pValue, &pSize), 0);

//For CurrentVersion Failure
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(7)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).WillOnce(Return(1));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).WillRepeatedly(Return(0));
    EXPECT_NE(Interface1_GetParamStringValue((ANSC_HANDLE)pMoCAIfFull, paramName7, pValue, &pSize), 0);

//For FreqCurrentMaskSetting Failure
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(9)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).WillOnce(Return(1));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).WillRepeatedly(Return(0));
    EXPECT_NE(Interface1_GetParamStringValue((ANSC_HANDLE)pMoCAIfFull, paramName9, pValue, &pSize), 0);

//For X_CISCO_COM_ChannelScanMask Failure
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(10)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).WillOnce(Return(1));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).WillRepeatedly(Return(0));
    EXPECT_NE(Interface1_GetParamStringValue((ANSC_HANDLE)pMoCAIfFull, paramName10, pValue, &pSize), 0);

//For KeyPassphrase Failure
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(12)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).WillOnce(Return(1));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).WillRepeatedly(Return(0));
    EXPECT_NE(Interface1_GetParamStringValue((ANSC_HANDLE)pMoCAIfFull, paramName12, pValue, &pSize), 0);

//For NetworkTabooMask Failure
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(13)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).WillOnce(Return(1));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).WillRepeatedly(Return(0));
    EXPECT_NE(Interface1_GetParamStringValue((ANSC_HANDLE)pMoCAIfFull, paramName13, pValue, &pSize), 0);

//For NodeTabooMask Failure
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(14)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).WillOnce(Return(1));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).WillRepeatedly(Return(0));
    EXPECT_NE(Interface1_GetParamStringValue((ANSC_HANDLE)pMoCAIfFull, paramName14, pValue, &pSize), 0);

//For X_CISCO_NetworkCoordinatorMACAddress Failure
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(15)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).WillOnce(Return(1));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).WillRepeatedly(Return(0));
    EXPECT_NE(Interface1_GetParamStringValue((ANSC_HANDLE)pMoCAIfFull, paramName15, pValue, &pSize), 0);

    free(pMoCAIfFull);
    pMoCAIfFull = NULL;

    free(g_MoCAObject);
    g_MoCAObject = NULL;
}

//Test for Interface1_SetParamBoolValue - Success
TEST_F(CcspMoCATestFixture, Interface1_SetParamBoolValueSuccess)
{
    BOOL bValue = TRUE;
    char pValueTrue[] = "1";
    char paramName1[64] = "Enable";
    char paramName2[64] = "PreferredNC";
    char paramName3[64] = "PrivacyEnabledSetting";
    char paramName4[64] = "X_CISCO_COM_Reset";
    char paramName5[64] = "X_CISCO_COM_MixedMode";
    char paramName6[64] = "X_CISCO_COM_ChannelScanning";
    char paramName7[64] = "X_CISCO_COM_AutoPowerControlEnable";
    char paramName8[64] = "X_CISCO_COM_EnableTabooBit";

    ANSC_HANDLE hInsContext = (ANSC_HANDLE)1;
    extern ANSC_HANDLE g_MoCAObject;
    g_MoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));

    PCOSA_DATAMODEL_MOCA        pTestObject   = (PCOSA_DATAMODEL_MOCA    )g_MoCAObject;
    PCOSA_DML_MOCA_IF_CFG       pCfg          = &pTestObject->MoCAIfFullTable[0].MoCAIfFull.Cfg;

    PCOSA_DML_MOCA_IF_FULL pMoCAIfFull = (PCOSA_DML_MOCA_IF_FULL)malloc(sizeof(COSA_DML_MOCA_IF_FULL));
    memset(pMoCAIfFull, 0, sizeof(COSA_DML_MOCA_IF_FULL));

//For Enable Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_syscfgMock, syscfg_get(_,_,_,_)).Times(1).WillOnce(DoAll(SetArrayArgument<2>(pValueTrue, pValueTrue + sizeof(pValueTrue)), Return(0)));
    EXPECT_EQ(Interface1_SetParamBoolValue((ANSC_HANDLE)pMoCAIfFull, paramName1, bValue), TRUE);

//For PreferredNC Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(2)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Interface1_SetParamBoolValue((ANSC_HANDLE)pMoCAIfFull, paramName2, bValue), TRUE);

//For PrivacyEnabledSetting Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(3)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Interface1_SetParamBoolValue((ANSC_HANDLE)pMoCAIfFull, paramName3, bValue), TRUE);

//For X_CISCO_COM_Reset Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(4)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Interface1_SetParamBoolValue((ANSC_HANDLE)pMoCAIfFull, paramName4, bValue), TRUE);

//For X_CISCO_COM_MixedMode Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(5)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Interface1_SetParamBoolValue((ANSC_HANDLE)pMoCAIfFull, paramName5, bValue), TRUE);

//For X_CISCO_COM_ChannelScanning Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(6)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Interface1_SetParamBoolValue((ANSC_HANDLE)pMoCAIfFull, paramName6, bValue), TRUE);

//For X_CISCO_COM_AutoPowerControlEnable Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(7)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Interface1_SetParamBoolValue((ANSC_HANDLE)pMoCAIfFull, paramName7, bValue), TRUE);

//For X_CISCO_COM_EnableTabooBit Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(8)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Interface1_SetParamBoolValue((ANSC_HANDLE)pMoCAIfFull, paramName8, bValue), TRUE);

    free(pMoCAIfFull);
    pMoCAIfFull = NULL;

    free(g_MoCAObject);
    g_MoCAObject = NULL;
}

//Test for Interface1_SetParamBoolValue - Failure
TEST_F(CcspMoCATestFixture, Interface1_SetParamBoolValueFailure)
{
    BOOL bValue = FALSE;
    char pValueTrue[] = "1";
    char paramName1[64] = "Enable";

    ANSC_HANDLE hInsContext = (ANSC_HANDLE)1;
    extern ANSC_HANDLE g_MoCAObject;
    g_MoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));

    PCOSA_DATAMODEL_MOCA        pTestObject   = (PCOSA_DATAMODEL_MOCA    )g_MoCAObject;
    PCOSA_DML_MOCA_IF_CFG       pCfg          = &pTestObject->MoCAIfFullTable[0].MoCAIfFull.Cfg;
    PCOSA_DML_MOCA_CFG          mCfg          = &pTestObject->MoCACfg;

    PCOSA_DML_MOCA_IF_FULL pMoCAIfFull = (PCOSA_DML_MOCA_IF_FULL)malloc(sizeof(COSA_DML_MOCA_IF_FULL));
    memset(pMoCAIfFull, 0, sizeof(COSA_DML_MOCA_IF_FULL));

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_syscfgMock, syscfg_get(_,_,_,_)).Times(1).WillOnce(DoAll(SetArrayArgument<2>(pValueTrue, pValueTrue + sizeof(pValueTrue)), Return(1)));
    EXPECT_EQ(Interface1_SetParamBoolValue((ANSC_HANDLE)pMoCAIfFull, paramName1, bValue), FALSE);

    free(pMoCAIfFull);
    pMoCAIfFull = NULL;

    free(g_MoCAObject);
    g_MoCAObject = NULL;
}

//Test for Interface1_SetParamIntValue - Success
TEST_F(CcspMoCATestFixture, Interface1_SetParamIntValueSuccess)
{
    int iValue = 100;
    char paramName1[16] = "TxPowerLimit";

    ANSC_HANDLE hInsContext = (ANSC_HANDLE)1;
    extern ANSC_HANDLE g_MoCAObject;
    g_MoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));

    PCOSA_DATAMODEL_MOCA        pTestObject   = (PCOSA_DATAMODEL_MOCA    )g_MoCAObject;
    PCOSA_DML_MOCA_IF_CFG       pCfg          = &pTestObject->MoCAIfFullTable[0].MoCAIfFull.Cfg;

    PCOSA_DML_MOCA_IF_FULL pMoCAIfFull = (PCOSA_DML_MOCA_IF_FULL)malloc(sizeof(COSA_DML_MOCA_IF_FULL));
    memset(pMoCAIfFull, 0, sizeof(COSA_DML_MOCA_IF_FULL));

//For TxPowerLimit Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Interface1_SetParamIntValue((ANSC_HANDLE)pMoCAIfFull, paramName1, iValue), TRUE);

    free(pMoCAIfFull);
    pMoCAIfFull = NULL;

    free(g_MoCAObject);
    g_MoCAObject = NULL;
}

//Test for Interface1_SetParamUlongValue - Success
TEST_F(CcspMoCATestFixture, Interface1_SetParamUlongValueSuccess)
{
    ULONG ulValue = 12;
    char paramName1[32] = "PowerCntlPhyTarget";
    char paramName2[32] = "BeaconPowerLimit";
    char paramName3[32] = "AutoPowerControlPhyRate";

    ANSC_HANDLE hInsContext = (ANSC_HANDLE)1;
    extern ANSC_HANDLE g_MoCAObject;
    g_MoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));

    PCOSA_DATAMODEL_MOCA        pTestObject   = (PCOSA_DATAMODEL_MOCA    )g_MoCAObject;
    PCOSA_DML_MOCA_IF_CFG       pCfg          = &pTestObject->MoCAIfFullTable[0].MoCAIfFull.Cfg;

    PCOSA_DML_MOCA_IF_FULL pMoCAIfFull = (PCOSA_DML_MOCA_IF_FULL)malloc(sizeof(COSA_DML_MOCA_IF_FULL));
    memset(pMoCAIfFull, 0, sizeof(COSA_DML_MOCA_IF_FULL));

//For PowerCntlPhyTarget Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Interface1_SetParamUlongValue((ANSC_HANDLE)pMoCAIfFull, paramName1, ulValue), TRUE);

//For BeaconPowerLimit Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(2)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Interface1_SetParamUlongValue((ANSC_HANDLE)pMoCAIfFull, paramName2, ulValue), TRUE);

//For AutoPowerControlPhyRate Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(3)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Interface1_SetParamUlongValue((ANSC_HANDLE)pMoCAIfFull, paramName3, ulValue), TRUE);

    free(pMoCAIfFull);
    pMoCAIfFull = NULL;

    free(g_MoCAObject);
    g_MoCAObject = NULL;
}

//Test for Interface1_SetParamStringValue - Success
TEST_F(CcspMoCATestFixture, Interface1_SetParamStringValueSuccess)
{
    char pValue[64] = "Test";
    ULONG pSize = 64;

    char paramName1[64] = "KeyPassphrase";
    char paramName2[64] = "Alias";
    char paramName3[64] = "LowerLayers";
    char paramName4[64] = "FreqCurrentMaskSetting";
    char paramName5[64] = "X_CISCO_COM_ChannelScanMask";
    char paramName6[64] = "NodeTabooMask";

    ANSC_HANDLE hInsContext = (ANSC_HANDLE)1;
    extern ANSC_HANDLE g_MoCAObject;
    g_MoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));

    PCOSA_DATAMODEL_MOCA        pTestObject   = (PCOSA_DATAMODEL_MOCA    )g_MoCAObject;
    PCOSA_DML_MOCA_IF_CFG       pCfg          = &pTestObject->MoCAIfFullTable[0].MoCAIfFull.Cfg;

    PCOSA_DML_MOCA_IF_FULL pMoCAIfFull = (PCOSA_DML_MOCA_IF_FULL)malloc(sizeof(COSA_DML_MOCA_IF_FULL));
    memset(pMoCAIfFull, 0, sizeof(COSA_DML_MOCA_IF_FULL));

//For KeyPassphrase Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(6)
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).WillOnce(Return(0));
    EXPECT_EQ(Interface1_SetParamStringValue((ANSC_HANDLE)pMoCAIfFull, paramName1, pValue), TRUE);

//For Alias Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(2)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).WillOnce(Return(0));
    EXPECT_EQ(Interface1_SetParamStringValue((ANSC_HANDLE)pMoCAIfFull, paramName2, pValue), TRUE);

//For LowerLayers Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(3)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Interface1_SetParamStringValue((ANSC_HANDLE)pMoCAIfFull, paramName3, pValue), FALSE);

//For FreqCurrentMaskSetting Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(4)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).WillOnce(Return(0));
    EXPECT_EQ(Interface1_SetParamStringValue((ANSC_HANDLE)pMoCAIfFull, paramName4, pValue), TRUE);

//For X_CISCO_COM_ChannelScanMask Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(5)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).WillOnce(Return(0));
    EXPECT_EQ(Interface1_SetParamStringValue((ANSC_HANDLE)pMoCAIfFull, paramName5, pValue), TRUE);

//For NodeTabooMask Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(7)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).WillOnce(Return(0));
    EXPECT_EQ(Interface1_SetParamStringValue((ANSC_HANDLE)pMoCAIfFull, paramName6, pValue), TRUE);

    free(pMoCAIfFull);
    pMoCAIfFull = NULL;

    free(g_MoCAObject);
    g_MoCAObject = NULL;
}

//Test for Interface1_Validate - Success
TEST_F(CcspMoCATestFixture, Interface1_ValidateSuccess)
{
    char pReturnParamName[64] = {0};
    ULONG pSize = 0;

    PCOSA_DML_MOCA_IF_FULL_TABLE pMoCAIfFullTable = (PCOSA_DML_MOCA_IF_FULL_TABLE)malloc(sizeof(COSA_DML_MOCA_IF_FULL_TABLE));
    memset(pMoCAIfFullTable, 0, sizeof(COSA_DML_MOCA_IF_FULL_TABLE));

    strcpy(pMoCAIfFullTable->MoCAIfFull.Cfg.KeyPassphrase, "1234567890123");

    EXPECT_EQ(Interface1_Validate((ANSC_HANDLE)pMoCAIfFullTable, pReturnParamName, &pSize), TRUE);

    free(pMoCAIfFullTable);
    pMoCAIfFullTable = NULL;
}

//Test for Interface1_Valiate - Failure
TEST_F(CcspMoCATestFixture, Interface1_ValidateFailure)
{
    char pReturnParamName[64] = {0};
    ULONG pSize = 0;

//For KeyPassphrase Failure Due to characters used
    PCOSA_DML_MOCA_IF_FULL_TABLE pMoCAIfFullTable = (PCOSA_DML_MOCA_IF_FULL_TABLE)malloc(sizeof(COSA_DML_MOCA_IF_FULL_TABLE));
    memset(pMoCAIfFullTable, 0, sizeof(COSA_DML_MOCA_IF_FULL_TABLE));

    strcpy(pMoCAIfFullTable->MoCAIfFull.Cfg.KeyPassphrase, "Password12345");

    EXPECT_EQ(Interface1_Validate((ANSC_HANDLE)pMoCAIfFullTable, pReturnParamName, &pSize), FALSE);

    free(pMoCAIfFullTable);
    pMoCAIfFullTable = NULL;

//For KeyPassphrase EOK Failure
    pMoCAIfFullTable = (PCOSA_DML_MOCA_IF_FULL_TABLE)malloc(sizeof(COSA_DML_MOCA_IF_FULL_TABLE));
    memset(pMoCAIfFullTable, 0, sizeof(COSA_DML_MOCA_IF_FULL_TABLE));

    strcpy(pMoCAIfFullTable->MoCAIfFull.Cfg.KeyPassphrase, "Test");

    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).WillOnce(Return(1));
    EXPECT_EQ(Interface1_Validate((ANSC_HANDLE)pMoCAIfFullTable, pReturnParamName, &pSize), FALSE);

    free(pMoCAIfFullTable);
    pMoCAIfFullTable = NULL;

//For KeyPassphrase Failure Due to length
    pMoCAIfFullTable = (PCOSA_DML_MOCA_IF_FULL_TABLE)malloc(sizeof(COSA_DML_MOCA_IF_FULL_TABLE));
    memset(pMoCAIfFullTable, 0, sizeof(COSA_DML_MOCA_IF_FULL_TABLE));

    strcpy(pMoCAIfFullTable->MoCAIfFull.Cfg.KeyPassphrase, "Test");

    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).WillOnce(Return(0));
    EXPECT_EQ(Interface1_Validate((ANSC_HANDLE)pMoCAIfFullTable, pReturnParamName, &pSize), FALSE);

    free(pMoCAIfFullTable);
    pMoCAIfFullTable = NULL;
}

//Test for Interface1_Commit - Success
TEST_F(CcspMoCATestFixture, Interface1_CommitSuccess)
{
    PCOSA_DML_MOCA_IF_FULL_TABLE pMoCAIfFullTable = (PCOSA_DML_MOCA_IF_FULL_TABLE)malloc(sizeof(COSA_DML_MOCA_IF_FULL_TABLE));
    memset(pMoCAIfFullTable, 0, sizeof(COSA_DML_MOCA_IF_FULL_TABLE));

    pMoCAIfFullTable->MoCAIfFull.Cfg.X_CISCO_COM_Reset = FALSE;
    pMoCAIfFullTable->MoCAIfFull.Cfg.InstanceNumber = 1;

    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_syscfgMock, syscfg_get(_,_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_safecLibMock, _memcpy_s_chk(_,_,_,_,_,_)).Times(4).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_mocaHALMock, moca_SetIfConfig(_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_syscfgMock, syscfg_set_nns_commit(_,_)).Times(11).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_mocaHALMock, moca_FreqMaskToValue(_)).Times(1).WillOnce(Return(0));
    EXPECT_EQ(Interface1_Commit((ANSC_HANDLE)pMoCAIfFullTable), 0);

    free(pMoCAIfFullTable);
    pMoCAIfFullTable = NULL;
}

//Test for Interface1_Commit - Failure and Interface1_Rollback - Success
TEST_F(CcspMoCATestFixture, Interface1_CommitFailure)
{
    PCOSA_DML_MOCA_IF_FULL_TABLE pMoCAIfFullTable = (PCOSA_DML_MOCA_IF_FULL_TABLE)malloc(sizeof(COSA_DML_MOCA_IF_FULL_TABLE));
    memset(pMoCAIfFullTable, 0, sizeof(COSA_DML_MOCA_IF_FULL_TABLE));

    pMoCAIfFullTable->MoCAIfFull.Cfg.X_CISCO_COM_Reset = FALSE;
    pMoCAIfFullTable->MoCAIfFull.Cfg.InstanceNumber = 0;

    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_syscfgMock, syscfg_get(_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_NE(Interface1_Commit((ANSC_HANDLE)pMoCAIfFullTable), 0);

    free(pMoCAIfFullTable);
    pMoCAIfFullTable = NULL;
}

//Test for X_CISCO_COM_PeerTable_GetEntryCount - Success
TEST_F(CcspMoCATestFixture, X_CISCO_COM_PeerTable_GetEntryCountSuccess)
{
    extern ANSC_HANDLE g_MoCAObject;
    g_MoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));

    PCOSA_DATAMODEL_MOCA        pTestObject   = (PCOSA_DATAMODEL_MOCA    )g_MoCAObject;
    PCOSA_DML_MOCA_IF_FULL_TABLE pMoCAIfFullTable = &pTestObject->MoCAIfFullTable[0];

    pMoCAIfFullTable = (PCOSA_DML_MOCA_IF_FULL_TABLE)malloc(sizeof(COSA_DML_MOCA_IF_FULL_TABLE));
    memset(pMoCAIfFullTable, 0, sizeof(COSA_DML_MOCA_IF_FULL_TABLE));

    pMoCAIfFullTable->MoCAIfFull.Cfg.InstanceNumber = 1;
    pTestObject->MoCAIfFullTable[0].ulMoCAPeerTableCount = 1;

    EXPECT_EQ(X_CISCO_COM_PeerTable_GetEntryCount((ANSC_HANDLE)pMoCAIfFullTable), 1);

    free(pMoCAIfFullTable);
    pMoCAIfFullTable = NULL;

    free(g_MoCAObject);
    g_MoCAObject = NULL;
}

//Test for X_CISCO_COM_PeerTable_GetEntry - Success
TEST_F(CcspMoCATestFixture, X_CISCO_COM_PeerTable_GetEntrySuccess)
{
    ULONG ulIndex = 1;
    ULONG pNumber = 0;

    extern ANSC_HANDLE g_MoCAObject;
    g_MoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));

    PCOSA_DATAMODEL_MOCA        pTestObject   = (PCOSA_DATAMODEL_MOCA    )g_MoCAObject;
    PCOSA_DML_MOCA_IF_FULL_TABLE pMoCAIfFullTable = &pTestObject->MoCAIfFullTable[0];

    pMoCAIfFullTable = (PCOSA_DML_MOCA_IF_FULL_TABLE)malloc(sizeof(COSA_DML_MOCA_IF_FULL_TABLE));
    memset(pMoCAIfFullTable, 0, sizeof(COSA_DML_MOCA_IF_FULL_TABLE));

    PCOSA_DML_MOCA_IF_FULL pMoCAIfFull = &pMoCAIfFullTable->MoCAIfFull;
    pMoCAIfFull = (PCOSA_DML_MOCA_IF_FULL)malloc(sizeof(COSA_DML_MOCA_IF_FULL));
    memset(pMoCAIfFull, 0, sizeof(COSA_DML_MOCA_IF_FULL));

    pMoCAIfFull->Cfg.InstanceNumber = 1;
    pTestObject->MoCAIfFullTable[0].ulMoCAPeerTableCount = 2;

    PCOSA_DML_MOCA_PEER pMoCAPeer = (PCOSA_DML_MOCA_PEER)malloc(sizeof(COSA_DML_MOCA_PEER));
    memset(pMoCAPeer, 0, sizeof(COSA_DML_MOCA_PEER));

    pMoCAPeer = (PCOSA_DML_MOCA_PEER)X_CISCO_COM_PeerTable_GetEntry((ANSC_HANDLE)pMoCAIfFull, ulIndex, &pNumber);

    EXPECT_NE(pMoCAPeer, nullptr);

    free(pMoCAIfFullTable);
    pMoCAIfFullTable = NULL;

    free(g_MoCAObject);
    g_MoCAObject = NULL;
}

//Test for X_CISCO_COM_PeerTable_IsUpdated - Success
TEST_F(CcspMoCATestFixture, X_CISCO_COM_PeerTable_IsUpdatedSuccess)
{
//For tick = 0
    EXPECT_CALL(*g_usertimeMock, UserGetTickInSeconds2()).Times(1).WillOnce(Return(5));
    EXPECT_EQ(X_CISCO_COM_PeerTable_IsUpdated(NULL), TRUE);

//For tick with current time value
    EXPECT_CALL(*g_usertimeMock, UserGetTickInSeconds2()).Times(2).WillRepeatedly(Return(6));
    EXPECT_EQ(X_CISCO_COM_PeerTable_IsUpdated(NULL), FALSE);

//For tick with more than MoCA refresh time value
    EXPECT_CALL(*g_usertimeMock, UserGetTickInSeconds2()).Times(3).WillRepeatedly(Return(15));
    EXPECT_EQ(X_CISCO_COM_PeerTable_IsUpdated(NULL), TRUE);
}

//Test for X_CISCO_COM_PeerTable_Synchronize - Success
TEST_F(CcspMoCATestFixture, X_CISCO_COM_PeerTable_SynchronizeSuccess)
{
    ULONG ulIndex = 1;
    ULONG pNumber = 0;

    extern ANSC_HANDLE g_MoCAObject;
    g_MoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));

    PCOSA_DATAMODEL_MOCA        pTestObject   = (PCOSA_DATAMODEL_MOCA    )g_MoCAObject;
    PCOSA_DML_MOCA_IF_FULL_TABLE pMoCAIfFullTable = &pTestObject->MoCAIfFullTable[0];

    pMoCAIfFullTable = (PCOSA_DML_MOCA_IF_FULL_TABLE)malloc(sizeof(COSA_DML_MOCA_IF_FULL_TABLE));
    memset(pMoCAIfFullTable, 0, sizeof(COSA_DML_MOCA_IF_FULL_TABLE));

    PCOSA_DML_MOCA_IF_FULL pMoCAIfFull = &pMoCAIfFullTable->MoCAIfFull;
    pMoCAIfFull = (PCOSA_DML_MOCA_IF_FULL)malloc(sizeof(COSA_DML_MOCA_IF_FULL));
    memset(pMoCAIfFull, 0, sizeof(COSA_DML_MOCA_IF_FULL));

    pMoCAIfFull->Cfg.InstanceNumber = 1;
    pTestObject->MoCAIfFullTable[0].ulMoCAPeerTableCount = 2;

    pTestObject->MoCAIfFullTable[0].pMoCAPeerTable = (PCOSA_DML_MOCA_PEER)malloc(sizeof(COSA_DML_MOCA_PEER));
    memset(&pTestObject->MoCAIfFullTable[0].pMoCAPeerTable[0], 0, sizeof(COSA_DML_MOCA_PEER));

    EXPECT_CALL(*g_anscMemoryMock, AnscFreeMemory(_)).Times(1);
    EXPECT_EQ(X_CISCO_COM_PeerTable_Synchronize((ANSC_HANDLE)pMoCAIfFull), 0);

    free(pTestObject->MoCAIfFullTable[0].pMoCAPeerTable);
    pTestObject->MoCAIfFullTable[0].pMoCAPeerTable = NULL;

    free(pMoCAIfFullTable);
    pMoCAIfFullTable = NULL;

    free(g_MoCAObject);
    g_MoCAObject = NULL;
}

//Test for X_CISCO_COM_PeerTable_GetParamBoolValue - Success
TEST_F(CcspMoCATestFixture, X_CISCO_COM_PeerTable_GetParamBoolValueSuccess)
{
    EXPECT_EQ(X_CISCO_COM_PeerTable_GetParamBoolValue(NULL, NULL, NULL), FALSE);
}

//Test for X_CISCO_COM_PeerTable_GetParamIntValue - Success
TEST_F(CcspMoCATestFixture, X_CISCO_COM_PeerTable_GetParamIntValueSuccess)
{
    EXPECT_EQ(X_CISCO_COM_PeerTable_GetParamIntValue(NULL, NULL, NULL), FALSE);
}

//Test for X_CISCO_COM_PeerTable_GetParamUlongValue - Success
TEST_F(CcspMoCATestFixture, X_CISCO_COM_PeerTable_GetParamUlongValueSuccess)
{
    char paramName[16] = "PeerSpeed";
    ULONG ulValue = 0;
    PCOSA_DML_MOCA_PEER pMoCAPeer = (PCOSA_DML_MOCA_PEER)malloc(sizeof(COSA_DML_MOCA_PEER));
    memset(pMoCAPeer, 0, sizeof(COSA_DML_MOCA_PEER));

    pMoCAPeer->PeerSpeed = 100;

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(X_CISCO_COM_PeerTable_GetParamUlongValue((ANSC_HANDLE)pMoCAPeer, paramName, &ulValue), TRUE);
    EXPECT_EQ(ulValue, 100);

    free(pMoCAPeer);
    pMoCAPeer = NULL;
}

//Test for X_CISCO_COM_PeerTable_GetParamStringValue - Success
TEST_F(CcspMoCATestFixture, X_CISCO_COM_PeerTable_GetParamStringValueSuccess)
{
    EXPECT_NE(X_CISCO_COM_PeerTable_GetParamStringValue(NULL, NULL, NULL, NULL), 0);
}

//Test for MeshTable_GetEntryCount - Success
TEST_F(CcspMoCATestFixture, MeshTable_GetEntryCountSuccess)
{
    PCOSA_DML_MOCA_IF_FULL_TABLE pMoCAIfFullTable = (PCOSA_DML_MOCA_IF_FULL_TABLE)malloc(sizeof(COSA_DML_MOCA_IF_FULL_TABLE));
    memset(pMoCAIfFullTable, 0, sizeof(COSA_DML_MOCA_IF_FULL_TABLE));

    pMoCAIfFullTable->ulMoCAMeshTableCount = 1;

    EXPECT_EQ(MeshTable_GetEntryCount((ANSC_HANDLE)pMoCAIfFullTable), 1);

    free(pMoCAIfFullTable);
    pMoCAIfFullTable = NULL;
}

//Test for MeshTable_GetEntry - Success
TEST_F(CcspMoCATestFixture, MeshTable_GetEntrySuccess)
{
    ULONG ulIndex = 0;
    ULONG pNumber = 0;

    PCOSA_DML_MOCA_IF_FULL_TABLE pMoCAIfFullTable = (PCOSA_DML_MOCA_IF_FULL_TABLE)malloc(sizeof(COSA_DML_MOCA_IF_FULL_TABLE));
    memset(pMoCAIfFullTable, 0, sizeof(COSA_DML_MOCA_IF_FULL_TABLE));

    pMoCAIfFullTable->pMoCAMeshTable = (PCOSA_DML_MOCA_MESH)malloc(sizeof(COSA_DML_MOCA_MESH));
    memset(&pMoCAIfFullTable->pMoCAMeshTable[0], 0, sizeof(COSA_DML_MOCA_MESH));
    
    pMoCAIfFullTable->ulMoCAMeshTableCount = 1;
    
    EXPECT_NE(MeshTable_GetEntry((ANSC_HANDLE)pMoCAIfFullTable, ulIndex, &pNumber), nullptr);

    free(pMoCAIfFullTable->pMoCAMeshTable);
    pMoCAIfFullTable->pMoCAMeshTable = NULL;

    free(pMoCAIfFullTable);
    pMoCAIfFullTable = NULL;
}

//Test for MeshTable_IsUpdated - Success
TEST_F(CcspMoCATestFixture, MeshTable_IsUpdatedSuccess)
{
//For tick = 0
    EXPECT_CALL(*g_usertimeMock, UserGetTickInSeconds2()).Times(1).WillOnce(Return(5));
    EXPECT_EQ(MeshTable_IsUpdated(NULL), TRUE);

//For tick with current time value
    EXPECT_CALL(*g_usertimeMock, UserGetTickInSeconds2()).Times(2).WillRepeatedly(Return(6));
    EXPECT_EQ(MeshTable_IsUpdated(NULL), FALSE);

//For tick with more than MoCA refresh time value
    EXPECT_CALL(*g_usertimeMock, UserGetTickInSeconds2()).Times(3).WillRepeatedly(Return(15));
    EXPECT_EQ(MeshTable_IsUpdated(NULL), TRUE);
}

//Test for MeshTable_Synchronize - Success
TEST_F(CcspMoCATestFixture, MeshTable_SynchronizeSuccess)
{
    ULONG ulIndex = 0;
    ULONG pNumber = 0;

    PCOSA_DML_MOCA_IF_FULL_TABLE pMoCAIfFullTable = (PCOSA_DML_MOCA_IF_FULL_TABLE)malloc(sizeof(COSA_DML_MOCA_IF_FULL_TABLE));
    memset(pMoCAIfFullTable, 0, sizeof(COSA_DML_MOCA_IF_FULL_TABLE));

    pMoCAIfFullTable->pMoCAMeshTable = (PCOSA_DML_MOCA_MESH)malloc(sizeof(COSA_DML_MOCA_MESH));
    memset(&pMoCAIfFullTable->pMoCAMeshTable[0], 0, sizeof(COSA_DML_MOCA_MESH));

    pMoCAIfFullTable->ulMoCAMeshTableCount = 1;

    pMoCAIfFullTable->MoCAIfFull.Cfg.InstanceNumber = 1;

    EXPECT_CALL(*g_mocaHALMock, moca_GetFullMeshRates(_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_anscMemoryMock, AnscFreeMemory(_)).Times(2);
    EXPECT_EQ(MeshTable_Synchronize((ANSC_HANDLE)pMoCAIfFullTable), 0);

    free(pMoCAIfFullTable->pMoCAMeshTable);
    pMoCAIfFullTable->pMoCAMeshTable = NULL;

    free(pMoCAIfFullTable);
    pMoCAIfFullTable = NULL;
}

//Test for MeshTable_GetParamUlongValue - Success
TEST_F(CcspMoCATestFixture, MeshTable_GetParamUlongValueSuccess)
{
    ULONG ulValue = 0;
    char paramName1[16] = "MeshTxNodeId";
    char paramName2[16] = "MeshRxNodeId";
    char paramName3[16] = "MeshPHYTxRate";

    PCOSA_DML_MOCA_MESH pMoCAMesh = (PCOSA_DML_MOCA_MESH)malloc(sizeof(COSA_DML_MOCA_MESH));
    memset(pMoCAMesh, 0, sizeof(COSA_DML_MOCA_MESH));

    pMoCAMesh->TxNodeID = 100;
    pMoCAMesh->RxNodeID = 200;
    pMoCAMesh->TxRate = 300;

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(MeshTable_GetParamUlongValue((ANSC_HANDLE)pMoCAMesh, paramName1, &ulValue), TRUE);
    EXPECT_EQ(ulValue, 100);

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(2)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(MeshTable_GetParamUlongValue((ANSC_HANDLE)pMoCAMesh, paramName2, &ulValue), TRUE);
    EXPECT_EQ(ulValue, 200);

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(3)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(MeshTable_GetParamUlongValue((ANSC_HANDLE)pMoCAMesh, paramName3, &ulValue), TRUE);
    EXPECT_EQ(ulValue, 300);

    free(pMoCAMesh);
    pMoCAMesh = NULL;
}

//Test for Stats2_GetParamBoolValue - Success
TEST_F(CcspMoCATestFixture, Stats2_GetParamBoolValueSuccess)
{
    char paramName[8] = "Reset";
    BOOL bValue = TRUE;

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Stats2_GetParamBoolValue(NULL, paramName, &bValue), TRUE);
    EXPECT_EQ(bValue, FALSE);
}

//Test for Stats2_GetParamIntValue - Success
TEST_F(CcspMoCATestFixture, Stats2_GetParamIntValueSuccess)
{
    EXPECT_EQ(Stats2_GetParamIntValue(NULL, NULL, NULL), FALSE);
}

//Test for Stats2_GetParamUlongValue - Success
TEST_F(CcspMoCATestFixture, Stats2_GetParamUlongValueSuccess)
{
    ULONG ulValue = 0;

    char paramName1[12] = "BytesSent";
    char paramName2[16] = "BytesReceived";
    char paramName3[16] = "PacketsSent";
    char paramName4[32] = "PacketsReceived";
    char paramName5[12] = "ErrorsSent";
    char paramName6[16] = "ErrorsReceived";
    char paramName7[24] = "UnicastPacketsSent";
    char paramName8[24] = "UnicastPacketsReceived";
    char paramName9[24] = "DiscardPacketsSent";
    char paramName10[24] = "DiscardPacketsReceived";
    char paramName11[24] = "MulticastPacketsSent";
    char paramName12[32] = "MulticastPacketsReceived";
    char paramName13[24] = "BroadcastPacketsSent";
    char paramName14[32] = "BroadcastPacketsReceived";
    char paramName15[32] = "UnknownProtoPacketsReceived";
    char paramName16[32] = "X_CISCO_COM_ExtAggrAverageTx";
    char paramName17[32] = "X_CISCO_COM_ExtAggrAverageRx";


    extern ANSC_HANDLE g_MoCAObject;
    g_MoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));

    PCOSA_DATAMODEL_MOCA        pTestObject   = (PCOSA_DATAMODEL_MOCA    )g_MoCAObject;
    PCOSA_DML_MOCA_IF_FULL_TABLE pMoCAIfFullTable = &pTestObject->MoCAIfFullTable[0];

    pMoCAIfFullTable = (PCOSA_DML_MOCA_IF_FULL_TABLE)malloc(sizeof(COSA_DML_MOCA_IF_FULL_TABLE));
    memset(pMoCAIfFullTable, 0, sizeof(COSA_DML_MOCA_IF_FULL_TABLE));

//For BytesSent Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).Times(1).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_EQ(Stats2_GetParamUlongValue((ANSC_HANDLE)g_MoCAObject, paramName1, &ulValue), TRUE);

//For BytesReceived Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(2)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_EQ(Stats2_GetParamUlongValue((ANSC_HANDLE)g_MoCAObject, paramName2, &ulValue), TRUE);

//For PacketsSent Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(3)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_EQ(Stats2_GetParamUlongValue((ANSC_HANDLE)g_MoCAObject, paramName3, &ulValue), TRUE);

//For PacketsReceived Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(4)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_EQ(Stats2_GetParamUlongValue((ANSC_HANDLE)g_MoCAObject, paramName4, &ulValue), TRUE);

//For ErrorsSent Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(5)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_EQ(Stats2_GetParamUlongValue((ANSC_HANDLE)g_MoCAObject, paramName5, &ulValue), TRUE);

//For ErrorsReceived Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(6)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_EQ(Stats2_GetParamUlongValue((ANSC_HANDLE)g_MoCAObject, paramName6, &ulValue), TRUE);

//For UnicastPacketsSent Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(7)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_EQ(Stats2_GetParamUlongValue((ANSC_HANDLE)g_MoCAObject, paramName7, &ulValue), TRUE);

//For UnicastPacketsReceived Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(8)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_EQ(Stats2_GetParamUlongValue((ANSC_HANDLE)g_MoCAObject, paramName8, &ulValue), TRUE);

//For DiscardPacketsSent Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(9)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_EQ(Stats2_GetParamUlongValue((ANSC_HANDLE)g_MoCAObject, paramName9, &ulValue), TRUE);

//For DiscardPacketsReceived Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(10)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_EQ(Stats2_GetParamUlongValue((ANSC_HANDLE)g_MoCAObject, paramName10, &ulValue), TRUE);

//For MulticastPacketsSent Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(11)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_EQ(Stats2_GetParamUlongValue((ANSC_HANDLE)g_MoCAObject, paramName11, &ulValue), TRUE);

//For MulticastPacketsReceived Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(12)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_EQ(Stats2_GetParamUlongValue((ANSC_HANDLE)g_MoCAObject, paramName12, &ulValue), TRUE);

//For BroadcastPacketsSent Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(13)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_EQ(Stats2_GetParamUlongValue((ANSC_HANDLE)g_MoCAObject, paramName13, &ulValue), TRUE);

//For BroadcastPacketsReceived Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(14)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_EQ(Stats2_GetParamUlongValue((ANSC_HANDLE)g_MoCAObject, paramName14, &ulValue), TRUE);

//For UnknownProtoPacketsReceived Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(15)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_EQ(Stats2_GetParamUlongValue((ANSC_HANDLE)g_MoCAObject, paramName15, &ulValue), TRUE);

//For X_CISCO_COM_ExtAggrAverageTx Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(16)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_EQ(Stats2_GetParamUlongValue((ANSC_HANDLE)g_MoCAObject, paramName16, &ulValue), TRUE);

//For X_CISCO_COM_ExtAggrAverageRx Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(17)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_EQ(Stats2_GetParamUlongValue((ANSC_HANDLE)g_MoCAObject, paramName17, &ulValue), TRUE);

    free(g_MoCAObject);
    g_MoCAObject = NULL;
}

//Test for Stats2_GetParamStringValue - Success
TEST_F(CcspMoCATestFixture, Stats2_GetParamStringValueSuccess)
{
    EXPECT_NE(Stats2_GetParamStringValue(NULL, NULL, NULL, NULL), 0);
}

//Test for Stats2_SetParamBoolValue - Success
TEST_F(CcspMoCATestFixture, Stats2_SetParamBoolValueSuccess)
{
    char paramName[8] = "Reset";
    BOOL bValue = TRUE;

    PCOSA_DML_MOCA_IF_FULL_TABLE pMoCAIfFullTable = (PCOSA_DML_MOCA_IF_FULL_TABLE)malloc(sizeof(COSA_DML_MOCA_IF_FULL_TABLE));
    memset(pMoCAIfFullTable, 0, sizeof(COSA_DML_MOCA_IF_FULL_TABLE));

    pMoCAIfFullTable->MoCAIfFull.Cfg.InstanceNumber = 1;

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Stats2_SetParamBoolValue((ANSC_HANDLE)pMoCAIfFullTable, paramName, bValue), TRUE);
}

//Test for Stats2_SetParamIntValue - Success
TEST_F(CcspMoCATestFixture, Stats2_SetParamIntValueSuccess)
{
    EXPECT_EQ(Stats2_SetParamIntValue(NULL, NULL, 0), FALSE);
}

//Test for Stats2_SetParamUlongValue - Success
TEST_F(CcspMoCATestFixture, Stats2_SetParamUlongValueSuccess)
{
    EXPECT_EQ(Stats2_SetParamUlongValue(NULL, NULL, 0), FALSE);
}

//Test for Stats2_SetParamStringValue - Success
TEST_F(CcspMoCATestFixture, Stats2_SetParamStringValueSuccess)
{
    EXPECT_EQ(Stats2_SetParamStringValue(NULL, NULL, NULL), 0);
}

//Test for Stats2_Validate - Success
TEST_F(CcspMoCATestFixture, Stats2_ValidateSuccess)
{
    EXPECT_EQ(Stats2_Validate(NULL, NULL, NULL), TRUE);
}

//Test for Stats2_Commit - Success
TEST_F(CcspMoCATestFixture, Stats2_CommitSuccess)
{
    EXPECT_EQ(Stats2_Commit(NULL), 0);
}

//Test for Stats2_Rollback - Success
TEST_F(CcspMoCATestFixture, Stats2_RollbackSuccess)
{
    EXPECT_EQ(Stats2_Rollback(NULL), 0);
}

//Test for X_CISCO_COM_ExtCounterTable_GetEntryCount - Success
TEST_F(CcspMoCATestFixture, X_CISCO_COM_ExtCounterTable_GetEntryCountSuccess)
{
    PCOSA_DML_MOCA_IF_FULL_TABLE pMoCAIfFullTable = (PCOSA_DML_MOCA_IF_FULL_TABLE)malloc(sizeof(COSA_DML_MOCA_IF_FULL_TABLE));
    memset(pMoCAIfFullTable, 0, sizeof(COSA_DML_MOCA_IF_FULL_TABLE));

    PCOSA_DML_MOCA_IF_FULL pMoCAIfFull = (PCOSA_DML_MOCA_IF_FULL)malloc(sizeof(COSA_DML_MOCA_IF_FULL));
    memset(pMoCAIfFull, 0, sizeof(COSA_DML_MOCA_IF_FULL));

    pMoCAIfFull->Cfg.InstanceNumber = 3;

    EXPECT_EQ(X_CISCO_COM_ExtCounterTable_GetEntryCount((ANSC_HANDLE)pMoCAIfFullTable), 0);

    free(pMoCAIfFull);
    pMoCAIfFull = NULL;

    free(pMoCAIfFullTable);
    pMoCAIfFullTable = NULL;
}

//Test for X_CISCO_COM_ExtCounterTable_GetEntry - Success
TEST_F(CcspMoCATestFixture, X_CISCO_COM_ExtCounterTable_GetEntrySuccess)
{
    ULONG ulIndex = 0;
    ULONG pNumber = 0;

    extern ANSC_HANDLE g_MoCAObject;
    g_MoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));

    PCOSA_DML_MOCA_IF_FULL pMoCAIfFull = (PCOSA_DML_MOCA_IF_FULL)malloc(sizeof(COSA_DML_MOCA_IF_FULL));
    memset(pMoCAIfFull, 0, sizeof(COSA_DML_MOCA_IF_FULL));

    pMoCAIfFull->Cfg.InstanceNumber = 1;

    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_mocaHALMock, moca_GetNumAssociatedDevices(_,_)).Times(1).WillOnce(DoAll(SetArgPointee<1>(1), Return(0)));
    EXPECT_CALL(*g_mocaHALMock, moca_IfGetExtCounter(_,_)).Times(1).WillOnce(Return(0));
    EXPECT_NE(X_CISCO_COM_ExtCounterTable_GetEntry((ANSC_HANDLE)pMoCAIfFull, ulIndex, &pNumber), nullptr);

    free(pMoCAIfFull);
    pMoCAIfFull = NULL;

    free(g_MoCAObject);
    g_MoCAObject = NULL;
}

//Test for X_CISCO_COM_ExtCounterTable_GetParamBoolValue - Success
TEST_F(CcspMoCATestFixture, X_CISCO_COM_ExtCounterTable_GetParamBoolValueSuccess)
{
    EXPECT_EQ(X_CISCO_COM_ExtCounterTable_GetParamBoolValue(NULL, NULL, NULL), FALSE);
}

//Test for X_CISCO_COM_ExtCounterTable_GetParamIntValue - Success
TEST_F(CcspMoCATestFixture, X_CISCO_COM_ExtCounterTable_GetParamIntValueSuccess)
{
    EXPECT_EQ(X_CISCO_COM_ExtCounterTable_GetParamIntValue(NULL, NULL, NULL), FALSE);
}

//Test for X_CISCO_COM_ExtCounterTable_GetParamUlongValue - Success
TEST_F(CcspMoCATestFixture, X_CISCO_COM_ExtCounterTable_GetParamUlongValueSuccess)
{
    ULONG ulValue = 0;

    char paramName1[8] = "Type";
    char paramName2[8] = "Map";
    char paramName3[8] = "Rsrv";
    char paramName4[8] = "Lc";
    char paramName5[8] = "Adm";
    char paramName6[8] = "Probe";
    char paramName7[8] = "Async";

    PCOSA_CONTEXT_MOCA_LINK_OBJECT pCosaContext = (PCOSA_CONTEXT_MOCA_LINK_OBJECT)malloc(sizeof(COSA_CONTEXT_MOCA_LINK_OBJECT));
    memset(pCosaContext, 0, sizeof(COSA_CONTEXT_MOCA_LINK_OBJECT));

    PCOSA_DML_MOCA_EXTCOUNTER pMoCAExtCounter = (PCOSA_DML_MOCA_EXTCOUNTER)malloc(sizeof(COSA_DML_MOCA_EXTCOUNTER));
    memset(pMoCAExtCounter, 0, sizeof(COSA_DML_MOCA_EXTCOUNTER));

    pCosaContext->hContext = (ANSC_HANDLE)pMoCAExtCounter;
    pCosaContext->InterfaceIndex = 1;
    pCosaContext->Index = 0;

    pMoCAExtCounter->Type = 1;
    pMoCAExtCounter->Map = 2;
    pMoCAExtCounter->Rsrv = 3;
    pMoCAExtCounter->Lc = 4;
    pMoCAExtCounter->Adm = 5;
    pMoCAExtCounter->Probe = 6;
    pMoCAExtCounter->Async = 7;

//For Type Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).Times(1).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_EQ(X_CISCO_COM_ExtCounterTable_GetParamUlongValue((ANSC_HANDLE)pCosaContext, paramName1, &ulValue), TRUE);
    EXPECT_EQ(ulValue, 1);

//For Map Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(2)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_EQ(X_CISCO_COM_ExtCounterTable_GetParamUlongValue((ANSC_HANDLE)pCosaContext, paramName2, &ulValue), TRUE);
    EXPECT_EQ(ulValue, 2);

//For Rsrv Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(3)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_EQ(X_CISCO_COM_ExtCounterTable_GetParamUlongValue((ANSC_HANDLE)pCosaContext, paramName3, &ulValue), TRUE);
    EXPECT_EQ(ulValue, 3);

//For Lc Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(4)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_EQ(X_CISCO_COM_ExtCounterTable_GetParamUlongValue((ANSC_HANDLE)pCosaContext, paramName4, &ulValue), TRUE);
    EXPECT_EQ(ulValue, 4);

//For Adm Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(5)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_EQ(X_CISCO_COM_ExtCounterTable_GetParamUlongValue((ANSC_HANDLE)pCosaContext, paramName5, &ulValue), TRUE);
    EXPECT_EQ(ulValue, 5);

//For Probe Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(6)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_EQ(X_CISCO_COM_ExtCounterTable_GetParamUlongValue((ANSC_HANDLE)pCosaContext, paramName6, &ulValue), TRUE);
    EXPECT_EQ(ulValue, 6);

//For Async Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(7)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_EQ(X_CISCO_COM_ExtCounterTable_GetParamUlongValue((ANSC_HANDLE)pCosaContext, paramName7, &ulValue), TRUE);
    EXPECT_EQ(ulValue, 7);

    free(pMoCAExtCounter);
    pMoCAExtCounter = NULL;

    free(pCosaContext);
    pCosaContext = NULL;
}

//Test for X_CISCO_COM_ExtCounterTable_GetParamStringValue - Success
TEST_F(CcspMoCATestFixture, X_CISCO_COM_ExtCounterTable_GetParamStringValueSuccess)
{
    EXPECT_NE(X_CISCO_COM_ExtCounterTable_GetParamStringValue(NULL, NULL, NULL, NULL), 0);
}

//Test for X_CISCO_COM_ExtAggrCounterTable_GetEntryCount - Success
TEST_F(CcspMoCATestFixture, X_CISCO_COM_ExtAggrCounterTable_GetEntryCountSuccess)
{
    PCOSA_DML_MOCA_IF_FULL pMoCAIfFull = (PCOSA_DML_MOCA_IF_FULL)malloc(sizeof(COSA_DML_MOCA_IF_FULL));
    memset(pMoCAIfFull, 0, sizeof(COSA_DML_MOCA_IF_FULL));

    pMoCAIfFull->Cfg.InstanceNumber = 1;

    EXPECT_EQ(X_CISCO_COM_ExtAggrCounterTable_GetEntryCount((ANSC_HANDLE)pMoCAIfFull), 1);

    free(pMoCAIfFull);
    pMoCAIfFull = NULL;
}

//Test for X_CISCO_COM_ExtAggrCounterTable_GetEntry - Success
TEST_F(CcspMoCATestFixture, X_CISCO_COM_ExtAggrCounterTable_GetEntrySuccess)
{
    ULONG ulIndex = 0;
    ULONG pNumber = 0;

    extern ANSC_HANDLE g_MoCAObject;
    g_MoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));

    PCOSA_DML_MOCA_IF_FULL pMoCAIfFull = (PCOSA_DML_MOCA_IF_FULL)malloc(sizeof(COSA_DML_MOCA_IF_FULL));
    memset(pMoCAIfFull, 0, sizeof(COSA_DML_MOCA_IF_FULL));

    pMoCAIfFull->Cfg.InstanceNumber = 1;

    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_mocaHALMock, moca_IfGetExtAggrCounter(_,_)).Times(1).WillOnce(Return(0));
    EXPECT_NE(X_CISCO_COM_ExtAggrCounterTable_GetEntry((ANSC_HANDLE)pMoCAIfFull, ulIndex, &pNumber), nullptr);

    free(pMoCAIfFull);
    pMoCAIfFull = NULL;

    free(g_MoCAObject);
    g_MoCAObject = NULL;
}

//Test for X_CISCO_COM_ExtAggrCounterTable_GetParamBoolValue - Success
TEST_F(CcspMoCATestFixture, X_CISCO_COM_ExtAggrCounterTable_GetParamBoolValueSuccess)
{
    EXPECT_EQ(X_CISCO_COM_ExtAggrCounterTable_GetParamBoolValue(NULL, NULL, NULL), FALSE);
}

//Test for X_CISCO_COM_ExtAggrCounterTable_GetParamIntValue - Success
TEST_F(CcspMoCATestFixture, X_CISCO_COM_ExtAggrCounterTable_GetParamIntValueSuccess)
{
    EXPECT_EQ(X_CISCO_COM_ExtAggrCounterTable_GetParamIntValue(NULL, NULL, NULL), FALSE);
}

//Test for X_CISCO_COM_ExtAggrCounterTable_GetParamUlongValue - Success
TEST_F(CcspMoCATestFixture, X_CISCO_COM_ExtAggrCounterTable_GetParamUlongValueSuccess)
{
    ULONG ulValue = 0;

    char paramName1[4] = "Tx";
    char paramName2[4] = "Rx";

    PCOSA_CONTEXT_MOCA_LINK_OBJECT pCosaContext = (PCOSA_CONTEXT_MOCA_LINK_OBJECT)malloc(sizeof(COSA_CONTEXT_MOCA_LINK_OBJECT));
    memset(pCosaContext, 0, sizeof(COSA_CONTEXT_MOCA_LINK_OBJECT));

    PCOSA_DML_MOCA_EXTAGGRCOUNTER pMoCAExtCounter = (PCOSA_DML_MOCA_EXTAGGRCOUNTER)malloc(sizeof(COSA_DML_MOCA_EXTAGGRCOUNTER));
    memset(pMoCAExtCounter, 0, sizeof(COSA_DML_MOCA_EXTAGGRCOUNTER));
    pCosaContext->hContext = (ANSC_HANDLE)pMoCAExtCounter;

    pCosaContext->InterfaceIndex = 1;
    pCosaContext->Index = 0;

    pMoCAExtCounter->Tx = 1;
    pMoCAExtCounter->Rx = 2;

//For Tx Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).Times(1).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_EQ(X_CISCO_COM_ExtAggrCounterTable_GetParamUlongValue((ANSC_HANDLE)pCosaContext, paramName1, &ulValue), TRUE);
    EXPECT_EQ(ulValue, 1);

//For Rx Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(2)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_EQ(X_CISCO_COM_ExtAggrCounterTable_GetParamUlongValue((ANSC_HANDLE)pCosaContext, paramName2, &ulValue), TRUE);
    EXPECT_EQ(ulValue, 2);

    free(pMoCAExtCounter);
    pMoCAExtCounter = NULL;

    free(pCosaContext);
    pCosaContext = NULL;
}

//Test for X_CISCO_COM_ExtAggrCounterTable_GetParamStringValue - Success
TEST_F(CcspMoCATestFixture, X_CISCO_COM_ExtAggrCounterTable_GetParamStringValueSuccess)
{
    EXPECT_NE(X_CISCO_COM_ExtAggrCounterTable_GetParamStringValue(NULL, NULL, NULL, NULL), 0);
}

//Test for QoS_GetParamBoolValue - Success
TEST_F(CcspMoCATestFixture, QoS_GetParamBoolValueSuccess)
{
    BOOL bValue = FALSE;
    char paramName[24] = "X_CISCO_COM_Enabled";

    PCOSA_DML_MOCA_IF_FULL_TABLE pMoCAIfFullTable = (PCOSA_DML_MOCA_IF_FULL_TABLE)malloc(sizeof(COSA_DML_MOCA_IF_FULL_TABLE));
    memset(pMoCAIfFullTable, 0, sizeof(COSA_DML_MOCA_IF_FULL_TABLE));

    pMoCAIfFullTable->MoCAIfQos.Enabled = TRUE;

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(QoS_GetParamBoolValue((ANSC_HANDLE)pMoCAIfFullTable, paramName, &bValue), TRUE);
    EXPECT_EQ(bValue, TRUE);
}

//Test for QoS_GetParamIntValue - Success
TEST_F(CcspMoCATestFixture, QoS_GetParamIntValueSuccess)
{
    EXPECT_EQ(QoS_GetParamIntValue(NULL, NULL, NULL), FALSE);
}

//Test for QoS_GetParamUlongValue - Success
TEST_F(CcspMoCATestFixture, QoS_GetParamUlongValueSuccess)
{
    char paramName1[16] = "EgressNumFlows";
    char paramName2[16] = "IngressNumFlows";

//For EgressNumFlows Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).Times(1).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(QoS_GetParamUlongValue(NULL, paramName1, NULL), TRUE);

//For IngressNumFlows Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(2)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(QoS_GetParamUlongValue(NULL, paramName2, NULL), TRUE);
}

//Test for QoS_GetParamStringValue - Success
TEST_F(CcspMoCATestFixture, QoS_GetParamStringValueSuccess)
{
    EXPECT_NE(QoS_GetParamStringValue(NULL, NULL, NULL, NULL), 0);
}

//Test for Qos_SetParamBoolValue - Success
TEST_F(CcspMoCATestFixture, Qos_SetParamBoolValueSuccess)
{
    char paramName[24] = "X_CISCO_COM_Enabled";
    BOOL bValue = TRUE;

    PCOSA_DML_MOCA_IF_FULL_TABLE pMoCAIfFullTable = (PCOSA_DML_MOCA_IF_FULL_TABLE)malloc(sizeof(COSA_DML_MOCA_IF_FULL_TABLE));
    memset(pMoCAIfFullTable, 0, sizeof(COSA_DML_MOCA_IF_FULL_TABLE));

    pMoCAIfFullTable->MoCAIfQos.Enabled = FALSE;

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Qos_SetParamBoolValue((ANSC_HANDLE)pMoCAIfFullTable, paramName, bValue), TRUE);
    EXPECT_EQ(pMoCAIfFullTable->MoCAIfQos.Enabled, TRUE);
}

//Test for Qos_SetParamIntValue - Success
TEST_F(CcspMoCATestFixture, Qos_SetParamIntValueSuccess)
{
    EXPECT_EQ(Qos_SetParamIntValue(NULL, NULL, 0), FALSE);
}

//Test for Qos_SetParamUlongValue - Success
TEST_F(CcspMoCATestFixture, Qos_SetParamUlongValueSuccess)
{
    EXPECT_EQ(Qos_SetParamUlongValue(NULL, NULL, 0), FALSE);
}

//Test for Qos_SetParamStringValue - Success
TEST_F(CcspMoCATestFixture, Qos_SetParamStringValueSuccess)
{
    EXPECT_EQ(Qos_SetParamStringValue(NULL, NULL, NULL), 0);
}

//Test for Qos_Validate - Success
TEST_F(CcspMoCATestFixture, Qos_ValidateSuccess)
{
    EXPECT_EQ(Qos_Validate(NULL, NULL, NULL), TRUE);
}

//Test for Qos_Commit - Success
TEST_F(CcspMoCATestFixture, Qos_CommitSuccess)
{
    extern ANSC_HANDLE g_MoCAObject;

    PCOSA_DATAMODEL_MOCA pMoCA = (PCOSA_DATAMODEL_MOCA)malloc(sizeof(COSA_DATAMODEL_MOCA));
    memset(pMoCA, 0, sizeof(COSA_DATAMODEL_MOCA));

    PCOSA_DML_MOCA_IF_FULL pMoCAIfFull = (PCOSA_DML_MOCA_IF_FULL)malloc(sizeof(COSA_DML_MOCA_IF_FULL));
    memset(pMoCAIfFull, 0, sizeof(COSA_DML_MOCA_IF_FULL));

    PCOSA_DML_MOCA_IF_FULL_TABLE pMoCAIfFullTable = (PCOSA_DML_MOCA_IF_FULL_TABLE)malloc(sizeof(COSA_DML_MOCA_IF_FULL_TABLE));
    memset(pMoCAIfFullTable, 0, sizeof(COSA_DML_MOCA_IF_FULL_TABLE));

    PCOSA_DML_MOCA_QOS pMoCAQos = &pMoCAIfFullTable->MoCAIfQos;
    pMoCAQos = (PCOSA_DML_MOCA_QOS)malloc(sizeof(COSA_DML_MOCA_QOS));
    memset(pMoCAQos, 0, sizeof(COSA_DML_MOCA_QOS));

    pMoCA->MoCAIfFullTable[0] = *pMoCAIfFullTable;
    g_MoCAObject = (ANSC_HANDLE)pMoCA;

    pMoCAQos->Enabled = FALSE;
    pMoCAIfFull->Cfg.InstanceNumber = 1;

    EXPECT_EQ(Qos_Commit(pMoCAIfFull), 0);

    free(pMoCAQos);
    pMoCAQos = NULL;

    free(pMoCAIfFullTable);
    pMoCAIfFullTable = NULL;

    free(pMoCAIfFull);
    pMoCAIfFull = NULL;

    free(pMoCA);
    pMoCA = NULL;
}

//Test for Qos_Rollback - Success
TEST_F(CcspMoCATestFixture, Qos_RollbackSuccess)
{
    EXPECT_EQ(Qos_Rollback(NULL), 0);
}

//Test for FlowStats_GetEntryCount - Success
TEST_F(CcspMoCATestFixture, FlowStats_GetEntryCountSuccess)
{
    extern ANSC_HANDLE g_MoCAObject;

    PCOSA_DATAMODEL_MOCA pMoCA = (PCOSA_DATAMODEL_MOCA)malloc(sizeof(COSA_DATAMODEL_MOCA));
    memset(pMoCA, 0, sizeof(COSA_DATAMODEL_MOCA));

    PCOSA_DML_MOCA_IF_FULL pMoCAIfFull = (PCOSA_DML_MOCA_IF_FULL)malloc(sizeof(COSA_DML_MOCA_IF_FULL));
    memset(pMoCAIfFull, 0, sizeof(COSA_DML_MOCA_IF_FULL));

    pMoCAIfFull->Cfg.InstanceNumber = 1;
    pMoCA->MoCAIfFullTable[0].ulMoCAFlowTableCount = 1;
    g_MoCAObject = (ANSC_HANDLE)pMoCA;

    EXPECT_EQ(FlowStats_GetEntryCount((ANSC_HANDLE)pMoCAIfFull), 1);

    free(pMoCAIfFull);
    pMoCAIfFull = NULL;

    free(pMoCA);
    pMoCA = NULL;
}

//Test for FlowStats_GetEntry - Success
TEST_F(CcspMoCATestFixture, FlowStats_GetEntrySuccess)
{
    ULONG ulIndex = 0;
    ULONG pNumber = 0;

    extern ANSC_HANDLE g_MoCAObject;

    PCOSA_DATAMODEL_MOCA pMoCA = (PCOSA_DATAMODEL_MOCA)malloc(sizeof(COSA_DATAMODEL_MOCA));
    memset(pMoCA, 0, sizeof(COSA_DATAMODEL_MOCA));

    PCOSA_DML_MOCA_FLOW pMoCAFlow = (PCOSA_DML_MOCA_FLOW)malloc(sizeof(COSA_DML_MOCA_FLOW));
    memset(pMoCAFlow, 0, sizeof(COSA_DML_MOCA_FLOW));

    PCOSA_DML_MOCA_IF_FULL pMoCAIfFull = (PCOSA_DML_MOCA_IF_FULL)malloc(sizeof(COSA_DML_MOCA_IF_FULL));
    memset(pMoCAIfFull, 0, sizeof(COSA_DML_MOCA_IF_FULL));

    pMoCAIfFull->Cfg.InstanceNumber = 1;
    pMoCA->MoCAIfFullTable[0].ulMoCAFlowTableCount = 1;
    pMoCA->MoCAIfFullTable[0].pMoCAFlowTable = &pMoCAFlow[0];

    g_MoCAObject = (ANSC_HANDLE)pMoCA;

    EXPECT_NE(FlowStats_GetEntry((ANSC_HANDLE)pMoCAIfFull, ulIndex, &pNumber), nullptr);
    EXPECT_EQ(pNumber, 1);

    free(pMoCAIfFull);
    pMoCAIfFull = NULL;

    free(pMoCAFlow);
    pMoCAFlow = NULL;

    free(pMoCA);
    pMoCA = NULL;
}

//Test for FlowStats_IsUpdated - Success
TEST_F(CcspMoCATestFixture, FlowStats_GetParamBoolValueSuccess)
{
//For tick = 0
    EXPECT_CALL(*g_usertimeMock, UserGetTickInSeconds2()).Times(1).WillOnce(Return(5));
    EXPECT_EQ(FlowStats_IsUpdated(NULL), TRUE);

//For tick with current time value
    EXPECT_CALL(*g_usertimeMock, UserGetTickInSeconds2()).Times(2).WillRepeatedly(Return(6));
    EXPECT_EQ(FlowStats_IsUpdated(NULL), FALSE);

//For tick with more than MoCA refresh time value
    EXPECT_CALL(*g_usertimeMock, UserGetTickInSeconds2()).Times(3).WillRepeatedly(Return(15));
    EXPECT_EQ(FlowStats_IsUpdated(NULL), TRUE);
}

//Test for FlowStats_Synchronize - Success
TEST_F(CcspMoCATestFixture, FlowStats_SynchronizeSuccess)
{
    extern ANSC_HANDLE g_MoCAObject;

    PCOSA_DATAMODEL_MOCA pMoCA = (PCOSA_DATAMODEL_MOCA)malloc(sizeof(COSA_DATAMODEL_MOCA));
    memset(pMoCA, 0, sizeof(COSA_DATAMODEL_MOCA));

    PCOSA_DML_MOCA_FLOW pMoCAFlow = (PCOSA_DML_MOCA_FLOW)malloc(sizeof(COSA_DML_MOCA_FLOW));
    memset(pMoCAFlow, 0, sizeof(COSA_DML_MOCA_FLOW));

    PCOSA_DML_MOCA_IF_FULL pMoCAIfFull = (PCOSA_DML_MOCA_IF_FULL)malloc(sizeof(COSA_DML_MOCA_IF_FULL));
    memset(pMoCAIfFull, 0, sizeof(COSA_DML_MOCA_IF_FULL));

    pMoCAIfFull->Cfg.InstanceNumber = 1;
    pMoCA->MoCAIfFullTable[0].ulMoCAFlowTableCount = 1;
    pMoCA->MoCAIfFullTable[0].pMoCAFlowTable = &pMoCAFlow[0];

    g_MoCAObject = (ANSC_HANDLE)pMoCA;

    EXPECT_CALL(*g_mocaHALMock, moca_GetFlowStatistics(_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_anscMemoryMock, AnscFreeMemory(_)).Times(1);
    EXPECT_EQ(FlowStats_Synchronize((ANSC_HANDLE)pMoCAIfFull), 0);

    free(pMoCAIfFull);
    pMoCAIfFull = NULL;

    free(pMoCAFlow);
    pMoCAFlow = NULL;

    free(pMoCA);
    pMoCA = NULL;
}

//Test for FlowStats_GetParamUlongValue - Success
TEST_F(CcspMoCATestFixture, FlowStats_GetParamUlongValueSuccess)
{
    ULONG ulValue = 0;

    char paramName1[16] = "FlowID";
    char paramName2[16] = "IngressNodeID";
    char paramName3[16] = "EgressNodeID";
    char paramName4[16] = "LeaseTimeLeft";
    char paramName5[16] = "FlowPackets";
    char paramName6[16] = "MaxRate";
    char paramName7[16] = "MaxBurstSize";
    char paramName8[16] = "FlowTag";
    char paramName9[16] = "LeaseTime";

    PCOSA_DML_MOCA_FLOW pMoCAFlow = (PCOSA_DML_MOCA_FLOW)malloc(sizeof(COSA_DML_MOCA_FLOW));
    memset(pMoCAFlow, 0, sizeof(COSA_DML_MOCA_FLOW));

    pMoCAFlow->FlowID = 1;
    pMoCAFlow->IngressNodeID = 2;
    pMoCAFlow->EgressNodeID = 3;
    pMoCAFlow->FlowTimeLeft = 4;
    pMoCAFlow->PacketSize = 5;
    pMoCAFlow->PeakDataRate = 6;
    pMoCAFlow->BurstSize = 7;
    pMoCAFlow->FlowTag = 8;
    pMoCAFlow->LeaseTime = 9;

//For FlowID Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).Times(1).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(FlowStats_GetParamUlongValue((ANSC_HANDLE)pMoCAFlow, paramName1, &ulValue), TRUE);
    EXPECT_EQ(ulValue, 1);

//For IngressNodeID Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(2)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(FlowStats_GetParamUlongValue((ANSC_HANDLE)pMoCAFlow, paramName2, &ulValue), TRUE);
    EXPECT_EQ(ulValue, 2);

//For EgressNodeID Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(3)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(FlowStats_GetParamUlongValue((ANSC_HANDLE)pMoCAFlow, paramName3, &ulValue), TRUE);
    EXPECT_EQ(ulValue, 3);

//For LeaseTimeLeft Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(4)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(FlowStats_GetParamUlongValue((ANSC_HANDLE)pMoCAFlow, paramName4, &ulValue), TRUE);
    EXPECT_EQ(ulValue, 4);

//For FlowPackets Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(5)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(FlowStats_GetParamUlongValue((ANSC_HANDLE)pMoCAFlow, paramName5, &ulValue), TRUE);
    EXPECT_EQ(ulValue, 5);

//For MaxRate Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(6)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(FlowStats_GetParamUlongValue((ANSC_HANDLE)pMoCAFlow, paramName6, &ulValue), TRUE);
    EXPECT_EQ(ulValue, 6);

//For MaxBurstSize Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(7)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(FlowStats_GetParamUlongValue((ANSC_HANDLE)pMoCAFlow, paramName7, &ulValue), TRUE);
    EXPECT_EQ(ulValue, 7);

//For FlowTag Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(8)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(FlowStats_GetParamUlongValue((ANSC_HANDLE)pMoCAFlow, paramName8, &ulValue), TRUE);
    EXPECT_EQ(ulValue, 8);

//For LeaseTime Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(9)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(FlowStats_GetParamUlongValue((ANSC_HANDLE)pMoCAFlow, paramName9, &ulValue), TRUE);
    EXPECT_EQ(ulValue, 9);

    free(pMoCAFlow);
    pMoCAFlow = NULL;
}

//Test for FlowStats_GetParamStringValue - Success and Failure
TEST_F(CcspMoCATestFixture, FlowStats_GetParamStringValueSuccess)
{
    char paramName[16] = "PacketDA";
    char pValue[18] = {0};
    ULONG ulSize = 0;

    PCOSA_DML_MOCA_FLOW pMoCAFlow = (PCOSA_DML_MOCA_FLOW)malloc(sizeof(COSA_DML_MOCA_FLOW));
    memset(pMoCAFlow, 0, sizeof(COSA_DML_MOCA_FLOW));

    strcpy(pMoCAFlow->DestinationMACAddress, "1234567890AB");

//For PacketDA Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_EQ(FlowStats_GetParamStringValue((ANSC_HANDLE)pMoCAFlow, paramName, pValue, &ulSize), 0);

//For PacketDA Failure
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).Times(1).WillOnce(Return(1));
    EXPECT_NE(FlowStats_GetParamStringValue((ANSC_HANDLE)pMoCAFlow, paramName, pValue, &ulSize), 0);

    free(pMoCAFlow);
    pMoCAFlow = NULL;
}

//Test for AssociatedDevice_GetEntryCount - Success
TEST_F(CcspMoCATestFixture, AssociatedDevice_GetEntryCountSuccess)
{
    PCOSA_DML_MOCA_IF_FULL_TABLE pMoCAIfFullTable = (PCOSA_DML_MOCA_IF_FULL_TABLE)malloc(sizeof(COSA_DML_MOCA_IF_FULL_TABLE));
    memset(pMoCAIfFullTable, 0, sizeof(COSA_DML_MOCA_IF_FULL_TABLE));

    pMoCAIfFullTable->ulMoCAAssocDeviceCount = 1;

    EXPECT_EQ(AssociatedDevice_GetEntryCount((ANSC_HANDLE)pMoCAIfFullTable), 1);

    free(pMoCAIfFullTable);
    pMoCAIfFullTable = NULL;
}

//Test for AssociatedDevice_GetEntry - Success
TEST_F(CcspMoCATestFixture, AssociatedDevice_GetEntrySuccess)
{
    ULONG ulIndex = 0;
    ULONG pNumber = 0;


    PCOSA_DML_MOCA_ASSOC_DEVICE pMoCAAssocDevice = (PCOSA_DML_MOCA_ASSOC_DEVICE)malloc(sizeof(COSA_DML_MOCA_ASSOC_DEVICE));
    memset(pMoCAAssocDevice, 0, sizeof(COSA_DML_MOCA_ASSOC_DEVICE));

    PCOSA_DML_MOCA_IF_FULL_TABLE pMoCAIfFullTable = (PCOSA_DML_MOCA_IF_FULL_TABLE)malloc(sizeof(COSA_DML_MOCA_IF_FULL_TABLE));
    memset(pMoCAIfFullTable, 0, sizeof(COSA_DML_MOCA_IF_FULL_TABLE));

    pMoCAIfFullTable->ulMoCAAssocDeviceCount = 1;
    pMoCAIfFullTable->pMoCAAssocDevice = &pMoCAAssocDevice[0];

    EXPECT_NE(AssociatedDevice_GetEntry((ANSC_HANDLE)pMoCAIfFullTable, ulIndex, &pNumber), nullptr);
    EXPECT_EQ(pNumber, 1);

    free(pMoCAAssocDevice);
    pMoCAAssocDevice = NULL;

    free(pMoCAIfFullTable);
    pMoCAIfFullTable = NULL;
}

//Test for AssociatedDevice_IsUpdated - Success
TEST_F(CcspMoCATestFixture, AssociatedDevice_IsUpdatedSuccess)
{
//For tick = 0
    EXPECT_CALL(*g_usertimeMock, UserGetTickInSeconds2()).Times(1).WillOnce(Return(5));
    EXPECT_EQ(AssociatedDevice_IsUpdated(NULL), TRUE);

//For tick with current time value
    EXPECT_CALL(*g_usertimeMock, UserGetTickInSeconds2()).Times(2).WillRepeatedly(Return(6));
    EXPECT_EQ(AssociatedDevice_IsUpdated(NULL), FALSE);

//For tick with more than MoCA refresh time value
    EXPECT_CALL(*g_usertimeMock, UserGetTickInSeconds2()).Times(3).WillRepeatedly(Return(15));
    EXPECT_EQ(AssociatedDevice_IsUpdated(NULL), TRUE);
}

//Test for AssociatedDevice_Synchronize - Success
TEST_F(CcspMoCATestFixture, AssociatedDevice_SynchronizeSuccess)
{

    PCOSA_DML_MOCA_ASSOC_DEVICE pMoCAAssocDevice = (PCOSA_DML_MOCA_ASSOC_DEVICE)malloc(sizeof(COSA_DML_MOCA_ASSOC_DEVICE));
    memset(pMoCAAssocDevice, 0, sizeof(COSA_DML_MOCA_ASSOC_DEVICE));

    PCOSA_DML_MOCA_IF_FULL_TABLE pMoCAIfFullTable = (PCOSA_DML_MOCA_IF_FULL_TABLE)malloc(sizeof(COSA_DML_MOCA_IF_FULL_TABLE));
    memset(pMoCAIfFullTable, 0, sizeof(COSA_DML_MOCA_IF_FULL_TABLE));


    pMoCAIfFullTable->ulMoCAAssocDeviceCount = 1;
    pMoCAIfFullTable->MoCAIfFull.Cfg.InstanceNumber = 1;
    pMoCAIfFullTable->pMoCAAssocDevice = &pMoCAAssocDevice[0];

    EXPECT_CALL(*g_mocaHALMock, moca_GetMocaCPEs(_,_,_)).Times(1).WillOnce(DoAll(SetArgPointee<2>(1), Return(0)));
    EXPECT_CALL(*g_mocaHALMock, moca_GetNumAssociatedDevices(_,_)).Times(1).WillOnce(DoAll(SetArgPointee<1>(1), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_,_,_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_safecLibMock, _memcpy_s_chk(_,_,_,_,_,_)).Times(2).WillRepeatedly(Return(0));
    EXPECT_CALL(*g_mocaHALMock, moca_GetAssociatedDevices(_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_anscMemoryMock, AnscFreeMemory(_)).Times(2);
    EXPECT_EQ(AssociatedDevice_Synchronize((ANSC_HANDLE)pMoCAIfFullTable), 0);

    free(pMoCAAssocDevice);
    pMoCAAssocDevice = NULL;

    free(pMoCAIfFullTable);
    pMoCAIfFullTable = NULL;
}

//Test for AssociatedDevice_GetParamBoolValue - Success
TEST_F(CcspMoCATestFixture, AssociatedDevice_GetParamBoolValueSuccess)
{
    BOOL bValue = FALSE;
    char paramName1[16] = "PreferredNC";
    char paramName2[16] = "QAM256Capable";
    char paramName3[8] = "Active";

    PCOSA_DML_MOCA_ASSOC_DEVICE pMoCAAssocDevice = (PCOSA_DML_MOCA_ASSOC_DEVICE)malloc(sizeof(COSA_DML_MOCA_ASSOC_DEVICE));
    memset(pMoCAAssocDevice, 0, sizeof(COSA_DML_MOCA_ASSOC_DEVICE));

    pMoCAAssocDevice->PreferredNC = TRUE;
    pMoCAAssocDevice->QAM256Capable = TRUE;
    pMoCAAssocDevice->Active = TRUE;

//For PreferredNC Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(AssociatedDevice_GetParamBoolValue((ANSC_HANDLE)pMoCAAssocDevice, paramName1, &bValue), TRUE);
    EXPECT_EQ(bValue, TRUE);

//For QAM256Capable Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(2)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(AssociatedDevice_GetParamBoolValue((ANSC_HANDLE)pMoCAAssocDevice, paramName2, &bValue), TRUE);
    EXPECT_EQ(bValue, TRUE);

//For Active Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(3)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(AssociatedDevice_GetParamBoolValue((ANSC_HANDLE)pMoCAAssocDevice, paramName3, &bValue), TRUE);
    EXPECT_EQ(bValue, TRUE);

    free(pMoCAAssocDevice);
    pMoCAAssocDevice = NULL;
}

//Test for AssociatedDevice_GetParamIntValue - Success
TEST_F(CcspMoCATestFixture, AssociatedDevice_GetParamIntValueSuccess)
{
    int iValue = 0;
    char paramName1[16] = "RxPowerLevel";
    char paramName2[24] = "RxBcastPowerLevel";

    PCOSA_DML_MOCA_ASSOC_DEVICE pMoCAAssocDevice = (PCOSA_DML_MOCA_ASSOC_DEVICE)malloc(sizeof(COSA_DML_MOCA_ASSOC_DEVICE));
    memset(pMoCAAssocDevice, 0, sizeof(COSA_DML_MOCA_ASSOC_DEVICE));

    pMoCAAssocDevice->RxPowerLevel = 1;
    pMoCAAssocDevice->RxBcastPowerLevel = 2;

//For RxPowerLevel Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(AssociatedDevice_GetParamIntValue((ANSC_HANDLE)pMoCAAssocDevice, paramName1, &iValue), TRUE);
    EXPECT_EQ(iValue, 1);

//For RxBcastPowerLevel Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(2)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(AssociatedDevice_GetParamIntValue((ANSC_HANDLE)pMoCAAssocDevice, paramName2, &iValue), TRUE);
    EXPECT_EQ(iValue, 2);

    free(pMoCAAssocDevice);
    pMoCAAssocDevice = NULL;
}

//Test for AssociatedDevice_GetParamUlongValue - Success
TEST_F(CcspMoCATestFixture, AssociatedDevice_GetParamUlongValueSuccess)
{
    ULONG ulValue = 0;
    char paramName1[8] = "NodeID";
    char paramName2[16] = "PHYTxRate";
    char paramName3[16] = "PHYRxRate";
    char paramName4[32] = "TxPowerControlReduction";
    char paramName5[16] = "TxBcastRate";
    char paramName6[16] = "TxPackets";
    char paramName7[16] = "RxPackets";
    char paramName8[32] = "RxErroredAndMissedPackets";
    char paramName9[32] = "PacketAggregationCapability";
    char paramName10[8] = "RxSNR";
    char paramName11[32] = "X_CISCO_COM_RxBcastRate";
    char paramName12[32] = "X_CISCO_COM_NumberOfClients";

    PCOSA_DML_MOCA_ASSOC_DEVICE pMoCAAssocDevice = (PCOSA_DML_MOCA_ASSOC_DEVICE)malloc(sizeof(COSA_DML_MOCA_ASSOC_DEVICE));
    memset(pMoCAAssocDevice, 0, sizeof(COSA_DML_MOCA_ASSOC_DEVICE));

    pMoCAAssocDevice->NodeID = 1;
    pMoCAAssocDevice->PHYTxRate = 2;
    pMoCAAssocDevice->PHYRxRate = 3;
    pMoCAAssocDevice->TxPowerControlReduction = 4;
    pMoCAAssocDevice->TxBcastRate = 5;
    pMoCAAssocDevice->TxPackets = 6;
    pMoCAAssocDevice->RxPackets = 7;
    pMoCAAssocDevice->RxErroredAndMissedPackets = 8;
    pMoCAAssocDevice->PacketAggregationCapability = 9;
    pMoCAAssocDevice->RxSNR = 10;
    pMoCAAssocDevice->X_CISCO_COM_RxBcastRate = 11;
    pMoCAAssocDevice->X_CISCO_COM_NumberOfClients = 12;

//For NodeID Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).Times(1).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(AssociatedDevice_GetParamUlongValue((ANSC_HANDLE)pMoCAAssocDevice, paramName1, &ulValue), TRUE);
    EXPECT_EQ(ulValue, 1);

//For PHYTxRate Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(2)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(AssociatedDevice_GetParamUlongValue((ANSC_HANDLE)pMoCAAssocDevice, paramName2, &ulValue), TRUE);
    EXPECT_EQ(ulValue, 2);

//For PHYRxRate Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(3)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(AssociatedDevice_GetParamUlongValue((ANSC_HANDLE)pMoCAAssocDevice, paramName3, &ulValue), TRUE);
    EXPECT_EQ(ulValue, 3);

//For TxPowerControlReduction Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(4)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(AssociatedDevice_GetParamUlongValue((ANSC_HANDLE)pMoCAAssocDevice, paramName4, &ulValue), TRUE);
    EXPECT_EQ(ulValue, 4);

//For TxBcastRate Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(5)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(AssociatedDevice_GetParamUlongValue((ANSC_HANDLE)pMoCAAssocDevice, paramName5, &ulValue), TRUE);
    EXPECT_EQ(ulValue, 5);

//For TxPackets Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(6)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(AssociatedDevice_GetParamUlongValue((ANSC_HANDLE)pMoCAAssocDevice, paramName6, &ulValue), TRUE);
    EXPECT_EQ(ulValue, 6);

//For RxPackets Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(7)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(AssociatedDevice_GetParamUlongValue((ANSC_HANDLE)pMoCAAssocDevice, paramName7, &ulValue), TRUE);
    EXPECT_EQ(ulValue, 7);

//For RxErroredAndMissedPackets Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(8)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(AssociatedDevice_GetParamUlongValue((ANSC_HANDLE)pMoCAAssocDevice, paramName8, &ulValue), TRUE);
    EXPECT_EQ(ulValue, 8);

//For PacketAggregationCapability Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(9)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(AssociatedDevice_GetParamUlongValue((ANSC_HANDLE)pMoCAAssocDevice, paramName9, &ulValue), TRUE);
    EXPECT_EQ(ulValue, 9);

//For RxSNR Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(10)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(AssociatedDevice_GetParamUlongValue((ANSC_HANDLE)pMoCAAssocDevice, paramName10, &ulValue), TRUE);
    EXPECT_EQ(ulValue, 10);

//For X_CISCO_COM_RxBcastRate Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(11)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(AssociatedDevice_GetParamUlongValue((ANSC_HANDLE)pMoCAAssocDevice, paramName11, &ulValue), TRUE);
    EXPECT_EQ(ulValue, 11);

//For X_CISCO_COM_NumberOfClients Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(12)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(AssociatedDevice_GetParamUlongValue((ANSC_HANDLE)pMoCAAssocDevice, paramName12, &ulValue), TRUE);
    EXPECT_EQ(ulValue, 12);

    free(pMoCAAssocDevice);
    pMoCAAssocDevice = NULL;
}

//Test for AssociatedDevice_GetParamStringValue - Success and Failure
TEST_F(CcspMoCATestFixture, AssociatedDevice_GetParamStringValueSuccess)
{
    char pValue[18] = {0};
    ULONG ulSize = 0;
    char paramName1[16] = "MACAddress";
    char paramName2[16] = "HighestVersion";

    PCOSA_DML_MOCA_ASSOC_DEVICE pMoCAAssocDevice = (PCOSA_DML_MOCA_ASSOC_DEVICE)malloc(sizeof(COSA_DML_MOCA_ASSOC_DEVICE));
    memset(pMoCAAssocDevice, 0, sizeof(COSA_DML_MOCA_ASSOC_DEVICE));

    strcpy((char*)pMoCAAssocDevice->MACAddress, "1234567890AB");
    strcpy(pMoCAAssocDevice->HighestVersion, "1");

//For MACAddress Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(AssociatedDevice_GetParamStringValue((ANSC_HANDLE)pMoCAAssocDevice, paramName1, pValue, &ulSize), 0);

//For HighestVersion Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(2)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_EQ(AssociatedDevice_GetParamStringValue((ANSC_HANDLE)pMoCAAssocDevice, paramName2, pValue, &ulSize), 0);

//For HighestVersion Failure
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_))
                .Times(2)
                .WillOnce(DoAll(SetArgPointee<3>(1), Return(0)))
                .WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).Times(1).WillOnce(Return(1));
    EXPECT_NE(AssociatedDevice_GetParamStringValue((ANSC_HANDLE)pMoCAAssocDevice, paramName2, pValue, &ulSize), 0);

    free(pMoCAAssocDevice);
    pMoCAAssocDevice = NULL;
}

//Test for Logging_GetParamBoolValue - Success
TEST_F(CcspMoCATestFixture, Logging_GetParamBoolValueSuccess)
{
    BOOL bValue = FALSE;
    char paramName[24] = "xOpsDMMoCALogEnabled";

    extern ANSC_HANDLE g_MoCAObject;

    PCOSA_DATAMODEL_MOCA pMoCA = (PCOSA_DATAMODEL_MOCA)malloc(sizeof(COSA_DATAMODEL_MOCA));
    memset(pMoCA, 0, sizeof(COSA_DATAMODEL_MOCA));

    pMoCA->LogStatus.Log_Enable = TRUE;
    g_MoCAObject = (ANSC_HANDLE)pMoCA;

//For xOpsDMMoCALogEnabled Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Logging_GetParamBoolValue(NULL, paramName, &bValue), TRUE);
    EXPECT_EQ(bValue, TRUE);

    free(pMoCA);
    pMoCA = NULL;
}

//Test for Logging_GetParamUlongValue - Success
TEST_F(CcspMoCATestFixture, Logging_GetParamUlongValueSuccess)
{
    ULONG ulValue = 0;
    char paramName[24] = "xOpsDMMoCALogPeriod";

    extern ANSC_HANDLE g_MoCAObject;

    PCOSA_DATAMODEL_MOCA pMoCA = (PCOSA_DATAMODEL_MOCA)malloc(sizeof(COSA_DATAMODEL_MOCA));
    memset(pMoCA, 0, sizeof(COSA_DATAMODEL_MOCA));

    pMoCA->LogStatus.Log_Period = 100;
    g_MoCAObject = (ANSC_HANDLE)pMoCA;

//For xOpsDMMoCALogPeriod Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_EQ(Logging_GetParamUlongValue(NULL, paramName, &ulValue), TRUE);
    EXPECT_EQ(ulValue, 100);

    free(pMoCA);
    pMoCA = NULL;
}

//Test for Logging_SetParamBoolValue - Success and Failure
TEST_F(CcspMoCATestFixture, Logging_SetParamBoolValueSuccess)
{
    char paramName[24] = "xOpsDMMoCALogEnabled";

    extern ANSC_HANDLE g_MoCAObject;

    PCOSA_DATAMODEL_MOCA pMoCA = (PCOSA_DATAMODEL_MOCA)malloc(sizeof(COSA_DATAMODEL_MOCA));
    memset(pMoCA, 0, sizeof(COSA_DATAMODEL_MOCA));

    pMoCA->LogStatus.Log_Enable = FALSE;
    g_MoCAObject = (ANSC_HANDLE)pMoCA;

//For xOpsDMMoCALogEnabled True set Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_syscfgMock, syscfg_get(_,_,_,_))
                .Times(2)
                .WillOnce(Return(0))
                .WillRepeatedly(Return(1));
    EXPECT_CALL(*g_syscfgMock, syscfg_set_nns_commit(_,_)).Times(1).WillRepeatedly(Return(0));
    EXPECT_EQ(Logging_SetParamBoolValue(NULL, paramName, TRUE), TRUE);
    EXPECT_EQ(pMoCA->LogStatus.Log_Enable, TRUE);

//For xOpsDMMoCALogEnabled False set Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_syscfgMock, syscfg_get(_,_,_,_))
                .Times(2)
                .WillOnce(Return(1))
                .WillRepeatedly(Return(0));
    EXPECT_CALL(*g_syscfgMock, syscfg_set_nns_commit(_,_)).Times(1).WillRepeatedly(Return(0));
    EXPECT_EQ(Logging_SetParamBoolValue(NULL, paramName, FALSE), TRUE);
    EXPECT_EQ(pMoCA->LogStatus.Log_Enable, FALSE);

//For xOpsDMMoCALogEnabled True set Failure
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).Times(1).WillOnce(Return(1));
    EXPECT_EQ(Logging_SetParamBoolValue(NULL, paramName, TRUE), FALSE);

//For xOpsDMMoCALogEnabled False set Failure
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_,_,_,_)).Times(1).WillOnce(Return(1));
    EXPECT_EQ(Logging_SetParamBoolValue(NULL, paramName, FALSE), FALSE);

    free(pMoCA);
    pMoCA = NULL;
}

//Test for Logging_SetParamUlongValue - Success
TEST_F(CcspMoCATestFixture, Logging_SetParamUlongValueSuccess)
{
    char paramName[24] = "xOpsDMMoCALogPeriod";

    extern ANSC_HANDLE g_MoCAObject;

    PCOSA_DATAMODEL_MOCA pMoCA = (PCOSA_DATAMODEL_MOCA)malloc(sizeof(COSA_DATAMODEL_MOCA));
    memset(pMoCA, 0, sizeof(COSA_DATAMODEL_MOCA));

    pMoCA->LogStatus.Log_Period = 100;
    g_MoCAObject = (ANSC_HANDLE)pMoCA;

//For xOpsDMMoCALogPeriod Success
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_,_,_,_,_,_)).WillOnce(DoAll(SetArgPointee<3>(0), Return(0)));
    EXPECT_CALL(*g_syscfgMock, syscfg_get(_,_,_,_))
                .Times(2)
                .WillOnce(Return(1))
                .WillRepeatedly(Return(0));
    EXPECT_CALL(*g_syscfgMock, syscfg_set_nns_u_commit(_,_)).Times(1).WillOnce(Return(0));
    EXPECT_EQ(Logging_SetParamUlongValue(NULL, paramName, 200), TRUE);
    EXPECT_EQ(pMoCA->LogStatus.Log_Period, 200);

    free(pMoCA);
    pMoCA = NULL;
}

//Test for Logging_Validate - Success
TEST_F(CcspMoCATestFixture, Logging_ValidateSuccess)
{
    EXPECT_EQ(Logging_Validate(NULL, NULL, NULL), TRUE);
}

//Test for Logging_Commit - Success
TEST_F(CcspMoCATestFixture, Logging_CommitSuccess)
{
    EXPECT_EQ(Logging_Commit(NULL), 0);
}

//Test for Logging_Rollback - Success
TEST_F(CcspMoCATestFixture, Logging_RollbackSuccess)
{
    EXPECT_EQ(Logging_Rollback(NULL), 0);
}