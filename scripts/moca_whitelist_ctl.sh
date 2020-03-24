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
#source /etc/device.properties

touch /tmp/moca_whitelist.txt

case $1 in

    add)
        exist=`grep "$2\." /tmp/moca_whitelist.txt`
        if [ "$exist" != "" ]
        then
            echo "moca-ip-address $2 already exist in whitelist"
            exit 0
        fi
        echo "$2." >> /tmp/moca_whitelist.txt
        sysevent set firewall-restart
        echo "moca-whitelist"
        cat /tmp/moca_whitelist.txt
        ;;

    del)
        exist=`grep "$2\." /tmp/moca_whitelist.txt`
        if [ "$exist" = "" ]
        then
            echo "moca-ip-address $2 does not exist in whitelist"
            exit 0
        fi
        grep -v "$2\." /tmp/moca_whitelist.txt > /tmp/moca_whitelist.txt.tmp
        mv /tmp/moca_whitelist.txt.tmp /tmp/moca_whitelist.txt
        sysevent set firewall-restart
        echo "moca-whitelist"
        cat /tmp/moca_whitelist.txt
        ;;

    check)
        proxy_arps=`arp -n | grep PERM | awk '{print $2}' | sed 's/(//' | sed 's/)//' | grep 169.254`
        #proxy_arps=`arp -na | awk '{print $2}' | sed 's/(//' | sed 's/)//' | grep 96.118`
        #proxy_arps=`arp -na | awk '{print $2}' | sed 's/(//' | sed 's/)//' | grep 169.254`
        #echo $proxy_arps
        for parp in $proxy_arps
        do
            exist=`grep "$parp\." /tmp/moca_whitelist.txt`
            if [ "$exist" = "" ]
            then
                echo "Warning: $parp not in moca-whitelist"
            fi
        done
        ;;

    show)
        echo "moca-whitelist"
        cat /tmp/moca_whitelist.txt
        ;;

    *)
        echo "Usage: moca_whitelist.sh [add|del|show|check] [moca-ip-address]"
        ;;

esac



