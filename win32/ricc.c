/*
    ricc.c
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
#ifndef WIN32
#include <stdint.h>
#include <unistd.h>
#endif
#include <string.h>

#include <gd.h>

#include "nxtfile/ric.h"

typedef enum {
  NONE,
  PNG,
  JPEG,
  GIF,
  RIC
} format_t;

#define MAX_FILE_PATH_LEN 1024
char dropped_file[MAX_FILE_PATH_LEN] = { 0 };

static void usage(char *progname,int ret) {
  FILE *stream = ret==0?stdout:stderr;
  fprintf(stream,"Usage: %s [OPTION] FILE\n",progname);
  fprintf(stream,"Converts between several image formats and RIC\n");
#ifdef WIN32
  if (ret==1)
    fprintf(stream,"\nThis is a commandline program: start it from the cmd.exe program\ntogether with the options and the file you want convert\n\n");
#endif
  fprintf(stream,"Options:\n");
  fprintf(stream,"\t-o\tSet name of output file\n\t\t(default: filename with output format as extension)\n");
  fprintf(stream,"\t-f\tSet output format.\n");
  fprintf(stream,"\t\tOnly required for RIC to PNG/JPEG conversion.\n");
  fprintf(stream,"\t-q\tSet quality (Only required when saving to JPEG)\n");
  fprintf(stream,"\t\tDefault: -1, that is the best quality/memory ratio.\n");
  fprintf(stream,"\t-t\tUse transparency instead of white (Works only for PNG or GIF)\n");
  fprintf(stream,"\t-h\tShow help\n");
  fprintf(stream,"\t-v\tVerbose mode\n");
#ifdef WIN32
  if (ret==1) {
    fprintf(stream,"\nEnter filename to convert (you may drop in a file here):\n");
	fgets(dropped_file, MAX_FILE_PATH_LEN - 1, stdin);
  }
  if (ret!=1)
#endif
  exit(ret);
}

static format_t recognize_format(char *file) {
  char buf[10];
  FILE *fd = fopen(file,"rb");
  if (fd!=NULL) {
    fread(buf,10,1,fd);
    fclose(fd);

    if (memcmp(buf+1,"PNG",3)==0) return PNG;
    else if (memcmp(buf+6,"JFIF",4)==0) return JPEG;
    else if (memcmp(buf,"GIF",3)==0) return GIF;
    else if (memcmp(buf,"\10\0\0\0\0\0",6)==0) return RIC;

  }
  return NONE;
}

static format_t str2format(char *str) {
  if (strcmp(str,"png")==0) return PNG;
  else if (strcmp(str,"jpg")==0 || strcmp(str,"jpeg")==0) return JPEG;
  else if (strcmp(str,"gif")==0) return GIF;
  else return NONE;
}

static char *format2ext(format_t format) {
  if (format==PNG) return ".png";
  else if (format==JPEG) return ".jpg";
  else if (format==GIF) return ".gif";
  else if (format==RIC) return ".ric";
  else return "";
}

static gdImagePtr open_gdimage(FILE *img,format_t format) {
  if (format==PNG) return gdImageCreateFromPng(img);
  else if (format==JPEG) return gdImageCreateFromJpeg(img);
  else if (format==GIF) return gdImageCreateFromGif(img);
  else return NULL;
}

static void save_gdimage(gdImagePtr im,FILE *img,format_t format,int quality,int black) {
  if (format==PNG) gdImagePng(im,img);
  else if (format==JPEG) gdImageJpeg(im,img,quality);
  else if (format==GIF) gdImageGif(im,img);
}

static int img2ric(FILE *img,format_t format,FILE *ric,int invert,int verbose) {
  gdImagePtr im = open_gdimage(img,format);
  unsigned int width = gdImageSX(im);
  unsigned int height = gdImageSY(im);
  unsigned int x,y;
  void *ric_data = NULL;
  uint8_t *bitmap;
  size_t size;

  if (verbose) {
    printf("Width:         %d\n",width);
    printf("Height:        %d\n",height);
  }

  // Translate to bitmap with 1 byte per pixel (gray)
  bitmap = malloc(width*height);
  for (y=0;y<height;y++) {
    for (x=0;x<width;x++) {
      int color = gdImageGetPixel(im,x,y);
      bitmap[y*width+x] = (gdImageRed(im,color)+gdImageGreen(im,color)+gdImageBlue(im,color))/3;
      if (invert) bitmap[y*width+x] = 0xFF-bitmap[y*width+x];
    }
  }

  // Convert to RIC, write to file and free buffers
  size = ric_encode(&ric_data,width,height,bitmap);
  fwrite(ric_data,1,size,ric);
//  free(ric_data);
//  free(bitmap);
  gdImageDestroy(im);

  return 0;
}

static int ric2img(FILE *ric,FILE *img,format_t format,int invert,int verbose,int quality,int transparency) {
  // Read RIC data
  size_t size = 0;
  char *ric_data = NULL;
  uint8_t *bitmap;
  unsigned int width,height;
  gdImagePtr im;
  int black;
  int white;
  unsigned int x,y;

  while (!feof(ric)) {
    ric_data = realloc(ric_data,size+1024);
    size += fread(ric_data+size,1,1024,ric);
  }

  // Convert RIC to bitmap
  ric_decode((void**)&bitmap,ric_data,size,&width,&height);

  if (verbose) {
    printf("Width:         %u\n",width);
    printf("Height:        %u\n",height);
  }

  // Translate image data
  im = gdImageCreate(width,height);
  black = gdImageColorAllocate(im,0,0,0);
  white = gdImageColorAllocate(im,0xFF,0xFF,0xFF);
  if (transparency) gdImageColorTransparent(im,white);
  
  for (y=0;y<height;y++) {
    for (x=0;x<width;x++) {
      if (invert) bitmap[y*width+x] = 0xFF-bitmap[y*width+x];
      gdImageSetPixel(im,x,y,bitmap[y*width+x]<0x80?black:white);
    }
  }

  // Write to file
  save_gdimage(im,img,format,quality,black);

  // Free buffers
  free(ric_data);
  free(bitmap);
  gdImageDestroy(im);

  return 0;
}

int main(int argc,char *argv[]) {
  int c,ret;
  int verbose = 0;
  int invert = 0;
  int transparency = 0;
  int quality = -1;
  char *output = NULL;
  char *input;
  FILE *input_stream;
  FILE *output_stream;

  format_t input_format = NONE;
  format_t output_format = NONE;

  while ((c = getopt(argc,argv,":o:f:iq:thv"))!=-1) {
    switch(c) {
      case 'o':
        output = strdup(optarg);
        break;
      case 'f':
        output_format = str2format(optarg);
        break;
      case 'i':
        invert = 1;
        break;
      case 'q':
        quality = atoi(optarg);
        if (quality>100) quality = -1;
        break;
      case 't':
        transparency = 1;
        break;
      case 'h':
        usage(argv[0],0);
        break;
      case 'v':
        verbose = 1;
        break;
      case ':':
        fprintf(stderr,"Option -%c requires an operand\n",optopt);
        usage(argv[0],1);
        break;
      case '?':
        fprintf(stderr,"Error: Unrecognized option: -%c\n", optopt);
        usage(argv[0],1);
        break;
    }
  }

  // get input file
  if (optind==argc) {
    fprintf(stderr,"Error: No file specified\n");
    usage(argv[0],1);
#ifdef WIN32
	if (strlen(dropped_file)==0)
	  exit(1);
	else
	  input = dropped_file;
#endif
  }
  else input = argv[optind];

#ifdef WIN32
  // handle input in case of a dropped Windows file
  // strip linefeed first...
  if (input[strlen(input)-1]==10)  
    input[strlen(input)-1]=0;
  // strip " characters
  if (input[0]=='"')
    input++;
  if (input[0]==0)
    exit(1);
  if (input[strlen(input)-1]=='"')
    input[strlen(input)-1]=0;
#endif

  // get input and output format
  input_format = recognize_format(input);
  if (input_format==NONE) {
    fprintf(stderr,"Error: Unknown file format: %s\n",input);
    usage(argv[0],1);
  }
  if (input_format!=RIC) output_format = RIC;
  if (output_format==NONE) {
    fprintf(stderr,"Error: No output format specified\n");
    usage(argv[0],1);
  }

  // get output file
  if (output==NULL) {
	char *last_dot;
    output = malloc(strlen(input)+5);
    strcpy(output,input);
	last_dot=strrchr(output,'.');
	if (last_dot != NULL)
	  *last_dot = 0;
    strcat(output,format2ext(output_format));
  }

  if (verbose) {
    printf("Input file:    %s\n",input);
    printf("Input format:  %s\n",format2ext(input_format));
    printf("Output file:   %s\n",output);
    printf("Output format: %s\n",format2ext(output_format));
    printf("Invert:        %s\n",invert?"yes":"no");
    if (output_format==JPEG) {
      if (quality>0) printf("Quality:       %d%\n",quality);
      else printf("Quality:       best possible\n");
    }
    if (output_format==PNG || output_format==GIF) {
      printf("Transparency:  %s\n",transparency?"yes":"no");
    }
  }

  input_stream = fopen(input,"rb");
  if (input_stream==NULL) {
    perror("fopen");
    return 1;
  }
  output_stream = fopen(output,"wb");
  if (output_stream==NULL) {
    perror("fopen");
    return 1;
  }
  if (input_format==RIC) ret = ric2img(input_stream,output_stream,output_format,invert,verbose,quality,transparency);
  else ret = img2ric(input_stream,input_format,output_stream,invert,verbose);
  fclose(input_stream);
  fclose(output_stream);
  free(output);
  return ret;
}
