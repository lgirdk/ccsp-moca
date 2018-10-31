/************************************************************************************
  If not stated otherwise in this file or this component's Licenses.txt file the
  following copyright and licenses apply:

  Copyright 2018 RDK Management

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
**************************************************************************/
#ifndef  _CCSP_MOCALOG_WRPPER_H_ 
#define  _CCSP_MOCALOG_WRPPER_H_

#include "ccsp_custom_logs.h"

extern int consoleDebugEnable;
extern FILE* debugLogFile;

/*
 * Logging wrapper APIs g_Subsystem
 */
#define  CcspTraceBaseStr(arg ...)                                                                  \
            do {                                                                                    \
                snprintf(pTempChar1, 4095, arg);                                                    \
            } while (FALSE)


#define  CcspMoCAConsoleTrace(msg)                                                                  \
{\
            if(consoleDebugEnable)                                                                  \
            {\
                char pTempChar1[4096];                                                              \
                CcspTraceBaseStr msg;                                                       \
                fprintf(debugLogFile, "%s:%d: ", __FILE__, __LINE__);                       \
                fprintf(debugLogFile, "%s", pTempChar1);                                    \
                fflush(debugLogFile);                                                       \
            }\
}


#endif
