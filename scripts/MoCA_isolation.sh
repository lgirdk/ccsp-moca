#! /bin/sh
####################################################################################
# If not stated otherwise in this file or this component's Licenses.txt file the
# following copyright and licenses apply:

#  Copyright 2018 RDK Management

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at

# http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#######################################################################################
source /etc/device.properties

HOME_LAN_ISOLATION=`psmcli get dmsb.l2net.HomeNetworkIsolation`
if [ "$HOME_LAN_ISOLATION" -eq 1 ]; then
echo "Starting brlan10 initialization, check whether brlan10 is there or not"
ifconfig | grep brlan10
if [ $? == 1 ]; then
    echo "brlan10 not present go ahead and create it"
    if [ "$BOX_TYPE" = "XF3" ]; then
        sh /usr/ccsp/lan_handler.sh home_lan_isolation_enable
    else
        sysevent set multinet-up 9
    fi

fi

# Waiting for brlan10 -MoCA bridge interface creation for 30 sec
iter=0
max_iter=12
while [ ! -f /tmp/MoCABridge_up ] && [ "$iter" -le $max_iter ]
do
    iter=$((iter+1))
    echo "Sleep Iteration# $iter"
    sleep 10
done

if [ ! -f /tmp/MoCABridge_up ]; then
    echo "brlan10 is not up after maximum iterations i.e 30 sec go ahead with the execution"
else
    echo "brlan10 is created after interation $iter go ahead with the execution"
    killall igmpproxy
    killall MRD
    killall mcsender
    ifconfig brlan0:0 down
    sleep 1
    if [ "$BOX_TYPE" = "XF3" ] || [ "$MODEL_NUM" = "TG4482A" ] || [ "$MODEL_NUM" = "TG3482G" ]; then
    	sh /etc/utopia/service.d/service_mcastproxy.sh mcastproxy-restart
    else 
        sysevent set mcastproxy-restart
    fi
    #smcroute -f /usr/ccsp/moca/smcroute.conf -d
    MRD &
    mcsender -ibrlan0 -t3 239.255.255.250:1900 &
    sysevent set firewall-restart

fi
fi
# restart xupnp services at this stage 
# ie for enable/disable of homenetwork isolation.
if [ "$BOX_TYPE" = "XB3" ]; then
   # for XB3 this would restart_upnp.sh
   # which stop/start and monitor xupnp services 
   sleep 5
   sh /lib/rdk/start-upnp-service restart > /dev/null  2>&1 &
else
   # for other devices killing the process is enough, services would restart.
   pid_xcal_device=`pidof xcal-device`
   if [ "x$pid_xcal_device" != "x" ];then
   	killall xcal-device
   fi
   pid_xdiscovery=`pidof xdiscovery`
   if [ "x$pid_xdiscovery" != "x" ];then
   	killall xdiscovery
   fi
fi
