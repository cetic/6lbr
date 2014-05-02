
CONTIKI=../../../..
OUTFILE=data.log

echo "------ SIMULATION LINE ------"

for FILENAME in line/*.csc
do
	echo "------ $FILENAME ------"
	#NUM=`perl -e 'my $strn = "${FILENAME}"; $strn =~ m/-([0-9]*)-/; print $1;'`
	sh $CONTIKI/regression-tests/simexec.sh "true" "$FILENAME" "$CONTIKI" "line/temp" "1"
	echo "------ $FILENAME ------" >> line/$OUTFILE
	cat line/temp.testlog >> line/$OUTFILE
done;

# #----------------------- MULTI PROCESSUS --- /!\ DON'T WORK !!!
# # execute simulation
# I=0
# PIDMAX=0
# PIDMIN=99999999
# for FILENAME in line/*.csc
# do
# 	echo "--- $FILENAME ---"
# 	java -jar $CONTIKI/tools/cooja/dist/cooja.jar -nogui=$FILENAME -contiki=$CONTIKI -random-seed=1 > line/temp$I.log &
# 	#sh $CONTIKI/regression-tests/simexec.sh "true" "$FILENAME" "$CONTIKI" "line/temp$I" "1" &
# 	PIDM=$!
# 	PIDMAX=$(($PIDM>$PIDMAX?$PIDM:$PIDMAX))
# 	PIDMIN=$(($PIDM<$PIDMIN?$PIDM:$PIDMIN))
# 	cat line/temp.testlog >> line/$OUTFILE
# 	I=$(($I+1))
# done;

# # Wait until all done
# for ((j=$PIDMIN; j<=$PIDMAX; j++)); do
# 	echo $j
# 	wait $j
# done

# #Merge file
# rm line/$OUTFILE
# I=0
# for FILENAME in line/*.csc
# do
# 	echo "------ $FILENAME ------" >> line/$OUTFILE
# 	cat "line/temp$I.testlog" >> line/$OUTFILE
# 	I=$(($I+1))
# done;


echo "--------- END -------------"