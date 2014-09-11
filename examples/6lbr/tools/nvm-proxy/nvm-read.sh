#!/bin/sh

NVM_FILE='nvm.dat'
IP='bbbb::100'
PORT=4000
NC=nc
DELAY=1

OPTS=`getopt -o h -l nvm:,ip:,port:,nc:,delay: -- "$@"`
if [ $? != 0 ]
then
    exit 1
fi

eval set -- "$OPTS"

while true ; do
    case "$1" in
        -h) echo  $USAGE; exit;;
        --nvm) NVM_FILE=$2; shift 2;;
        --ip) IP=$2; shift 2;;
        --port) PORT=$2; shift 2;;
        --nc) NC=$2; shift 2;;
        --delay) DELAY=$2; shift 2;;
        --) shift; break;;
    esac
done

echo | $NC -6 -u -w$DELAY $IP $PORT > $NVM_FILE