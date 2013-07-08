#!/bin/bash

#sudo watch -n 1 "brctl show;echo "===";ifconfig eth0;ifconfig br0;echo "===";ps aux|grep -e nc -e tcpdump -e udpserver -e java|grep -v watch|grep -v sh|grep -v grep;echo "===";tree report/"
#sudo watch -n 1 "brctl show;echo "===";ifconfig br0;echo "===";ps aux|grep -e nc -e tcpdump -e udpserver -e java|grep -v grep;echo "===";tree -h report/"
#sudo watch -n 1 "brctl show;echo "===";ifconfig br0;echo "===";route --inet6|grep br0;echo "===";top -b -n 1|head -n 5;echo "===";ps aux|grep -e nc -e tcpdump -e udpserver -e java|grep -v grep;echo "===";tree -P time.log report/"
sudo watch -n 1 "brctl show;echo ===;ifconfig br0;echo ===;ip -6 route show;echo ===;top -b -n 1|head -n 5;echo ===;ps aux|grep -e nc -e tcpdump -e udpserver -e java -e ping -e radvd -e cetic_6lbr|grep -v grep;echo ===;echo Success;echo -------;find report/ -type f -name time.log -exec dirname {} \;|sort;echo ===;echo Fail;echo ----;find report/ -mindepth 4 -maxdepth 4 -type d -exec bash -c 'ls {}/time.log > /dev/null 2>&1 || echo {}' \; | sort|grep -v test_current;echo ===;echo Current Test;find -type d -name test_current -exec tree -h {} \;"
