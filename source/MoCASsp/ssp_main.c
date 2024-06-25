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
/*********************************************************************************

    description:

        This is the template file of ssp_main.c for XxxxSsp.
        Please replace "XXXX" with your own ssp name with the same up/lower cases.

  ------------------------------------------------------------------------------

    revision:

        09/08/2011    initial revision.

**********************************************************************************/


#ifdef __GNUC__
#ifndef _BUILD_ANDROID
#include <execinfo.h>
#endif
#endif

#include "ssp_global.h"
#include "stdlib.h"
#include "ccsp_dm_api.h"
#include "safec_lib_common.h"
#include "webconfig_framework.h"
#include "secure_wrapper.h"
#include <sys/stat.h>
//  Existing moca_initialized is removed from ccsp_restart.sh . Created this file to determine if component is coming after crashed to sync values from server.
#define MOCA_INIT_FILE_BOOTUP "/tmp/moca_initialized_bootup"

#ifdef INCLUDE_BREAKPAD
#include "breakpad_wrapper.h"
#endif
#define DEBUG_INI_NAME "/etc/debug.ini"

#include "cap.h"
static cap_user appcaps;

extern char*                                pComponentName;
char                                        g_Subsystem[32]         = {0};
int consoleDebugEnable = 0;
FILE* debugLogFile;


int  cmd_dispatch(int  command)
{
    switch ( command )
    {
        case    'e' :

            CcspTraceInfo(("Connect to bus daemon...\n"));

            {
                char                            CName[256];

                if ( g_Subsystem[0] != 0 )
                {
                    _ansc_sprintf(CName, "%s%s", g_Subsystem, CCSP_COMPONENT_ID_MoCA);
                }
                else
                {
                    _ansc_sprintf(CName, "%s", CCSP_COMPONENT_ID_MoCA);
                }

                ssp_Mbi_MessageBusEngage
                    ( 
                        CName,
                        CCSP_MSG_BUS_CFG,
                        CCSP_COMPONENT_PATH_MoCA
                    );
            }

            ssp_create();
            ssp_engage();

            break;

        case    'm':

                AnscPrintComponentMemoryTable(pComponentName);

                break;

        case    't':

                AnscTraceMemoryTable();

                break;

        case    'c':
                
                ssp_cancel();

                break;

        default:
            break;
    }

    return 0;
}

static void _print_stack_backtrace(void)
{
#ifdef __GNUC__
#ifndef _BUILD_ANDROID
	void* tracePtrs[100];
	char** funcNames = NULL;
	int i, count = 0;

	count = backtrace( tracePtrs, 100 );
	backtrace_symbols_fd( tracePtrs, count, 2 );

	funcNames = backtrace_symbols( tracePtrs, count );

	if ( funcNames ) {
            // Print the stack trace
	    for( i = 0; i < count; i++ )
		printf("%s\n", funcNames[i] );

            // Free the string pointers
            free( funcNames );
	}
#endif
#endif
}

static void daemonize(void) {
	switch (fork()) {
	case 0:
		break;
	case -1:
		// Error
		CcspTraceInfo(("Error daemonizing (fork)! %d - %s\n", errno, strerror(
				errno)));
		exit(0);
		break;
	default:
		_exit(0);
	}

	if (setsid() < 	0) {
		CcspTraceInfo(("Error demonizing (setsid)! %d - %s\n", errno, strerror(errno)));
		exit(0);
	}

//	chdir("/");


#ifndef  _DEBUG

	fd = open("/dev/null", O_RDONLY);
	if (fd != 0) {
		dup2(fd, 0);
		close(fd);
	}
	fd = open("/dev/null", O_WRONLY);
	if (fd != 1) {
		dup2(fd, 1);
		close(fd);
	}
	fd = open("/dev/null", O_WRONLY);
	if (fd != 2) {
		dup2(fd, 2);
		close(fd);
	}
#endif
}

void sig_handler(int sig)
{
    if ( sig == SIGINT ) {
    	signal(SIGINT, sig_handler); /* reset it to this function */
    	CcspTraceInfo(("SIGINT received!\n"));
	exit(0);
    }
    else if ( sig == SIGUSR1 ) {
    	signal(SIGUSR1, sig_handler); /* reset it to this function */
    	CcspTraceInfo(("SIGUSR1 received!\n"));
    }
    else if ( sig == SIGUSR2 ) {
    	CcspTraceInfo(("SIGUSR2 received!\n"));
    }
    else if ( sig == SIGCHLD ) {
    	signal(SIGCHLD, sig_handler); /* reset it to this function */
    	CcspTraceInfo(("SIGCHLD received!\n"));
    }
    else if ( sig == SIGPIPE ) {
    	signal(SIGPIPE, sig_handler); /* reset it to this function */
    	CcspTraceInfo(("SIGPIPE received!\n"));
    }
    else {
    	/* get stack trace first */
    	_print_stack_backtrace();
    	CcspTraceInfo(("Signal %d received, exiting!\n", sig));
    	exit(0);
    }

}


int drop_root(void)
{
  int retval = 0;
  bool blocklist_ret = false;
  blocklist_ret = isBlocklisted();
  if(blocklist_ret)
  {
    CcspTraceInfo(("NonRoot feature is disabled\n"));
  }
  else
  {
    CcspTraceInfo(("NonRoot feature is enabled, dropping root privileges for CcspMoCA process\n"));
    if(init_capability() != NULL) {
       if(drop_root_caps(&appcaps) != -1) {
          if(update_process_caps(&appcaps) != -1) {
             read_capability(&appcaps);
             retval = 1;
          }
       }
    }
  }
  return retval;
}

int main(int argc, char* argv[])
{
    BOOL                            bRunAsDaemon       = TRUE;
    int                             cmdChar            = 0;
    int                             idx = 0;
    errno_t rc       = -1;
    int     ind      = -1;

    extern ANSC_HANDLE bus_handle;
    char *subSys            = NULL;
    DmErr_t    err;
    debugLogFile = stderr;

#ifdef FEATURE_SUPPORT_RDKLOG
    RDK_LOGGER_INIT();
#endif

    // Buffer characters till newline for stdout and stderr
    setlinebuf(stdout);
    setlinebuf(stderr);

    for (idx = 1; idx < argc; idx++)
    {
        rc = strcmp_s("-subsys",strlen("-subsys"),argv[idx],&ind);
        ERR_CHK(rc);
        if ((!ind) && (rc == EOK))
        {
            rc = strcpy_s(g_Subsystem,sizeof(g_Subsystem),argv[idx+1]);
            if(rc != EOK)
            {
               ERR_CHK(rc);
               return ANSC_STATUS_FAILURE;
            }
            
        }
        else
        {
            rc = strcmp_s( "-c",strlen( "-c"),argv[idx],&ind);
            ERR_CHK(rc);
        if ((!ind) && (rc == EOK))   
        {
             bRunAsDaemon = FALSE;
        }
        else
        {
           rc = strcmp_s("-DEBUG",strlen("-DEBUG"),argv[idx],&ind);
           ERR_CHK(rc);
        if ((!ind) && (rc == EOK))
        {
            consoleDebugEnable = 1;
            fprintf(stderr, "DEBUG ENABLE ON \n");
        }
        else
        { 
            rc = strcmp_s("-LOGFILE",strlen("-LOGFILE"),argv[idx],&ind);
            ERR_CHK(rc);
        if ((!ind) && (rc == EOK))
        {
            // We assume argv[1] is a filename to open
            debugLogFile = fopen( argv[idx + 1], "a+" );

            /* fopen returns 0, the NULL pointer, on failure */
            if ( debugLogFile == 0 )
            {
                debugLogFile = stderr;
                fprintf(debugLogFile, "Invalid Entry for -LOGFILE input \n" );
            }
            else 
            {
                fprintf(debugLogFile, "Log File [%s] Opened for Writing in Append Mode \n",  argv[idx+1]);
            }

        }
        }
        }
        }         
    }

    pComponentName          = CCSP_COMPONENT_NAME_MoCA;

    if(!drop_root()) {
          CcspTraceInfo(("drop_root method failed!\n"));
    }

    if ( bRunAsDaemon )
        daemonize();
#ifdef INCLUDE_BREAKPAD
    breakpad_ExceptionHandler();
#else
    signal(SIGTERM, sig_handler);
    signal(SIGINT, sig_handler);
    /*signal(SIGCHLD, sig_handler);*/
    signal(SIGUSR1, sig_handler);
    signal(SIGUSR2, sig_handler);

    signal(SIGSEGV, sig_handler);
    signal(SIGBUS, sig_handler);
    signal(SIGKILL, sig_handler);
    signal(SIGFPE, sig_handler);
    signal(SIGILL, sig_handler);
    signal(SIGQUIT, sig_handler);
    signal(SIGHUP, sig_handler);
#endif
    cmd_dispatch('e');
#ifdef _COSA_SIM_
    subSys = "";        /* PC simu use empty string as subsystem */
#else
    subSys = NULL;      /* use default sub-system */
#endif
    err = Cdm_Init(bus_handle, subSys, NULL, NULL, pComponentName);
    if (err != CCSP_SUCCESS)
    {
        fprintf(stderr, "Cdm_Init: %s\n", Cdm_StrError(err));
        exit(1);
    }

    check_component_crash(MOCA_INIT_FILE_BOOTUP);
  //CID 276408 : UNCHECKED RETURN VALUE
    int check_var = creat("/tmp/moca_initialized",S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if ( check_var == -1 ){
        fprintf(stderr, "Error in creating /tmp/moca_initialized : %s\n", strerror(errno));
        exit(1);
    }
    v_secure_system("sysevent set moca_init completed");
    check_var = creat(MOCA_INIT_FILE_BOOTUP,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if ( check_var == -1 ){
        fprintf(stderr, "Error in creating %s : %s\n", MOCA_INIT_FILE_BOOTUP, strerror(errno));
        exit(1);
    }

    //LM_main();
    if ( bRunAsDaemon )
    {
        while(1)
        {
            sleep(30);
        }
    }
    else
    {
        while ( cmdChar != 'q' )
        {
            cmdChar = getchar();

            cmd_dispatch(cmdChar);
        }
    }

	err = Cdm_Term();
	if (err != CCSP_SUCCESS)
	{
	fprintf(stderr, "Cdm_Term: %s\n", Cdm_StrError(err));
	exit(1);
	}

	ssp_cancel();

    if(debugLogFile)
    {
        fclose(debugLogFile);
    }

    return 0;
}

