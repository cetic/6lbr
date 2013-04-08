#!/bin/bash

CALLDIR=$(dirname $(readlink -f $0))
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
sudo ${CALLDIR}/top.py
