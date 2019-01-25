#!/bin/bash
BULK_SEND_MAX_BYTES=2097152
ATTACKER_RATE=7000
ON_TIME=0.25
BURST_PERIOD=1
SENDER_START=($BURST_PERIOD-$ON_TIME)
OFF_TIME=$SENDER_START

echo "PRESET PARAMETER:"
echo "BYTES=$BULK_SEND_MAX_BYTES RATE=$ATTACKER_RATE ON=$ON_TIME "
echo "PERIOD=$BURST_PERIOD OFF=$OFF_TIME"

echo  " "

ATTACKER_RATE=7000
ON_TIME=0.25
BURST_PERIOD=0.4

sed -i '59c #define BULK_SEND_MAX_BYTES '$BULK_SEND_MAX_BYTES'' scratch/tcp-low-rate.cc
sed -i '62c #define ATTACKER_RATE (std::string)"'$ATTACKER_RATE'kb/s"' scratch/tcp-low-rate.cc

echo " " >> log
echo " " >> log
echo " " >> log
echo " " >> log

mkdir data/3/

for i in $(seq 1 1 32) #
do
echo `date`  >> log
echo "run 3-$i start"  >> log

#	if [ $i -eq 2 ]
#	then
#		BURST_PERIOD=0.8
#	elif [ $i -gt 2 ]
#	then
		BURST_PERIOD=`echo "scale=3;$BURST_PERIOD+0.05" | bc`
#	fi
	echo "PERIOD = $BURST_PERIOD"
	SENDER_START=($BURST_PERIOD-$ON_TIME)	
sed -i '63c #define ON_TIME (std::string)"'$ON_TIME'"' scratch/tcp-low-rate.cc
sed -i '64c #define BURST_PERIOD ('$BURST_PERIOD')' scratch/tcp-low-rate.cc
sed -i '66c #define SENDER_START ('$SENDER_START')' scratch/tcp-low-rate.cc

./waf --run scratch/tcp-low-rate 
#mkdir data/3-$i
mv PCAPs/*4* data/3/3-$i-$BURST_PERIOD.pcap

echo `date`  >> log
echo "run 3-$i done"  >> log
echo "--------------------------" >> log
done



