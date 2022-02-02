#!/bin/sh
filter=`ip -s mroute |grep 'Iif: brlan0' | cut -d '(' -f 2 | cut -d ',' -f 1`
for val in $filter
do
    ip -4 nei show |grep brlan0 |grep -v FAILED |cut -d '(' -f 2 | cut -d ',' -f 1|awk '{print $1}' | grep $val
done   