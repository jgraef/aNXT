/*
    nxtdisplay.c
    aNXT - a NXt Toolkit
    Libraries and tools for LEGO Mindstorms NXT robots
    Copyright (C) 2008  Janosch Gräf <janosch.graef@gmx.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "nxt.h"
#include "nxtdisplay.h"

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

/**
 * Opens display
 *  @param nxt NXT with display to open
 *  @return Display
 */
nxt_display_t *nxt_display_open(nxt_t *nxt) {
  nxt_display_t *display;
  int modid;

  if ((modid = nxt_mod_getid(nxt,NXT_DISPLAY_MODFILE))!=-1) {
    display = malloc(sizeof(nxt_display_t));
    display->nxt = nxt;
    display->modid = modid;
    nxt_display_clear(display,NXT_DISPLAY_COLOR_WHITE);
  }

  return display;
}

/**
 * Closes display
 *  @param display Display
 */
void nxt_display_close(nxt_display_t *display) {
  free(display);
}

/**
 * Gets display buffer from NXT
 *  @param display Display
 *  @return Success?
 */
int nxt_display_refresh(nxt_display_t *display) {
  char screen[NXT_DISPLAY_HEIGHT/8][NXT_DISPLAY_WIDTH];
  int x,y;

  if ((display->modid = nxt_mod_getid(display->nxt,NXT_DISPLAY_MODFILE))!=-1) {
    nxt_mod_read(display->nxt,display->modid,screen,NXT_DISPLAY_BITMAP,NXT_DISPLAY_BUFSIZE);
    for (y=0;y<64;y++) {
      for (x=0;x<100;x++) display->buffer[y][x] = screen[y/8][x]&(1<<(y%8));
    }
    return 0;
  }
  else return -1;
}

/**
 * Flushs local display buffer to NXT
 *  @param display Display
 *  @param notdirty Whether to flush even if the local buffer is not dirty
 *  @return Success?
 */
int nxt_display_flush(nxt_display_t *display,int notdirty) {
  char screen[NXT_DISPLAY_HEIGHT/8][NXT_DISPLAY_WIDTH];
  int x,y;

  if (!notdirty && !display->dirty) return 0;

  if ((display->modid = nxt_mod_getid(display->nxt,NXT_DISPLAY_MODFILE))!=-1) {
    for (y=0;y<64;y++) {
      for (x=0;x<100;x++) {
        if (display->buffer[y][x]) screen[y/8][x] |= 1<<(y%8);
        else screen[y/8][x] &= ~(1<<(y%8));
      }
    }
    nxt_mod_write(display->nxt,display->modid,screen,NXT_DISPLAY_BITMAP,NXT_DISPLAY_BUFSIZE);
    display->dirty = 0;
    return 0;
  }
  else return -1;
}

/**
 * Draws a line on display
 *  @param display Display
 *  @param color Color
 *  @param x1 Point 1 - X
 *  @param y1 Point 1 - Y
 *  @param x2 Point 2 - X
 *  @param y2 Point 2 - Y
 */
void nxt_display_line(nxt_display_t *display,nxt_display_color_t color,int x1,int y1,int x2,int y2) {
  int x,y,ynext;
  float m;

  if (x1==x2) {
    for (y=y1;y<=y2;y++) nxt_display_point(display,color,x1,y);
  }
  else if (y1==y2) {
    for (x=x1;x<=x2;x++) nxt_display_point(display,color,x,y1);
  }
  else {
    m = ((float)(y2-y1))/((float)(x2-x1));

    for (x=min(x1,x2);x<max(x1,x2);x++) {
      for (y=(int)((x-x1)*m+y1);y<(int)((x-x1+(m>0.?1:-1))*m+y1);y++) {
        nxt_display_point(display,color,x,y);
      }
      if (y==(int)((x-x1)*m+y1)) nxt_display_point(display,color,x,y);
    }
  }
}

/**
 * Draws polygon
 *  @param display Display
 *  @param color Color (Line)
 *  @param points Number of points (min. 2)
 *  @param ... Points (x1,y1,...,xn,yn)
 */
void nxt_display_polygon(nxt_display_t *display,nxt_display_color_t color,int points,...) {
  va_list args;
  int x,y,xlast,ylast,x1,y1;

  if (points<2) return;

  va_start(args,points);

  x1 = xlast = va_arg(args,int);
  y1 = ylast = va_arg(args,int);

  for (;points>1;points--) {
    x = va_arg(args,int);
    y = va_arg(args,int);

    nxt_display_line(display,color,xlast,ylast,x,y);

    xlast = x;
    ylast = y;
  }

  nxt_display_line(display,color,xlast,ylast,x1,y1);

  va_end(args);
}

/**
 * Draw text
 *  @param display Display
 *  @param color Color (text)
 *  @param x1 Start position X (upper left corner)
 *  @param y1 Start position Y (upper left corner)
 *  @param text Text to draw
 */
int nxt_display_text(nxt_display_t *display,nxt_display_color_t color,int x1,int y1,const char *text) {
  int i,x,y;

  for (i=0;text[i];i++) {
    switch (text[i]) {
      case '\n':
        x1 = 0;
        y1 += nxt_display_font.height+nxt_display_font.vspace;
        if (y1+nxt_display_font.width+nxt_display_font.vspace>=NXT_DISPLAY_HEIGHT) break;
        break;
      case '\r':
        x1 = 0;
        break;
      case '\b':
        x1 -= max(0,nxt_display_font.width+nxt_display_font.hspace);
        break;
      case '\f':
        nxt_display_clear(display,color==NXT_DISPLAY_COLOR_WHITE?NXT_DISPLAY_COLOR_BLACK:NXT_DISPLAY_COLOR_WHITE);
        x1 = 0;
        y1 = 0;
        break;
      default:
        for (y=0;y<nxt_display_font.height;y++) {
          for (x=0;x<nxt_display_font.width;x++) {
            if (nxt_display_font.bitmaps[text[i]][y][x]) nxt_display_point(display,color,x1+x,y1+y);
          }
        }
        x1 += nxt_display_font.width+nxt_display_font.hspace;
        if (x1+nxt_display_font.width+nxt_display_font.hspace>=NXT_DISPLAY_WIDTH) {
          x1 = 0;
          y1 += nxt_display_font.height+nxt_display_font.vspace;
          if (y1+nxt_display_font.width+nxt_display_font.vspace>=NXT_DISPLAY_HEIGHT) break;
        }
        break;
    }
  }

  return i;
}
