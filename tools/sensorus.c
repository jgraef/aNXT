/*
    tools/sensorus.c
    aNXT - a NXt Toolkit
    Libraries and tools for LEGO Mindstorms NXT robots
    Copyright (C) 2008  Janosch Gräf <janosch.graef@gmx.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <anxt/nxt.h>
#include <anxt/i2c/us.h>

void usage(char *cmd,int r) {
  FILE *out = r==0?stdout:stderr;
  fprintf(out,"Usage: %s [OPTION]...\n",cmd);
  fprintf(out,"Get measurement from Ultrasonic sensor from NXT\n");
  fprintf(out,"Options:\n");
  fprintf(out,"\t-h         Show help\n");
  fprintf(out,"\t-n NXTNAME Name of NXT (Default: first found) or bluetooth address\n");
  fprintf(out,"\t-s SENSOR  Specify sensor port (Default: 4)\n");
  fprintf(out,"\t-r         Reset sensor after reading\n");
  fprintf(out,"\t-v         Verbose mode\n");
  exit(r);
}

int main(int argc,char *argv[]) {
  char *name = NULL;
  int c,newport;
  int port = 3;
  int verbose = 0;
  int reset = 0;
  int dist;

  while ((c = getopt(argc,argv,":hn:s:vr"))!=-1) {
    switch(c) {
      case 'h':
        usage(argv[0],0);
        break;
      case 'n':
        name = strdup(optarg);
        break;
      case 'v':
        verbose = 1;
        break;
      case 'r':
        reset = 1;
        break;
      case 's':
        newport = atoi(optarg)-1;
        if (NXT_VALID_SENSOR(newport)) port = newport;
        else {
          fprintf(stderr,"Invalid sensor: %s\n",optarg);
          usage(argv[0],1);
        }
        break;
      case ':':
        fprintf(stderr,"Option -%c requires an operand\n",optopt);
        usage(argv[0],1);
        break;
      case '?':
        fprintf(stderr,"Unrecognized option: -%c\n", optopt);
        usage(argv[0],1);
        break;
    }
  }

  nxt_t *nxt = nxt_open(name);
  if (nxt==NULL) {
    fprintf(stderr,"Could not find NXT\n");
    return 1;
  }

  nxt_set_sensor_mode(nxt,port,NXT_SENSOR_TYPE_LOWSPEED,NXT_SENSOR_MODE_RAW);
  nxt_wait_after_communication_command();

  dist = nxt_us_get_dist(nxt,port);
  if (reset) {
    nxt_set_sensor_mode(nxt,port,NXT_SENSOR_TYPE_NONE,NXT_SENSOR_MODE_RAW);
  }
  if (dist<0) {
    fprintf(stderr,"Error: %s\n",nxt_strerror(nxt_error(nxt)));
  }
  else {
    if (verbose) {
      printf("Sensor %d: ",port+1);
    }
    if (dist==0xFF) {
      printf("?\n");
    }
    else {
      printf("%d%s\n",dist,verbose?"cm":"");
    }
  }

  int ret = nxt_error(nxt);
  if (name!=NULL) free(name);
  nxt_close(nxt);

  return ret;
}
