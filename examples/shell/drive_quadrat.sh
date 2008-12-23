#/bin/sh
DIR=`dirname $0`
ps -ef | grep -v grep | grep nxtd > /dev/null
if test "X_$?" != "X_0"; then
   echo starting nxtd
   ../../bin/nxtd $* &
   echo please wait
   sleep 11
fi

echo drive a quadrat with synchronised motors B and C and motorcross wheels
for i in 1 2 3 4; do 
   $DIR/../../bin/nxt_motor_travel -p 40
   sleep 3
   $DIR/../../bin/nxt_motor_travel -p 75 -t 100
   sleep 0.45
done
$DIR/../../bin/nxt_motor_travel -p 0

