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

/// Max. number of sockets
#define NXTNET_SRV_MAXSOCKS   256
/// Backlog size for listen
#define NXTNET_SRV_LISTEN_MAX 32
/// Send/Recv buffer size
#define NXTNET_BUFSIZE        128
/// Password length
#define NXTNET_PWD_LEN        16
/// Max. length of NXT name
#define NXTNET_NXTNAME_LEN    16

/// Packet signature
/// @note If you read these 4 bytes as string you get "NXT\0"
#define NXTNET_PROTO_SIG         ('N'|('X'<<8)|('T'<<16))
/// Packet direction - Client to server - or'd with command
#define NXTNET_PROTO_DIR_CS      0x00
/// Packet direction - Server to client - or'd with command
#define NXTNET_PROTO_DIR_SC      0x80
/// Packet direction - Mask to get packet direction from command byte
#define NXTNET_PROTO_DIR_MASK    0x80
/// Packet command - Hello
#define NXTNET_PROTO_CMD_HELLO   0x01
/// Packet command - List NXTs
#define NXTNET_PROTO_CMD_LIST    0x02
/// Packet command - Send data to NXT
#define NXTNET_PROTO_CMD_SEND    0x03
/// Packet command - Recv data from NXT
#define NXTNET_PROTO_CMD_RECV    0x04

/// Error - No error
#define NXTNET_ERROR_NOERROR 0
/// Error - Wrong password
#define NXTNET_ERROR_WROPWD  1
/// Error - Function not implemented
#define NXTNET_ERROR_NOTIMPL 2

/// Default TCP port
///  @note Changed to a private/dynamic port.
///  @todo Assign a correct port: http://www.iana.org/cgi-bin/usr-port-number.pl
#define NXTNET_DEFAULT_PORT 51337

/// Timeout in select() in seconds
#define NXTNET_SELECT_TIMEOUT 10

/// Packet
struct nxtnet_proto_packet {
  /// Signature (must be NXTNET_PROTO_SIG)
  uint32_t sig;
  /// Command
  uint8_t cmd;
  /// Size of whole packet
  uint16_t size;
  /// Error code
  uint8_t error;
  /// Password
  char password[NXTNET_PWD_LEN];
  /// Data
  char data[0];
} __attribute__ ((packed));

/// List item for LIST's NXT list
struct nxtnet_proto_list_nxts {
  /// NXT's handle
  uint32_t handle;
  /// NXT's name
  char name[NXTNET_NXTNAME_LEN];
  /// NXT's ID/bluetooth address
  uint8_t id[6];
  /// If NXT is connected via BT
  int is_bt;
} __attribute__ ((packed)) ;

/// Server to client data for LIST command
struct nxtnet_proto_list_sc {
  /// Number of items in 'nxts'
  uint32_t num_items;
  /// NXT list
  struct nxtnet_proto_list_nxts nxts[0];
} __attribute__ ((packed));

/// Client to server data for SEND command
struct nxtnet_proto_send_cs {
  /// NXT ID
  uint32_t handle;
  /// How many bytes to send
  uint32_t size;
  /// Data to send
  char data[0];
} __attribute__ ((packed));

/// Server to client data for SEND command
struct nxtnet_proto_send_sc {
  /// NXT ID
  uint32_t handle;
  /// How many bytes sent
  uint32_t size;
} __attribute__ ((packed));

/// Client to server data for RECV command
struct nxtnet_proto_recv_cs {
  /// NXT ID
  uint32_t handle;
  /// How many bytes to receive
  uint32_t size;
} __attribute__ ((packed));

/// Server to client data for RECV command
struct nxtnet_proto_recv_sc {
  /// NXT ID
  uint32_t handle;
  /// How many bytes received
  uint32_t size;
  /// Received data
  char data[0];
} __attribute__ ((packed));

/// Descriptor for client's network connection
typedef struct {
  /// Socket
  int sock;
  /// Send/Recv buffer
  struct nxtnet_proto_packet *buf;
  /// Password
  char password[NXTNET_PWD_LEN];
} nxtnet_cli_t;

/// Descriptor for server's network connection
typedef struct {
  /// Server socket
  int sock;
  /// If server is only local
  int local;
  /// Operations
  struct {
    /**
     * Lists all NXTs
     *  @param packer Packer function. Call this to add NXT to list
     */
    void (*list)(void (*packer)(int handle, char *name, void *id, int is_bt));
    /**
     * Sends data to NXT
     *  @param handle NXT handle
     *  @param buf Data to send
     *  @param size How many bytes to send
     *  @return How many bytes sent
     */
    ssize_t (*send)(int handle,const void *buf,size_t size);
    /**
     * Receives data from NXT
     *  @param handle NXT handle
     *  @param buf Buffer for received data
     *  @param size How many bytes to receive
     *  @return How many bytes received
     */
    ssize_t (*recv)(int handle,void *buf,size_t size);
  } ops;
  /// Client sockets
  int clients[NXTNET_SRV_MAXSOCKS];
  /// Log file descriptor
  FILE *log;
  /// Password
  char password[NXTNET_PWD_LEN];
  /// Send/Recv buffer
  struct nxtnet_proto_packet *buf;
} nxtnet_srv_t;

nxtnet_cli_t *nxtnet_cli_connect(const char *hostname,int port,const char *password);
struct nxtnet_proto_list_sc *nxtnet_cli_list(nxtnet_cli_t *cli);
ssize_t nxtnet_cli_send(nxtnet_cli_t *cli,int handle,const void *buf,size_t size);
ssize_t nxtnet_cli_recv(nxtnet_cli_t *cli,int handle,void *buf,size_t size);
void nxtnet_cli_disconnect(nxtnet_cli_t *cli);

nxtnet_srv_t *nxtnet_srv_create(int port,const char *password,FILE *logfile,int local);
int nxtnet_srv_mainloop(nxtnet_srv_t *srv);
void nxtnet_srv_destroy(nxtnet_srv_t *srv);

#endif /* _NXTNET_H_ */
