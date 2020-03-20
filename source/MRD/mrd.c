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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include "syscfg/syscfg.h"
#include "safec_lib_common.h"
#include "secure_wrapper.h"


#define PNAME "/tmp/icebergwedge_t"
#define PID 21699
#define WAN_MCAST_ENTRIES 20
#define MAX_BUF_SIZE 2048
#define MAC_ADDRESS_SIZE 20
#define MRD_ARP_CACHE       "/proc/net/arp"
#define MRD_ARP_STRING_LEN  1023
#define MRD_ARP_BUFFER_LEN  (MRD_ARP_STRING_LEN + 1)
/* Format for reading the 1st, and 4th space-delimited fields */
#define MRD_ARP_LINE_FORMAT "%1023s %*s %*s " \
                        "%1023s %*s %*s"
#define MRD_SHM_EXISTS 17
#define MRD_MAX_LOG 20000
#define MRD_LOG_FILE "/rdklogs/logs/mrdtrace.log"
#define MRD_LOG_BUFSIZE 200
#define MAX_SUBNET_SIZE 25

char LanMCastTable[WAN_MCAST_ENTRIES][20];
char WanMCastTable[WAN_MCAST_ENTRIES][20];
unsigned int WanMCastStatus[WAN_MCAST_ENTRIES]={0, };
char LanMCcount = 0;
char WanMCcount = 0;

typedef enum { DP_SUCCESS=0, DP_INVALID_MAC, DP_COLLISION} mrd_wlist_ss_t;

// white list for storing verified connection
typedef struct mrd_wlist {
     long ipaddr;
     char macaddr[MAC_ADDRESS_SIZE];
     short ofb_index;
}mrd_wlist_t;

mrd_wlist_t *mrd_wlist;
int shmid;

static void GetSubnet(char *ip, char *subnet)
{
	int token = 0;
	int i = 0;
	int len = strlen(ip);
        errno_t rc = -1;
	for(i = 0; i <len ; i++)
	{
		subnet[i] = ip[i];
		if(ip[i] == '.')
		{
			token++;
		}
		if(token == 3)
		{
		      rc = strcat_s(subnet,MAX_SUBNET_SIZE,"0/24");
                      ERR_CHK(rc);
		      break;
		}
	}
	printf("IP = %s\n", ip);
	printf("Subnet = %s\n", subnet);
}

static void mrd_signal_handlr(int sig)
{
     if (mrd_wlist && (shmid > 0)) 
     {
        shmdt((void *) mrd_wlist);
     }
     exit(0);
}

static int mrd_getMACAddress(char *ipaddress, char *mac)
{
    FILE *arpCache;
    char header[MRD_ARP_BUFFER_LEN];
    int count = 0;
    char ipAddr[MAC_ADDRESS_SIZE];
    errno_t rc = -1;
    int ind = -1;

    if (ipaddress == NULL) return 1;
    arpCache = fopen(MRD_ARP_CACHE, "r");
    if (!arpCache)
    {
        return 1;
    }
    /* Ignore the first line, which contains the header */
    if (!fgets(header, sizeof(header), arpCache))
    {
        fclose(arpCache);
        return 1;
    }

    while (2 == fscanf(arpCache, MRD_ARP_LINE_FORMAT, ipAddr, mac))
    {
        rc = strcmp_s(ipAddr,sizeof(ipAddr),ipaddress,&ind);
        ERR_CHK(rc);
        if((!ind) && (rc == EOK)) 
        {
               rc = strcmp_s("00:00:00",strlen("00:00:00"), mac,&ind);
               ERR_CHK(rc);
               if((ind) && (rc == EOK))
               { 
              // compare if the mac address is zero
              // if zero continue reading until nonzero
              // address located.
              fclose(arpCache);
              return 0;
           }
        }
    }
    fclose(arpCache);
    return 1;
}

static void mrd_log(char* msg)
{
   FILE *pFile = NULL;
   char timestring[100]={0};
   time_t timer;
   struct tm* tm_info;
   static unsigned int rcount=0;

   if (msg != NULL) {
      // Get local time and store it in timestring
      time(&timer);
      tm_info = localtime(&timer);
      strftime(timestring, 100, "%Y-%m-%d %H:%M:%S", tm_info);
      pFile = fopen(MRD_LOG_FILE,"a+");
      if(pFile)
      {
         if (rcount >= MRD_MAX_LOG) {
            fclose(pFile);
            pFile = fopen(MRD_LOG_FILE,"w");
            rcount = 0;
         }
         else rcount++;
         if (msg != NULL) {
            fprintf(pFile, "MRD : %s %s", timestring, msg);
         }
         fflush(pFile);
         fclose(pFile);
      }
   }
   return;
}


static unsigned short mrd_hashindex (long ipaddr)
{
    unsigned short ret;
    // Fold the ip address to get a 9 bit hansh index
    ret =(ipaddr & 0x3FF) + ((ipaddr >> 10)&0x3FF) + ((ipaddr >> 20)&0x3FF) 
                                                   + ((ipaddr >> 30) & 0x3FF);
    ret = ((ret & 0x3FF) + ((ret >> 10) && 0x3FF)) &0x3FF;
    if (ret) { 
       ret = ret-1;
    }
    return(ret);
}


static int mrd_wlistInit()
{
   int ret=0;
   key_t key;
   int i;
   char buf[MRD_LOG_BUFSIZE];
   static errcount=0;
   char cmd[200] = {0};
   errno_t rc = -1;

   signal(SIGHUP,mrd_signal_handlr);
   signal(SIGINT,mrd_signal_handlr);
   signal(SIGQUIT,mrd_signal_handlr);
   signal(SIGABRT,mrd_signal_handlr);
   signal(SIGTSTP,mrd_signal_handlr);
   signal(SIGKILL,mrd_signal_handlr);


   if ((key =  ftok(PNAME, PID)) != (key_t) -1) { 
      //Allocate shared memory    
      shmid = shmget(key, MAX_BUF_SIZE*sizeof(mrd_wlist_t), S_IRUSR|S_IRGRP|S_IROTH);
      if (shmid < 0) 
      {
         errcount++;
         if (errcount < 100) {
            snprintf(buf, MRD_LOG_BUFSIZE, "shmget  failed %d %s \n",errno, strerror(errno));
            mrd_log(buf);
         }
         ret = -1;
      }
      else {
         mrd_wlist = (mrd_wlist_t *) shmat(shmid, 0, 0);
         if (mrd_wlist == (void *) -1)
         {
             errcount++;
             if (errcount < 100) {
                snprintf(buf, MRD_LOG_BUFSIZE,"shmat failed %d %s\n", errno, strerror(errno));
                mrd_log(buf);
             }
             ret = -1;
         }
         else {
            snprintf(buf, MRD_LOG_BUFSIZE,"dp_wlistInit:attaching to an existing shared memory \n");
            mrd_log(buf);
            // Clean up all routes and arp entries 
            for (i=0; i<WanMCcount; i++) {
	      snprintf(cmd, sizeof(cmd), "ip route delete %s table moca", WanMCastTable[i]);
	      system(cmd);
              mrd_log(cmd);
              rc =  memset_s(cmd,sizeof(cmd),0, sizeof(cmd));
              ERR_CHK(rc);
	      snprintf(cmd, sizeof(cmd), "arp -d  %s", WanMCastTable[i]);
	      system(cmd);
              mrd_log(cmd);
              rc = memset_s(cmd, sizeof(cmd),0, sizeof(cmd));
              ERR_CHK(rc);
              rc = memset_s(WanMCastTable[i], sizeof(WanMCastTable[i]), 0, sizeof(WanMCastTable[i]));
              ERR_CHK(rc);
              WanMCastStatus[i]=0;
            }
            WanMCcount = 0;
        }
     }
   }
   else {  
      ret = -1;
   }
   return (ret);
}

short mrdnode_lookup(long ipaddr, char MAC[], mrd_wlist_ss_t *stat)
{
    short index;
    char buf[MRD_LOG_BUFSIZE];

    if (mrd_wlist == (void *) -1) return -1;
    index = mrd_hashindex(ipaddr); 
    //snprintf(buf, sizeof(buf), "mrdnode_lookup ip %d : index %d\n", ipaddr, index);
    //mrd_log(buf);
    //check if the ip address is already existing in the white list
    if (mrd_wlist[index].ofb_index == (short) -1) return -1; 
    else 
    {
       if (mrd_wlist[index].ipaddr == ipaddr) 
       {
         
          *stat = DP_SUCCESS;
          return (index);; 
       }
       else 
       {
           //snprintf(buf, sizeof(buf), "searching overflow table \n");
           //mrd_log(buf);
          //search overflow table
          while (mrd_wlist[index].ofb_index > 0) 
          {
             index = mrd_wlist[index].ofb_index;
             if (mrd_wlist[index].ipaddr == ipaddr) 
             {
                *stat = DP_COLLISION;
                snprintf(buf, sizeof(buf), "Found in overflow table %d \n", index);
                mrd_log(buf);
                return (index);
             }
          }
          *stat = DP_COLLISION;
       }
    }
    return -1;
}


int main()
{
    FILE *fp = NULL;
    char buf[256]={0};
    char cmd[256] = {0};
    char cmd1[256] = {0};
    char mac[MAC_ADDRESS_SIZE] = {0};
    char FirstTime = 1;
    char wFirstTime = 1;
    int i = 0;
    unsigned long ipaddr;
    short index;
    mrd_wlist_ss_t stat; 
    unsigned int mrd_flag=1, wan_stat=0;
    int wlist_stat=-1;
    char logbuf[MRD_LOG_BUFSIZE] = {0};
    struct in_addr x_r;
    int ret;
    errno_t rc = -1;
    int ind = -1;

    rc = memset_s(LanMCastTable,sizeof(LanMCastTable[0][0]) * 20 * 20, 0, sizeof(LanMCastTable[0][0]) * 20 * 20);
    ERR_CHK(rc);
    rc = memset_s(WanMCastTable,sizeof(WanMCastTable[0][0]) * 20 * 20 , 0, sizeof(WanMCastTable[0][0]) * 20 * 20);
    ERR_CHK(rc);

    // whilte list is populated by xupnp device protection service
    // mrd service access the white list
    wlist_stat = mrd_wlistInit();
    snprintf(logbuf, MRD_LOG_BUFSIZE, "wlist_stat %d\n", wlist_stat);
    mrd_log(logbuf);
    //v_secure_system("ip route del 169.254.0.0/16 dev brlan10");
    //v_secure_system("ip route add 169.254.0.0/16 dev brlan0");
    v_secure_system("brctl stp brlan10 on");
    while(1)
    {
       rc = memset_s(buf,sizeof(buf), 0, sizeof(buf));
       ERR_CHK(rc);
       rc = memset_s(cmd,sizeof(cmd), 0, sizeof(cmd));
       ERR_CHK(rc);
       snprintf(cmd, sizeof(cmd), "filter=`ip -s mroute |grep 'Iif: brlan0' | cut -d '(' -f 2 | cut -d ',' -f 1` ; for val in $filter; do ip -4 nei show |grep brlan0 |grep -v FAILED |cut -d '(' -f 2 | cut -d ',' -f 1|awk '{print $1}' | grep $val ; done");
       if(!(fp = popen(cmd, "r")))
       {
	       return -1;
       }
       while ( fgets(buf, sizeof(buf), fp)!= NULL )
       {
          char *pos;
          if ((pos=strchr(buf, '\n')) != NULL)
          *pos = '\0';
                
	  if (FirstTime)
	  {
             rc = strcpy_s(LanMCastTable[LanMCcount],sizeof(LanMCastTable[LanMCcount]),buf);
             if(rc != EOK)
             {
                  ERR_CHK(rc);
                  return -1;
             }
	     FirstTime = 0;
             if (!strncmp(buf, "169.", 4)) 
             {
	        v_secure_system("ip route add %s dev brlan0", buf);
             } 
	     v_secure_system("arp -i brlan10 -Ds %s brlan0 pub", buf);
	     LanMCcount++;
	  }
	  else
	  {
	     for(i = 0;i<LanMCcount;i++)
	     {
                 
	        rc = strcmp_s(LanMCastTable[i],sizeof(LanMCastTable[i]),buf,&ind);
                ERR_CHK(rc);
                if((!ind) && (rc == EOK))
	        {
	           i = 0;
		   break;
	        }
			     	
	     }
	     if(i)
	     {
                rc = strcpy_s(LanMCastTable[LanMCcount],sizeof(LanMCastTable[LanMCcount]),buf);
                if(rc != EOK)
                {
                   ERR_CHK(rc);
                   return -1;
                }
                if (!strncmp(buf, "169.", 4)) 
                {
		    v_secure_system("ip route add %s dev brlan0", buf);
                }
		v_secure_system("arp -i brlan10 -Ds %s brlan0 pub", buf);
		LanMCcount++;
	     }
          }
          rc = memset_s(buf,sizeof(buf), 0, sizeof(buf));
          ERR_CHK(rc);
       }
       pclose(fp);
	
       rc = memset_s(buf,sizeof(buf), 0, sizeof(buf));
       ERR_CHK(rc);
       rc = memset_s(cmd,sizeof(cmd), 0, sizeof(cmd));
       ERR_CHK(rc);
       snprintf(cmd, sizeof(cmd), "filter=`ip -s mroute |grep 'Iif: brlan10' |grep 169 | cut -d '(' -f 2 | cut -d ',' -f 1` ; for val in $filter; do ip -4 nei show |grep brlan10 |grep -v FAILED |cut -d '(' -f 2 | cut -d ',' -f 1|awk '{print $1}' | grep $val ; done");
       if(!(fp = popen(cmd, "r")))
       {
          printf("error\n");
          return -1;
       }
       while ( fgets(buf, sizeof(buf), fp)!= NULL )
       {
          char *pos;
          if ((pos=strchr(buf, '\n')) != NULL)
             *pos = '\0';
          mrd_flag = 0;
          if (wlist_stat==0)
          { 
             ret = inet_aton(buf, &x_r);
             if (ret) {
                ipaddr = x_r.s_addr;
	        rc =  memset_s(mac,sizeof(mac), 0, sizeof(mac));
                ERR_CHK(rc);
                mrd_getMACAddress(buf, mac);
                if ((index = mrdnode_lookup(ipaddr, mac, &stat)) != (short) -1)
                {
                   if ((stat == DP_COLLISION) || (stat == DP_SUCCESS)) 
                   {
                      mrd_flag = 1;    
                   }
                }
             }
          }
          else  {
             mrd_flag = 1;
          }
                  
	  if (wFirstTime)
	  {
             rc = strcpy_s(WanMCastTable[WanMCcount],sizeof(WanMCastTable[WanMCcount]),buf);
             if (rc != EOK)
             {
                ERR_CHK(rc);
                return -1;
             } 
	     wFirstTime = 0;
             if (mrd_flag) {
                if (!strncmp(buf, "169.", 4)) 
                {
	           v_secure_system("ip route add %s dev brlan10 table moca", buf);
                }
	        v_secure_system("arp -i brlan0 -Ds %s brlan10 pub", buf);
                snprintf(logbuf, MRD_LOG_BUFSIZE, "wFirstTime: ip address %s added to mcast list %d \n", buf, mrd_flag);
                mrd_log(logbuf);
             }
             else  {
                if (!strncmp(buf, "169.", 4)) {
	           v_secure_system("ip route delete %s table moca", buf);
                }
	        v_secure_system("arp -d %s", buf);
                snprintf(logbuf, MRD_LOG_BUFSIZE, "ip address %s deleted from mcast list \n", buf);
                mrd_log(logbuf);
             }
             WanMCastStatus[WanMCcount] = mrd_flag;
	     WanMCcount++;
	  }
	  else
	  {
	     for(i = 0;i<WanMCcount;i++)
	     {
                rc = strcmp_s(WanMCastTable[i],sizeof(WanMCastTable[i]),buf,&ind);
                ERR_CHK(rc);
                if ((!ind) && (rc == EOK))
	        {
                   wan_stat = WanMCastStatus[i];
                   WanMCastStatus[i] = mrd_flag;;
		   i = 0;
		   break;
	        }
	     }
	     if(i)
	     {
                rc = strcpy_s(WanMCastTable[WanMCcount],sizeof(WanMCastTable[WanMCcount]),buf);
                if(rc != EOK)
                {
                   ERR_CHK(rc);
                   return -1;
                }
                WanMCastStatus[WanMCcount] = mrd_flag;
	        WanMCcount++;
             }
             if ((i) || (wan_stat != mrd_flag)) 
             {
                if (mrd_flag==1)
                {
                   if (!strncmp(buf, "169.", 4)) 
                   {
	              v_secure_system("ip route add %s dev brlan10 table moca", buf);
                   }
	           v_secure_system("arp -i brlan0 -Ds %s brlan10 pub", buf);
                   snprintf(logbuf, MRD_LOG_BUFSIZE, "ip address %s added to mcast list \n", buf);
                   mrd_log(logbuf);
                }
                else  {
                   if (!strncmp(buf, "169.", 4)) {
	              v_secure_system("ip route delete %s table moca", buf);
                   }
	           v_secure_system("arp -d %s", buf);
                   snprintf(logbuf, MRD_LOG_BUFSIZE, "ip address %s deleted from mcast list \n", buf);
                   mrd_log(logbuf);
               }
	     }
	  }
          rc = memset_s(buf,sizeof(buf), 0, sizeof(buf));
          ERR_CHK(rc);
       }
       pclose(fp);
       sleep(30);
       if (wlist_stat != 0) 
       { 
          wlist_stat = mrd_wlistInit();
       }
    } //while

}
