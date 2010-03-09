/*
    libanxt/display.c
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

#include <anxt/nxt.h>
#include <anxt/display.h>
#include <anxt/mod.h>

// TODO move to libanxt_tools
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
    nxt_display_clear(display,NXT_DISPLAY_WHITE);
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
      for (x=0;x<100;x++) {
        display->buffer[y][x] = screen[y/8][x]&(1<<(y%8))?NXT_DISPLAY_BLACK:NXT_DISPLAY_WHITE;
      }
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
 *  @see Bresenham's Line Algorithm
 */
void nxt_display_line(nxt_display_t *display,nxt_display_color_t color,int x0, int y0, int x1, int y1) {
  int Dx = x1 - x0;
  int Dy = y1 - y0;
  int steep = (abs(Dy) >= abs(Dx));
  if (steep) {
     int tmp = x0;
     x0 = y0;
     y0 = tmp;
     tmp = x1;
     x1 = y1;
     y1 = tmp;
     // recompute Dx, Dy after swap
     Dx = x1 - x0;
     Dy = y1 - y0;
  }
  int xstep = 1;
  if (Dx < 0) {
     xstep = -1;
     Dx = -Dx;
  }
  int ystep = 1;
  if (Dy < 0) {
     ystep = -1;
     Dy = -Dy;
  }
  int TwoDy = 2*Dy;
  int TwoDyTwoDx = TwoDy - 2*Dx; // 2*Dy - 2*Dx
  int E = TwoDy - Dx; //2*Dy - Dx
  int y = y0;
  int xDraw, yDraw;
  int x;
  for (x = x0; x != x1; x += xstep) {
    if (steep) {
      xDraw = y;
      yDraw = x;
    }
    else {
      xDraw = x;
      yDraw = y;
    }
    // plot
    nxt_display_point(display,color,xDraw, yDraw);
    // next
    if (E > 0) {
      E += TwoDyTwoDx; //E += 2*Dy - 2*Dx;
      y = y + ystep;
    }
    else {
      E += TwoDy; //E += 2*Dy;
    }
  }
}

/**
 * Draws a circle onto display
 *  @param display NXT Display
 *  @param color Color
 *  @param x0 Midpoint X
 *  @param y0 Midpoint Y
 *  @param radius Circle radius
 *  @see Midpoint circle algorithm
 */
void nxt_display_circle(nxt_display_t *display,nxt_display_color_t color,int x0,int y0,int radius) {
  int f = 1-radius;
  int ddF_x = 1;
  int ddF_y = -2*radius;
  int x = 0;
  int y = radius;

  nxt_display_point(display,color,x0,y0+radius);
  nxt_display_point(display,color,x0,y0-radius);
  nxt_display_point(display,color,x0+radius,y0);
  nxt_display_point(display,color,x0-radius,y0);

  while(x<y) {
    //assert(ddF_x==2*x+1);
    //assert(ddF_y==-2*y);
    //assert(f == x*x + y*y - radius*radius + 2*x - y + 1);

    if (f>=0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    nxt_display_point(display,color,x0+x,y0+y);
    nxt_display_point(display,color,x0-x,y0+y);
    nxt_display_point(display,color,x0+x,y0-y);
    nxt_display_point(display,color,x0-x,y0-y);
    nxt_display_point(display,color,x0+y,y0+x);
    nxt_display_point(display,color,x0-y,y0+x);
    nxt_display_point(display,color,x0+y,y0-x);
    nxt_display_point(display,color,x0-y,y0-x);
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
 *  @param x1 Reference for start position X (upper left corner)
 *  @param y1 Reference for start position Y (upper left corner)
 *  @param text Text to draw
 *  @param beep Whether to beep when a '\a' appears
 *  @return How many characters written
 */
int nxt_display_text_ext(nxt_display_t *display,nxt_display_color_t color,int *x1,int *y1,const char *text,int beep) {
  int i,x,y;

  for (i=0;text[i];i++) {
    switch (text[i]) {
      case '\a':
        if (beep) nxt_beep(display->nxt,440,150);
        break;
      case '\n':
        *x1 = 0;
        *y1 += nxt_display_font.height+nxt_display_font.vspace;
        if (*y1+nxt_display_font.width+nxt_display_font.vspace>=NXT_DISPLAY_HEIGHT) break;
        break;
      case '\r':
        *x1 = 0;
        break;
      case '\b':
        *x1 -= max(0,nxt_display_font.width+nxt_display_font.hspace);
        break;
      case '\f':
        nxt_display_clear(display,color==NXT_DISPLAY_WHITE?NXT_DISPLAY_BLACK:NXT_DISPLAY_WHITE);
        *x1 = 0;
        *y1 = 0;
        break;
      default:
        for (y=0;y<nxt_display_font.height;y++) {
          for (x=0;x<nxt_display_font.width;x++) {
            if (nxt_display_font.bitmaps[text[i]][y][x]) nxt_display_point(display,color,*x1+x,*y1+y);
          }
        }
        *x1 += nxt_display_font.width+nxt_display_font.hspace;
        if (*x1+nxt_display_font.width+nxt_display_font.hspace>=NXT_DISPLAY_WIDTH) {
          *x1 = 0;
          *y1 += nxt_display_font.height+nxt_display_font.vspace;
          if (*y1+nxt_display_font.width+nxt_display_font.vspace>=NXT_DISPLAY_HEIGHT) break;
        }
        break;
    }
  }

  return i;
}
