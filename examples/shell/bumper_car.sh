#/bin/sh
DIR=`dirname $0`
ps -ef | grep -v grep | grep nxtd > /dev/null
if test "X_$?" != "X_0"; then
   echo starting nxtd
   ../../bin/nxtd $* &
   echo please wait
   sleep 11
fi

echo bumper car with ultrasonic sensor on port 4 and synchronised motors B and C
while true; do
   $DIR/../../bin/nxt_motor_travel -p 30
   if test `$DIR/../../bin/nxt_sensorus` -le 20 ; then
      $DIR/../../bin/nxt_motor_travel -p -40
      sleep 1.2
      $DIR/../../bin/nxt_motor_travel -p -40 -t100 
      sleep 1.3
   fi
done
$DIR/../../bin/nxt_motor_travel -p 0 -b
$DIR/../../bin/nxt_motor -mB -s 
$DIR/../../bin/nxt_motor -mC -s 

