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
#include <nxt_i2c/nxtcam.h>
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
  int c,i,n,newport;
  int port = 0;
  int verbose = 0;
  int reset = 0;
  int mode = -1;
  nxt_cam_object_t objects[8];

  while ((c = getopt(argc,argv,":hn:s:vrs:ol"))!=-1) {
    switch(c) {
      case 'h':
        usage(argv[0],0);
        break;
      case 'n':
        name = optarg;
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
      case 'o':
        mode = NXT_CAM_TRACKING_OBJECT;
        break;
      case 'l':
        mode = NXT_CAM_TRACKING_LINE;
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

  if (mode!=-1) {
    nxt_cam_set_trackingmode(nxt,port,mode);
    nxt_wait_after_communication_command();
  }
  nxt_cam_enable_tracking(nxt,port,1);

  n = nxt_cam_num_objects(nxt,port);
  if (n==-1) {
    fprintf(stderr,"Error: %s\n",nxt_strerror(nxt_error(nxt)));
  }
  else if (nxt_cam_get_objects(nxt,port,0,n,objects)==-1) {
    fprintf(stderr,"Error: %s\n",nxt_strerror(nxt_error(nxt)));
  }
  else {
    if (verbose) {
      printf("Sensor %d:\n",port+1);
      printf("Objects: %d\n",n);
      for (i=0;i<n;i++) {
        printf("pos = (%d, %d);\tsize = (%d, %d);\tcolor = %d\n", objects[i].x, objects[i].y, objects[i].w, objects[i].h,objects[i].color);
      }
    }
    else {
      printf("%d\n",n);
      for (i=0;i<n;i++) {
        printf("%d %d %d %d %d\n", objects[i].x, objects[i].y, objects[i].w, objects[i].h, objects[i].color);
      }
    }
  }

  if (reset) {
    nxt_cam_enable_tracking(nxt,port,0);
    nxt_wait_after_communication_command();
    nxt_setsensormode(nxt,port,NXT_SENSOR_TYPE_NONE,NXT_SENSOR_MODE_RAW);
  }

  int ret = nxt_error(nxt);
  nxt_close(nxt);

  return ret;
}
