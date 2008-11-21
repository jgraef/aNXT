/*
    rso.c
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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <sndfile.h>

#include "nxtfile/ric.h"

typedef enum {
  NONE,
  WAV,
  RSO
} format_t;

static void usage(char *progname,int ret) {
  FILE *stream = ret==0?stdout:stderr;
  fprintf(stream,"Usage: %s [OPTION] FILE\n",progname);
  fprintf(stream,"Converts between WAV files and RSO\n");
  fprintf(stream,"Options:\n");
  fprintf(stream,"\t-o\tSet name of output file\n");
  fprintf(stream,"\t-h\tShow help\n");
  fprintf(stream,"\t-v\tVerbose mode\n");
  exit(ret);
}

static char *format2ext(format_t format) {
  if (format==RSO) return ".rso";
  else if (format==WAV) return ".wav";
  else return "";
}

static format_t recognize_format(char *file) {
  char buf[4];
  FILE *fd = fopen(file,"r");
  if (fd!=NULL) {
    fread(buf,4,1,fd);
    fclose(fd);

    if (memcmp(buf,"RIFF",4)==0) return WAV;
    else if (memcmp(buf,"\1\0",2)==0) return RSO;
  }
  return NONE;
}

static int wav2rso(FILE *wav,FILE *rso,int verbose) {
  // open sound file
  SF_INFO sf_info;
  SNDFILE *sf = sf_open_fd(fileno(wav),SFM_READ,&sf_info,0);
  if (sf==NULL) {
    perror("sf_open");
    return -1;
  }

  // Check for channel count and sample rate
  if (sf_info.samplerate<2000 || sf_info.samplerate>16000) {
    fprintf(stderr,"ERROR: Illegal sample rate: %d\n",sf_info.samplerate);
    sf_close(sf);
    return -1;
  }

  // Print information
  if (verbose) {
    printf("Frames:        %d\n",sf_info.frames);
    printf("Samplerate:    %d\n",sf_info.samplerate);
  }

  // Read samples
  size_t size = sf_info.frames;
  uint16_t *samples = malloc(size*sf_info.channels*2);
  uint8_t *samples8 = (uint8_t*)samples;
  sf_readf_short(sf,samples,sf_info.frames);

  // Translate to NXT wave data
  size_t i;
  for (i=0;i<sf_info.frames;i++) {
    samples8[i] = (samples[i*sf_info.channels]/0x100)-0x80;
  }

  // Convert to RSO
  void *rso_data = NULL;
  size = rso_encode(&rso_data,sf_info.samplerate,size,samples);

  // Write RSO data to file
  fwrite(rso_data,1,size,rso);

  // Release resources
  free(rso_data);
  free(samples);
  sf_close(sf);

  return 0;
}

static int rso2wav(FILE *rso,FILE *wav,int verbose) {
  // read RSO data from file
  void *rso_data = NULL;
  size_t size = 0;
  while (!feof(rso)) {
    rso_data = realloc(rso_data,size+1024);
    size += fread(rso_data+size,1,1024,rso);
  }

  // Extract wave data
  uint8_t *samples = NULL;
  unsigned int samplerate;
  size = rso_decode(&samples,rso_data,&samplerate);

  if (verbose) printf("Samplerate:    %d\n",samplerate);

  // Open sound file
  SF_INFO sf_info = {
    .samplerate = samplerate,
    .channels = 1,
    .format = SF_FORMAT_WAV|SF_FORMAT_PCM_16
  };
  SNDFILE *sf = sf_open_fd(fileno(wav),SFM_WRITE,&sf_info,0);

  // Translate to signed 16 bit wave data
  size_t i;
  for (i=0;i<size;i++) {
    uint16_t buf = (samples[i]+0x80)*0x100;
    sf_writef_short(sf,&buf,1);
  }

  // Release resources
  free(rso_data);
  sf_close(sf);
}

int main(int argc,char *argv[]) {
  int c,ret;
  int verbose = 0;
  char *output = NULL;
  char *input;
  format_t input_format = NONE;
  format_t output_format = NONE;

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
  else if (input_format==RSO) output_format = WAV;
  else output_format = RSO;

  // get output file
  if (output==NULL) {
    output = malloc(strlen(input)+5);
    strcpy(output,input);
    strcat(output,format2ext(output_format));
  }

  if (verbose) {
    printf("Input file:    %s\n",input);
    printf("Input format:  %s\n",format2ext(input_format));
    printf("Output file:   %s\n",output);
    printf("Output format: %s\n",format2ext(output_format));
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
  if (input_format==RSO) ret = rso2wav(input_stream,output_stream,verbose);
  else ret = wav2rso(input_stream,output_stream,verbose);
  fclose(input_stream);
  fclose(output_stream);
  free(output);
  return -ret;
}
