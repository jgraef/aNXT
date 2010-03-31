/*
    tools/pilot.c
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
#include <stdint.h>
#include <unistd.h>
#include <SDL.h>
#include <SDL_image.h>

#include <anxt/nxt.h>
#include <anxt/mod.h>
#include <anxt/display.h>

// Images
#include "pilot_bg.xpm"
#include "pilot_enter.xpm"
#include "pilot_exit.xpm"
#include "pilot_left.xpm"
#include "pilot_right.xpm"

#define in_rect(rect,mouse) ((mouse).x>=(rect).x && (mouse).x<(rect).x+(rect).w && (mouse).y>=(rect).y && (mouse).y<(rect).y+(rect).h)

static void usage(char *cmd,int r) {
  FILE *out = r==0?stdout:stderr;
  fprintf(out,"Usage: %s [OPTIONS]\n",cmd);
  fprintf(out,"Control the NXT UI without touching it\n");
  fprintf(out,"Options:\n");
  fprintf(out,"\t-h           Show help\n");
  fprintf(out,"\t-n NXTNAME   Name of NXT (Default: first found) or bluetooth address\n");
  fprintf(out,"\t-f           Force. Don't ask anything\n");
  fprintf(out,"\t-d DURATION  Duration (Default: 200ms)\n");
  exit(r);
}

static SDL_Surface *load_display(nxt_display_t *display) {
  uint32_t image[70][100];
  size_t x,y;

  if (nxt_display_refresh(display)==0) {
    for (y=0;y<64;y++) {
      for (x=0;x<100;x++) image[y][x] = display->buffer[y][x]==NXT_DISPLAY_BLACK?0x000000FF:0xFFFFFFFF;
    }

    return SDL_CreateRGBSurfaceFrom(image,100,64,32,400,0xFF000000,0x00FF0000,0x0000FF00,0x000000FF);
  }
  else return NULL;
}

int main(int argc,char *argv[]) {
  SDL_Event event;
  SDL_Surface *screen;
  SDL_Surface *bg,*left,*enter,*right,*exit,*display_surface;
  SDL_Rect rect_left = {
    .x = 51,
    .y = 172,
    .w = 27,
    .h = 31,
  };
  SDL_Rect rect_enter = {
    .x = 91,
    .y = 173,
    .w = 32,
    .h = 31,
  };
  SDL_Rect rect_right = {
    .x = 136,
    .y = 172,
    .w = 27,
    .h = 31,
  };
  SDL_Rect rect_exit = {
    .x = 91,
    .y = 216,
    .w = 31,
    .h = 20,
  };
  SDL_Rect rect_display = {
    .x = 57,
    .y = 62,
    .w = 100,
    .h = 64,
  };
  int c;
  char *name = NULL;
  int force = 0;

  while ((c = getopt(argc,argv,":hn:f"))!=-1) {
    switch (c) {
      case 'h':
        usage(argv[0],0);
        break;
      case 'f':
        force = 1;
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

  // init NXT
  nxt_t *nxt = nxt_open(name);
  if (nxt==NULL) {
    fprintf(stderr,"Could not find NXT\n");
    return 1;
  }
  if (nxt_get_connection_type(nxt)==NXT_CON_BT && !force) {
    fprintf(stderr,"Warning! Using NXT Pilot over Bluetooth can make trouble. Are you sure to continue (y/n)[n]: ");
    if (fgetc(stdin)!='y') {
      nxt_close(nxt);
      return 0;
    }
  }
  nxt_display_t *display = nxt_display_open(nxt);
  if (display==NULL) {
    fprintf(stderr,"Could not open display\n");
    nxt_close(nxt);
    return 1;
  }

  // init window
  if (SDL_Init(SDL_INIT_VIDEO)==-1) {
    fprintf(stderr,"Can't init SDL:  %s\n",SDL_GetError());
    return 1;
  }
  atexit(SDL_Quit);
  screen = SDL_SetVideoMode(215,322,32,SDL_HWSURFACE);
  if (screen==NULL) {
    fprintf(stderr,"Can't open window: %s\n",SDL_GetError());
    return 1;
  }
  SDL_WM_SetCaption("NXT Pilot","NXT Pilot");

  // load images
  bg = IMG_ReadXPMFromArray(pilot_bg_xpm);
  left = IMG_ReadXPMFromArray(pilot_left_xpm);
  enter = IMG_ReadXPMFromArray(pilot_enter_xpm);
  right = IMG_ReadXPMFromArray(pilot_right_xpm);
  exit = IMG_ReadXPMFromArray(pilot_exit_xpm);

  // display NXT
  SDL_BlitSurface(bg,NULL,screen,NULL);
  SDL_BlitSurface(left,NULL,screen,&rect_left);
  SDL_BlitSurface(enter,NULL,screen,&rect_enter);
  SDL_BlitSurface(right,NULL,screen,&rect_right);
  SDL_BlitSurface(exit,NULL,screen,&rect_exit);

  int done = 0;
  unsigned int pause = nxt_get_connection_type(nxt)==NXT_CON_BT?500:100;
  while (!done) {
    while (SDL_PollEvent(&event)) {
      switch(event.type) {
        case SDL_QUIT:
          done = 1;
          break;
        case SDL_KEYUP:
          if (event.key.keysym.sym==SDLK_ESCAPE) done = 1;
          else if (event.key.keysym.sym==SDLK_RETURN) nxt_set_button(nxt,NXT_UI_BUTTON_ENTER);
          else if (event.key.keysym.sym==SDLK_BACKSPACE) nxt_set_button(nxt,NXT_UI_BUTTON_EXIT);
          else if (event.key.keysym.sym==SDLK_LEFT) nxt_set_button(nxt,NXT_UI_BUTTON_LEFT);
          else if (event.key.keysym.sym==SDLK_RIGHT) nxt_set_button(nxt,NXT_UI_BUTTON_RIGHT);
          break;
        case SDL_MOUSEBUTTONUP:
          if (in_rect(rect_left,event.button)) nxt_set_button(nxt,NXT_UI_BUTTON_LEFT);
          else if (in_rect(rect_enter,event.button)) nxt_set_button(nxt,NXT_UI_BUTTON_ENTER);
          else if (in_rect(rect_right,event.button)) nxt_set_button(nxt,NXT_UI_BUTTON_RIGHT);
          else if (in_rect(rect_exit,event.button)) nxt_set_button(nxt,NXT_UI_BUTTON_EXIT);
          break;
      }
    }

    display_surface = load_display(display);
    if (display!=NULL) {
      SDL_BlitSurface(display_surface,NULL,screen,&rect_display);
      SDL_UpdateRect(screen,0,0,0,0);
      SDL_FreeSurface(display_surface);
    }

    SDL_Delay(pause);
  }

  // free images
  SDL_FreeSurface(bg);
  SDL_FreeSurface(left);
  SDL_FreeSurface(enter);
  SDL_FreeSurface(right);
  SDL_FreeSurface(exit);

  // close nxt
  int ret = nxt_error(nxt);
  nxt_display_flush(display,1);
  nxt_display_close(display);
  nxt_close(nxt);

  return ret;
}

