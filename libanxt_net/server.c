/*
    libanxt_net/server.c
    aNXT - a NXt Toolkit
    Libraries and tools for LEGO Mindstorms NXT robots
    Copyright (C) 2008  Janosch Gräf <janosch.graef@gmx.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdarg.h>

#include <anxt/net.h>

static struct nxtnet_proto_list_sc *packer_buf;

/**
 * Writes a log message
 *  @param srv NXTNET server descriptor
 *  @param fmt Format
 *  @param ... Parameters
 */
static int nxtnet_srv_log(nxtnet_srv_t *srv,const char *fmt,...) {
  int ret = 0;
  if (srv->log!=NULL) {
    va_list args;
    va_start(args,fmt);
    time_t timer = time(NULL);
    struct tm *tm = localtime(&timer);
    ret += fprintf(srv->log,"[%02d:%02d:%02d %04d/%02d/%02d] ",tm->tm_hour,tm->tm_min,tm->tm_sec,tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday);
    ret += vfprintf(srv->log,fmt,args);
    va_end(args);
  }
  return ret;
}

/**
 * Creates a NXTNET server
 *  @param port TCP port to listen on
 *  @param password Server password (NULL for no password)
 *  @param logfile Logfile stream (NULL for no logfile)
 *  @return NXTNET server descriptor
 */
nxtnet_srv_t *nxtnet_srv_create(int port,const char *password,FILE *logfile,int local) {
  int sock;
  struct sockaddr_in address;
  nxtnet_srv_t *srv;

  // create master socket
  sock = socket(AF_INET,SOCK_STREAM,0);
  if (sock==-1) return NULL;

  // bind socket
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);
  if (bind(sock,(struct sockaddr*)&address,sizeof(address))==-1) return NULL;

  // create server descriptor
  srv = malloc(sizeof(nxtnet_srv_t));
  memset(srv,0,sizeof(nxtnet_srv_t));
  srv->sock = sock;
  if (password!=NULL) strncpy(srv->password,password,NXTNET_PWD_LEN);
  srv->log = logfile;
  srv->buf = malloc(NXTNET_BUFSIZE);
  srv->local = local;

  nxtnet_srv_log(srv,"NXTNET Server is running now\n");

  return srv;
}

static void packer_func(int handle, char *name, void *id, int is_bt) {
  size_t i = ntohl(packer_buf->num_items);
  packer_buf->nxts[i].handle = htonl(handle);
  packer_buf->nxts[i].is_bt = is_bt;
  strncpy(packer_buf->nxts[i].name,name,NXTNET_NXTNAME_LEN);
  memcpy(packer_buf->nxts[i].id, id,6);
  packer_buf->num_items = htonl(i+1);
}

/**
 * Lists NXTs
 *  @param packet NXTNET packet
 */
static void nxtnet_srv_list(nxtnet_srv_t *srv, struct nxtnet_proto_packet *packet) {
  struct nxtnet_proto_list_sc *list = (struct nxtnet_proto_list_sc*)packet->data;

  nxtnet_srv_log(srv,"Listing NXTs\n");

  list->num_items = 0;
  packer_buf = list;

  if (srv->ops.list!=NULL) {
    srv->ops.list(packer_func);
    packet->error = 0;
  }
  else packet->error = NXTNET_ERROR_NOTIMPL;

  packet->cmd = NXTNET_PROTO_DIR_SC|NXTNET_PROTO_CMD_LIST;
  packet->size = sizeof(struct nxtnet_proto_packet)+sizeof(struct nxtnet_proto_list_sc)+ntohl(list->num_items)*sizeof(struct nxtnet_proto_list_nxts);
  packet->error = 0;
}

static void nxtnet_srv_send(nxtnet_srv_t *srv,struct nxtnet_proto_packet *packet) {
  struct nxtnet_proto_send_cs *send_cs = (struct nxtnet_proto_send_cs*)packet->data;
  struct nxtnet_proto_send_sc *send_sc = (struct nxtnet_proto_send_sc*)packet->data;
  size_t size = ntohl(send_cs->size);
  int handle = ntohl(send_cs->handle);

  //nxtnet_srv_log(srv,"Sending %u bytes to NXT %u: %02x %02x\n",size,handle,send_cs->data[0]&0xFF,send_cs->data[1]&0xFF);

  if (srv->ops.send!=NULL) {
    size = srv->ops.send(handle,send_cs->data,size);
    packet->error = 0;
  }
  else packet->error = NXTNET_ERROR_NOTIMPL;

  send_sc->handle = htonl(handle);
  send_sc->size = htonl(size);
  packet->cmd = NXTNET_PROTO_DIR_SC|NXTNET_PROTO_CMD_SEND;
  packet->size = sizeof(struct nxtnet_proto_packet)+sizeof(struct nxtnet_proto_send_sc);
  packet->error = 0;
}

static void nxtnet_srv_recv(nxtnet_srv_t *srv,struct nxtnet_proto_packet *packet) {
  struct nxtnet_proto_recv_cs *recv_cs = (struct nxtnet_proto_recv_cs*)packet->data;
  struct nxtnet_proto_recv_sc *recv_sc = (struct nxtnet_proto_recv_sc*)packet->data;
  ssize_t size = ntohl(recv_cs->size);
  int handle = ntohl(recv_cs->handle);

  //nxtnet_srv_log(srv,"Receiving %u bytes from NXT %u\n",size,handle);

  if (srv->ops.recv!=NULL) {
    size = srv->ops.recv(handle,recv_sc->data,size);
    packet->error = 0;
  }
  else packet->error = NXTNET_ERROR_NOTIMPL;

  recv_sc->handle = htonl(handle);
  recv_sc->size = htonl(size);
  packet->cmd = NXTNET_PROTO_DIR_SC|NXTNET_PROTO_CMD_SEND;
  packet->size = sizeof(struct nxtnet_proto_packet)+sizeof(struct nxtnet_proto_recv_sc)+(size>0?size:0);
  packet->error = 0;
}

int nxtnet_srv_mainloop(nxtnet_srv_t *srv) {
  fd_set fds_master;
  fd_set fds_read;
  int fds_max = srv->sock;
  int i;
  struct timeval timeout;

  if (listen(srv->sock,NXTNET_SRV_LISTEN_MAX)==-1) return;

  FD_ZERO(&fds_master);
  FD_ZERO(&fds_read);
  FD_SET(srv->sock,&fds_master);

  while (1) {
    fds_read = fds_master;
    timeout.tv_sec = NXTNET_SELECT_TIMEOUT;
    timeout.tv_usec = 0;
    if (select(fds_max+1,&fds_read,NULL,NULL,&timeout)==-1) {
      perror("select of server socket");
    }
    for (i=0;i<=fds_max;i++) {
      if (FD_ISSET(i,&fds_read)) {
        if (i==srv->sock) { // master socket
          struct sockaddr_in addr;
          int sock;
          int addrlen = sizeof(struct sockaddr_in);
          if ((sock = accept(srv->sock,(struct sockaddr*)&addr,&addrlen))>=0) {
            if (srv->local && addr.sin_addr.s_addr!=0x0100007F) close(sock);
            else {
              FD_SET(sock,&fds_master);
              if (sock>fds_max) fds_max = sock;
              char hostname[INET_ADDRSTRLEN];
              nxtnet_srv_log(srv,"New client %s on sock %d\n",inet_ntop(addr.sin_family,&addr.sin_addr,hostname,INET_ADDRSTRLEN),sock);
            }
          }
        }
        else { // client socket
          struct nxtnet_proto_packet *packet = nxtnet_recv(i,srv->buf,0);
          if (packet==NULL) { // client disconnected/error
            nxtnet_srv_log(srv,"Client sock %d hang up\n",i);
            close(i);
            FD_CLR(i,&fds_master);
            if (i==fds_max) fds_max--;
          }
          else {
            if (strcmp(packet->password,srv->password)==0) {
              if (packet->cmd==NXTNET_PROTO_CMD_LIST) {
                nxtnet_srv_list(srv,packet);
                nxtnet_send(i,packet);
              }
              else if (packet->cmd==NXTNET_PROTO_CMD_SEND) {
                nxtnet_srv_send(srv,packet);
                nxtnet_send(i,packet);
              }
              else if (packet->cmd==NXTNET_PROTO_CMD_RECV) {
                nxtnet_srv_recv(srv,packet);
                nxtnet_send(i,packet);
              }
            }
            else {
              packet->cmd = (packet->cmd&(~NXTNET_PROTO_DIR_MASK))|NXTNET_PROTO_DIR_SC;
              packet->error = NXTNET_ERROR_WROPWD;
              packet->size = 0;
              nxtnet_send(i,packet);
            }
          }
        }
      }
    }
  }

  return 0;
}

void nxtnet_srv_destroy(nxtnet_srv_t *srv) {
  int i;

  nxtnet_srv_log(srv,"Shutting server down\n");
  for (i=0;i<NXTNET_SRV_MAXSOCKS;i++) {
    if (srv->clients[i]>0) close(srv->clients[i]);
  }
  close(srv->sock);
  free(srv->buf);
  free(srv);
}
