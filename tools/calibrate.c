/*
    tools/calibrate.c
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
#include <string.h>
#include <stdlib.h>

#include <anxt/nxt.h>
#include <anxt/file.h>
#include <anxt/file/cal.h>

void usage(char *cmd,int r) {
  FILE *out = r==0?stdout:stderr;
  fprintf(out,"Usage: %s [OPTIONS]\n",cmd);
  fprintf(out,"Calibrate a sensor\n");
  fprintf(out,"Options:\n");
  fprintf(out,"\t-h         Show help\n");
  fprintf(out,"\t-n NXTNAME Name of NXT (Default: first found) or bluetooth address\n");
  fprintf(out,"\t-s SENSOR  Select sensor (Default: Choosen by type)\n");
  fprintf(out,"\t-t TYPE    Select sensor type: light or sound\n");
  fprintf(out,"\n");
  exit(r);
}

void calibrate_light(nxt_t *nxt,int sensor,unsigned int *min,unsigned int *max) {
  nxt_setsensormode(nxt,sensor,NXT_SENSOR_TYPE_LIGHT_ACTIVE,NXT_SENSOR_MODE_RAW);

  printf("Hold the light sensor over a black field of you test pad\n");
  printf("Press Enter to continue");
  fflush(stdout);
  while (getchar()!='\n');
  *min = nxt_getsensorval(nxt,sensor);
  printf("Minimum value (raw): %d\n\n",*min);

  printf("Hold the light sensor over a white field of you test pad\n");
  printf("Press Enter to continue");
  fflush(stdout);
  while (getchar()!='\n');
  *max = nxt_getsensorval(nxt,sensor);
  printf("Maximum value (raw): %d\n\n",*max);

  nxt_setsensormode(nxt,sensor,NXT_SENSOR_TYPE_NONE,NXT_SENSOR_MODE_RAW);
}

void calibrate_sound(nxt_t *nxt,int sensor,unsigned int *min,unsigned int *max) {
  nxt_setsensormode(nxt,sensor,NXT_SENSOR_TYPE_SOUND_DBA,NXT_SENSOR_MODE_RAW);

  printf("Be quiet around your sound sensor\n");
  printf("Press Enter to continue");
  fflush(stdout);
  while (getchar()!='\n');
  *min = nxt_getsensorval(nxt,sensor);
  printf("Minimum value (raw): %d\n\n",*min);

  printf("Make noise around your sound sensor\n");
  printf("Press Enter to continue");
  fflush(stdout);
  while (getchar()!='\n');
  *max = nxt_getsensorval(nxt,sensor);
  printf("Maximum value (raw): %d\n\n",*max);

  nxt_setsensormode(nxt,sensor,NXT_SENSOR_TYPE_NONE,NXT_SENSOR_MODE_RAW);
}

int main(int argc,char *argv[]) {
  char *name = NULL;
  enum {
    NONE,
    LIGHT,
    SOUND
  } type = NONE;
  int c,newsensor;
  int sensor = -1;
  int mode;

  while ((c = getopt(argc,argv,":hs:m:t:n:"))!=-1) {
    switch(c) {
      case 'h':
        usage(argv[0],0);
        break;
      case 's':
        newsensor = atoi(optarg)-1;
        if (newsensor<0 || newsensor>3) {
          fprintf(stderr,"Invalid sensor: %s\n",optarg);
          usage(argv[0],1);
        }
        else sensor = newsensor;
        break;
      case 't':
        if (strcmp(optarg,"light")==0) type = LIGHT;
        else if (strcmp(optarg,"sound")==0) type = SOUND;
        else {
          fprintf(stderr,"Invalid sensor type: %s\n",optarg);
          usage(argv[0],1);
        }
        break;
      case 'n':
        name = strdup(optarg);
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

  if (type==NONE) {
    fprintf(stderr,"No sensor type selected\n");
    usage(argv[0],1);
  }

  if (sensor==-1) {
    if (type==LIGHT) sensor = 2;
    else sensor = 1;
  }

  printf("Initializing calibration. Please wait...\n\n");

  nxt_t *nxt = nxt_open(name);
  if (nxt==NULL) {
    fprintf(stderr,"Could not find NXT\n");
    return 1;
  }

  unsigned int min,max;
  type==LIGHT?calibrate_light(nxt,sensor,&min,&max):calibrate_sound(nxt,sensor,&min,&max);

  printf("Writing calibration to brick\n");

  void *cal_data = NULL;
  size_t size = cal_encode(&cal_data,min,max);
  if (cal_data!=NULL) {
    int fh = nxt_file_open(nxt,type==LIGHT?"Light Sensor.cal":"Sound Sensor.cal",NXT_OWOVER,size);
    if (fh!=-1) {
      nxt_file_write(nxt,fh,cal_data,size);
      nxt_file_close(nxt,fh);
    }
    else fprintf(stderr,"Error: %s\n",nxt_strerror(nxt_error(nxt)));
    free(cal_data);
  }
  else fprintf(stderr,"Error: Could not generate calibration file\n");

  int ret = nxt_error(nxt);
  if (name!=NULL) free(name);
  nxt_close(nxt);

  printf("Calibration finished\n");

  return ret;
}
