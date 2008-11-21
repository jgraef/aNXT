/*
    screenshot.c
    libnxt - A C library for using LEGO Mindstorms NXT
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
#include <string.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <gd.h>

void usage(char *cmd,int r) {
  FILE *out = r==0?stdout:stderr;
  fprintf(out,"Usage: %s [OPTION]...\n",cmd);
  fprintf(out,"Takes a picture of the display of NXT\n");
  fprintf(out,"Options:\n");
  fprintf(out,"\t-h         Show help\n");
  fprintf(out,"\t-n NXTNAME Name of NXT (Default: first found) or bluetooth address\n");
  fprintf(out,"\t-f FORMAT  Select file format (Default: png)\n");
  fprintf(out,"\t\tValid file formats are: png, jpeg\n");
  fprintf(out,"\t-o OUTPUT  Output file (Default: screen.png or screen.jpg)\n");
  fprintf(out,"\t-t         Select transparency. Does not work for JPEG\n");
  exit(r);
}

int main(int argc,char *argv[]) {
  char screen[64][100];
  char *name = NULL;
  char *file = NULL;
  int format = NXT_PNG;
  int transparency = 0;
  int c,x,y,newfmt;

  while ((c = getopt(argc,argv,":hn:f:o:t"))!=-1) {
    switch(c) {
      case 'h':
        usage(argv[0],0);
        break;
      case 'n':
        name = strdup(optarg);
        break;
      case 'f':
        newfmt = nxt_str2fmt(optarg);
        if (newfmt==0) {
          fprintf(stderr,"Invalid file format\n");
          usage(argv[0],1);
        }
        else format = newfmt;
        break;
      case 'o':
        file = strdup(optarg);
        break;
      case 't':
        transparency = 1;
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

  nxt_init();
  nxt_t *nxt = nxt_open(name);
  if (nxt==NULL) {
    fprintf(stderr,"Could not find NXT\n");
    return 1;
  }

  if (nxt_screenshot(nxt,screen)==0) {
    gdImagePtr im;
    int black,white;
    im = gdImageCreate(100,64);
    black = gdImageColorAllocate(im,0,0,0);
    white = gdImageColorAllocate(im,255,255,255);
    if (transparency) gdImageColorTransparent(im,white);
    for (y=0;y<64;y++) {
      for (x=0;x<100;x++) gdImageSetPixel(im,x,y,screen[y][x]?black:white);
    }
    FILE *out = fopen(file!=NULL?file:(format==NXT_JPEG?"display.jpg":"display.png"),"w");
    if (format==NXT_JPEG) gdImageJpeg(im,out,-1);
    else if (format==NXT_PNG) gdImagePng(im,out);
    fclose(out);
    gdImageDestroy(im);
  }

  int ret = nxt_error(nxt);
  if (name!=NULL) free(name);
  nxt_close(nxt);

  return ret;
}
