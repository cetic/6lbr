#!/bin/bash

#sudo watch -n 1 "brctl show;echo "===";ifconfig eth0;ifconfig br0;echo "===";ps aux|grep -e nc -e tcpdump -e udpserver -e java|grep -v watch|grep -v sh|grep -v grep;echo "===";tree report/"
#sudo watch -n 1 "brctl show;echo "===";ifconfig br0;echo "===";ps aux|grep -e nc -e tcpdump -e udpserver -e java|grep -v grep;echo "===";tree -h report/"
sudo watch -n 1 "brctl show;echo "===";ifconfig br0;echo "===";ps aux|grep -e nc -e tcpdump -e udpserver -e java|grep -v grep;echo "===";tree -P time.log report/"
