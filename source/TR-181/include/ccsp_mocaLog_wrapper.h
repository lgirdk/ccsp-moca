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
