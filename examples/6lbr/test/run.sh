#!/bin/bash

CONTIKIDIR="/home/test/git/contiki"
CALLDIR=$(dirname $(readlink -f $0))

while getopts ":c" opt; do
 case $opt in
  c)
   pushd ${CONTIKIDIR}/tools/cooja
   ant clean
   ant jar
   cd apps/serial2pty
   ant clean
   ant jar
   cd ../radiologger-headless
   ant clean
   ant jar
   popd
  ;;
 esac
done

sudo rm -rf ${CALLDIR}/*.pyc
sudo pkill -9 radvd
sudo pkill -9 ping
sudo pkill -9 java
sudo pkill -9 tcpdump
sudo pkill -9 nc
sudo pkill -9 udpserver
sudo pkill cetic_6lbr_rpl_root
sudo pkill cetic_6lbr_router
sudo pkill cetic_6lbr_smart_bridge
sudo pkill cetic_6lbr_transparent_bridge
sudo ip link set br0 down
sudo brctl delbr br0

sudo rm -f ${CALLDIR}/COOJA.*log
sudo rm -f ${CALLDIR}/radiolog-*.pcap


sudo ${CALLDIR}/top.py

sudo chown -R test:test report/
