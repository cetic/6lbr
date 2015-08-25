#!/bin/bash

if [[ $# < 1 ]]; then
	echo "Invalid number of parameters"
	exit 1
fi

for resourceid in $@; do
	cannon_resourceid=`echo $resourceid | tr '-' '_'`
	RESOURCEID=`echo $cannon_resourceid | tr '[:lower:]' '[:upper:]'`

	sed "s/resourceid/$cannon_resourceid/g; s/RESOURCEID/$RESOURCEID/g" `dirname $0`/resource-template.h > $resourceid-resource.h
done