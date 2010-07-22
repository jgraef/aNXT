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

#include <string.h>

#include <anxt/nxt.h>

#define NXT_DISPLAY_WIDTH   100
#define NXT_DISPLAY_HEIGHT  64
#define NXT_DISPLAY_BUFSIZE (NXT_DISPLAY_WIDTH*NXT_DISPLAY_HEIGHT/8)

#define nxt_display_triangle(display,color,x1,y1,x2,y2,x3,y3) nxt_display_polygon(display,color,x1,y1,x2,y2,x3,y3)
#define nxt_display_rectangle(display,color,x1,y1,x2,y2)      nxt_display_polygon(display,color,x1,y1,x1,y2,x2,y2,x2,y1)

typedef enum {
  NXT_DISPLAY_WHITE = 0,
  NXT_DISPLAY_BLACK = 1
} nxt_display_color_t;

typedef struct {
  nxt_t *nxt;
  nxt_display_color_t buffer[NXT_DISPLAY_HEIGHT][NXT_DISPLAY_WIDTH];
  int modid;
  int dirty;
} nxt_display_t;

nxt_display_t *nxt_display_open(nxt_t *nxt);
void nxt_display_close(nxt_display_t *display);
int nxt_display_refresh(nxt_display_t *display);
int nxt_display_flush(nxt_display_t *display,int notdirty);
void nxt_display_clear(nxt_display_t *display,nxt_display_color_t color);
void nxt_display_point(nxt_display_t *display,nxt_display_color_t color,int x,int y);
void nxt_display_line(nxt_display_t *display,nxt_display_color_t color,int x1,int y1,int x2,int y2);
void nxt_display_circle(nxt_display_t *display,nxt_display_color_t color,int x0,int y0,int radius);
void nxt_display_polygon(nxt_display_t *display,nxt_display_color_t color,int points,...);
int nxt_display_text_ext(nxt_display_t *display,nxt_display_color_t color,int *x1,int *y1,const char *text,int beep);

static __inline__ int nxt_display_text(nxt_display_t *display,nxt_display_color_t color,int x,int y,const char *text) {
  int x2 = x;
  int y2 = y;
  return nxt_display_text_ext(display,color,&x2,&y2,text,0);
}
