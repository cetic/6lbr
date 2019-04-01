#!/bin/bash

set -e

# Simulation configuration

ACTION=run

if [ "$1" = "--clean" ]; then
	ACTION=clean
	shift
elif [ "$1" = "--run" ]; then
	ACTION=run
	shift
fi

SETUP=$1
shift

SIXLBR_LIST=$@

if [ "$SIXLBR_LIST" == "-" ]; then
	SIXLBR_LIST=
elif [ "$SIXLBR_LIST" == "" ]; then
	SIXLBR_LIST="6lbr"
fi

DEV_TAP_MAC="02:a:b:c:d:%x"

OS=`uname`

case $OS in
    Linux)
        ;;
    Darwin)
        export JAVA_HOME=$(/usr/libexec/java_home)
        ;;
   *)
        echo Unknown OS
        ;;
esac

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
	mac=$(printf $DEV_TAP_MAC $2)
	sudo ip link set $1 address $mac up
	if [ "$BRIDGE" = "" ]; then
		sudo sysctl -w net.ipv6.conf.$1.accept_ra=2
		sudo sysctl -w net.ipv6.conf.$1.accept_ra_rt_info_max_plen=64
		if [ "$DEV_TAP_IP6" != "" ]; then
			sudo ip addr add $DEV_TAP_IP6 dev $1
		fi
		if [ "$DEV_TAP_IP4" != "" ]; then
			sudo ip addr add $DEV_TAP_IP4 dev $1
		fi
		if [ "$ROUTE" != "" ]; then
			sudo ip route add $ROUTE via $GW
		fi
	else
		sudo brctl addif $BRIDGE $1
		sudo ip addr flush dev $1
	fi
}
	
function remove-tap() {
	sudo ip link set $1 down
	if [ "$BRIDGE" != "" ]; then
		sudo brctl delif $BRIDGE $1
	fi
	sudo tunctl -d $1
}

function create-bridge() {
	if [ "$BRIDGE" != "" ]; then
		sudo brctl addbr $BRIDGE
		sudo brctl setfd $BRIDGE 0
		sudo ip link set $BRIDGE up
		sudo sysctl -w net.ipv6.conf.$BRIDGE.accept_ra=2
		sudo sysctl -w net.ipv6.conf.$BRIDGE.accept_ra_rt_info_max_plen=64
		if [ "$DEV_TAP_IP6" != "" ]; then
			sudo ip addr add $DEV_TAP_IP6 dev $BRIDGE
		fi
		if [ "$DEV_TAP_IP4" != "" ]; then
			sudo ip addr add $DEV_TAP_IP4 dev $BRIDGE
		fi
		if [ "$ROUTE" != "" ]; then
			sudo ip route add $ROUTE via $GW
		fi
	fi
}

function remove-bridge() {
	if [ "$BRIDGE" != "" ]; then
		sudo ip link set $BRIDGE down
		sudo ip addr flush dev $BRIDGE
		sudo brctl delbr $BRIDGE
	fi
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

function clean() {
	#The cleansing might fail
	set +e

	killall 6lbr || true

        sleep 1

	TAP_ID=0

	for sixlbr in $SIXLBR_LIST; do
		remove-tap "tap$TAP_ID"
		TAP_ID=$((TAP_ID + 1))
	done

	remove-bridge
}

function run() {
	find_xterm

	create-bridge

	TAP_ID=0
	TAP_MAC=14
	for sixlbr in $SIXLBR_LIST; do
		create-tap "tap$TAP_ID" $TAP_MAC
		TAP_ID=$((TAP_ID + 1))
		TAP_MAC=$((TAP_MAC + 1))
	done

	build-cooja
	launch-cooja
	PID_COOJA=$!

	TAP_ID=0
	for sixlbr in $SIXLBR_LIST; do
		echo "Launching '$sixlbr'"
		launch-6lbr $sixlbr
		TAP_ID=$((TAP_ID + 1))
	done

	echo Press enter to close simulation...
	read dummy

	kill $PID_COOJA || true
	wait $PID_COOJA > /dev/null 2>&1 || true

        clean
}

case $ACTION in
	run)
		run
		;;

	clean)
		clean
		;;

	*)
		echo "Unknown action '$ACTION'"
		;;
esac
