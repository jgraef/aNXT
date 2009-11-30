/*
    psp.c - example program to blink with the light sensor
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
#include <stdlib.h>
#include <string.h>
#include <SDL.h>
#include <SDL_gfxPrimitives.h>

#include <nxt.h>
#include <nxt_i2c/psp.h>

#define LIGHT_COLOR 0x0000FFFF
#define LINE_COLOR  0x00FF00FF

void light_circle(SDL_Surface *screen,int x,int y) {
  filledCircleColor(screen,x,y,9,LIGHT_COLOR);
}
void light_bigcircle(SDL_Surface *screen,int x,int y) {
  filledCircleColor(screen,x,y,19,LIGHT_COLOR);
}
void light_box(SDL_Surface *screen,int x,int y) {
  boxColor(screen,x,y,x+33,y+6,LIGHT_COLOR);
}

int main(int argc,char *argv[]) {
  struct nxt_psp_buttons buttons;
  struct nxt_psp_joystick joysticks[2];
  int sensor = NXT_SENSOR1; // Sensor port at which the light sensor is connected
  SDL_Surface *screen;
  SDL_Surface *bgimg;
  SDL_Event event;

  nxt_t *nxt = nxt_open(NULL); // Opens a NXT handle. 'NULL' means that we do
                               // not care about which NXT to open (good if
                               // you have only one NXT)
  if (nxt==NULL) { // Don't forget to check if you find a NXT
    fprintf(stderr,"Could not find NXT\n");
    return 1;
  }
  nxt_setsensormode(nxt,sensor,NXT_SENSOR_TYPE_LOWSPEED,NXT_SENSOR_MODE_RAW);

  // setup SDL
  if (SDL_Init(SDL_INIT_VIDEO)==-1) {
    fprintf(stderr,"Can't init SDL: %s\n",SDL_GetError());
    return 1;
  }
  atexit(SDL_Quit);

  // load background image
  bgimg = SDL_LoadBMP("psp_bg.bmp");
  if (bgimg==NULL) {
    fprintf(stderr,"Can't load image: %s\n",SDL_GetError());
    return 1;
  }

  // create window
  screen = SDL_SetVideoMode(bgimg->w,bgimg->h,32,SDL_HWSURFACE);
  if (screen==NULL) {
    fprintf(stderr,"Can't set video mode: %s\n",SDL_GetError());
    return 1;
  }

  while (1) {
    // do SDL Events
    while (SDL_PollEvent(&event)) {
      switch(event.type) {
        case SDL_QUIT:
          return 1;
      }
    }

    // get joystick values
    if (nxt_psp_get_joystick(nxt,sensor,NXT_PSP_JOY_LEFT|NXT_PSP_JOY_RIGHT,joysticks)==-1) {
      memset(joysticks,0,sizeof(joysticks));
    }
    nxt_wait_after_direct_command();

    // get button states
    if (nxt_psp_get_buttons(nxt,sensor,&buttons)==-1) {
      memset(&buttons,0,sizeof(buttons));
    }
    nxt_wait_after_direct_command();

    // draw on screen
    SDL_BlitSurface(bgimg,NULL,screen,NULL);

    aalineColor(screen,105,140,105+joysticks[0].x/5,140+joysticks[0].y/5,LINE_COLOR);
    aalineColor(screen,190,140,190+joysticks[1].x/5,140+joysticks[1].y/5,LINE_COLOR);

    if (buttons.left) {
      light_circle(screen,48,98);
    }
    if (buttons.down) {
      light_circle(screen,62,113);
    }
    if (buttons.up) {
      light_circle(screen,64,84);
    }
    if (buttons.right) {
      light_circle(screen,80,98);
    }
    if (buttons.l3) {
      light_bigcircle(screen,105,140);
    }
    if (buttons.r3) {
      light_bigcircle(screen,190,140);
    }
    if (buttons.square) {
      light_circle(screen,209,99);
    }
    if (buttons.x) {
      light_circle(screen,233,120);
    }
    if (buttons.o) {
      light_circle(screen,257,98);
    }
    if (buttons.triangle) {
      light_circle(screen,233,78);
    }
    if (buttons.l1) {
      light_box(screen,51,35);
    }
    if (buttons.l2) {
      light_box(screen,51,26);
    }
    if (buttons.r1) {
      light_box(screen,215,35);
    }
    if (buttons.r2) {
      light_box(screen,215,26);
    }
    SDL_UpdateRect(screen,0,0,0,0);
  }

  nxt_close(nxt); // Don't forget to close your NXT handle

  return 0;
}
