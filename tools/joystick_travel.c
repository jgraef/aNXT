/*
    joystick_travel.c
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
#include <string.h>
#include <SDL.h>

#define NXT_DIRECTION_FORWARD 0
#define NXT_DIRECTION_TURN 1
#define NXT_MAPPING_MAXNUM 64

struct mapping {
  int present;
  int direction;
  int motor;
  int axis;
  float factor;
};

static int num_mappings = 0;
static struct mapping mappings[NXT_MAPPING_MAXNUM];
SDL_Joystick *joy;
nxt_t *nxt;

static void usage(char *cmd,int r) {
  FILE *out = r==0?stdout:stderr;
  fprintf(out,"Usage: %s [OPTIONS]\n",cmd);
  fprintf(out,"Control your NXT with your joystick similar to nxt_motor_travel\n");
  fprintf(out,"Options:\n");
  fprintf(out,"\t-h             Show help\n");
  fprintf(out,"\t-n NXTNAME     Name of NXT (Default: first found) or bluetooth address\n");
  fprintf(out,"\t-j JOYSTICK    ID of your Joystick\n");
  fprintf(out,"\t-m MOTORS      Select motors (Default: BC. Valid values are: AB, BC, AC)\n");
  fprintf(out,"\t-v             verbose mode: show power and turnratio\n");
  fprintf(out,"\t-d DIR_MAPPING Map a axis to a direction or motor\n");
  fprintf(out,"DIR_MAPPING format:\n");
  fprintf(out,"\tDIRECTION=AXIS*SCALING\n");
  fprintf(out,"\tvalid as DIRECTION: f (forward) or t (turn) or a or b or c (motor)\n");
  fprintf(out,"\tExample: t=0*327.67\n");
  fprintf(out,"\tThis will map axis 0 (0..32767) to turnratio (0..100)\n");
  fprintf(out,"\tExample: a=0*327.67\n");
  fprintf(out,"\tThis will map axis 0 (0..32767) to power of motor a (0..100)\n");
  exit(r);
}

static int mapping_add(const char *str) {
  size_t i;
  for (i=0;i<NXT_MAPPING_MAXNUM;i++) {
    if (!mappings[i].present) {
      char direction;
      mappings[i].present = 1;
      mappings[i].direction = -1;
      mappings[i].motor = -1;
      if (sscanf(str,"%c=%u*%f",&direction,&mappings[i].axis,&mappings[i].factor)!=3) return -1;
      if ((direction == 'f') || (direction == 'F'))
        mappings[i].direction = NXT_DIRECTION_FORWARD;
      else if ((direction == 't') || (direction == 'T'))
        mappings[i].direction = NXT_DIRECTION_TURN;
      else {
        mappings[i].direction = -1;
        char motor;
        if (sscanf(str,"%c=%u*%f",&motor,&mappings[i].axis,&mappings[i].factor)!=3) return -1;
        mappings[i].motor = tolower(motor)-'a';
        if (mappings[i].motor<0 || mappings[i].motor>2) return -1;
      } 
      num_mappings++;
      return 0;
    }
  }
  return -1;
}

static void quit() {
  SDL_JoystickClose(joy);
  nxt_setmotorrotation(nxt,NXT_MOTORA,0,0);
  nxt_setmotorrotation(nxt,NXT_MOTORB,0,0);
  nxt_setmotorrotation(nxt,NXT_MOTORC,0,0);
  nxt_close(nxt);
  SDL_Quit();
  nxt_close(nxt);
}

int main(int argc,char *argv[]) {
  char *name = NULL;
  int c;
  int joystick = -1;
  int num_joysticks;
  unsigned int i;
  int old_power = 0;
  int old_turnratio = 0;
  int power = 0;
  int turnratio = 0;
  int verbose = 0;

  memset(mappings,0,sizeof(mappings));

  while ((c = getopt(argc,argv,":hvn:j:m:"))!=-1) {
    switch (c) {
      case 'h':
        usage(argv[0],0);
        break;
      case 'j':
        joystick = atoi(optarg);
        break;
      case 'm':
        if (mapping_add(optarg)==-1) {
          fprintf(stderr,"Failed to add mapping\n");
          usage(argv[0],1);
        }
        break;
      case 'v':
        verbose = 1;
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

  // initialize NXT
  nxt = nxt_open(name);
  if (nxt==NULL) {
    fprintf(stderr,"Could not find NXT\n");
    return 1;
  }

  // initialize Joystick
  if (SDL_Init(SDL_INIT_JOYSTICK)==-1) {
    printf("Can't init SDL:  %s\n", SDL_GetError());
    return 1;
  }
  num_joysticks = SDL_NumJoysticks();
  if (num_joysticks<joystick) {
    fprintf(stderr,"Joystick #%u doesn't exist\n",joystick);
    return 1;
  }
  if (joystick==-1) {
    for (i=0;i<num_joysticks;i++) {
      printf("#%u:\t%s\n",i,SDL_JoystickName(i));
    }
    return 0;
  }
  joy = SDL_JoystickOpen(joystick);
  SDL_JoystickEventState(SDL_IGNORE);

  atexit(quit);

  // run mappings
  while (1) {
    int brake = 1;
    int synchronise = 1;
    int motor1 = 1;
    int motor2 = 2;
    int mode;
    int regmode;

    SDL_JoystickUpdate();

    for (i=0;i<num_mappings;i++) {
      if (mappings[i].present) {
        int axis = SDL_JoystickGetAxis(joy,mappings[i].axis);
        if (mappings[i].direction!=-1) {
          if (mappings[i].direction==NXT_DIRECTION_FORWARD)
            power = (int)((float)axis/mappings[i].factor);
          else if (mappings[i].direction==NXT_DIRECTION_TURN)
            turnratio = (int)((float)axis/mappings[i].factor);
          else {
            int axis = SDL_JoystickGetAxis(joy,mappings[i].axis);
            if (mappings[i].motor!=-1) {
              int motor_power = (int)((float)axis/mappings[i].factor);
              nxt_setmotorrotation(nxt,mappings[i].motor,0,motor_power);
            }
          }
        }
      }
    }

    if ((turnratio!=0) && (power==0))
        power=50+abs(turnratio)/2;
    if (verbose)
        printf("power=%d turnratio=%d\n",power,turnratio);
    if (power==0) {
      mode = brake?0:power,NXT_MOTORON|(brake?NXT_BRAKE|NXT_REGULATED:0);
      regmode = NXT_REGMODE_IDLE;
    } else {
      mode = NXT_MOTORON|(brake||(synchronise && (turnratio == 0))?NXT_BRAKE:0)|NXT_REGULATED;
      regmode = synchronise && (power != 0)?NXT_REGMODE_MOTOR_SYNC:NXT_REGMODE_MOTOR_SPEED;
    }
    if ((power!=old_power) || (turnratio!=old_turnratio)) {
      nxt_motor(nxt,motor1,0,0,0,NXT_REGMODE_IDLE,turnratio);
      nxt_motor(nxt,motor2,0,0,0,NXT_REGMODE_IDLE,turnratio);

      nxt_resettacho(nxt,motor1,1);
      nxt_resettacho(nxt,motor2,1);

      nxt_motor(nxt,motor1,0,power,mode,regmode,turnratio);
      nxt_motor(nxt,motor2,0,power,mode,regmode,turnratio);

      old_power = power;
      old_turnratio = turnratio;
    }

    SDL_Delay(10);
  }

  return nxt_error(nxt);

}
