/*
    delflash.c
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
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

void usage(char *cmd,int r) {
  FILE *out = r==0?stdout:stderr;
  fprintf(out,"Usage: %s [OPTION]...\n",cmd);
  fprintf(out,"Delete all user flash data on NXT\n");
  fprintf(out,"Options:\n");
  fprintf(out,"\t-h         Show help\n");
  fprintf(out,"\t-n NXTNAME Name of NXT (Default: first found) or bluetooth address\n");
  fprintf(out,"\t-f         Force; Do not ask\n");

  exit(r);
}

int main(int argc,char *argv[]) {
  char *name = NULL;
  int c,really;
  int force = 0;

  while ((c = getopt(argc,argv,":hn:f"))!=-1) {
    switch(c) {
      case 'h':
        usage(argv[0],0);
        break;
      case 'n':
        name = strdup(optarg);
        break;
      case 'f':
        force = 1;
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

  if (!force) {
    really = 0;
    printf("Are you really sure to delete all user flash data on NXT (y/n) [n]: ");
    scanf("%c",&really);
    really = tolower(really);
  }
  else really = 'y';

  if (really=='y') {
    printf("Deleting user flash. Please wait...\n");
    nxt_deluserflash(nxt);
  }

  int ret = nxt_error(nxt);
  if (name!=NULL);
  nxt_close(nxt);

  return ret;
}
