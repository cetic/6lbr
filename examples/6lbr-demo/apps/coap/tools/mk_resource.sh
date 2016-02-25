#!/bin/bash

if (( $# < 2 )); then
	echo "Invalid number of parameters $#"
	exit 1
fi

TEMPLATE=$1
shift

for resourceid in $@; do
	cannon_resourceid=`echo $resourceid | tr '-' '_'`
	RESOURCEID=`echo $cannon_resourceid | tr '[:lower:]' '[:upper:]'`

	sed "s/resourceid/$cannon_resourceid/g; s/RESOURCEID/$RESOURCEID/g" $TEMPLATE > $resourceid-resource.h
done