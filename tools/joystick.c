/*
    joystick.c
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
#include <string.h>
#include <SDL.h>

#define MAPPING_MAXNUM 64

struct mapping {
  int present;
  int motor;
  int axis;
  float factor;
};

static struct mapping mappings[MAPPING_MAXNUM];
SDL_Joystick *joy;
nxt_t *nxt;

static void usage(char *cmd,int r) {
  FILE *out = r==0?stdout:stderr;
  fprintf(out,"Usage: %s [OPTIONS]\n",cmd);
  fprintf(out,"Control your NXT with your joystick\n");
  fprintf(out,"Options:\n");
  fprintf(out,"\t-h           Show help\n");
  fprintf(out,"\t-n NXTNAME   Name of NXT (Default: first found) or bluetooth address\n");
  fprintf(out,"\t-j JOYSTICK  ID of your Joystick\n");
  fprintf(out,"\t-m MAPPING   Map a axis to a motor\n");
  fprintf(out,"MAPPING format:\n");
  fprintf(out,"\tMOTOR=AXIS*SCALING\n");
  fprintf(out,"\tExample: A=0*0.00305\n");
  fprintf(out,"\tThis will map axis 0 (0..32767) to motor A (0..100)\n");
  exit(r);
}

static int mapping_add(const char *str) {
  size_t i;
  for (i=0;i<MAPPING_MAXNUM;i++) {
    if (!mappings[i].present) {
      char motor;
      mappings[i].present = 1;
      if (sscanf(str,"%c=%u*%f",&motor,&mappings[i].axis,&mappings[i].factor)!=3) return -1;
      mappings[i].motor = tolower(motor)-'a';
      if (mappings[i].motor<0 || mappings[i].motor>2) return -1;
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

  memset(mappings,0,sizeof(mappings));

  while ((c = getopt(argc,argv,":hn:j:m:"))!=-1) {
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
  if (joystick==-1) {
    for (i=0;i<num_joysticks;i++) {
      printf("#%u:\t%s\n",i,SDL_JoystickName(i));
    }
    return 0;
  }
  if (joystick>=num_joysticks) {
    fprintf(stderr,"Joystick #%u doesn't exist\n",joystick);
    return 1;
  }
  joy = SDL_JoystickOpen(joystick);
  SDL_JoystickEventState(SDL_IGNORE);

  atexit(quit);

  // run mappings
  while (1) {
    SDL_JoystickUpdate();

    for (i=0;i<MAPPING_MAXNUM;i++) {
      if (mappings[i].present) {
        int axis = SDL_JoystickGetAxis(joy,mappings[i].axis);
        if (axis!=-1) {
          int power = (int)((float)axis*mappings[i].factor);
          nxt_setmotorrotation(nxt,mappings[i].motor,0,power);
        }
      }
    }

    SDL_Delay(10);
  }

  return nxt_error(nxt);

}
