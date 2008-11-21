/*
    nxtd.c
    nxtd - A daemon for managing NXT connections
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

#include <nxtnet.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>

#include "nxtd.h"

char *pidfile;
nxtnet_srv_t *server;

/**
 * Registers a NXT in NXT list
 *  @param nxt NXT
 *  @return Success?
 */
int nxtd_nxt_reg(struct nxtd_nxt *nxt) {
  size_t i;

  for (i=0;i<NXT_MAXNUM;i++) {
    if (nxts[i]==NULL) {
      nxts[i] = nxt;
      return 0;
    }
  }
  return -1;
}

void nxtd_scan() {
  nxtd_usb_scan();
  nxtd_bt_scan();
}

void nxtd_listusb(void (*packer)(int nxtid,char *name)) {
  size_t i;
  for (i=0;i<NXT_MAXNUM;i++) {
    if (nxts[i]!=NULL) {
      if (nxts[i]->conn_type==CONN_USB) packer(i,nxts[i]->nxt_name);
    }
  }
}

void nxtd_listbt(void (*packer)(int nxtid,char *name,void *bt_addr)) {
  size_t i;
  for (i=0;i<NXT_MAXNUM;i++) {
    if (nxts[i]!=NULL) {
      if (nxts[i]->conn_type==CONN_BT) {
        struct nxtd_nxt_bt *nxt = (struct nxtd_nxt_bt*)nxts[i];
        packer(i,nxt->nxt.nxt_name,&(nxt->bt_addr));
      }
    }
  }
}

ssize_t nxtd_send(int nxtid,const void *buf,size_t size) {
  if (nxts[nxtid]!=NULL) {
    if (nxts[nxtid]->conn_type==CONN_USB) return nxtd_usb_send((struct nxtd_nxt_usb*)nxts[nxtid],buf,size);
    else if (nxts[nxtid]->conn_type==CONN_BT) return nxtd_bt_send((struct nxtd_nxt_bt*)nxts[nxtid],buf,size);
  }
  else return 0;
}

ssize_t nxtd_recv(int nxtid,void *buf,size_t size) {
  if (nxts[nxtid]!=NULL) {
    if (nxts[nxtid]->conn_type==CONN_USB) return nxtd_usb_recv((struct nxtd_nxt_usb*)nxts[nxtid],buf,size);
    else if (nxts[nxtid]->conn_type==CONN_BT) return nxtd_bt_recv((struct nxtd_nxt_bt*)nxts[nxtid],buf,size);
  }
  else return 0;
}

void usage(char *cmd,int r) {
  FILE *out = r==0?stdout:stderr;
  fprintf(out,"Usage: %s [OPTIONS]\n",cmd);
  fprintf(out,"A daemon for managing NXT connections\n");
  fprintf(out,"Options:\n");
  fprintf(out,"\t-h           Show help\n");
  fprintf(out,"\t-p PORT      Set TCP port\n");
  fprintf(out,"\t-l FILE      Set log file\n");
  fprintf(out,"\t-P PASSWORD  Set password\n");
  fprintf(out,"\t-l FILE      Use local mode\n");
  exit(r);
}

void quit() {
  size_t i;

  nxtnet_srv_destroy(server);

  for (i=0;i<NXT_MAXNUM;i++) {
    if (nxts[i]!=NULL) {
      if (nxts[i]->conn_type==CONN_USB) nxtd_usb_close((struct nxtd_nxt_usb*)nxts[i]);
      else if (nxts[i]->conn_type==CONN_BT) nxtd_bt_close((struct nxtd_nxt_bt*)nxts[i]);
    }
  }

  nxtd_usb_shutdown();
  nxtd_bt_shutdown();

  unlink(pidfile);
}

/// @todo Disconnect NXTs after timeout
/// @todo Own thread for scanning Bluetooth in background
int main(int argc,char *argv[]) {
  int port = 13370;
  char *logfile = "/tmp/nxtd.log";
  FILE *logfd,*pidfd;
  char *password = NULL;
  int c;
  int local = 0;
  int run_as_daemon = 0;
  pidfile = "/var/run/nxtd.pid";

  while ((c = getopt(argc,argv,":hp:P:l:Ldi:"))!=-1) {
    switch (c) {
      case 'h':
        usage(argv[0],0);
        break;
      case 'p':
        port = atoi(optarg);
        break;
      case 'P':
        password = optarg;
        break;
      case 'l':
        logfile = optarg;
        break;
      case 'L':
        local = 1;
        break;
      case 'd':
        run_as_daemon = 1;
        break;
      case 'i':
        pidfile = optarg;
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

  memset(nxts,0,sizeof(nxts));

  nxtd_usb_init();
  nxtd_bt_init();

  pidfd = fopen(pidfile,"w");
  if (pidfd) {
    fprintf(pidfd,"%d\n",getpid());
    fclose(pidfd);
  }
  else perror("fopen");

  if (strcmp(logfile,"-")==0) logfd = stdout;
  else logfd = fopen(logfile,"a");
  server = nxtnet_srv_create(port,password,logfd,local);
  if (server==NULL) {
    perror("creating nxt daemon");
    return 1;
  }
  server->ops.list_usb = nxtd_listusb;
  server->ops.list_bt = nxtd_listbt;
  server->ops.recv = nxtd_recv;
  server->ops.send = nxtd_send;
  atexit(quit);

  signal(SIGTERM,exit);
  signal(SIGQUIT,exit);
  signal(SIGINT,exit);

  printf("Scanning for NXTs (this can take a while)...\n");
  nxtd_scan();

  printf("nxtd is running now\n");

  if (run_as_daemon) daemon(0,0);
  nxtnet_srv_mainloop(server);

  return 0;
}
