/*
    motor_playback.c
    aNXT - a NXt Toolkit
    Libraries and tools for LEGO Mindstorms NXT robots
    Copyright (C) 2008  Janosch Gräf <janosch.graef@gmx.net>
    Copyright (C) 2008  J. Scheurich (IITS Universitaet Stuttgart)

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
#include <unistd.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>

#define NXT_PLAYBACK_DEFAULT_POWER 58

void usage(char *cmd,int r) {
  FILE *out = r==0?stdout:stderr;
  fprintf(out,"Usage: %s [OPTIONS]\n",cmd);
  fprintf(out,"Get values from standard input and try to move motor accordingly\n");
  fprintf(out,"Options:\n");
  fprintf(out,"\t-h          Show help\n");
  fprintf(out,"\t-n NXTNAME  Name of NXT (Default: first found) or bluetooth address\n");
  fprintf(out,"\t-m MOTOR    Select motor (Default: A. Valid values are: A, B, C, ABC)\n");
  fprintf(out,"\t-p POWER    Set power (Default: (");
  fprintf(out,"%d (\"weak\")))\n",NXT_PLAYBACK_DEFAULT_POWER);
  fprintf(out,"\t-s          Stop (coast) after final brake of motor (Default: keep brake of motor)\n");
  fprintf(out,"\t-v          be verbose: show real and target tacho values\n");
  exit(r);
}

int main(int argc,char *argv[]) {
  char *name = NULL;
  int stop = 0;
  int motor = 0;
  int power = NXT_PLAYBACK_DEFAULT_POWER;
  int numvalues = 0;
  int i = 0;
  int c,newmotor,newpower;
  int verbose = 0;
  double *times;
  int *rotations;

  while ((c = getopt(argc,argv,":hsm:p:n:v"))!=-1) {
    switch(c) {
      case 'h':
        usage(argv[0],0);
        break;
      case 'm':
        newmotor = nxt_str2motorport(optarg);
        if (newmotor==-1) {
          fprintf(stderr,"Invalid motor: %s\n",optarg);
          usage(argv[0],1);
        }
        else motor = newmotor;
        break;
      case 'p':
        newpower = atoi(optarg);
        if (newpower<-100 | newpower>100) {
          fprintf(stderr,"Invalid power: %d\n",newpower);
          usage(argv[0],1);
        }
        else power = newpower;
        break;
      case 's':
        stop = 1;
        break;
      case 'v':
        verbose = 1;
        break;
      case 'n':
        name = strdup(optarg);
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

  if (nxt_read_recorded_file(&numvalues,&times,&rotations,stdin)!=0)
    return 1;

  nxt_t *nxt = nxt_open(name);
  if (nxt==NULL) {
    fprintf(stderr,"Could not find NXT\n");
    return 1;
  }

  nxt_motor_playback(nxt,motor,power,numvalues,times,rotations,stop,verbose); 

  int ret = nxt_error(nxt);
  if (name!=NULL) free(name);
  nxt_close(nxt);

  return ret;
}
