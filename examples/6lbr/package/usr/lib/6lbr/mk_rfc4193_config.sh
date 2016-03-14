#!/bin/sh

DIR=`dirname $0`
WSN_SUBNET=aaaa
ETH_SUBNET=bbbb

if [ -n "$1" ]; then
	NVM_DAT=$1
	shift
else
	NVM_DAT=/etc/6lbr/nvm.dat
fi

if [ -n "$1" ]; then
	NVM_TOOL=$1
	shift
else
	NVM_TOOL=$DIR/bin/nvm_tool
fi

echo "Generating RFC4193 prefix"

GEN_RFC4193=$DIR/generate-rfc4193-addr.sh
PREFIX=`$GEN_RFC4193`
echo "Prefix: $PREFIX"


echo "Updating configuration"

WSN_PREFIX=$PREFIX:$WSN_SUBNET::
ETH_PREFIX=$PREFIX:$ETH_SUBNET::

$NVM_TOOL --update \
  --wsn-prefix $WSN_PREFIX --wsn-prefix-len 64 --wsn-ip $WSN_PREFIX::100 \
  --eth-prefix $ETH_PREFIX --eth-prefix-len 64 --eth-ip $ETH_PREFIX::100 \
  $NVM_DAT