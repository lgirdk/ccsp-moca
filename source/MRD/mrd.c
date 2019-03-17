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
#include "secure_wrapper.h"
char LanMCastTable[20][20];
char WanMCastTable[20][20];
char LanMCcount = 0;
char WanMCcount = 0;
void GetSubnet(char *ip, char *subnet)
{
	int token = 0;
	int i = 0;
	int len = strlen(ip);
	for(i = 0; i <len ; i++)
	{
		subnet[i] = ip[i];
		if(ip[i] == '.')
		{
			token++;
		}
		if(token == 3)
		{
			strcat(subnet,"0/24");
			break;
		}
	}
	printf("IP = %s\n", ip);
	printf("Subnet = %s\n", subnet);
}
int main()
{
    FILE *fp = NULL;
    char buf[200] = {0};
    char subnet[25] = {0};
    char FirstTime = 1;
    char wFirstTime = 1;
	int i = 0;
    memset(LanMCastTable, 0, sizeof(LanMCastTable[0][0]) * 20 * 20);
    memset(WanMCastTable, 0, sizeof(WanMCastTable[0][0]) * 20 * 20);
    memset(subnet, 0, sizeof(subnet));
   while(1)
    {
     snprintf(buf, sizeof(buf), "ip -s mroute |grep 'Iif: brlan0' |grep 169 | cut -d '(' -f 2 | cut -d ',' -f 1|awk '{print $1}'");
     system(buf); 
	if(!(fp = popen(buf, "r")))
	{
	printf("error\n");
	return -1;
	}
    while ( fgets(buf, sizeof(buf), fp)!= NULL )
    {
		char *pos;
		if ((pos=strchr(buf, '\n')) != NULL)
		*pos = '\0';
		if(FirstTime)
		{
			strcpy(LanMCastTable[LanMCcount],buf);
			FirstTime = 0;
			GetSubnet(buf,subnet);
			v_secure_system("ip route add %s dev brlan0", subnet);
			v_secure_system("arp -i brlan10 -Ds %s brlan0 pub", buf);
			LanMCcount++;
		}
		else
		{
			for(i = 0;i<LanMCcount;i++)
			{
				if(strcmp(LanMCastTable[i],buf) == 0)
				{
					i = 0;
					break;
				}
				
			}
			if(i)
			{
				strcpy(LanMCastTable[LanMCcount],buf);
				GetSubnet(buf,subnet);
				v_secure_system("ip route add %s dev brlan0", subnet);
				v_secure_system("arp -i brlan10 -Ds %s brlan0 pub", buf);
				printf("new LanMCastTable[%d] = %s\n",LanMCcount,LanMCastTable[LanMCcount]);
				LanMCcount++;
			}
		}
    }
    pclose(fp);
	
	memset(buf, 0, sizeof(buf));

	snprintf(buf, sizeof(buf), "ip -s mroute |grep 'Iif: brlan10' |grep 169 | cut -d '(' -f 2 | cut -d ',' -f 1|awk '{print $1}'");
	system(buf); 
	if(!(fp = popen(buf, "r")))
	{
	printf("error\n");
	return -1;
	}
    while ( fgets(buf, sizeof(buf), fp)!= NULL )
    {
		char *pos;
		if ((pos=strchr(buf, '\n')) != NULL)
		*pos = '\0';
		if(wFirstTime)
		{
			strcpy(WanMCastTable[WanMCcount],buf);
			wFirstTime = 0;
			GetSubnet(buf,subnet);
			v_secure_system("ip route add %s dev brlan10 table moca", subnet);
			v_secure_system("arp -i brlan0 -Ds %s brlan10 pub", buf);
			WanMCcount++;
		}
		else
		{
			for(i = 0;i<WanMCcount;i++)
			{
				if(strcmp(WanMCastTable[i],buf) == 0)
				{
					i = 0;
					break;
				}
				printf("WanMCastTable[%d] = %s\n",i,WanMCastTable[i]);
				
			}
			if(i)
			{
				strcpy(WanMCastTable[WanMCcount],buf);
				GetSubnet(buf,subnet);
				v_secure_system("ip route add %s dev brlan10 table moca", subnet);
				v_secure_system("arp -i brlan0 -Ds %s brlan10 pub", buf);
				printf("new WanMCastTable[%d] = %s\n",WanMCcount,WanMCastTable[WanMCcount]);
				WanMCcount++;
			}
		}
    }
    pclose(fp);
	sleep(10);
    }

}

