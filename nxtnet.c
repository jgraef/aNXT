/*
    nxtnet.c
    libnxt - A C library for using LEGO Mindstorms NXT
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
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <time.h>
#include <errno.h>

#include "nxtnet.h"

static struct nxtnet_proto_listusb_sc *packer_usb_buf;
static struct nxtnet_proto_listbt_sc *packer_bt_buf;

/**
 * Receives NXTNET packet
 *  @param sock Socket
 *  @param buf Buffer
 *  @param cmd Command byte to check for (0 for all)
 *  @return Packet received
 */
static struct nxtnet_proto_packet *nxtnet_recv(int sock,struct nxtnet_proto_packet *buf,int cmd) {
  ssize_t size;
  uint32_t packet_sig;
  uint16_t packet_size;

  // Receive header
  do size = recv(sock,buf,sizeof(struct nxtnet_proto_packet),MSG_PEEK);
  while (size>0 && size<sizeof(struct nxtnet_proto_packet));

  packet_sig = ntohl(buf->sig);
  packet_size = ntohs(buf->size);

  /*if (size>0) {
    printf("[RECV PACKET(%d)]\n",size);
    printf("Sig:      %s\n",&(packet_sig));
    printf("Size:     %d\n",packet_size);
    printf("Command:  0x%x\n",buf->cmd);
    printf("Error:    0x%x\n",buf->error);
    printf("Password: %s\n",buf->password);
  }*/

  // Check header
  if (size>0 && packet_sig==NXTNET_PROTO_SIG && (cmd==0 || buf->cmd==cmd) && packet_size<=NXTNET_BUFSIZE) {
    // Receive whole packet
    size = 0;
    while (size<packet_size) size += recv(sock,buf+size,packet_size-size,0);

    // Bring header in host byteorder
    buf->sig = packet_sig;
    buf->size = packet_size;

    return buf;
  }
  else return NULL;
}

/**
 * Sends NXTNET packet
 *  @param sock Socket
 *  @param buf Buffer
 *  @return Number of bytes sent
 */
static ssize_t nxtnet_send(int sock,struct nxtnet_proto_packet *buf) {
  size_t size = 0;
  size_t packet_size = buf->size;

/*
  printf("[SEND PACKET]\n");
  printf("Sig:      %s\n",&(buf->sig));
  printf("Size:     %d\n",buf->size);
  printf("Command:  0x%x\n",buf->cmd);
  printf("Error:    0x%x\n",buf->error);
  printf("Password: %s\n",buf->password);
*/

  // Bring header in network byte order
  buf->sig = htonl(buf->sig);
  buf->size = htons(packet_size);

  // Send packet
  while (size<packet_size) size += send(sock,buf,packet_size-size,0);

  return size;
}

///////////// Client part /////////////////

/**
 * Connects to NXTNET server
 *  @param hostname Hostname of NXTNET server
 *  @param port TCP port
 *  @param password Server password (NULL for no password)
 *  @return NXTNET client descriptor
 */
nxtnet_cli_t *nxtnet_cli_connect(const char *hostname,int port,const char *password) {
  nxtnet_cli_t *cli;
  struct hostent *hent;
  struct sockaddr_in server;
  int sock;

  // create socket
  sock = socket(AF_INET,SOCK_STREAM,0);
  if (sock==-1) return NULL;

  // get hostname
  hent = gethostbyname(hostname);
  if (hent==NULL) {
    close(sock);
    return NULL;
  }

  // connect
  memcpy(&server.sin_addr,hent->h_addr_list[0],hent->h_length);
  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  if (connect(sock,(struct sockaddr *)&server,sizeof(server))<0) {
    close(sock);
    return NULL;
  }

  // Build client descriptor
  cli = malloc(sizeof(nxtnet_cli_t));
  memset(cli,0,sizeof(nxtnet_cli_t));
  cli->sock = sock;
  cli->buf = malloc(NXTNET_BUFSIZE);
  if (password!=NULL) strncpy(cli->password,password,NXTNET_PWD_LEN);

  return cli;
}

/**
 * Lists NXTs that are connected via USB
 */
struct nxtnet_proto_listusb_sc *nxtnet_cli_listusb(nxtnet_cli_t *cli) {
  cli->buf->sig = NXTNET_PROTO_SIG;
  cli->buf->cmd = NXTNET_PROTO_DIR_CS|NXTNET_PROTO_CMD_LISTUSB;
  cli->buf->size = sizeof(struct nxtnet_proto_packet);
  cli->buf->error = 0;
  strcpy(cli->buf->password,cli->password);
  nxtnet_send(cli->sock,cli->buf);

  if (nxtnet_recv(cli->sock,cli->buf,NXTNET_PROTO_DIR_SC|NXTNET_PROTO_CMD_LISTUSB)!=NULL) {
    struct nxtnet_proto_listusb_sc* listusb = (struct nxtnet_proto_listusb_sc*)cli->buf->data;
    size_t i;

    // convert all multibyte values to host byte order
    listusb->num_items = ntohl(listusb->num_items);
    for (i=0;i<listusb->num_items;i++) {
      listusb->nxts[i].nxtid = ntohl(listusb->nxts[i].nxtid);
    }

    return listusb;
  }
  else return NULL;
}

/**
 * Lists NXTs that are connected via Bluetooth
 */
struct nxtnet_proto_listbt_sc *nxtnet_cli_listbt(nxtnet_cli_t *cli) {
  cli->buf->sig = NXTNET_PROTO_SIG;
  cli->buf->cmd = NXTNET_PROTO_DIR_CS|NXTNET_PROTO_CMD_LISTBT;
  cli->buf->size = sizeof(struct nxtnet_proto_packet);
  cli->buf->error = 0;
  strcpy(cli->buf->password,cli->password);
  nxtnet_send(cli->sock,cli->buf);
  if (nxtnet_recv(cli->sock,cli->buf,NXTNET_PROTO_DIR_SC|NXTNET_PROTO_CMD_LISTBT)!=NULL) {
    struct nxtnet_proto_listbt_sc* listbt = (struct nxtnet_proto_listbt_sc*)cli->buf->data;
    size_t i;

    // convert all multibyte values to host byte order
    listbt->num_items = ntohl(listbt->num_items);
    for (i=0;i<listbt->num_items;i++) {
      listbt->nxts[i].nxtid = ntohl(listbt->nxts[i].nxtid);
    }

    return listbt;
  }
  else return NULL;
}

ssize_t nxtnet_cli_send(nxtnet_cli_t *cli,int nxtid,const void *buf,size_t size) {
  struct nxtnet_proto_send_cs *send_cs = (struct nxtnet_proto_send_cs*)cli->buf->data;

  cli->buf->sig = NXTNET_PROTO_SIG;
  cli->buf->cmd = NXTNET_PROTO_DIR_CS|NXTNET_PROTO_CMD_SEND;
  cli->buf->size = sizeof(struct nxtnet_proto_packet)+sizeof(struct nxtnet_proto_send_cs)+size;
  cli->buf->error = 0;
  strcpy(cli->buf->password,cli->password);

  send_cs->nxtid = htonl(nxtid);
  send_cs->size = htonl(size);
  memcpy(send_cs->data,buf,size);

  nxtnet_send(cli->sock,cli->buf);

  if (nxtnet_recv(cli->sock,cli->buf,NXTNET_PROTO_DIR_SC|NXTNET_PROTO_CMD_SEND)!=NULL) {
    struct nxtnet_proto_send_sc *send_sc = (struct nxtnet_proto_send_sc*)cli->buf->data;

    return ntohl(send_sc->size);
  }
  else return 0;
}

ssize_t nxtnet_cli_recv(nxtnet_cli_t *cli,int nxtid,void *buf,size_t size) {
  struct nxtnet_proto_recv_cs *recv_cs = (struct nxtnet_proto_recv_cs*)cli->buf->data;

  cli->buf->sig = NXTNET_PROTO_SIG;
  cli->buf->cmd = NXTNET_PROTO_DIR_CS|NXTNET_PROTO_CMD_RECV;
  cli->buf->size = sizeof(struct nxtnet_proto_packet)+sizeof(struct nxtnet_proto_recv_cs);
  cli->buf->error = 0;
  strcpy(cli->buf->password,cli->password);

  recv_cs->nxtid = htonl(nxtid);
  recv_cs->size = htonl(size);

  nxtnet_send(cli->sock,cli->buf);

  if (nxtnet_recv(cli->sock,cli->buf,NXTNET_PROTO_DIR_SC|NXTNET_PROTO_CMD_SEND)!=NULL) {
    struct nxtnet_proto_recv_sc *recv_sc = (struct nxtnet_proto_recv_sc*)cli->buf->data;

    size = ntohl(recv_sc->size);
    memcpy(buf,recv_sc->data,size);
    return size;
  }
  else return 0;
}

/**
 * Disconnects from NXTNET server
 *  @param cli NXTNET client descriptor
 */
void nxtnet_cli_disconnect(nxtnet_cli_t *cli) {
  close(cli->sock);
  free(cli->buf);
  free(cli);
}

///////////// Server part /////////////////

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

static void packer_usb_func(int nxtid,char *name) {
  size_t i = ntohl(packer_usb_buf->num_items);
  packer_usb_buf->nxts[i].nxtid = htonl(nxtid);
  strncpy(packer_usb_buf->nxts[i].name,name,NXTNET_NXTNAME_LEN);
  packer_usb_buf->num_items = htonl(i+1);
}

static void packer_bt_func(int nxtid,char *name,void *bt_addr) {
  size_t i = ntohl(packer_bt_buf->num_items);
  packer_bt_buf->nxts[i].nxtid = htonl(nxtid);
  strncpy(packer_bt_buf->nxts[i].name,name,NXTNET_NXTNAME_LEN);
  memcpy(packer_bt_buf->nxts[i].bt_addr,bt_addr,6);
  packer_bt_buf->num_items = htonl(i+1);
}

/**
 * Lists USB devices
 *  @param packet NXTNET packet
 */
static void nxtnet_srv_listusb(nxtnet_srv_t *srv,struct nxtnet_proto_packet *packet) {
  struct nxtnet_proto_listusb_sc *listusb = (struct nxtnet_proto_listusb_sc*)packet->data;

  nxtnet_srv_log(srv,"Listing NXTs that are connected via USB\n");
  listusb->num_items = 0;
  packer_usb_buf = listusb;

  if (srv->ops.list_usb!=NULL) {
    srv->ops.list_usb(packer_usb_func);
    packet->error = 0;
  }
  else packet->error = NXTNET_ERROR_NOTIMPL;

  packet->cmd = NXTNET_PROTO_DIR_SC|NXTNET_PROTO_CMD_LISTUSB;
  packet->size = sizeof(struct nxtnet_proto_packet)+sizeof(struct nxtnet_proto_listusb_sc)+ntohl(listusb->num_items)*sizeof(struct nxtnet_proto_listusb_nxts);
}

/**
 * Lists Bluetooth devices
 *  @param packet NXTNET packet
 */
static void nxtnet_srv_listbt(nxtnet_srv_t *srv,struct nxtnet_proto_packet *packet) {
  struct nxtnet_proto_listbt_sc *listbt = (struct nxtnet_proto_listbt_sc*)packet->data;

  nxtnet_srv_log(srv,"Listing NXTs that are connected via Bluetooth\n");

  listbt->num_items = 0;
  packer_bt_buf = listbt;

  if (srv->ops.list_bt!=NULL) {
    srv->ops.list_bt(packer_bt_func);
    packet->error = 0;
  }
  else packet->error = NXTNET_ERROR_NOTIMPL;

  packet->cmd = NXTNET_PROTO_DIR_SC|NXTNET_PROTO_CMD_LISTBT;
  packet->size = sizeof(struct nxtnet_proto_packet)+sizeof(struct nxtnet_proto_listbt_sc)+ntohl(listbt->num_items)*sizeof(struct nxtnet_proto_listbt_nxts);
  packet->error = 0;
}

static void nxtnet_srv_send(nxtnet_srv_t *srv,struct nxtnet_proto_packet *packet) {
  struct nxtnet_proto_send_cs *send_cs = (struct nxtnet_proto_send_cs*)packet->data;
  struct nxtnet_proto_send_sc *send_sc = (struct nxtnet_proto_send_sc*)packet->data;
  size_t size = ntohl(send_cs->size);
  int nxtid = ntohl(send_cs->nxtid);

  nxtnet_srv_log(srv,"Sending %u bytes to NXT %u\n",size,nxtid);

  if (srv->ops.send!=NULL) {
    size = srv->ops.send(nxtid,send_cs->data,size);
    packet->error = 0;
  }
  else packet->error = NXTNET_ERROR_NOTIMPL;

  send_sc->nxtid = htonl(nxtid);
  send_sc->size = htonl(size);
  packet->cmd = NXTNET_PROTO_DIR_SC|NXTNET_PROTO_CMD_SEND;
  packet->size = sizeof(struct nxtnet_proto_packet)+sizeof(struct nxtnet_proto_send_sc);
  packet->error = 0;
}

static void nxtnet_srv_recv(nxtnet_srv_t *srv,struct nxtnet_proto_packet *packet) {
  struct nxtnet_proto_recv_cs *recv_cs = (struct nxtnet_proto_recv_cs*)packet->data;
  struct nxtnet_proto_recv_sc *recv_sc = (struct nxtnet_proto_recv_sc*)packet->data;
  size_t size = ntohl(recv_cs->size);
  int nxtid = ntohl(recv_cs->nxtid);

  nxtnet_srv_log(srv,"Receiving %u bytes from NXT %u\n",size,nxtid);

  if (srv->ops.recv!=NULL) {
    size = srv->ops.recv(nxtid,recv_sc->data,size);
    packet->error = 0;
  }
  else packet->error = NXTNET_ERROR_NOTIMPL;

  recv_sc->nxtid = htonl(nxtid);
  recv_sc->size = htonl(size);
  packet->cmd = NXTNET_PROTO_DIR_SC|NXTNET_PROTO_CMD_SEND;
  packet->size = sizeof(struct nxtnet_proto_packet)+sizeof(struct nxtnet_proto_recv_sc)+size;
  packet->error = 0;
}

int nxtnet_srv_mainloop(nxtnet_srv_t *srv) {
  fd_set fds_master;
  fd_set fds_read;
  int fds_max = srv->sock;
  int i;

  if (listen(srv->sock,NXTNET_SRV_LISTEN_MAX)==-1) return;

  FD_ZERO(&fds_master);
  FD_ZERO(&fds_read);
  FD_SET(srv->sock,&fds_master);

  while (1) {
    fds_read = fds_master;
    if (select(fds_max+1,&fds_read,NULL,NULL,NULL)==-1) {
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
            if (i==fds_max) fds_max--;
          }
          else {
            if (strcmp(packet->password,srv->password)==0) {
              if (packet->cmd==NXTNET_PROTO_CMD_LISTUSB) {
                nxtnet_srv_listusb(srv,packet);
                nxtnet_send(i,packet);
              }
              else if (packet->cmd==NXTNET_PROTO_CMD_LISTBT) {
                nxtnet_srv_listbt(srv,packet);
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
  if (srv->log!=NULL) fclose(srv->log);
  for (i=0;i<NXTNET_SRV_MAXSOCKS;i++) {
    if (srv->clients[i]>0) close(srv->clients[i]);
  }
  close(srv->sock);
  free(srv->buf);
  free(srv);
}
