/*
    libanxt_net/client.c
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
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>

#include <anxt/net.h>

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
 * Lists all NXTs
 */
struct nxtnet_proto_list_sc *nxtnet_cli_list(nxtnet_cli_t *cli) {
  cli->buf->sig = NXTNET_PROTO_SIG;
  cli->buf->cmd = NXTNET_PROTO_DIR_CS|NXTNET_PROTO_CMD_LIST;
  cli->buf->size = sizeof(struct nxtnet_proto_packet);
  cli->buf->error = 0;
  strcpy(cli->buf->password,cli->password);
  nxtnet_send(cli->sock,cli->buf);
  if (nxtnet_recv(cli->sock,cli->buf,NXTNET_PROTO_DIR_SC|NXTNET_PROTO_CMD_LIST)!=NULL) {
    struct nxtnet_proto_list_sc *list = (struct nxtnet_proto_list_sc*)cli->buf->data;
    size_t i;

    // convert all multibyte values to host byte order
    list->num_items = ntohl(list->num_items);
    for (i=0;i<list->num_items;i++) {
      list->nxts[i].handle = ntohl(list->nxts[i].handle);
    }

    return list;
  }
  else return NULL;
}

ssize_t nxtnet_cli_send(nxtnet_cli_t *cli,int handle,const void *buf,size_t size) {
  struct nxtnet_proto_send_cs *send_cs = (struct nxtnet_proto_send_cs*)cli->buf->data;

  cli->buf->sig = NXTNET_PROTO_SIG;
  cli->buf->cmd = NXTNET_PROTO_DIR_CS|NXTNET_PROTO_CMD_SEND;
  cli->buf->size = sizeof(struct nxtnet_proto_packet)+sizeof(struct nxtnet_proto_send_cs)+size;
  cli->buf->error = 0;
  strcpy(cli->buf->password,cli->password);

  send_cs->handle = htonl(handle);
  send_cs->size = htonl(size);
  memcpy(send_cs->data,buf,size);

  nxtnet_send(cli->sock,cli->buf);

  if (nxtnet_recv(cli->sock,cli->buf,NXTNET_PROTO_DIR_SC|NXTNET_PROTO_CMD_SEND)!=NULL) {
    struct nxtnet_proto_send_sc *send_sc = (struct nxtnet_proto_send_sc*)cli->buf->data;
    return ntohl(send_sc->size);
  }
  else return -1;
}

ssize_t nxtnet_cli_recv(nxtnet_cli_t *cli,int handle,void *buf,size_t size) {
  struct nxtnet_proto_recv_cs *recv_cs = (struct nxtnet_proto_recv_cs*)cli->buf->data;

  cli->buf->sig = NXTNET_PROTO_SIG;
  cli->buf->cmd = NXTNET_PROTO_DIR_CS|NXTNET_PROTO_CMD_RECV;
  cli->buf->size = sizeof(struct nxtnet_proto_packet)+sizeof(struct nxtnet_proto_recv_cs);
  cli->buf->error = 0;
  strcpy(cli->buf->password,cli->password);

  recv_cs->handle = htonl(handle);
  recv_cs->size = htonl(size);

  nxtnet_send(cli->sock,cli->buf);
  if (nxtnet_recv(cli->sock,cli->buf,NXTNET_PROTO_DIR_SC|NXTNET_PROTO_CMD_SEND)!=NULL) {
    struct nxtnet_proto_recv_sc *recv_sc = (struct nxtnet_proto_recv_sc*)cli->buf->data;
    ssize_t size = ntohl(recv_sc->size);

    if (size>0) {
      memcpy(buf,recv_sc->data,size);
    }
    return size;
  }
  else return -1;
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