#/bin/sh
DIR=`dirname $0`
ps -ef | grep -v grep | grep nxtd > /dev/null
if test "X_$?" != "X_0"; then
   echo starting nxtd
   ../../bin/nxtd $* &
   echo please wait
   sleep 11
fi

echo drive a quadrat with synchronised motors B and C
for i in 1 2 3 4; do 
   $DIR/../../bin/nxt_motor_travel -p 50
   sleep 1.5
   $DIR/../../bin/nxt_motor_travel -p 20
   $DIR/../../bin/nxt_motor_travel -p 45 -t 100
   sleep 1.3
   $DIR/../../bin/nxt_motor_travel -p 20 -t 100
done
$DIR/../../bin/nxt_motor_travel -p 0 -b
$DIR/../../bin/nxt_motor -mB -s 
$DIR/../../bin/nxt_motor -mC -s 

