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

#include <gmock/gmock.h>
#include "moca_mock.h"

PlatformHalMock *g_platformHALMock = NULL;
SecureWrapperMock * g_securewrapperMock = NULL;
SocketMock * g_socketMock = NULL;
SafecLibMock * g_safecLibMock = NULL;
AnscDebugMock * g_anscDebugMock = NULL;
UserTimeMock * g_usertimeMock = NULL;
BaseAPIMock * g_baseapiMock = NULL;
SyscfgMock * g_syscfgMock = NULL;
PsmMock *g_psmMock = NULL;
parodusMock *g_parodusMock = NULL;
utopiaMock *g_utopiaMock = NULL;
AnscMemoryMock * g_anscMemoryMock = NULL;
AnscWrapperApiMock * g_anscWrapperApiMock = NULL;
UserRuntimeMock* g_userRuntimeMock = NULL;
CcspDmApiMock* g_ccspDmApiMock = NULL;
telemetryMock * g_telemetryMock = NULL;
DslhDmagntExportedMock* g_dslhDmagntExportedMock = NULL;
TraceMock * g_traceMock = NULL;
SyseventMock *g_syseventMock = NULL;
MocaHalMock *g_mocaHALMock = NULL;
msgpackMock *g_msgpackMock = NULL;
webconfigFwMock *g_webconfigFwMock = NULL;
base64Mock *g_base64Mock = NULL;

CcspMoCATestFixture::CcspMoCATestFixture()
{
    g_platformHALMock = new PlatformHalMock;
    g_securewrapperMock = new SecureWrapperMock;
    g_socketMock = new SocketMock;
    g_safecLibMock = new SafecLibMock;
    g_anscDebugMock = new AnscDebugMock;
    g_usertimeMock = new UserTimeMock;
    g_baseapiMock = new BaseAPIMock;
    g_syscfgMock = new SyscfgMock;
    g_psmMock = new PsmMock;
    g_parodusMock = new parodusMock;
    g_utopiaMock = new utopiaMock;
    g_anscMemoryMock = new AnscMemoryMock;
    g_anscWrapperApiMock = new AnscWrapperApiMock;
    g_userRuntimeMock = new UserRuntimeMock;
    g_ccspDmApiMock = new CcspDmApiMock;
    g_telemetryMock = new telemetryMock;
    g_dslhDmagntExportedMock = new DslhDmagntExportedMock;
    g_traceMock = new TraceMock;
    g_mocaHALMock = new MocaHalMock;
    g_msgpackMock = new msgpackMock;
    g_syseventMock = new SyseventMock;
    g_webconfigFwMock = new webconfigFwMock;
    g_base64Mock = new base64Mock;
    
}

CcspMoCATestFixture::~CcspMoCATestFixture()
{
    delete g_platformHALMock;
    delete g_securewrapperMock;
    delete g_socketMock;
    delete g_safecLibMock;
    delete g_anscDebugMock;
    delete g_usertimeMock;
    delete g_baseapiMock;
    delete g_syscfgMock;
    delete g_psmMock;
    delete g_parodusMock;
    delete g_utopiaMock;
    delete g_anscMemoryMock;
    delete g_anscWrapperApiMock;
    delete g_userRuntimeMock;
    delete g_ccspDmApiMock;
    delete g_telemetryMock;
    delete g_dslhDmagntExportedMock;
    delete g_traceMock;
    delete g_mocaHALMock;
    delete g_msgpackMock;
    delete g_syseventMock;
    delete g_webconfigFwMock;
    delete g_base64Mock;
    g_platformHALMock = nullptr;
    g_securewrapperMock = nullptr;
    g_socketMock = nullptr;
    g_safecLibMock = nullptr;
    g_anscDebugMock = nullptr;
    g_usertimeMock = nullptr;
    g_baseapiMock = nullptr;
    g_syscfgMock = nullptr;
    g_psmMock = nullptr;
    g_parodusMock = nullptr;
    g_utopiaMock = nullptr;
    g_anscMemoryMock = nullptr;
    g_anscWrapperApiMock = nullptr;
    g_userRuntimeMock = nullptr;
    g_ccspDmApiMock = nullptr;
    g_telemetryMock = nullptr;
    g_dslhDmagntExportedMock = nullptr;
    g_traceMock = nullptr;
    g_mocaHALMock = nullptr;
    g_msgpackMock = nullptr;
    g_syseventMock = nullptr;
    g_webconfigFwMock = nullptr;
    g_base64Mock = nullptr;
}

void CcspMoCATestFixture::SetUp()
{
}
void CcspMoCATestFixture::TearDown() {}
void CcspMoCATestFixture::TestBody() {}
// end of file