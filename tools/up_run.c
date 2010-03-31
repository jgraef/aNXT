/*
    tools/up_run.c
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

/*
  A shortcut for
  nxt_stop 2>/dev/null; nxt_remove $1 2>/dev/null; nxt_upload $1 && nxt_run $1
*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include <errno.h>

#include <anxt/nxt.h>
#include <anxt/file.h>
#include <anxt/tools.h>

void usage(char *cmd,int r) {
  FILE *out = r==0?stdout:stderr;
  fprintf(out,"Usage: %s [OPTION]... INPUT [OUTPUT]\n",cmd);
  fprintf(out,"Upload program from computer to NXT and start it\n");
  fprintf(out,"Options:\n");
  fprintf(out,"\t-h                                Show help\n");
  fprintf(out,"\t-n NXTNAME                        Name of NXT (Default: first found) or bluetooth address\n");
  exit(r);
}

int main(int argc,char *argv[]) {
  char *name = NULL;
  char *src,*dest;
  int c,ret;
  int oflag = NXT_OWLINE;

  while ((c = getopt(argc,argv,":hmfn:"))!=-1) {
    switch(c) {
      case 'h':
        usage(argv[0],0);
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

  nxt_t *nxt = nxt_open(name);
  if (nxt==NULL) {
    fprintf(stderr,"Could not find NXT\n");
    return 1;
  }

  if (optind<argc) {
    int i;
    src = argv[optind];

    optind++;
    if (optind<argc) dest = argv[optind];
    else dest = basename(src);

    nxt_stop_program(nxt);

    nxt_wait_after_direct_command();

    oflag |= NXT_OWOVER;
    nxt_upload(nxt,src,dest,oflag);

    nxt_wait_extra_long_after_communication_command();

    ret = nxt_error(nxt);
    if (ret == 0)
      if (nxt_run_program(nxt,dest)<0) {
        fprintf(stderr,"Error: %s\n",nxt_strerror(nxt_error(nxt)));
      }
  } else {
    fprintf(stderr,"No input file\n");
    usage(argv[0],1);
  }

  ret = nxt_error(nxt);
  if (name!=NULL) free(name);
  nxt_close(nxt);

  return ret;
}
