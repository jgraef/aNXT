/*
    tools/sensor.c
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
#include <string.h>
#include <stdlib.h>
#include <SDL.h>

#include <anxt/nxt.h>
#include <anxt/tools.h>

struct screen {
  unsigned int width;
  unsigned int height;
  unsigned int depth;
};

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
  for (i=0;i<NXT_NUM_TYPES;i++) fprintf(out,"\t\t%s\n",nxt_get_type(i));
  fprintf(out,"\n");
  fprintf(out,"\t-m MODE    Select sensor mode (Default: raw)\n");
  fprintf(out,"\tValid sensor modes are:\n");
  for (i=0;i<NXT_NUM_MODES;i++) fprintf(out,"\t\t%s\n",nxt_get_mode(i));
  fprintf(out,"\n");
  fprintf(out,"\t-r         Reset sensor after reading\n");
  fprintf(out,"\t-v         Verbose mode\n");
  exit(r);
}

int str2resolution(struct screen *screen,const char *str) {
  int ret = sscanf(str,"%ux%ux%u",&(screen->width),&(screen->height),&(screen->depth));
  if (ret==2) screen->depth = 16;
  else if (ret<2) return -1;
  else if (ret>2) return 0;
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
  int type = NXT_SENSOR_TYPE_NONE;
  int mode = NXT_SENSOR_MODE_RAW;
  int reset = 0;
  int c,newsensor,newtype,newmode;
  int sensor = -1;
  int fullscreen = 0;
  int done = 0;
  unsigned int x = 0;
  struct screen screen = {
    .width = 640,
    .height = 480,
    .depth = 16
  };
  SDL_Surface *display;

  while ((c = getopt(argc,argv,":hs:m:t:n:rx:fv"))!=-1) {
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
      case 'x':
        if (str2resolution(&screen,optarg)<0) {
          fprintf(stderr,"Invalid resolution: %s\n",optarg);
          usage(argv[0],1);
        }
        break;
      case 'f':
        fullscreen = 1;
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

  nxt_t *nxt = nxt_open(name);
  if (nxt==NULL) {
    fprintf(stderr,"Could not find NXT\n");
    return 1;
  }

  if (SDL_Init(SDL_INIT_VIDEO)<0) {
    fprintf(stderr,"Could not initialize SDL: %s\n",SDL_GetError());
    return 1;
  }
  atexit(SDL_Quit);
  display = SDL_SetVideoMode(screen.width,screen.height,screen.depth,SDL_SWSURFACE|(fullscreen?SDL_FULLSCREEN:0));
  if (display==NULL) {
    fprintf(stderr,"Could not set video mode: %s\n",SDL_GetError());
    nxt_close(nxt);
    return 1;
  }
  ClearDisplay(display);
  SDL_WM_SetCaption("NXT Sensor Graph","NXT Sensor Grap");
  SDL_Flip(display);

  nxt_set_sensor_mode(nxt,sensor,type,NXT_SENSOR_MODE_RAW);

  int lasty = 0;
  while (!done) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch(event.type) {
        case SDL_QUIT:
          done = 1;
          break;
      }
    }

    int val = nxt_get_sensor(nxt,sensor);
    unsigned int y = screen.height-(val*screen.height/1023);
    if (x==screen.width) {
      x = 0;
      ClearDisplay(display);
    }
    if (val>=0) {
      unsigned int i;
      if (lasty>y) {
        for (i=y;i<lasty;i++) DrawPixel(display,x,i,0,0,0);
      }
      else if (lasty<y) {
        for (i=lasty;i<y;i++) DrawPixel(display,x,i,0,0,0);
      }
      else DrawPixel(display,x,y,0,0,0);
    }
    else {
      fprintf(stderr,"Error: %s\n",nxt_strerror(nxt_error(nxt)));
      done = 1;
    }
    x++;
    lasty = y;
    SDL_Flip(display);
    SDL_Delay(10);
  }

  if (reset) nxt_set_sensor_mode(nxt,sensor,NXT_SENSOR_TYPE_NONE,NXT_SENSOR_MODE_RAW);

  int ret = nxt_error(nxt);
  if (name!=NULL) free(name);
  nxt_close(nxt);

  return ret;
}
