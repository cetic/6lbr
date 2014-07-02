#!/bin/sh

NVM_FILE='nvm.dat'
DEV='/dev/ttyUSB1'
BOARD='econotag'
MC1322X_LOAD=`dirname $0`/../../../../cpu/mc1322x/tools/mc1322x-load.pl
BBMC_BIN='bbmc'
FLASHER=''
BBMC_BIN=`dirname $0`/../../../../cpu/mc1322x/tools/ftditools/bbmc
BBMC_LAYOUT='econotag'
DELAY=5000

OPTS=`getopt -o h -l delay:,nvm:,dev:,board:,loader:,bbmc:,bbmc-layout:,flasher: -- "$@"`
if [ $? != 0 ]
then
    exit 1
fi

eval set -- "$OPTS"

while true ; do
    case "$1" in
        -h) echo  $USAGE; exit;;
        --nvm) NVM_FILE=$2; shift 2;;
        --dev) DEV=$2; shift 2;;
        --board) BOARD=$2; shift 2;;
        --loader) MC1322X_LOAD=$2; shift 2;;
        --bbmc) BBMC_BIN=$2; shift 2;;
        --bbmc-layout) BBMC_LAYOUT=$2; shift 2;;
        --flasher) FLASHER=$2; shift 2;;
	--delay) DELAY=$2; shift 2;;
        --) shift; break;;
    esac
done


if [ ! -e "$NVM_FILE" ]; then
	echo "NVM file '$NVM_FILE' not found"
	exit 1
fi

if [ ! -e "$DEV" ]; then
	echo "Device '$DEV' not found"
	exit 1
fi

FLASHER_PATH=`dirname $0`

if  [ "$BBMC_BIN" != "None" ]; then
	BBMC="$BBMC_BIN -l $BBMC_LAYOUT reset"
else
	BBMC="ls"
fi

if [ "$FLASHER" = "" ]; then
	FLASHER="$FLASHER_PATH/6lbr_nvm_flasher_$BOARD.bin"
fi

if [ ! -e "$FLASHER" ]; then
	echo "Flasher firmware '$FLASHER' not found"
	exit 1
fi

$MC1322X_LOAD -b $DELAY -t $DEV -f $FLASHER -c "$BBMC" -z $* 0x100,`hexdump -v -e '"0x" 1/4 "%08x" ","' $NVM_FILE`
