#/bin/sh
DIR=`dirname $0`
ps -ef | grep -v grep | grep nxtd > /dev/null
if test "X_$?" != "X_0"; then
   echo starting nxtd
   ../../bin/nxtd $* &
   echo please wait
   sleep 11
fi

NXTLIB_BIN=`dirname $0`/../../../bin

killall nxt_server 2>/dev/null

KEY=$$
IN=/tmp/nxt_server_in_$KEY
OUT=/tmp/nxt_server_out_$KEY
ERR=/tmp/nxt_server_err_$KEY

KILL_NXT_SERVER=1

function nxt_cleanup {
 rm -f $IN
 rm -f $OUT
 rm -f $ERR
 if test "X_$KILL_NXT_SERVER" = "1" ; then
   killall -15 $NXTLIB_BIN/nxt_server 2>/dev/null
 fi
}

# start nxt_server
# on failure of nxt_server cleanup and kill this script
($NXTLIB_BIN/nxt_server -o $NXT_NAME $KEY || (nxt_cleanup;kill -15 $KEY)) &

awk -v key=$$ '
BEGIN { 
   print "drive a quadrat with synchronised motors B and C and motorcross wheels";

   inp="/tmp/nxt_server_in_" key;
   out="/tmp/nxt_server_out_" key;
   err="/tmp/nxt_server_err_" key;

   # wait till server has started
   while (system("ls " inp " 2>/dev/null >/dev/null")!=0);

   for (i=1;i<=4;i++) {
      print "p 40" > inp;
      print "motor_travel" > inp;
      fflush(inp);
      getline error < err;
      getline output < out;
      close(inp);
      close(err);
      close(out);

      system("sleep 3");

      print "p 75" > inp;
      print "t 100" > inp;
      print "motor_travel" > inp;
      fflush(inp);
      getline error < err;
      getline output < out;
      close(inp);
      close(err);
      close(out);

      system("sleep 0.45");
   }

   print "p 0" > inp;
   print "motor_travel" > inp;
   fflush(inp);
   getline error < err;
   getline output < out;
   close(inp);
   close(err);
   close(out);
}'
