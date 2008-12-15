/*
    nxtnet.h
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

#ifndef _NXTNET_H_
#define _NXTNET_H_

#include <sys/types.h>
#include <stdio.h>
#include <netinet/in.h>
#include <stdint.h>

#define NXTNET_SRV_MAXSOCKS   256
#define NXTNET_SRV_LISTEN_MAX 32
#define NXTNET_BUFSIZE        128
#define NXTNET_PWD_LEN        16
#define NXTNET_NXTNAME_LEN    16

#define NXTNET_PROTO_SIG         ('N'|('X'<<8)|('T'<<16))
#define NXTNET_PROTO_DIR_CS      0x00 // or'd with command
#define NXTNET_PROTO_DIR_SC      0x80 // or'd with command
#define NXTNET_PROTO_DIR_MASK    0x80
#define NXTNET_PROTO_CMD_HELLO   0x01
#define NXTNET_PROTO_CMD_LISTUSB 0x02
#define NXTNET_PROTO_CMD_LISTBT  0x03
#define NXTNET_PROTO_CMD_SEND    0x04
#define NXTNET_PROTO_CMD_RECV    0x05

#define NXTNET_ERROR_NOERROR 0
#define NXTNET_ERROR_WROPWD  1
#define NXTNET_ERROR_NOTIMPL 2

struct nxtnet_proto_packet {
  uint32_t sig;
  uint8_t cmd;
  uint16_t size;
  uint8_t error;
  char password[NXTNET_PWD_LEN];
  char data[0];
} __attribute__ ((packed));

struct nxtnet_proto_listusb_nxts {
  uint32_t nxtid;
  char name[NXTNET_NXTNAME_LEN];
} __attribute__ ((packed)) ;

struct nxtnet_proto_listusb_sc {
  uint32_t num_items;
  struct nxtnet_proto_listusb_nxts nxts[0];
} __attribute__ ((packed));

struct nxtnet_proto_listbt_nxts {
  uint32_t nxtid;
  char name[NXTNET_NXTNAME_LEN];
  uint8_t bt_addr[6];
} __attribute__ ((packed)) ;

struct nxtnet_proto_listbt_sc {
  uint32_t num_items;
  struct nxtnet_proto_listbt_nxts nxts[0];
} __attribute__ ((packed));

struct nxtnet_proto_send_cs {
  uint32_t nxtid;
  uint32_t size;
  char data[0];
} __attribute__ ((packed));

struct nxtnet_proto_send_sc {
  uint32_t nxtid;
  uint32_t size;
} __attribute__ ((packed));

struct nxtnet_proto_recv_cs {
  uint32_t nxtid;
  uint32_t size;
} __attribute__ ((packed));

struct nxtnet_proto_recv_sc {
  uint32_t nxtid;
  uint32_t size;
  char data[0];
} __attribute__ ((packed));

typedef struct {
  int sock;
  struct nxtnet_proto_packet *buf;
  char password[NXTNET_PWD_LEN];
} nxtnet_cli_t;

typedef struct {
  int sock;
  int local;
  struct {
    void (*list_usb)(void (*packer)(int nxtid,char *name));
    void (*list_bt)(void (*packer)(int nxtid,char *name,void *bt_addr));
    ssize_t (*send)(int nxtid,const void *buf,size_t size);
    ssize_t (*recv)(int nxtid,void *buf,size_t size);
  } ops;
  int clients[NXTNET_SRV_MAXSOCKS];
  FILE *log;
  char password[NXTNET_PWD_LEN];
  struct nxtnet_proto_packet *buf;
} nxtnet_srv_t;

nxtnet_cli_t *nxtnet_cli_connect(const char *hostname,int port,const char *password);
struct nxtnet_proto_listusb_sc *nxtnet_cli_listusb(nxtnet_cli_t *cli);
struct nxtnet_proto_listbt_sc *nxtnet_cli_listbt(nxtnet_cli_t *cli);
ssize_t nxtnet_cli_send(nxtnet_cli_t *cli,int nxtid,const void *buf,size_t size);
ssize_t nxtnet_cli_recv(nxtnet_cli_t *cli,int nxtid,void *buf,size_t size);
void nxtnet_cli_disconnect(nxtnet_cli_t *cli);

nxtnet_srv_t *nxtnet_srv_create(int port,const char *password,FILE *logfile,int local);
int nxtnet_srv_mainloop(nxtnet_srv_t *srv);
void nxtnet_srv_destroy(nxtnet_srv_t *srv);

#endif
