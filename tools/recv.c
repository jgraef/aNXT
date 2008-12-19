/*
    recv.c
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
#include <string.h>
#include <stdlib.h>

void usage(char *cmd,int r) {
  FILE *out = r==0?stdout:stderr;
  fprintf(out,"Usage: %s [OPTIONS]\n",cmd);
  fprintf(out,"Receives a message from NXT\n");
  fprintf(out,"Options:\n");
  fprintf(out,"\t-h          Show help\n");
  fprintf(out,"\t-n NXTNAME  Name of NXT (Default: first found) or bluetooth address\n");
  fprintf(out,"\t-m          Select mailbox (1-10; Default: 1)\n");
  fprintf(out,"\t-c          Clear mailbox after receiving from it\n");
  fprintf(out,"\t-v          Verbose mode\n");
  exit(r);
}

int main(int argc,char *argv[]) {
  char *name = NULL;
  char *msg;
  int box = 0;
  int clear = 0;
  int verbose = 0;
  int c,newbox;

  while ((c = getopt(argc,argv,":hn:m:cv"))!=-1) {
    switch(c) {
      case 'h':
        usage(argv[0],0);
        break;
      case 'n':
        name = strdup(optarg);
        break;
      case 'm':
        newbox = atoi(optarg)-1;
        if (VALID_MAILBOX(newbox)) box = newbox;
        else {
          fprintf(stderr,"Invalid mailbox: %d",newbox+1);
          usage(argv[0],1);
        }
        break;
      case 'v':
        verbose = 1;
        break;
      case 'c':
        clear = 1;
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

  if ((msg = nxt_recvmsg(nxt,box,clear))!=NULL) {
    if (verbose) printf("Mailbox %d: ",box+1);
    printf("%s%c",msg,verbose?'\n':0);
    free(msg);
  }
  else fprintf(stderr,"Error: %s\n",nxt_strerror(nxt_error(nxt)));

  int ret = nxt_error(nxt);
  if (name!=NULL) free(name);
  nxt_close(nxt);

  return ret;
}

