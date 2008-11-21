/*
    list.c
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

#include <sys/types.h>
#include <nxt.h>
#include <nxttools.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void usage(char *cmd,int r) {
  FILE *out = r==0?stdout:stderr;
  fprintf(out,"Usage: %s [OPTION]... [WILDCARD]\n",cmd);
  fprintf(out,"List files of NXT\n");
  fprintf(out,"Options:\n");
  fprintf(out,"\t-h          Show help\n");
  fprintf(out,"\t-n NXTNAME  Name of NXT (Default: first found) or bluetooth address\n");
  fprintf(out,"\t-s          Show filesizes\n");
  fprintf(out,"\t-a          Show all files (also files starting with a .)\n");
  exit(r);
}

int main(int argc,char *argv[]) {
  char *name = NULL;
  char *wild = "*.*";
  int filesizes = 0;
  int hidden = 0;
  int c;

  while ((c = getopt(argc,argv,":hn:sa"))!=-1) {
    switch(c) {
      case 'h':
        usage(argv[0],0);
        break;
      case 's':
        filesizes = 1;
        break;
      case 'a':
        hidden = 1;
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

  if (optind<argc) wild = argv[optind];

  nxt_init();
  nxt_t *nxt = nxt_open(name);
  if (nxt==NULL) {
    fprintf(stderr,"Could not find NXT\n");
    return 1;
  }

  nxt_list_files(nxt,stdout,wild,filesizes,hidden);

  int ret = nxt_error(nxt);
  if (name!=NULL) free(name);
  nxt_close(nxt);

  return ret;
}
