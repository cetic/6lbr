
CONTIKI=../../../..
OUTFILE=data.log
TYPE=$1

echo "------ SIMULATION LINE ------"

for FILENAME in $TYPE/*.csc
do
	echo "------ $FILENAME ------"
	#NUM=`perl -e 'my $strn = "${FILENAME}"; $strn =~ m/-([0-9]*)-/; print $1;'`
	sh $CONTIKI/regression-tests/simexec.sh "true" "$FILENAME" "$CONTIKI" "$TYPE/temp" "1"
	echo "------ $FILENAME ------" >> $TYPE/$OUTFILE
	cat $TYPE/temp.testlog >> $TYPE/$OUTFILE
done;

# #----------------------- MULTI PROCESSUS --- /!\ DON'T WORK !!!
# # execute simulation
# I=0
# PIDMAX=0
# PIDMIN=99999999
# for FILENAME in $TYPE/*.csc
# do
# 	echo "--- $FILENAME ---"
# 	java -jar $CONTIKI/tools/cooja/dist/cooja.jar -nogui=$FILENAME -contiki=$CONTIKI -random-seed=1 > $TYPE/temp$I.log &
# 	#sh $CONTIKI/regression-tests/simexec.sh "true" "$FILENAME" "$CONTIKI" "$TYPE/temp$I" "1" &
# 	PIDM=$!
# 	PIDMAX=$(($PIDM>$PIDMAX?$PIDM:$PIDMAX))
# 	PIDMIN=$(($PIDM<$PIDMIN?$PIDM:$PIDMIN))
# 	cat $TYPE/temp.testlog >> $TYPE/$OUTFILE
# 	I=$(($I+1))
# done;

# # Wait until all done
# for ((j=$PIDMIN; j<=$PIDMAX; j++)); do
# 	echo $j
# 	wait $j
# done

# #Merge file
# rm $TYPE/$OUTFILE
# I=0
# for FILENAME in $TYPE/*.csc
# do
# 	echo "------ $FILENAME ------" >> $TYPE/$OUTFILE
# 	cat "$TYPE/temp$I.testlog" >> $TYPE/$OUTFILE
# 	I=$(($I+1))
# done;


echo "--------- END -------------"