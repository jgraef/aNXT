/*
    motor_travel.c
    aNXT - a NXt Toolkit
    Libraries and tools for LEGO Mindstorms NXT robots
    Copyright (C) 2008  Janosch Gräf <janosch.graef@gmx.net>
    Copyright (C) 2008  J. "MUFTI" Scheurich (IITS Universitaet Stuttgart)

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
  fprintf(out,"Control two synchronised NXT motors (reset relative tachos first)\n");
  fprintf(out,"Options:\n");
  fprintf(out,"\t-h           Show help\n");
  fprintf(out,"\t-n NXTNAME   Name of NXT (Default: first found) or bluetooth address\n");
  fprintf(out,"\t-m MOTORS    Select motors (Default: BC. Valid values are: AB, BC, AC)\n");
  fprintf(out,"\t-p POWER     Set power (Default: (50 (\"weak\")))\n");
  fprintf(out,"\t-r ROTATION  Set relative rotation (Default: 0. 0 means unlimited rotation)\n");
  fprintf(out,"\t-s           Stop motors (same as '-p 0')\n");
  fprintf(out,"\t-b           Use brakes\n");
  fprintf(out,"\t-i           Idle motors\n");
  fprintf(out,"\t-t TURNRATIO Set turnratio (Default: (0 (\"straight on\")))\n");
  exit(r);
}

int main(int argc,char *argv[]) {
  char *name = NULL;
  int power = 50;
  int rot = 0;
  int brake = 0;
  int synchronise = 1;
  int idle = 0;
  int turnratio = 0;
  int rotation1 = 0;
  int rotation2 = 0;
  int c,newmotor,newpower,newrot,newturnratio;
  int motor1 = 1;
  int motor2 = 2;
  char motor1string[2];
  char motor2string[2];

  while ((c = getopt(argc,argv,":hm:p:r:t:sibyn:"))!=-1) {
    switch(c) {
      case 'h':
        usage(argv[0],0);
        break;
      case 'm':
        if (strlen(optarg) != 2) {
          fprintf(stderr,"Invalid motors: %s\n",optarg);          
          usage(argv[0],1);
        }
        motor1string[0] = optarg[0];
        motor1string[1] = 0;
        newmotor = nxt_str2motorport(motor1string);
        if (newmotor==-1) {
          fprintf(stderr,"Invalid motor 1: %s\n",optarg);
          usage(argv[0],1);
        }
        else motor1 = newmotor;
        motor2string[0] = optarg[1];
        motor2string[1] = 0;
        newmotor = nxt_str2motorport(motor2string);
        if (newmotor==-1) {
          fprintf(stderr,"Invalid motor 2: %s\n",optarg);
          usage(argv[0],1);
        }
        else motor2 = newmotor;
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
        rot = atoi(optarg);
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
      case 't':
        newturnratio = atoi(optarg);
        if (newturnratio<-100 || newturnratio>100) {
          fprintf(stderr,"Invalid turnratio: %d\n",newturnratio);
          usage(argv[0],1);
        }
        else turnratio = newturnratio;
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

  rotation1=rot;
  rotation2=rot;

  nxt_resettacho(nxt,motor1,1);
  nxt_resettacho(nxt,motor2,1);

  if (idle)
    nxt_motor(nxt,motor1,rotation1,brake?0:power,NXT_MOTORON|(brake?NXT_BRAKE|NXT_REGULATED:0),NXT_REGMODE_IDLE,turnratio);
  else
    nxt_motor(nxt,motor1,rotation1,brake?0:power,NXT_MOTORON|(brake||synchronise?NXT_BRAKE|NXT_REGULATED:0),synchronise?NXT_REGMODE_MOTOR_SYNC:NXT_REGMODE_MOTOR_SPEED,turnratio);
  if (idle)
    nxt_motor(nxt,motor2,rotation2,brake?0:power,NXT_MOTORON|(brake?NXT_BRAKE|NXT_REGULATED:0),NXT_REGMODE_IDLE,turnratio);
  else
    nxt_motor(nxt,motor2,rotation2,brake?0:power,NXT_MOTORON|(brake||synchronise?NXT_BRAKE|NXT_REGULATED:0),synchronise?NXT_REGMODE_MOTOR_SYNC:NXT_REGMODE_MOTOR_SPEED,turnratio);

  int ret = nxt_error(nxt);
  if (name!=NULL) free(name);
  nxt_close(nxt);

  return ret;
}
