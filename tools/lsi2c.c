/*
    tools/lsi2c.c
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

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <anxt/nxt.h>
#include <anxt/i2c.h>

void usage(char *cmd,int r) {
  FILE *out = r==0?stdout:stderr;
  fprintf(out,"Usage: %s [OPTIONS] WILDCARD\n",cmd);
  fprintf(out,"List I2C sensors connected to NXT\n");
  fprintf(out,"Options:\n");
  fprintf(out,"\t-h          Show help\n");
  fprintf(out,"\t-n NXTNAME  Name of NXT (Default: first found) or bluetooth address\n");
  exit(r);
}

int main(int argc,char *argv[]) {
  char *name = NULL;
  int sensor = NXT_SENSOR1;
  int c, i, ret, newsensor;
  const char *device;

  while ((c = getopt(argc,argv,":hn:s:"))!=-1) {
    switch(c) {
      case 'h':
        usage(argv[0],0);
        break;
      case 'n':
        name = optarg;
        break;
      case 's':
        newsensor = atoi(optarg)-1;
        if (!NXT_VALID_SENSOR(newsensor)) {
          fprintf(stderr,"Invalid sensor: %s\n",optarg);
          usage(argv[0],1);
        }
        else {
          sensor = newsensor;
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

  for (i=0;i<0x100;i++) {
    printf("Scanning: 0x%02X %d%%\r",i,i*100/0x100);
    fflush(stdout);

    device = nxt_i2c_get_deviceid(nxt, sensor, i);
    nxt_wait_after_direct_command();
    if (device!=NULL) {
      printf("at I2C address 0x%02X: %s\n", i, device);
    }
  }

  ret = nxt_error(nxt);
  nxt_close(nxt);

  return ret;
}

 
