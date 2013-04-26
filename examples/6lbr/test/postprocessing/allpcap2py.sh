#!/bin/bash

CALLDIR=$(dirname $(readlink -f $0))

REPORTDIR=${CALLDIR}/../report
PROFILE=${HOME}/.wireshark/profiles/tshark-6lbr-raw-udp/disabled_protos
FILEMASKBR="br0.pcap"
FILEMASKWSN="radiolog.pcap"

which tshark > /dev/null 2>&1
if [ $? -gt 0 ]
then
	echo "Tshark is missing"
	exit 1
fi

if [ ! -f ${PROFILE} ]
then
	echo "Create the wireshark profile"
	mkdir -p $(dirname ${PROFILE})
	echo "dis" > ${PROFILE}
else
	PROTO=$(cat ${PROFILE})
	if [ $PROTO != "dis" ]
	then
		echo "dis" > ${PROFILE}
	fi
fi

find ${REPORTDIR} -type f -name ${FILEMASKBR} -exec python ${CALLDIR}/pcap2py.py {} \;
find ${REPORTDIR} -type f -name ${FILEMASKWSN} -exec python ${CALLDIR}/pcap2py.py {} \;

