/*
    scan.c
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

#include <sys/types.h>
#include <nxtnet.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

/**
 * Display usage
 *  @param cmd Program's name
 *  @param r Exit value
 */
void usage(char *cmd,int r) {
  FILE *out = r==0?stdout:stderr;
  fprintf(out,"Usage: %s [OPTION]... [HOST]...\n",cmd);
  fprintf(out,"Scans for NXT bricks\n");
  fprintf(out,"Options:\n");
  fprintf(out,"\t-h           Show help\n");
  fprintf(out,"\t-p PORT      Set port (Default: %d)\n",NXTNET_DEFAULT_PORT);
  exit(r);
}

/**
 * Scans for NXTs
 *  @param cli Client connection
 */
void nxtnet_scan(nxtnet_cli_t *cli) {
  size_t i;

  // Search USB
  printf("  USB:\n");
  struct nxtnet_proto_listusb_sc *listusb = nxtnet_cli_listusb(cli);
  if (listusb!=NULL) {
    for (i=0;i<listusb->num_items;i++) printf("    %s\t%d\n",listusb->nxts[i].name,listusb->nxts[i].nxtid);
  }

  // Search Bluetooth
  printf("  Bluetooth:\n");
  struct nxtnet_proto_listbt_sc *listbt = nxtnet_cli_listbt(cli);
  if (listbt!=NULL) {
    for (i=0;i<listbt->num_items;i++) {
      printf("    %s\t%d\t%02x:%02x:%02x:%02x:%02x:%02x\n",listbt->nxts[i].name,listbt->nxts[i].nxtid,listbt->nxts[i].bt_addr[0],listbt->nxts[i].bt_addr[1],listbt->nxts[i].bt_addr[2],listbt->nxts[i].bt_addr[3],listbt->nxts[i].bt_addr[4],listbt->nxts[i].bt_addr[5]);
    }
  }
}

/// Main function
int main(int argc,char *argv[]) {
  int c;
  int port = NXTNET_DEFAULT_PORT;

  while ((c = getopt(argc,argv,":hp:"))!=-1) {
    switch (c) {
      case 'h':
        usage(argv[0],0);
        break;
      case 'p':
        port = atoi(optarg);
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
    int i;
    for (i=optind;i<argc;i++) {
      /// @todo Password
      nxtnet_cli_t *cli = nxtnet_cli_connect(argv[i],port,NULL);
      if (cli!=NULL) {
        printf("%s:\n",argv[i]);
        nxtnet_scan(cli);
      }
      else fprintf(stderr,"nxt_scan: %s: %s",argv[i],strerror(errno));
    }
  }
  else {
    nxtnet_cli_t *cli = nxtnet_cli_connect("127.0.0.1",port,NULL);
    if (cli!=NULL) nxtnet_scan(cli);
    else fprintf(stderr,"nxt_scan: 127.0.0.1: %s",strerror(errno));
  }

  return 0;
}
