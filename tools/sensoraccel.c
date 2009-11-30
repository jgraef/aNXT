/*
    sensoraccel.c
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

#include <nxt.h>
#include <nxt_i2c/accel.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void usage(char *cmd,int r) {
  FILE *out = r==0?stdout:stderr;
  fprintf(out,"Usage: %s [OPTION]...\n",cmd);
  fprintf(out,"Get measurement from Acceletation sensor from NXT\n");
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
  struct nxt_accel_vector accel;

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
        if (VALID_SENSOR(newport)) port = newport;
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

  nxt_setsensormode(nxt,port,NXT_SENSOR_TYPE_LOWSPEED,NXT_SENSOR_MODE_RAW);
  nxt_wait_after_communication_command();

  if (nxt_accel_get_accel(nxt,port,&accel)==-1) {
    fprintf(stderr,"Error: %s\n",nxt_strerror(nxt_error(nxt)));
  }
  else {
    if (verbose) {
      printf("Sensor %d:\n",port+1);
      printf("x = %.3f\n",0.001*accel.x);
      printf("y = %.3f\n",0.001*accel.y);
      printf("z = %.3f\n",0.001*accel.z);
    }
    else {
      printf("%d\n%d\n%d\n",accel.x,accel.y,accel.z);
    }
  }

  if (reset) {
    nxt_setsensormode(nxt,port,NXT_SENSOR_TYPE_NONE,NXT_SENSOR_MODE_RAW);
  }

  int ret = nxt_error(nxt);
  if (name!=NULL) free(name);
  nxt_close(nxt);

  return ret;
}
