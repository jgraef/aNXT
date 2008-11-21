/*
    pollcmd.c
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

#include <nxt.h>
#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <stdlib.h>
#include <string.h>

void usage(char *cmd,int r) {
  FILE *out = r==0?stdout:stderr;
  fprintf(out,"Usage: %s [OPTION]...\n",cmd);
  fprintf(out,"Poll from NXT\n");
  fprintf(out,"Options:\n");
  fprintf(out,"\t-h         Show help\n");
  fprintf(out,"\t-n NXTNAME Name of NXT (Default: first found) or bluetooth address\n");
  fprintf(out,"\t-b BUFFER  Select buffer (Default: poll)\n");
  fprintf(out,"\tValid buffers are: poll,highspeed\n");
  exit(r);
}

int str2buf(char *str) {
  if (strcasecmp(str,"poll")==0) return NXT_BUFFER_POLL;
  else if (strcasecmp(str,"highspeed")==0) return NXT_BUFFER_HIGHSPEED;
  else return -1;
}

int main(int argc,char *argv[]) {
  char *name = NULL;
  int c,newbuf;
  int buf = NXT_BUFFER_POLL;

  while ((c = getopt(argc,argv,":hn:b:"))!=-1) {
    switch(c) {
      case 'h':
        usage(argv[0],0);
        break;
      case 'n':
        name = strdup(optarg);
        break;
      case 'b':
        newbuf = str2buf(optarg);
        if (newbuf==-1) {
          fprintf(stderr,"Invalid buffer: %s\n",optarg);
          return 1;
        }
        else buf = newbuf;
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

  void *cmd;
  ssize_t size = nxt_pollcmd(nxt,&cmd,buf);
  if (size>0) {
    fwrite(cmd,1,size,stdout);
    free(cmd);
  }

  int ret = nxt_error(nxt);
  if (name!=NULL) free(name);
  nxt_close(nxt);

  return ret;
}
