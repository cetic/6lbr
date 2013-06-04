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

echo "Clean residual processes..."
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

echo "Clean the network config..."
sudo ip link set br0 down
sudo brctl delbr br0

echo "Clean residual files"
sudo rm -f ${CALLDIR}/COOJA.*log
sudo rm -f ${CALLDIR}/radiolog-*.pcap

echo "Freeing memory..."
sudo sync
sudo bash -c "echo 3 > /proc/sys/vm/drop_caches"

echo "Run the tests..."
sudo ${CALLDIR}/top.py > >(tee ${CALLDIR}/console_out.log) 2> >(tee ${CALLDIR}/console_err.log >&2)

echo "Archiving log files"
reportdir=$(find ${CALLDIR}/report -maxdepth 1 -type d | sort | tail -n 1)
sudo mv ${CALLDIR}/console_out.log $reportdir
sudo mv ${CALLDIR}/console_err.log $reportdir
sudo chown -R test:test report/

echo "Drop skipped tests"
find report/ -type f -name SKIPPED -exec dirname {} \; > testtoremove
cat testtoremove | xargs -I{} rm -rf {}
rm testtoremove
