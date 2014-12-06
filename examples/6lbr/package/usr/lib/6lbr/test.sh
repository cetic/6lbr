#!/bin/sh

ETC_6LBR=.
for i in `ls $ETC_6LBR`; do
	file=$ETC_6LBR/$i
	if [ -x $file ]; then
		$file
	fi
done