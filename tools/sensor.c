/*
    sensor.c
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
#include <nxttools.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

void usage(char *cmd,int r) {
  int i;
  FILE *out = r==0?stdout:stderr;
  fprintf(out,"Usage: %s [OPTIONS]\n",cmd);
  fprintf(out,"Gets a sensor value\n");
  fprintf(out,"Options:\n");
  fprintf(out,"\t-h         Show help\n");
  fprintf(out,"\t-n NXTNAME Name of NXT (Default: first found) or bluetooth address\n");
  fprintf(out,"\t-s SENSOR  Select sensor (Default: Choosen by type, or 1)\n");
  fprintf(out,"\t-t TYPE    Select sensor type (Default: none)\n");
  fprintf(out,"\tValid sensor types are:\n");
  for (i=0;i<NXT_NUM_TYPES;i++) fprintf(out,"\t\t%s\n",nxt_getType(i));
  fprintf(out,"\n");
  fprintf(out,"\t-m MODE    Select sensor mode (Default: raw)\n");
  fprintf(out,"\tValid sensor modes are:\n");
  for (i=0;i<NXT_NUM_MODES;i++) fprintf(out,"\t\t%s\n",nxt_getMode(i));
  fprintf(out,"\n");
  fprintf(out,"\t-r         Reset sensor after reading\n");
  fprintf(out,"\t-v         Verbose mode\n");
  exit(r);
}

int main(int argc,char *argv[]) {
  char *name = NULL;
  int type = NXT_SENSOR_TYPE_NONE;
  int mode = NXT_SENSOR_MODE_RAW;
  int reset = 0;
  int c,newsensor,newtype,newmode;
  char *unit = NULL;
  int sensor = -1;
  int verbose = 0;
  int val;

  while ((c = getopt(argc,argv,":hs:m:t:n:rv"))!=-1) {
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
        newtype = nxt_str2type(optarg);
        if (newtype==-1) {
          fprintf(stderr,"Invalid sensor type: %s\n",optarg);
          usage(argv[0],1);
        }
        else type = newtype;
        break;
      case 'm':
        newmode = nxt_str2mode(optarg);
        if (newmode==-1) {
          fprintf(stderr,"Invalid sensor mode: %s\n",optarg);
          usage(argv[0],1);
        }
        else mode = newmode;
        break;
      case 'r':
        reset = 1;
        break;
      case 'n':
        name = strdup(optarg);
        break;
      case 'v':
        verbose = 1;
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

  if (sensor==-1) {
    if (type==NXT_SENSOR_TYPE_REFLECTION || type==NXT_SENSOR_TYPE_LIGHT_ACTIVE || type==NXT_SENSOR_TYPE_LIGHT_INACTIVE) sensor = 2;
    else if (type==NXT_SENSOR_TYPE_SOUND_DB || type==NXT_SENSOR_TYPE_SOUND_DBA) sensor = 1;
    else sensor = 0;
  }
  if (unit==NULL) {
    if (mode==NXT_SENSOR_MODE_CELSIUS) unit = "°C";
    else if (mode==NXT_SENSOR_MODE_FAHRENHEIT) unit = "°F";
    else if (mode==NXT_SENSOR_MODE_PERCENT) unit = "%";
    else unit = "";
  }

  nxt_t *nxt = nxt_open(name);
  if (nxt==NULL) {
    fprintf(stderr,"Could not find NXT\n");
    return 1;
  }

  nxt_setsensormode(nxt,sensor,type,mode);
  usleep(100000);
  val = nxt_getsensorval(nxt,sensor);
  if (reset) nxt_setsensormode(nxt,sensor,NXT_SENSOR_TYPE_NONE,NXT_SENSOR_MODE_RAW);

  if (val<0) fprintf(stderr,"Error: %s\n",nxt_strerror(nxt_error(nxt)));
  else {
    if (verbose) printf("Sensor %d: ",sensor+1,val);
    printf("%d%s\n",val,verbose?unit:"");
  }

  int ret = nxt_error(nxt);
  if (name!=NULL) free(name);
  nxt_close(nxt);

  return ret;
}
