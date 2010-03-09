/*
    libanxt_net/net.c
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
#include <stdint.h>

#include <anxt/net.h>

/**
 * Receives NXTNET packet
 *  @param sock Socket
 *  @param buf Buffer
 *  @param cmd Command byte to check for (0 for all)
 *  @return Packet received
 */
struct nxtnet_proto_packet *nxtnet_recv(int sock,struct nxtnet_proto_packet *buf,int cmd) {
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
ssize_t nxtnet_send(int sock,struct nxtnet_proto_packet *buf) {
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
