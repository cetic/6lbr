#!/bin/bash

CALLDIR=$(dirname $(readlink -f $0))

REPORTDIR=${CALLDIR}/../report
FILEMASK="br0.pcap"

find ${REPORTDIR} -type f -name ${FILEMASK} -exec python ${CALLDIR}/pcap2py.py {} \;

