#!/bin/bash

# General environment variables

#export CONTIKI=../../../..
#export SIXLBR=${CONTIKI}/examples/6lbr
#export COOJA=${CONTIKI}/tools/cooja

#export JAVA_HOME=/usr/lib/jvm/default-java

# Simulation configuration

SETUP=$1
shift

SIXLBR_LIST=$@

if [ "$SIXLBR_LIST" == "" ]; then
	SIXLBR_LIST="6lbr"
fi

DEV_TAP_MAC=02:a:b:c:d:e

function find_xterm() {
	if [ -x /etc/alternatives/x-terminal-emulator ]; then
		XTERM=/etc/alternatives/x-terminal-emulator
	elif [ -x /usr/bin/gnome-terminal ]; then
		XTERM=/usr/bin/gnome-terminal
	else
		XTERM=xterm
	fi
}

function create-tap() {
	sudo tunctl -t $1 -g netdev
	sudo ip link set $1 address $DEV_TAP_MAC up
}
	
function remove-tap() {
	sudo tunctl -d $1
}

function create-bridge() {
  echo
}

function remove-bridge() {
  echo
}
	
function launch-6lbr() {
	if [[ ! -e $1/nvm.dat && -e $1/nvm.init ]]; then
		params=`cat $1/nvm.init`
		${SIXLBR}/tools/nvm_tool --new $1/nvm.dat $params
	fi
	for plugin in $SIXLBR_PLUGINS; do
		cp $plugin $1/
	done
	export RUN_6LBR=$1
	$XTERM -T $1 -e "${SIXLBR}/package/usr/bin/6lbr $1/6lbr.conf" &
}

function build-cooja() {
	ant -f ${CONTIKI}/tools/cooja/build.xml jar
}

function launch-cooja() {
	$XTERM -T COOJA -e "java -jar ${COOJA}/dist/cooja.jar -quickstart=$SETUP -contiki=${CONTIKI}" &
}

find_xterm


TAP_ID=0
for sixlbr in $SIXLBR_LIST; do
	create-tap "tap$TAP_ID"
	TAP_ID=$((TAP_ID + 1))
done

build-cooja
launch-cooja
PID_COOJA=$!

TAP_ID=0
for sixlbr in $SIXLBR_LIST; do
	echo "Launching '$sixlbr'"
	launch-6lbr $sixlbr
	#PID_6LBR=$!
	TAP_ID=$((TAP_ID + 1))
done

echo Press enter to close simulation...
read dummy

#kill $PID_6LBR
#wait $PID_6LBR > /dev/null

killall 6lbr

kill $PID_COOJA
wait $PID_COOJA > /dev/null

sleep 1

TAP_ID=0

for sixlbr in $SIXLBR_LIST; do
	remove-tap "tap$TAP_ID"
	TAP_ID=$((TAP_ID + 1))
done
