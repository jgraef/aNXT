/*
    error.c
    libnxt - A C library for using LEGO Mindstorms NXT
    Copyright (C) 2008  Janosch Gräf <janosch.graef@gmx.net>
    Copyright (C) 2008  J. "MUFTI" Scheurich (IITS Universitaet Stuttgart)

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

#include <sys/types.h>
#include <nxt.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void usage(char *cmd,int r) {
  FILE *out = r==0?stdout:stderr;
  fprintf(out,"Usage: %s ERRORNUMBER\n",cmd);
  fprintf(out,"Display errorstring of NXT matching to ERRORNUMBER\n");
  fprintf(out,"Options:\n");
  fprintf(out,"\t-h          Show help\n");
  exit(r);
}

int main(int argc,char *argv[]) {
  int c;
  while ((c = getopt(argc,argv,":h"))!=-1) {
    switch(c) {
      case 'h':
        usage(argv[0],0);
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

  if (optind<argc) {
    unsigned int errornum;
    errornum = atoi(argv[optind]);
    printf("%s\n", nxt_strerror(errornum));
  } else
    usage(argv[0],0);
  return 0;
}
