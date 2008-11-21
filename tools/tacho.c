/*
    tacho.c
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
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

void usage(char *cmd,int r) {
  FILE *out = r==0?stdout:stderr;
  fprintf(out,"Usage: %s [OPTIONS]\n",cmd);
  fprintf(out,"Output tacho value of motor (amount motor moved so far)\n");
  fprintf(out,"Options:\n");
  fprintf(out,"\t-h         Show help\n");
  fprintf(out,"\t-n NXTNAME Name of NXT (Default: first found) or bluetooth address\n");
  fprintf(out,"\t-m MOTOR   Select motor (Default: A)\n");
  fprintf(out,"\t-r         Reset tacho count after reading it\n");
  fprintf(out,"\t-v         Verbose mode\n");
  exit(r);
}

int main(int argc,char *argv[]) {
  char *name = NULL;
  int motor = 0;
  int reset = 0;
  int verbose = 0;
  int c,newmotor,rot;

  while ((c = getopt(argc,argv,":hm:rn:v"))!=-1) {
    switch(c) {
      case 'h':
        usage(argv[0],0);
        break;
      case 'm':
        newmotor = tolower(*optarg)-'a';
        if (newmotor<0 || newmotor>3) {
          fprintf(stderr,"Invalid motor: %s\n",optarg);
          usage(argv[0],1);
        }
        else motor = newmotor;
        break;
      case 'r':
        reset = 1;
        break;
      case 'n':
        name = strdup(optarg);
        break;
      case 'v':
        verbose = 1;
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

  rot = nxt_tacho(nxt,motor);
  if (reset) nxt_resettacho(nxt,motor,0);
  if (verbose) printf("Motor %c: ",motor+'A');
  printf("%d%s\n",rot,verbose?"°":"");

  int ret = nxt_error(nxt);
  if (name!=NULL) free(name);
  nxt_close(nxt);

  return ret;
}
