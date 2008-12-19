/*
    motor_record.c
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
#include <string.h>
#include <stdlib.h>

void usage(char *cmd,int r) {
  FILE *out = r==0?stdout:stderr;
  fprintf(out,"Usage: %s [OPTIONS]\n",cmd);
  fprintf(out,"Record tacho, write values over time to standard output, beep at start and stop\n");
  fprintf(out,"Options:\n");
  fprintf(out,"\t-h         Show help\n");
  fprintf(out,"\t-n NXTNAME Name of NXT (Default: first found) or bluetooth address\n");
  fprintf(out,"\t-m MOTOR   Select motor (Default: A)\n");
  fprintf(out,"\t-t TIME    Recorded time (Default: 10 seconds)\n");
  exit(r);
}

void print_time_tacho(double time,int tacho) {
  printf("%lf %d\n",time,tacho);
}

int main(int argc,char *argv[]) {
  char *name = NULL;
  int motor = 0;
  double t = 10.0;
  int ret = 0;
  int c,newmotor;

  while ((c = getopt(argc,argv,":hm:n:t:"))!=-1) {
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
      case 'n':
        name = strdup(optarg);
        break;
      case 't':
        t = atof(optarg);
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
  nxt_beep(nxt,440,1);

  nxt_motor_record(nxt,motor,t,print_time_tacho);

  if (name!=NULL) free(name);
  nxt_beep(nxt,440,1);
  nxt_close(nxt);

  return ret;
}
