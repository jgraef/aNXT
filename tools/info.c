/*
    info.c
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
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define ERROR() fprintf(stderr,"Error: %s\n",nxt_strerror(nxt_error(nxt)));

void usage(char *cmd,int r) {
  FILE *out = r==0?stdout:stderr;
  fprintf(out,"Usage: %s [OPTION]...\n",cmd);
  fprintf(out,"Show information about NXT\n");
  fprintf(out,"Options:\n");
  fprintf(out,"\t-h         Show help\n");
  fprintf(out,"\t-n NXTNAME Name of NXT (Default: first found) or bluetooth address\n");
  exit(r);
}

int main(int argc,char *argv[]) {
  char *name = NULL;
  char *nxt_name;
  unsigned char bt_addr[6];
  unsigned int bt_strength,free_flash;
  int c,firmmaj,firmmin,protomaj,protomin,battery,devinfo,ver,volume;

  while ((c = getopt(argc,argv,":hn:"))!=-1) {
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

  nxt_init();
  nxt_t *nxt = nxt_open(name);
  if (nxt==NULL) {
    fprintf(stderr,"Could not find NXT\n");
    return 1;
  }

  nxt_name = NULL;
  memset(bt_addr,0,6);
  free_flash = 0;
  firmmaj = 0;
  firmmin = 0;
  protomaj = 0;
  protomin = 0;

  if ((volume = nxt_getvolume(nxt))==-1) ERROR();
  if ((battery = nxt_getbattery(nxt))==-1) ERROR();
  if ((devinfo = nxt_getdevinfo(nxt,&nxt_name,bt_addr,&bt_strength,&free_flash))==-1) ERROR();
  if ((ver = nxt_getver(nxt,&firmmaj,&firmmin,&protomaj,&protomin))==-1) ERROR();

  printf("Name:               %s\n",nxt_name);
  if (battery>=0) printf("Battery:            %dmV\n",battery);
  if (volume>=0) printf("Volume:             %d\n",volume);
  if (devinfo>=0) {
    printf("Bluetooth Address:  %02x:%02x:%02x:%02x:%02x:%02x\n",bt_addr[0],bt_addr[1],bt_addr[2],bt_addr[3],bt_addr[4],bt_addr[5]);
    printf("Bluetooth Strength: %u%%\n",bt_strength);
    printf("Free flash:         %u bytes\n",free_flash);
  }
  if (ver>=0) {
    printf("Firmware version:   %u.%u\n",firmmaj,firmmin);
    printf("Protocol version:   %u.%u\n",protomaj,protomin);
  }

  int ret = nxt_error(nxt);
  free(nxt_name);
  if (name!=NULL) free(name);
  nxt_close(nxt);

  return ret;
}
