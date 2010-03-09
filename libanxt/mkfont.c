/*
    libanxt/mkfont.c
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

#include <gd.h>

void getbitmap(gdImagePtr im,int c,int x0,int y0) {
  int x,y;

  printf("    { /* 0x%02x '%c' */ ",c,c<0x7f?c:' ');
  for (y=0;y<8;y++) {
    putchar('{');
    for (x=0;x<5;x++) {
      int color = gdImageGetPixel(im,x0+x,y0+y);
      putchar(((gdImageRed(im,color)+gdImageGreen(im,color)+gdImageBlue(im,color))<384)?'1':'0');
      if (x<4) putchar(',');
    }
    if (y<7) printf("},");
  }
  printf("} }%s\n",c<0x7f?",":"");
}

int main() {
  gdImagePtr im = gdImageCreateFromPng(stdin);
  int x,y,c;

  puts("#ifndef _FONT_H_");
  puts("#define _FONT_H_");
  puts("");
  puts("static struct {");
  puts("  int width;");
  puts("  int height;");
  puts("  int hspace;");
  puts("  int vspace;");
  puts("  char bitmaps[128][8][5];");
  puts("} nxt_display_font = {");
  puts("  .width = 5,");
  puts("  .height = 8,");
  puts("  .hspace = 1,");
  puts("  .vspace = 0,");
  puts("  .bitmaps = {");

  // blank characters
  for (c=0;c<0x20;c++) {
    printf("    { /* 0x%02x     */ {0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0} },\n",c);
  }

  // graphical characters
  for (y=0;y<6;y++) {
    for (x=0;x<16;x++) {
      getbitmap(im,c++,x*5,y*8);
    }
  }

  puts("  }");
  puts("};");
  puts("");
  puts("#endif /*_FONT_H_*/");

  gdImageDestroy(im);
  return 0;
}
