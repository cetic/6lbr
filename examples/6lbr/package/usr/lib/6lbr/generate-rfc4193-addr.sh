#!/bin/sh
#
#	@(#) generate-rfc4193-addr.sh (ULA)  (c) Sep 2004 - Jun 2011	Holger Zuleger 
#
#	do what the name suggest 
#
#	firstpart = 64-Bit NTP time
#	secondpart = EUI-64 Identifier or 48 Bit MAC-Adress
#	sha1sum ($firstpart | $secondpart )
#	use least significant 40 Bits of sha1sum
#	build global prefix (locally assigned == FD00::/8)
#
#	(M1) 11. May 2006
#	- a check added to complain if firstpart or secondpart is empty
#	- firstpart calculation changed in such a way, that only one transmit
#	  time is stored (ntpdate since version 4.2.0 use a list of ntp servers)
#
#	(M2) 27. Aug 2006
#	Fixed bug in using reference time instead of transmit timestamp.
#	Thanks to Marc A. Donges for finding this out
#
#	(M3) 4. Sep 2006
#	Use ntpq instead of ntpdate because the latter is deprecated.
#	This requires a local running and syncronized ntpd, but
#	speeds up the execution time
#
#	(M4) 29. Dec 2006
#	set LC_ALL=C at the beginning of the script, to be sure the grep command
#	used to scan the output of the ifconfig command finds the expected string
#	Thanks to Ted Percival for finding this out
#
#	(M5) 5. Jun 2011
#	tr command added to remove the trailing newline from the sha1 calculation
#	Thanks to Reinard Max for the fix
#	
PATH=/usr/local/bin:/bin:/usr/bin:/usr/sbin:/sbin

debug=0
USE_NTPQ=0
NTPSERVER=pool.ntp.org

#(M4)
LC_ALL=C
export LC_ALL

#(M3)
if test $USE_NTPQ -eq 1
then
	if time=`ntpq -c rv | grep clock=`
	then
		test $debug -eq 1 && echo "$time"
		firstpart=`echo $time | sed  -e "s/clock=//" -e "s/ .*//" -e "s/\.//"`
	else
		echo "no local ntpd running" 1>&2
		exit 1
	fi
else
	#(M1)
	#(M2)
	firstpart=`ntpdate -d -q $NTPSERVER 2>/dev/null | sed "/transmit timestamp/q" |
		sed  -n "/transmit time/s/^transmit timestamp: *\([^ ]*\) .*/\1/p" |
		tr -d "."`
fi

secondpart=`ifconfig eth0 |
	grep "inet6 addr: fe80" |
	sed  -n "s|^.*::\([^/]*\)/.*|\1|p" |
	tr -d ":"`

#(M1)
if test -z "$firstpart" -o -z "$secondpart"
then
	echo "$0: installation error: check if ntpdate and ifconfig is in search path"
	exit 1
fi

test $debug -eq 1 && echo "Firstpart: $firstpart"
test $debug -eq 1 && echo "Secondpart: $secondpart"
test $debug -eq 1 && echo "123456789o123456789o123456789o123456789o123456789o123456789o"
test $debug -eq 1 && echo ${firstpart}${secondpart} | sha1sum

#(M5)
globalid=`echo ${firstpart}${secondpart} | tr -d "\012" | sha1sum | cut -c31-40`
test $debug -eq 1 && echo $globalid


echo fd${globalid} | sed "s|\(....\)\(....\)\(....\)|\1:\2:\3|"