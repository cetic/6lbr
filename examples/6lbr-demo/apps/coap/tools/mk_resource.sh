#!/bin/bash

if [[ $# < 1 ]]; then
	echo "Invalid number of parameters"
	exit 1
fi

for resourceid in $@; do
	RESOURCEID=`echo $resourceid | tr '[:lower:]' '[:upper:]'`

	sed "s/resourceid/$resourceid/g; s/RESOURCEID/$RESOURCEID/g" `dirname $0`/resource-template.h > $resourceid-resource.h
done