#!/bin/sh

PIDFILE=/var/run/6lbr.pid
BIN_6LBR=/usr/bin/6lbr
CONF=/etc/6lbr/6lbr.conf

. $CONF

if [ ! -e $DEV_RADIO ]; then
	sleep 10
fi

$BIN_6LBR $CONF &
	
echo $! > $PIDFILE
