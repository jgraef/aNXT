/*
    motor.c
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
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>

void usage(char *cmd,int r) {
  FILE *out = r==0?stdout:stderr;
  fprintf(out,"Usage: %s [OPTIONS]\n",cmd);
  fprintf(out,"Control a NXT motor\n");
  fprintf(out,"Options:\n");
  fprintf(out,"\t-h          Show help\n");
  fprintf(out,"\t-n NXTNAME  Name of NXT (Default: first found) or bluetooth address\n");
  fprintf(out,"\t-m MOTOR    Select motor (Default: A. Valid values are: A, B, C, ABC)\n");
  fprintf(out,"\t-p POWER    Set power (Default: (50 (\"weak\")))\n");
  fprintf(out,"\t-r ROTATION Set rotation (Default: 0. 0 means unlimited rotation)\n");
  fprintf(out,"\t-s          Stop motor (same as '-p 0')\n");
  fprintf(out,"\t-b          Use brake\n");
  fprintf(out,"\t-y          Synchronise motor (be carefull, read manpage first)\n");
  fprintf(out,"\t-i          Idle motor\n");
  exit(r);
}

int main(int argc,char *argv[]) {
  char *name = NULL;
  int motor = 0;
  int power = 50;
  int rot = 0;
  int brake = 0;
  int synchronise = 0;
  int idle = 0;
  int c,newmotor,newpower,newrot;

  while ((c = getopt(argc,argv,":hm:p:r:sibyn:"))!=-1) {
    switch(c) {
      case 'h':
        usage(argv[0],0);
        break;
      case 'm':
        newmotor = nxt_str2motorport(optarg);
        if (newmotor==-1) {
          fprintf(stderr,"Invalid motor: %s\n",optarg);
          usage(argv[0],1);
        }
        else motor = newmotor;
        break;
      case 'p':
        newpower = atoi(optarg);
        if (newpower<-100 || newpower>100) {
          fprintf(stderr,"Invalid power: %d\n",newpower);
          usage(argv[0],1);
        }
        else power = newpower;
        break;
      case 'r':
        newrot = atoi(optarg);
        if (newrot<0) {
          fprintf(stderr,"Invalid rotation: %d\n",newrot);
          usage(argv[0],1);
        }
        else rot = newrot;
        break;
      case 's':
        power = 0;
        break;
      case 'b':
        brake = 1;
        break;
      case 'i':
        idle = 1;
        break;
      case 'y':
        synchronise = 1;
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

  nxt_init();
  nxt_t *nxt = nxt_open(name);
  if (nxt==NULL) {
    fprintf(stderr,"Could not find NXT\n");
    return 1;
  }

  if (idle)
    nxt_motor(nxt,motor,rot,brake?0:power,NXT_MOTORON|(brake?NXT_BRAKE|NXT_REGULATED:0),NXT_REGMODE_IDLE);
  else
    nxt_motor(nxt,motor,rot,brake?0:power,NXT_MOTORON|(brake?NXT_BRAKE|NXT_REGULATED:synchronise?NXT_REGULATED:0),synchronise?(brake?NXT_REGMODE_MOTOR_SPEED:NXT_REGMODE_MOTOR_SYNC):NXT_REGMODE_MOTOR_SPEED);

  int ret = nxt_error(nxt);
  if (name!=NULL) free(name);
  nxt_close(nxt);

  return ret;
}
