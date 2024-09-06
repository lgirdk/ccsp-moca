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
    #include "cosa_moca_webconfig_api.h"
    #include "cosa_moca_internal.h"
}

using namespace testing;

extern MocaHalMock *g_mocaHALMock;
extern SyscfgMock *g_syscfgMock;
extern webconfigFwMock *g_webconfigFwMock;

//Test for getBlobVersion - Success
TEST_F(CcspMoCATestFixture, getBlobVersionSuccess)
{
    EXPECT_CALL(*g_syscfgMock, syscfg_get(_,_,_,_)).Times(1).WillOnce(DoAll(SetArgPointee<2>(1), Return(0)));

    uint32_t ret = getBlobVersion("moca");

    EXPECT_EQ(ret, 0);
}

//Test for getBlobVersion - Failure
TEST_F(CcspMoCATestFixture, getBlobVersionFailure)
{
    EXPECT_CALL(*g_syscfgMock, syscfg_get(_,_,_,_)).Times(1).WillOnce(Return(1));

    uint32_t ret = getBlobVersion("moca");

    EXPECT_EQ(ret, 0);
}

//Test for setBlobVersion - Success
TEST_F(CcspMoCATestFixture, setBlobVersionSuccess)
{
    EXPECT_CALL(*g_syscfgMock, syscfg_set_nns_u_commit(_,_)).Times(1).WillOnce(Return(0));

    int ret = setBlobVersion("moca", 1);

    EXPECT_EQ(ret, 0);
}

//Test for setBlobVersion - Failure
TEST_F(CcspMoCATestFixture, setBlobVersionFailure)
{
    EXPECT_CALL(*g_syscfgMock, syscfg_set_nns_u_commit(_,_)).Times(1).WillOnce(Return(1));

    int ret = setBlobVersion("moca", 1);

    EXPECT_NE(ret, 0);
}

//Test for Process_Moca_WebConfigRequest - Success
TEST_F(CcspMoCATestFixture, Process_Moca_WebConfigRequestSuccess)
{
    char pValueTrue[] = "1";
    mocadoc_t *doc = (mocadoc_t *)AnscAllocateMemory(sizeof(mocadoc_t));
    memset(doc, 0, sizeof(mocadoc_t));

    doc->param = (mocaparam_t *)AnscAllocateMemory(sizeof(mocaparam_t));
    memset(doc->param, 0, sizeof(mocaparam_t));

    doc->subdoc_name = strdup("moca");
    doc->transaction_id = 1234;
    doc->version = 1;
    doc->param->enable = true;

    extern ANSC_HANDLE g_MoCAObject;

    PCOSA_DATAMODEL_MOCA pMoCAObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));
    memset(pMoCAObject, 0, sizeof(COSA_DATAMODEL_MOCA));

    g_MoCAObject = (ANSC_HANDLE)pMoCAObject;

    PCOSA_DML_MOCA_CFG PCFG = (PCOSA_DML_MOCA_CFG)AnscAllocateMemory(sizeof(COSA_DML_MOCA_CFG));
    memset(PCFG, 0, sizeof(COSA_DML_MOCA_CFG));

    PCFG->bForceEnabled = false;
    pMoCAObject->MoCACfg = *PCFG;

    PCOSA_DML_MOCA_IF_CFG pCfg = (PCOSA_DML_MOCA_IF_CFG)AnscAllocateMemory(sizeof(COSA_DML_MOCA_IF_CFG));
    memset(pCfg, 0, sizeof(COSA_DML_MOCA_IF_CFG));

    PCOSA_DML_MOCA_IF_FULL_TABLE pMoCAIfFullTable = (PCOSA_DML_MOCA_IF_FULL_TABLE)malloc(sizeof(COSA_DML_MOCA_IF_FULL_TABLE));
    memset(pMoCAIfFullTable, 0, sizeof(COSA_DML_MOCA_IF_FULL_TABLE));

    pMoCAIfFullTable->MoCAIfFull.Cfg = *pCfg;
    pMoCAObject->MoCAIfFullTable[0] = *pMoCAIfFullTable;

    pCfg->bEnabled = false;
    pCfg->InstanceNumber = 1;
    strcpy(pCfg->Alias, "moca0");
    pCfg->bPreferredNC = false;
    pCfg->PrivacyEnabledSetting = false;
    strcpy((char*)pCfg->FreqCurrentMaskSetting, "1");
    strcpy(pCfg->KeyPassphrase, "1122334455667");
    pCfg->TxPowerLimit = 1;
    pCfg->BeaconPowerLimit = 1;
    pCfg->MaxIngressBWThreshold = 1;
    pCfg->MaxEgressBWThreshold = 1;
    pCfg->X_CISCO_COM_Reset = true;
    pCfg->X_CISCO_COM_MixedMode = true;
    pCfg->X_CISCO_COM_ChannelScanning = true;
    pCfg->X_CISCO_COM_AutoPowerControlEnable = true;
    pCfg->X_CISCO_COM_EnableTabooBit = true;
    strcpy((char *)pCfg->NodeTabooMask, "3223232");
    pCfg->AutoPowerControlPhyRate = 1;
    strcpy((char *)pCfg->X_CISCO_COM_ChannelScanMask, "213131");

    EXPECT_CALL(*g_syscfgMock, syscfg_get(_,_,_,_))
                .Times(3)
                .WillOnce(DoAll(SetArrayArgument<2>(pValueTrue, pValueTrue + sizeof(pValueTrue)), Return(0)))
                .WillRepeatedly(Return(0));
    /*EXPECT_CALL(*g_syscfgMock, syscfg_get(_,StrEq("X_RDKCENTRAL-COM_VIDEOSERVICE"),_,_)).WillOnce(Return(0));
    EXPECT_CALL(*g_syscfgMock, syscfg_get(_,StrEq("bridge_mode"),_,_)).WillOnce(DoAll(SetArrayArgument<2>(pValueTrue, pValueTrue + sizeof(pValueTrue)), Return(0)));*/
    EXPECT_CALL(*g_mocaHALMock, moca_SetIfConfig(_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_syscfgMock, syscfg_set_nns_commit(_,_)).Times(1).WillRepeatedly(Return(0));
    pErr status = Process_Moca_WebConfigRequest((void *)doc);
    EXPECT_EQ(status->ErrorCode, BLOB_EXEC_SUCCESS);

    free(doc->param);
    doc->param = NULL;

    free(doc->subdoc_name);
    doc->subdoc_name = NULL;

    free(doc);
    doc = NULL;

    free(pCfg);
    pCfg = NULL;

    free(pMoCAIfFullTable);
    pMoCAIfFullTable = NULL;

    free(PCFG);
    PCFG = NULL;

    free(pMoCAObject);
    pMoCAObject = NULL;
}

//Test for rollback_moca_conf - Success
TEST_F(CcspMoCATestFixture, rollback_moca_confSuccess)
{
    extern ANSC_HANDLE g_MoCAObject;
    PCOSA_DATAMODEL_MOCA pMyObject = (PCOSA_DATAMODEL_MOCA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MOCA));
    memset(pMyObject, 0, sizeof(COSA_DATAMODEL_MOCA));

    g_MoCAObject = (ANSC_HANDLE)pMyObject;
    PCOSA_DML_MOCA_IF_CFG pCfg = &pMyObject->MoCAIfFullTable[0].MoCAIfFull.Cfg;

    moca_cfg_t mocaCfg;
    memset(&mocaCfg, 0, sizeof(moca_cfg_t));

    mocaCfg.bEnabled = true;

    EXPECT_CALL(*g_mocaHALMock, moca_GetIfConfig(_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_mocaHALMock, moca_SetIfConfig(_,_)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*g_syscfgMock, syscfg_set_nns_commit(_,_)).Times(1).WillOnce(Return(0));
    int ret = rollback_moca_conf();

    EXPECT_EQ(ret, 0);
}

//Test for freeResources_moca - Success
TEST_F(CcspMoCATestFixture, freeResources_mocaSuccess)
{
    execData *blob_exec_data  = (execData*)AnscAllocateMemory(sizeof(execData));
    memset(blob_exec_data, 0, sizeof(execData));

    mocadoc_t *doc = (mocadoc_t *)AnscAllocateMemory(sizeof(mocadoc_t));
    memset(doc, 0, sizeof(mocadoc_t));

    doc->param = (mocaparam_t *)AnscAllocateMemory(sizeof(mocaparam_t));
    memset(doc->param, 0, sizeof(mocaparam_t));

    doc->subdoc_name = strdup("moca");
    doc->transaction_id = 1234;
    doc->version = 1;
    doc->param->enable = true;

    blob_exec_data->user_data = (void*)doc;
    freeResources_moca((void *)blob_exec_data);
}