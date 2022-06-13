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
#include <unistd.h>
#include "ansc_platform.h"

#define PNAME "/tmp/xpki_cert"
#define PID 21699
#define MAX_MCAST_ENTRIES 20
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
#define MAX_MLIFE 10
#define MAX_SHMRCOUNT 6

typedef struct lan_mcast_table {
   char ipAddr[20];
   unsigned int mcastAliveCount;
}lan_mcast_table_t; 

static lan_mcast_table_t LanMCastTable[MAX_MCAST_ENTRIES];

typedef struct wan_mcast_table {
   char ipAddr[20];
   unsigned int mcastStatus;
   unsigned int mcastAliveCount;
}wan_mcast_table_t;

static wan_mcast_table_t WanMCastTable[MAX_MCAST_ENTRIES]; 

static int LanMCcount = 0;
static int WanMCcount = 0;

typedef enum { DP_SUCCESS=0, DP_INVALID_MAC, DP_COLLISION} mrd_wlist_ss_t;

// white list for storing verified connection
typedef struct mrd_wlist {
     long ipaddr;
     char macaddr[MAC_ADDRESS_SIZE];
     short ofb_index;
}mrd_wlist_t;

static mrd_wlist_t *mrd_wlist;

static int shmid;

#if 0
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
#endif

static void mrd_signal_handlr(int sig)
{
     UNREFERENCED_PARAMETER(sig);
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
      if (!pFile) 
      {
        // file open in append mode is failing
        // open file in the write mode and try
        pFile = fopen(MRD_LOG_FILE,"w");
        rcount = 0;
      }
      
      if (pFile) 
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
    ret = ((ret & 0x3FF) + ((ret >> 10) & 0x3FF)) &0x3FF;
    if (ret) { 
       ret = ret-1;
    }
    return(ret);
}

static void mrd_initTables()
{
   errno_t rc = -1;
   int i=0;
   for (i=0; i<MAX_MCAST_ENTRIES; i++)
   {
      rc = memset_s(WanMCastTable[i].ipAddr, sizeof(WanMCastTable[i].ipAddr),0, sizeof(WanMCastTable[i].ipAddr));
      ERR_CHK(rc);
      WanMCastTable[i].mcastStatus=0;
      WanMCastTable[i].mcastAliveCount=0;
      rc = memset_s(LanMCastTable[i].ipAddr, sizeof(LanMCastTable[i].ipAddr),0, sizeof(LanMCastTable[i].ipAddr));
      ERR_CHK(rc);
      LanMCastTable[i].mcastAliveCount=0;
   }
}

static void  mrd_updateMcastlistlifetime()
{
   int i;
   errno_t rc = -1;
   char cmd[600]={0};

   // Go through wantable and reduce life time of table entries 
   // Delete stale entries
   for (i=0; i<WanMCcount; i++) 
   {
      if (WanMCastTable[i].mcastAliveCount > 0)
      {
         WanMCastTable[i].mcastAliveCount--;
         if (WanMCastTable[i].mcastAliveCount == 0) 
         {
	    v_secure_system("ip route delete %s table moca", WanMCastTable[i].ipAddr);
            rc =  memset_s(cmd,sizeof(cmd),0, sizeof(cmd));
            ERR_CHK(rc);
	    snprintf(cmd, sizeof(cmd), "deleting stale wan route %s", WanMCastTable[i].ipAddr);
            mrd_log(cmd);
            rc =  memset_s(cmd,sizeof(cmd),0, sizeof(cmd));
            ERR_CHK(rc);
	    v_secure_system("arp -d  %s", WanMCastTable[i].ipAddr);
	    snprintf(cmd, sizeof(cmd), "deleting stale arp %s", WanMCastTable[i].ipAddr);
            mrd_log(cmd);
            if ((WanMCcount > 1) && (i != (WanMCcount-1))) 
            {
               rc = strcpy_s(WanMCastTable[i].ipAddr,sizeof(WanMCastTable[WanMCcount-1].ipAddr),
                                                             WanMCastTable[WanMCcount-1].ipAddr);
               if (rc != EOK)
               {
                  ERR_CHK(rc);
                  return;
               }
               WanMCastTable[i].mcastStatus = WanMCastTable[WanMCcount-1].mcastStatus;
               WanMCastTable[i].mcastAliveCount = WanMCastTable[WanMCcount-1].mcastAliveCount;
            }
            rc = memset_s(WanMCastTable[WanMCcount-1].ipAddr, 
                                              sizeof(WanMCastTable[WanMCcount-1].ipAddr),0, 
                                              sizeof(WanMCastTable[WanMCcount-1].ipAddr));
            ERR_CHK(rc);
            WanMCastTable[WanMCcount-1].mcastStatus=0;
            WanMCastTable[WanMCcount-1].mcastAliveCount=0;
            WanMCcount--;
         }
      }
   } // for loop

   // Go through Lantable and reduce life time of table entries 
   // Delete stale entries
   for (i=0; i<LanMCcount; i++) 
   {
      if (LanMCastTable[i].mcastAliveCount > 0)
      {
         LanMCastTable[i].mcastAliveCount--;
         if (LanMCastTable[i].mcastAliveCount == 0) 
         {
            v_secure_system("ip route delete %s dev brlan0", LanMCastTable[i].ipAddr);
            rc =  memset_s(cmd,sizeof(cmd),0, sizeof(cmd));
            ERR_CHK(rc);
	    snprintf(cmd, sizeof(cmd), "deleting stale lan route %s", LanMCastTable[i].ipAddr);
            mrd_log(cmd);
	    v_secure_system("arp -d %s",LanMCastTable[i].ipAddr);
            rc =  memset_s(cmd,sizeof(cmd),0, sizeof(cmd));
            ERR_CHK(rc);
	    snprintf(cmd, sizeof(cmd), "deleting stale arp %s", LanMCastTable[i].ipAddr);
            mrd_log(cmd);
            if ((LanMCcount > 1) && (i != (LanMCcount-1))) 
            {
               rc = strcpy_s(LanMCastTable[i].ipAddr,sizeof(LanMCastTable[LanMCcount-1].ipAddr),
                                                          LanMCastTable[LanMCcount-1].ipAddr);
               if (rc != EOK)
               {
                  ERR_CHK(rc);
                  return;
               }
               LanMCastTable[i].mcastAliveCount = LanMCastTable[LanMCcount-1].mcastAliveCount;
            }
            rc = memset_s(LanMCastTable[LanMCcount-1].ipAddr, 
                                          sizeof(LanMCastTable[LanMCcount-1].ipAddr),0, 
                                          sizeof(LanMCastTable[LanMCcount-1].ipAddr));
            ERR_CHK(rc);
            LanMCastTable[LanMCcount-1].mcastAliveCount=0;
            LanMCcount--;
         }
     }
  } // for loop
}

static int mrd_shmattach()
{
   key_t key;
   int ret=-1;
   static int errcount=0;
   char buf[MRD_LOG_BUFSIZE];

   if ((key =  ftok(PNAME, PID)) != (key_t) -1) 
   { 
      //Allocate shared memory    
      shmid = shmget(key, MAX_BUF_SIZE*sizeof(mrd_wlist_t), S_IRUSR|S_IRGRP|S_IROTH);
      if (shmid >= 0) 
      {
         mrd_wlist = (mrd_wlist_t *) shmat(shmid, 0, 0);
         if (mrd_wlist != (void *) -1)
         {
            ret = 0;
         }
         else
         {
             errcount++;
             if (errcount < 100) {
                snprintf(buf, MRD_LOG_BUFSIZE,"shmat failed %d %s\n", errno, strerror(errno));
                mrd_log(buf);
             }
         }
      }
      else 
      {
         errcount++;
         if (errcount < 100) {
            snprintf(buf, MRD_LOG_BUFSIZE, "shmget  failed %d %s \n",errno, strerror(errno));
            mrd_log(buf);
         }
      }
   }
   return(ret);
}

static int mrd_wlistInit()
{
   int ret=0;
   int i;
   char buf[MRD_LOG_BUFSIZE];
   errno_t rc = -1;

   signal(SIGHUP,mrd_signal_handlr);
   signal(SIGINT,mrd_signal_handlr);
   signal(SIGQUIT,mrd_signal_handlr);
   signal(SIGABRT,mrd_signal_handlr);
   signal(SIGTSTP,mrd_signal_handlr);
   signal(SIGKILL,mrd_signal_handlr);

   ret = mrd_shmattach();
   if (ret == 0)
   {
      snprintf(buf, MRD_LOG_BUFSIZE,"dp_wlistInit:attaching to an existing shared memory \n");
      mrd_log(buf);
      // Clean up all routes and arp entries 
      for (i=0; i<WanMCcount; i++) 
      {
	 v_secure_system("ip route delete %s table moca", WanMCastTable[i].ipAddr);
	 v_secure_system("arp -d  %s", WanMCastTable[i].ipAddr);
         rc = memset_s(WanMCastTable[i].ipAddr, sizeof(WanMCastTable[i].ipAddr),0, sizeof(WanMCastTable[i].ipAddr));
         ERR_CHK(rc);
         WanMCastTable[i].mcastStatus=0;
         WanMCastTable[i].mcastAliveCount=0;
      }
      WanMCcount = 0;
   }
   else 
   {
      ret = -1;
   }
   return (ret);
}

static short mrdnode_lookup(long ipaddr, char MAC[], mrd_wlist_ss_t *stat)
{
    UNREFERENCED_PARAMETER(MAC);
    short index;
#ifdef MRD_DEBUG
    char buf[MRD_LOG_BUFSIZE];
#endif
  
    UNREFERENCED_PARAMETER(MAC);
    if (mrd_wlist == (void *) -1) return -1;
    index = mrd_hashindex(ipaddr); 
#ifdef MRD_DEBUG
    snprintf(buf, sizeof(buf), "mrdnode_lookup ip %ld : index %d\n", ipaddr, index);
    mrd_log(buf);
#endif
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
#ifdef MRD_DEBUG
           snprintf(buf, sizeof(buf), "searching overflow table \n");
           mrd_log(buf);
#endif
           //search overflow table
          while (mrd_wlist[index].ofb_index > 0) 
          {
             index = mrd_wlist[index].ofb_index;
             if (mrd_wlist[index].ipaddr == ipaddr) 
             {
                *stat = DP_COLLISION;
#ifdef MRD_DEBUG
                snprintf(buf, sizeof(buf), "Found in overflow table %d \n", index);
                mrd_log(buf);
#endif
                return (index);
             }
          }
          *stat = DP_COLLISION;
       }
    }
    return -1;
}

void send_pings()
{
    FILE *fp0 = NULL;
    char Tbuf[256]={0};
    int i = 0;
    errno_t rc = -1;
    int ind = -1;
    rc = memset_s(Tbuf,sizeof(Tbuf), 0, sizeof(Tbuf));
    ERR_CHK(rc);
    if(!(fp0 = v_secure_popen("r","ip -s mroute |grep 'Iif: brlan0' |grep 169| cut -d '(' -f 2 | cut -d ',' -f 1|awk '{print $1}'")))
    {
        printf("error\n");
        return;
    }
    while ( fgets(Tbuf, sizeof(Tbuf), fp0)!= NULL )
      {
        char *pos;
        if ((pos=strchr(Tbuf, '\n')) != NULL)
        *pos = '\0';
        for(i = 0;i<LanMCcount;i++)
           {
                 
               rc = strcmp_s(LanMCastTable[i].ipAddr,sizeof(LanMCastTable[i].ipAddr),Tbuf,&ind);
                ERR_CHK(rc);
                if((!ind) && (rc == EOK))
                {
                   i = 0;
                   break;
                }
            
            }
            if(i)
            {
              v_secure_system("ping -c1 -I brlan0 %s",Tbuf);
            }
      }
      v_secure_pclose(fp0);                   
    i = 0;
    rc = memset_s(Tbuf,sizeof(Tbuf), 0, sizeof(Tbuf));
    ERR_CHK(rc);
    if(!(fp0 = v_secure_popen("r","ip -s mroute |grep 'Iif: brlan10' |grep 169|grep -v '169.254.30.1' | cut -d '(' -f 2 | cut -d ',' -f 1|awk '{print $1}'")))
    {
        printf("error\n");
        return;
    }
    while ( fgets(Tbuf, sizeof(Tbuf), fp0)!= NULL )
      {
        char *pos;
        if ((pos=strchr(Tbuf, '\n')) != NULL)
        *pos = '\0';
        for(i = 0;i<WanMCcount;i++)
           {
                 
               rc = strcmp_s(WanMCastTable[i].ipAddr,sizeof(WanMCastTable[i].ipAddr),Tbuf,&ind);
                ERR_CHK(rc);
                if((!ind) && (rc == EOK))
                {
                   i = 0;
                   break;
                }
            
            }
            if(i)
            {
              v_secure_system("ping -c1 -I brlan10 %s",Tbuf);
            }
      }
      v_secure_pclose(fp0);
}
int main()
{
    FILE *fp = NULL;
    char buf[256]={0};
    char mac[MAC_ADDRESS_SIZE] = {0};
    int i = 0;
    unsigned long ipaddr;
    short index;
    mrd_wlist_ss_t stat; 
    unsigned int mrd_flag=1, wan_stat=0;
    int wlist_stat=-1;
    char logbuf[MRD_LOG_BUFSIZE+120] = {0};
    struct in_addr x_r;
    int ret;
    int cnt = 0;
    errno_t rc = -1;
    int ind = -1;
    int shmrefreshCount=MAX_SHMRCOUNT;
    mrd_initTables();
    // whilte list is populated by xupnp device protection service
    // mrd service access the white list
    wlist_stat = mrd_wlistInit();
    snprintf(logbuf, MRD_LOG_BUFSIZE, "wlist_stat %d\n", wlist_stat);
    mrd_log(logbuf);
    v_secure_system("brctl stp brlan10 on");
    char* pBuf = buf;
    while(1)
    {
       rc = memset_s(buf,sizeof(buf), 0, sizeof(buf));
       ERR_CHK(rc);
       if (!(fp = v_secure_popen("r","/usr/ccsp/moca/moca_mroute.sh")))
       {
	  return -1;
       }
       while ( fgets(buf, sizeof(buf), fp)!= NULL )
       {
          char *pos;
          if ((pos=strchr(buf, '\n')) != NULL)
          *pos = '\0';
                
	  for (i = 0;i<LanMCcount;i++)
	  {
	     rc = strcmp_s(LanMCastTable[i].ipAddr,sizeof(LanMCastTable[i].ipAddr),buf,&ind);
             ERR_CHK(rc);
             if ( (!ind) && (rc == EOK))
	     {
                LanMCastTable[i].mcastAliveCount = MAX_MLIFE;
	        i = 0;
	        break;
	     }
	  }
	  if ((i>=LanMCcount) && (LanMCcount < MAX_MCAST_ENTRIES))
	  {
             rc = strcpy_s(LanMCastTable[LanMCcount].ipAddr,sizeof(LanMCastTable[LanMCcount].ipAddr),pBuf);
             if (rc != EOK)
             {
                ERR_CHK(rc);
                return -1;
             }
             LanMCastTable[LanMCcount].mcastAliveCount = MAX_MLIFE;
	     LanMCcount++;
             if (!strncmp(buf, "169.", 4)) 
             {
                v_secure_system("ip route add %s dev brlan0", buf);
             }
	     v_secure_system("arp -i brlan10 -Ds %s brlan0 pub", buf);
             snprintf(logbuf, sizeof(logbuf), "route and arp added for ip address %s \n", buf);
             mrd_log(logbuf);
	  }
          rc = memset_s(buf,sizeof(buf), 0, sizeof(buf));
          ERR_CHK(rc);
       }
       v_secure_pclose(fp);      
       rc = memset_s(buf,sizeof(buf), 0, sizeof(buf));
       ERR_CHK(rc);
       if(!(fp = v_secure_popen("r","/usr/ccsp/moca/moca_mroute_ip.sh")))
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
#ifdef MRD_DEBUG
                snprintf(logbuf, MRD_LOG_BUFSIZE, "ip address is in whitelist \n", buf);
                mrd_log(logbuf);
#endif
                   }
                }
             }
          }
                  
          wan_stat = 0;
	  for(i = 0;i<WanMCcount;i++)
	  {
             rc = strcmp_s(WanMCastTable[i].ipAddr,sizeof(WanMCastTable[i].ipAddr),buf,&ind);
             ERR_CHK(rc);
             if ((!ind) && (rc == EOK))
	     {
                wan_stat = WanMCastTable[i].mcastStatus;
                WanMCastTable[i].mcastStatus = mrd_flag;
                WanMCastTable[i].mcastAliveCount = MAX_MLIFE;
		i = 0;
		break;
	     }
	  }
	  if ((i>=WanMCcount) && (WanMCcount < MAX_MCAST_ENTRIES))
	  {
             rc = strcpy_s(WanMCastTable[WanMCcount].ipAddr,sizeof(WanMCastTable[WanMCcount].ipAddr),pBuf);
             if (rc != EOK)
             {
                ERR_CHK(rc);
                return -1;
             }
             WanMCastTable[WanMCcount].mcastStatus = mrd_flag;
             WanMCastTable[WanMCcount].mcastAliveCount = MAX_MLIFE;
	     WanMCcount++;
             i++;
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
                snprintf(logbuf, sizeof(logbuf), "ip address %s added to mcast list \n", buf);
                mrd_log(logbuf);
             }
             else  
             {
                if (!strncmp(buf, "169.", 4)) 
                {
	           v_secure_system("ip route delete %s table moca", buf);
                }
	        v_secure_system("arp -d %s", buf);
                snprintf(logbuf, sizeof(logbuf), "ip address %s deleted from mcast list \n", buf);
                mrd_log(logbuf);
	        for(i = 0;i<WanMCcount;i++)
	        {
                   rc = strcmp_s(WanMCastTable[i].ipAddr,sizeof(WanMCastTable[i].ipAddr),buf,&ind);
                   ERR_CHK(rc);
                   if ((!ind) && (rc == EOK))
                   {
                      WanMCastTable[i].mcastStatus = 0;
                   }
                }
            }
	 }
         rc = memset_s(buf,sizeof(buf), 0, sizeof(buf));
         ERR_CHK(rc);
     }
     v_secure_pclose(fp);
     mrd_updateMcastlistlifetime();
     sleep(10);
     if((cnt == 0)||(cnt ==3))
     {
     	send_pings();
        cnt = 0;
     }
     cnt++;
     shmrefreshCount--;
     if (shmrefreshCount== 0)
     {
        shmrefreshCount=MAX_SHMRCOUNT;
        if (mrd_wlist && (shmid > 0)) 
        {
           shmdt((void *) mrd_wlist);
           sleep(5);
           shmid = 0;
        }
        if (wlist_stat == 0)  
        {
           mrd_shmattach();
        }
     }
     if (wlist_stat != 0) { 
        wlist_stat = mrd_wlistInit();
     }
  } //while
}

