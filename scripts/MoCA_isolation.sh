#! /bin/sh

HOME_LAN_ISOLATION=`psmcli get dmsb.l2net.HomeNetworkIsolation`
if [ "$HOME_LAN_ISOLATION" -eq 1 ]; then
echo "Starting brlan10 initialization, check whether brlan10 is there or not"
ifconfig | grep brlan10
if [ $? == 1 ]; then
    echo "brlan10 not present go ahead and create it"
    sysevent set multinet-up 9
fi

# Waiting for brlan10 -MoCA bridge interface creation for 30 sec
iter=0
max_iter=2
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
    sleep 1
    sysevent set mcastproxy-restart
    #smcroute -f /usr/ccsp/moca/smcroute.conf -d
    MRD &
    sysevent set firewall-restart

fi
fi
