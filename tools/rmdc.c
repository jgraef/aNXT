/*
    rmdc.c
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
#include <unistd.h>
#include <string.h>
#include <sndfile.h>

#include "nxtfile/rmd.h"

typedef enum {
  NONE,
  MID,
  RMD
} format_t;

static void usage(char *progname,int ret) {
  FILE *stream = ret==0?stdout:stderr;
  fprintf(stream,"Usage: %s [OPTION] FILE\n",progname);
  fprintf(stream,"Converts between MIDI and RMD\n");
  fprintf(stream,"Options:\n");
  fprintf(stream,"\t-o\tSet name of output file\n");
  fprintf(stream,"\t-h\tShow help\n");
  fprintf(stream,"\t-v\tVerbose mode\n");
  exit(ret);
}

static format_t recognize_format(char *file) {
  char buf[10];
  FILE *fd = fopen(file,"r");
  if (fd!=NULL) {
    fread(buf,10,1,fd);
    fclose(fd);

    if (memcmp(buf,"\06\00",2)==0) return RMD;
    else if (memcmp(buf,"MThd",3)==0) return MID;
  }
  return NONE;
}

static int mid2rmd(FILE *mid,FILE *rmd,int verbose) {
  return -1;
}

static int rmd2mid(FILE *rmd,FILE *mid,int verbose) {
  return -1;
}

int main(int argc,char *argv[]) {
  int c,ret;
  int verbose = 0;
  char *input;
  char *output = NULL;
  format_t input_format,output_format;

  while ((c = getopt(argc,argv,":o:hv"))!=-1) {
    switch(c) {
      case 'o':
        output = strdup(optarg);
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
  }
  else input = argv[optind];

  // get input and output format
  input_format = recognize_format(input);
  if (input_format==NONE) {
    fprintf(stderr,"Error: Unknown file format: %s\n",input);
    usage(argv[0],1);
  }
  else if (input_format==RMD) output_format = MID;
  else output_format = RMD;

  // get output file
  if (output==NULL) {
    output = malloc(strlen(input)+5);
    strcpy(output,input);
    strcat(output,output_format==MID?".mid":".rmd");
  }

  if (verbose) {
    printf("Input file:    %s\n",input);
    //printf("Input format:  %s\n",format2ext(input_format));
    printf("Output file:   %s\n",output);
    //printf("Output format: %s\n",format2ext(output_format));
  }

  FILE *input_stream = fopen(input,"r");
  if (input_stream==NULL) {
    perror("fopen");
    return 1;
  }
  FILE *output_stream = fopen(output,"w");
  if (output_stream==NULL) {
    perror("fopen");
    return 1;
  }
  if (input_format==RMD) ret = rmd2mid(input_stream,output_stream,verbose);
  else ret = mid2rmd(input_stream,output_stream,verbose);
  fclose(input_stream);
  fclose(output_stream);
  free(output);
  return -ret;
}
