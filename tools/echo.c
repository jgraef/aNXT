/*
    tools/echo.c
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
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>

#include <anxt/nxt.h>
#include <anxt/display.h>

static void usage(char *prog,int ret) {
  FILE *stream = ret==0?stdout:stderr;
  fprintf(stream,"Usage: %s [OPTION]... [STRING]...\n",prog);
  fprintf(stream,"Echo the STRING(s) to NXT display\n");
  fprintf(stream,"\t-n NXTNAME   Name of NXT (Default: first found) or bluetooth address\n");
  fprintf(stream,"\t-p POSITION  Position to start writing at. Eg: 5x20 (Default: 0x0)\n");
  fprintf(stream,"\t-e           Enable interpretation of backslash escapes\n");
  fprintf(stream,"\t-E           Disable interpretation of backslash escapes (default)\n");
  fprintf(stream,"\t-h           Display this help and exit\n");
  exit(ret);
}

char *escape_string(char *str) {
  char *new = malloc(strlen(str)+1);
  size_t i,j;
  for (i=0,j=0;str[i];i++,j++) {
    if (str[i]=='\\' && str[i+1]!=0) {     // escape code
      i++;
      if (str[i]=='\\') new[j] = '\\';     // backslash
      else if (str[i]=='a') new[j] = '\a'; // alert
      else if (str[i]=='b') new[j] = '\b'; // backspace
      else if (str[i]=='f') new[j] = '\f'; // form feed
      else if (str[i]=='n') new[j] = '\n'; // new line
      else if (str[i]=='r') new[j] = '\r'; // carriage return
      else if (str[i]=='t') new[j] = '\t'; // horizontal tab
      else if (str[i]=='v') new[j] = '\v'; // vertical tab
      else if (str[i]=='0') {              // character in octal
        char *endp;
        int num = strtoul(str+i,&endp,8);
        if (errno==0) {
          new[j] = num;
          i += (endp-(str+i))-1;
        }
        else {
          new[j] = str[i];
          perror("strtoul");
        }
      }
      else new[j] = '\\';
    }
    else new[j] = str[i];
  }
  new[j] = 0;
  return new;
}

int main(int argc,char *argv[]) {
  char *name = NULL;
  int x = 0;
  int y = 0;
  int escape = 0;
  int c;

  while ((c = getopt(argc,argv,":eEhvn:p:c:"))!=-1) {
    switch (c) {
      case 'h':
        usage(argv[0],0);
        break;
      case 'n':
        name = optarg;
        break;
      case 'e':
        escape = 1;
        break;
      case 'E':
        escape = 0;
        break;
      case 'p':
        if (sscanf(optarg,"%dx%d",&x,&y)!=2) {
          fprintf(stderr,"Invalid position: %s\n",optarg);
          usage(argv[0],1);
        }
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
  nxt_display_t *display = nxt_display_open(nxt);
  if (display==NULL) {
    fprintf(stderr,"Could not open display\n");
    nxt_close(nxt);
    return 1;
  }

  for (c=optind;c<argc;c++) {
    char *string;
    if (escape) string = escape_string(argv[c]);
    else string = argv[c];

    nxt_display_text_ext(display,NXT_DISPLAY_BLACK,&x,&y,string,1);
    if (c<argc-1) nxt_display_text_ext(display,NXT_DISPLAY_BLACK,&x,&y," ",0);

    if (escape) free(string);
  }

  int ret = nxt_error(nxt);
  nxt_display_flush(display,1);
  nxt_display_close(display);
  nxt_close(nxt);

  return ret;
}
