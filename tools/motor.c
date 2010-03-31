/*
    tools/motor.c
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
#include <ctype.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>

#include <anxt/nxt.h>
#include <anxt/motor.h>
#include <anxt/tools.h>

void usage(char *cmd,int r) {
  FILE *out = (r==0?stdout:stderr);
  fprintf(out, "Usage: %s [OPTIONS]\n",cmd);
  fprintf(out, "Control a NXT motor\n");
  fprintf(out, "Options:\n");
  fprintf(out, "\t-h           Show help\n");
  fprintf(out, "\t-n NXTNAME   Name of NXT (Default: first found) or bluetooth address\n");
  fprintf(out, "\t-m MOTOR     Select motor (Default: A. Valid values are: A, B, C, AB, AC, BC, ABC)\n");
  fprintf(out, "\t-p POWER     Set power (Default: (50 (\"weak\")))\n");
  fprintf(out, "\t-r ROTATION  Set rotation (Default: 0. 0 means unlimited rotation)\n");
  fprintf(out, "\t-s           Stop motor (same as '-p 0'). Use -sb to stop and brake\n");
  fprintf(out, "\t-b           Use brake. Use -sb to stop and brake\n");
  fprintf(out, "\t-y TURNRATIO Synchronise motor (be carefull, read manpage first)\n");
  exit(r);
}

int main(int argc,char *argv[]) {
  char *name = NULL;
  char *motors = "A";
  int motor;
  int stop = 0;
  int power = 50;
  int rot = 0;
  int brake = 0;
  int synchronise = 0;
  int turnratio = 0;
  int c, i, newpower, newrot, newturnratio;

  while ((c = getopt(argc,argv,":hm:p:r:R:siby:n:"))!=-1) {
    switch(c) {
      case 'h':
        usage(argv[0],0);
        break;
      case 'm':
        motors = optarg;
        break;
      case 'p':
        newpower = atoi(optarg);
        if (!NXT_VALID_POWER(newpower)) {
          fprintf(stderr, "Invalid power: %d\n", newpower);
          usage(argv[0], 1);
        }
        else {
          power = newpower;
        }
        break;
      case 'r':
        newrot = atoi(optarg);
        if (newrot<0) {
          fprintf(stderr, "Invalid rotation: %d\n", newrot);
          usage(argv[0], 1);
        }
        else {
          rot = newrot;
        }
        break;
      case 'R':
        newrot = atoi(optarg)*360;
        if (newrot<0) {
          fprintf(stderr, "Invalid rotation: %d\n", newrot);
          usage(argv[0], 1);
        }
        else {
          rot = newrot;
        }
        break;
      case 's':
        stop = 1;
        break;
      case 'b':
        brake = 1;
        break;
      case 'y':
        synchronise = 1;
        newturnratio = atoi(optarg);
        if (!NXT_VALID_POWER(newturnratio)) {
          fprintf(stderr, "Invalid turnratio: %d\n", newturnratio);
          usage(argv[0], 1);
        }
        else {
          turnratio = newturnratio;
        }
        break;
      case 'n':
        name = optarg;
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

  if (stop || power==0) {
    for (i=0; motors[i]!=0; i++) {
      nxt_motor_stop(nxt, nxt_chr2motor(motors[i]), brake);
    }
  }
  else {
    for (i=0; motors[i]!=0; i++) {
      motor = nxt_chr2motor(motors[i]);

      nxt_motor_enable_autoset(nxt, motor, 0);
      nxt_motor_set_power(nxt, motor, power);
      nxt_motor_set_rotation(nxt, motor, rot);
      nxt_motor_use_brake(nxt, motor, brake);
      if (synchronise) {
        nxt_motor_sync(nxt, motor, turnratio);
      }
      nxt_motor_set_runstate(nxt, motor, NXT_MOTOR_RUNSTATE_RUNNING);
      nxt_motor_turn_on(nxt, motor, 1);
      nxt_motor_set_state(nxt, motor);
    }
  }

  int ret = nxt_error(nxt);
  nxt_close(nxt);

  return ret;
}
