/*
    tools/beep.c
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
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include <anxt/nxt.h>

static void usage(char *cmd,int r) {
  FILE *out = r==0?stdout:stderr;
  fprintf(out,"Usage: %s [OPTIONS]\n",cmd);
  fprintf(out,"Play sound on the NXT brick\n");
  fprintf(out,"Options:\n");
  fprintf(out,"\t-h           Show help\n");
  fprintf(out,"\t-n NXTNAME   Name of NXT (Default: first found) or bluetooth address\n");
  fprintf(out,"\t-f FREQUENCY Frequency (Default: 440Hz)\n");
  fprintf(out,"\t-d DURATION  Duration (Default: 200ms)\n");
  exit(r);
}

int main(int argc,char *argv[]) {
  char *name = NULL;
  int freq = 440;
  int dur = 200;
  int c,newfreq,newdur;

  while ((c = getopt(argc,argv,":hn:f:d:"))!=-1) {
    switch (c) {
      case 'h':
        usage(argv[0],0);
        break;
      case 'f':
        newfreq = atoi(optarg);
        if (newfreq<=0) {
          fprintf(stderr,"Invalid frequency: %d\n",newfreq);
          return 1;
        }
        else freq = newfreq;
        break;
      case 'd':
        newdur = atoi(optarg);
        if (newdur<=0) {
          fprintf(stderr,"Invalid duration: %d\n",newdur);
          return 1;
        }
        else dur = newdur;
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

  nxt_t *nxt = nxt_open(name);
  if (nxt==NULL) {
    fprintf(stderr,"Could not find NXT\n");
    return 1;
  }

  nxt_beep(nxt,freq,dur);

  int ret = nxt_error(nxt);
  nxt_close(nxt);

  return ret;
}
