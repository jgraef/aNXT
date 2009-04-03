/*
    graphic_test.c
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
#include <nxtdisplay.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <SDL.h>

static void usage(char *cmd,int r) {
  FILE *out = r==0?stdout:stderr;
  fprintf(out,"Usage: %s [OPTIONS]\n",cmd);
  fprintf(out,"Tests graphic on NXT\n");
  fprintf(out,"Options:\n");
  fprintf(out,"\t-h           Show help\n");
  fprintf(out,"\t-n NXTNAME   Name of NXT (Default: first found) or bluetooth address\n");
  exit(r);
}

static void DrawPixel(SDL_Surface *screen,int x,int y,uint8_t r,uint8_t g,uint8_t b) {
    uint32_t color = SDL_MapRGB(screen->format,r,g,b);

    if (SDL_MUSTLOCK(screen)) {
      if (SDL_LockSurface(screen)<0) return;
    }

    switch (screen->format->BytesPerPixel) {
      case 1: { /* vermutlich 8 Bit */
        uint8_t *bufp;
        bufp = (uint8_t *)screen->pixels+y*screen->pitch+x;
        *bufp = color;
        break;
      }
      case 2: { /* vermutlich 15 Bit oder 16 Bit */
        uint16_t *bufp;
        bufp = (uint16_t*)screen->pixels+y*screen->pitch/2+x;
        *bufp = color;
        break;
      }
      case 3: { /* langsamer 24-Bit-Modus, selten verwendet */
        uint8_t *bufp;
        bufp = (uint8_t*)screen->pixels+y*screen->pitch+x*3;
        if (SDL_BYTEORDER==SDL_LIL_ENDIAN) {
          bufp[0] = color;
          bufp[1] = color>>8;
          bufp[2] = color>>16;
        }
        else {
          bufp[2] = color;
          bufp[1] = color>>8;
          bufp[0] = color>>16;
        }
        break;
      }
      case 4: { /* vermutlich 32 Bit */
        uint32_t *bufp;
        bufp = (uint32_t*)screen->pixels+y*screen->pitch/4+x;
        *bufp = color;
        break;
      }
    }

    if (SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
}

static void ClearDisplay(SDL_Surface *display) {
  SDL_Rect dst;
  dst.x = 0;
  dst.y = 0;
  dst.w = display->w;
  dst.h = display->h;
  SDL_FillRect(display,&dst,SDL_MapRGB(display->format,0xFF,0xFF,0xFF));
}

int main(int argc,char *argv[]) {
  char *name = NULL;
  int c;
  int done = 0;
  int mouse = 0;
  int i = 0;
  SDL_Event event;
  SDL_Surface *screen;

  while ((c = getopt(argc,argv,":hn:"))!=-1) {
    switch (c) {
      case 'h':
        usage(argv[0],0);
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

  // open NXT
  nxt_t *nxt = nxt_open(name);
  if (nxt==NULL) {
    fprintf(stderr,"Could not find NXT\n");
    return 1;
  }
  nxt_display_t *display = nxt_display_open(nxt);

  // open window
  if (SDL_Init(SDL_INIT_VIDEO)==-1) {
    fprintf(stderr,"Can't init SDL:  %s\n",SDL_GetError());
    return 1;
  }
  atexit(SDL_Quit);
  screen = SDL_SetVideoMode(200,128,32,SDL_HWSURFACE);
  if (screen==NULL) {
    fprintf(stderr,"Can't open window: %s\n",SDL_GetError());
    return 1;
  }
  SDL_WM_SetCaption("NXT Graphic Test","NXT Graphic Test");
  ClearDisplay(screen);

  // run
  while (!done) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch(event.type) {
        case SDL_QUIT:
          done = 1;
          break;
        case SDL_MOUSEBUTTONDOWN:
          mouse = 1;
          break;
        case SDL_MOUSEBUTTONUP:
          mouse = 0;
          break;
      }
    }

    if (mouse) {
      int x,y;
      SDL_GetMouseState(&x,&y);
      DrawPixel(screen,x,y,0,0,0);
      DrawPixel(screen,x+1,y,0,0,0);
      DrawPixel(screen,x,y+1,0,0,0);
      DrawPixel(screen,x+1,y+1,0,0,0);
      nxt_display_point(display,NXT_DISPLAY_BLACK,x/2,y/2);
    }

    if (i%100==0) nxt_display_flush(display,1);
    SDL_Flip(screen);
    SDL_Delay(1);

    i++;
  }

  nxt_display_flush(display,0);

  // close NXT
  int ret = nxt_error(nxt);
  nxt_display_close(display);
  nxt_close(nxt);

  return ret;
}
