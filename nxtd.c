/*
    nxtd.c - A daemon for managing NXT connections
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

#include <nxtnet.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdarg.h>
#include <pthread.h>

#include "nxtd.h"

static pthread_t scanner_tid = -1;
static char *pidfile = "/var/run/nxtd.pid";
static nxtnet_srv_t *server = NULL;
static FILE *logfd = NULL;

/**
 * Prints log message
 *  @param fmt Format
 *  @param ... Parameters
 *  @see printf
 */
static void logmsg(const char *fmt,...) {
  if (logfd!=NULL) {
    va_list args;
    va_start(args,fmt);
    time_t timer = time(NULL);
    struct tm *tm = localtime(&timer);
    fprintf(logfd,"[%02d:%02d:%02d %04d/%02d/%02d] ",tm->tm_hour,tm->tm_min,tm->tm_sec,tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday);
    vfprintf(logfd,fmt,args);
    va_end(args);
  }
}

/**
 * Registers a NXT in NXT list
 *  @param nxt NXT
 *  @return Success?
 */
int nxtd_nxt_reg(struct nxtd_nxt *nxt) {
  size_t i;

  pthread_mutex_lock(&nxts.mutex);
  for (i=0;i<NXTD_MAXNUM;i++) {
    if (nxts.list[i]==NULL) {
      nxts.list[i] = nxt;
      logmsg("Added %s (%d;%s)\n",nxts.list[i]->name,i,nxts.list[i]->conn_type==NXTD_USB?"USB":"BT");
      pthread_mutex_unlock(&nxts.mutex);
      return 0;
    }
  }
  pthread_mutex_unlock(&nxts.mutex);
  return -1;
}

void nxtd_nxt_remove(int nxtid) {
  if (nxts.list[nxtid]==NULL) return;
  logmsg("Removing %s (%d;%s)\n",nxts.list[nxtid]->name,nxtid,nxts.list[nxtid]->conn_type==NXTD_USB?"USB":"BT");
  if (nxts.list[nxtid]->conn_type==NXTD_USB) nxtd_usb_close((struct nxtd_nxt_usb*)nxts.list[nxtid]);
  else if (nxts.list[nxtid]->conn_type==NXTD_BT) nxtd_bt_close((struct nxtd_nxt_bt*)nxts.list[nxtid]);
  nxts.list[nxtid] = 0;
}

/**
 * Finds NXT in NXT list
 *  @param name NXT name
 *  @return NXT handle
 */
struct nxtd_nxt *nxtd_nxt_find(const char *name,nxtd_conn_t conn_type) {
  size_t i;

  pthread_mutex_lock(&nxts.mutex);
  for (i=0;i<NXTD_MAXNUM;i++) {
    if (nxts.list[i]!=NULL) {
      if (strcmp(nxts.list[i]->name,name)==0 && nxts.list[i]->conn_type==conn_type) {
        pthread_mutex_unlock(&nxts.mutex);
        return nxts.list[i];
      }
    }
  }
  pthread_mutex_unlock(&nxts.mutex);
  return NULL;
}

/**
 * Scans for NXTs
 */
static void *nxtd_scanner(void *x) {
  while (1) {
    size_t i;

    // Scan for NXTs
    int usb = nxtd_usb_scan();
    int bt = nxtd_bt_scan();

    // Find NXTs that have timed out
    /*pthread_mutex_lock(&nxts.mutex);
    time_t now = time(NULL);
    for (i=0;i<NXTD_MAXNUM;i++) {
      if (nxts.list[i]!=NULL) {
        if (nxts.list[i]->conn_timeout!=0 && nxts.list[i]->conn_timeout<now) {
          if (nxts.list[i]->conn_type==NXTD_USB) nxtd_usb_disconnect((struct nxtd_nxt_usb*)nxts.list[i]);
          else if (nxts.list[i]->conn_type==NXTD_BT) nxtd_bt_disconnect((struct nxtd_nxt_bt*)nxts.list[i]);
          logmsg("Timeout: %s (%d;%s)\n",nxts.list[i]->name,i,nxts.list[i]->conn_type==NXTD_USB?"USB":"BT");
        }
      }
    }
    pthread_mutex_unlock(&nxts.mutex);*/

    pthread_testcancel();
  }
}

/**
 * Sends KEEPALIVE to NXT. To check that NXT is still there
 *  @param nxt NXT
 *  @return 0=NXT still there; -1=NXT gone
 */
static int nxtd_keepalive(struct nxtd_nxt *nxt) {
  char buf[] = { 0x00,0x0D,0,0,0,0,0 };
  if (nxt->conn_type==NXTD_USB) {
    if (nxtd_usb_send((struct nxtd_nxt_usb*)nxt,buf,2)!=2) return -1;
    if (nxtd_usb_recv((struct nxtd_nxt_usb*)nxt,buf,7)!=7) return -1;
  }
  else if (nxt->conn_type==NXTD_BT) {
    if (nxtd_bt_send((struct nxtd_nxt_bt*)nxt,buf,2)!=2) return -1;
    if (nxtd_bt_recv((struct nxtd_nxt_bt*)nxt,buf,7)!=7) return -1;
  }
  return 0;//buf[2]==0?0:-1;
}

/**
 * Lists all NXTs that are connected via USB
 *  @param packer Packer function
 */
static void nxtd_listusb(void (*packer)(int nxtid,char *name)) {
  size_t i;
  pthread_mutex_lock(&nxts.mutex);
  for (i=0;i<NXTD_MAXNUM;i++) {
    if (nxts.list[i]!=NULL) {
      if (nxts.list[i]->conn_type==NXTD_USB) {
        if (nxtd_keepalive(nxts.list[i])==0) packer(i,nxts.list[i]->name);
        else nxtd_nxt_remove(i);
      }
    }
  }
  pthread_mutex_unlock(&nxts.mutex);
}

/**
 * Lists all NXTs that are connected via BT
 *  @param packer Packer function
 */
static void nxtd_listbt(void (*packer)(int nxtid,char *name,void *bt_addr)) {
  size_t i;
  pthread_mutex_lock(&nxts.mutex);
  for (i=0;i<NXTD_MAXNUM;i++) {
    if (nxts.list[i]!=NULL) {
      if (nxts.list[i]->conn_type==NXTD_BT) {
        if (nxtd_keepalive(nxts.list[i])==0) {
          struct nxtd_nxt_bt *nxt = (struct nxtd_nxt_bt*)nxts.list[i];
          packer(i,nxt->nxt.name,&(nxt->bt_addr));
        }
        else nxtd_nxt_remove(i);
      }
    }
  }
  pthread_mutex_unlock(&nxts.mutex);
}

/**
 * Sends data to NXT
 *  @param nxtid NXT ID
 *  @param buf Buffer
 *  @param size How many bytes to send
 *  @return How many bytes sent
 */
static ssize_t nxtd_send(int nxtid,const void *buf,size_t size) {
  ssize_t ret = -1;
  pthread_mutex_lock(&nxts.mutex);
  if (nxts.list[nxtid]!=NULL) {
    if (nxts.list[nxtid]->conn_type==NXTD_USB) ret = nxtd_usb_send((struct nxtd_nxt_usb*)nxts.list[nxtid],buf,size);
    else if (nxts.list[nxtid]->conn_type==NXTD_BT) ret = nxtd_bt_send((struct nxtd_nxt_bt*)nxts.list[nxtid],buf,size);
  }
  pthread_mutex_unlock(&nxts.mutex);
  if (ret!=size) nxtd_nxt_remove(nxtid);
  return ret;
}

/**
 * Receives data from NXT
 *  @param nxtid NXT ID
 *  @param buf Buffer
 *  @param size How many bytes to receive
 *  @return How many bytes received
 */
static ssize_t nxtd_recv(int nxtid,void *buf,size_t size) {
  ssize_t ret = 0;
  pthread_mutex_lock(&nxts.mutex);
  if (nxts.list[nxtid]!=NULL) {
    if (nxts.list[nxtid]->conn_type==NXTD_USB) ret = nxtd_usb_recv((struct nxtd_nxt_usb*)nxts.list[nxtid],buf,size);
    else if (nxts.list[nxtid]->conn_type==NXTD_BT) ret = nxtd_bt_recv((struct nxtd_nxt_bt*)nxts.list[nxtid],buf,size);
  }
  pthread_mutex_unlock(&nxts.mutex);
  if (ret!=size) nxtd_nxt_remove(nxtid);
  return ret;
}

/**
 * Displays program usage
 *  @param cmd Program name
 *  @param r Return value
 */
static void usage(char *cmd,int r) {
  FILE *out = r==0?stdout:stderr;
  fprintf(out,"Usage: %s [OPTIONS]\n",cmd);
  fprintf(out,"A daemon for managing NXT connections\n");
  fprintf(out,"Options:\n");
  fprintf(out,"\t-h           Show help\n");
  fprintf(out,"\t-p PORT      Set TCP port (Default: %d)\n",NXTNET_DEFAULT_PORT);
  fprintf(out,"\t-l FILE      Set log file (Default: /var/log/nxtd.log)\n");
  fprintf(out,"\t-P PASSWORD  Set password\n");
  fprintf(out,"\t-d           Run as daemon\n");
  fprintf(out,"\t-i FILE      Set pid file (Default: /var/run/nxtd.pid)\n");
  fprintf(out,"\t-L           Use local mode\n");
  exit(r);
}

/**
 * Shuts nxtd down
 */
static void quit() {
  size_t i;

  if (scanner_tid!=-1) pthread_cancel(scanner_tid);

  if (server!=NULL) nxtnet_srv_destroy(server);

  for (i=0;i<NXTD_MAXNUM;i++) {
    if (nxts.list[i]!=NULL) {
      if (nxts.list[i]->conn_type==NXTD_USB) nxtd_usb_close((struct nxtd_nxt_usb*)nxts.list[i]);
      else if (nxts.list[i]->conn_type==NXTD_BT) nxtd_bt_close((struct nxtd_nxt_bt*)nxts.list[i]);
    }
  }

  nxtd_usb_shutdown();
  nxtd_bt_shutdown();

  unlink(pidfile);
}

/**
 * Runs nxtd
 *  @param argc Number of arguments
 *  @param argv Arguments
 *  @return Program's return value
 */
int main(int argc,char *argv[]) {
  int port = NXTNET_DEFAULT_PORT;
  char *logfile = "/var/log/nxtd.log";
  FILE *pidfd;
  char *password = NULL;
  int c;
  int local = 0;
  int run_as_daemon = 0;

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

  // Initialze
  if (run_as_daemon) daemon(0,0);
  atexit(quit);
  signal(SIGTERM,exit);
  signal(SIGQUIT,exit);
  signal(SIGINT,exit);
  memset(nxts.list,0,sizeof(nxts.list));
  pthread_mutex_init(&nxts.mutex,NULL);
  if (nxtd_usb_init()==-1 || nxtd_bt_init()==-1) {
    fprintf(stderr,"Could not initialize USB and/or Bluetooth\n");
    return 1;
  }

  // Write PID to file
  pidfd = fopen(pidfile,"w");
  if (pidfd) {
    fprintf(pidfd,"%d\n",getpid());
    fclose(pidfd);
  }
  else perror("fopen");

  // Open logfile
  if (strcmp(logfile,"-")==0) logfd = stdout;
  else logfd = fopen(logfile,"a");
  server = nxtnet_srv_create(port,password,logfd,local);
  if (server==NULL) {
    perror("creating nxt daemon");
    return 1;
  }

  // Spawn thread for scanning
  pthread_create(&scanner_tid,NULL,nxtd_scanner,NULL);

  // Start server
  server->ops.list_usb = nxtd_listusb;
  server->ops.list_bt = nxtd_listbt;
  server->ops.recv = nxtd_recv;
  server->ops.send = nxtd_send;
  nxtnet_srv_mainloop(server);

  return 0;
}
