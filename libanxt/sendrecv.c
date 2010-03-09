/*
    libanxt/sendrecv.c
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
#include <stdint.h>
#include <string.h>

#include <anxt/nxt.h>

#include "private.h"

/**
 * Send data
 *  @param nxt NXT handle
 *  @return How many bytes sent
 */
ssize_t nxt_con_send(nxt_t *nxt) {
  ssize_t ret = nxtnet_cli_send(nxt->cli, nxt->handle, nxt->buffer, nxt->ptr-nxt->buffer);
  if (ret==-1) nxt->error = NXT_ERR_CONNECTION;
  return ret;
}

/**
 * Receive data
 *  @param nxt NXT handle
 *  @param size How many bytes to receive
 *  @return How many bytes received
 */
ssize_t nxt_con_recv(nxt_t *nxt,size_t size) {
  ssize_t ret = nxtnet_cli_recv(nxt->cli, nxt->handle, nxt->buffer, size);
  if (ret==-1) nxt->error = NXT_ERR_CONNECTION;
  return ret;
}

/// Functions for packing packages
void nxt_pack_byte(nxt_t *nxt,uint8_t val) {
  *(nxt->ptr)++ = val;
}

void nxt_pack_word(nxt_t *nxt,uint16_t val) {
  set_word(nxt->ptr,val);
  nxt->ptr += 2;
}

void nxt_pack_dword(nxt_t *nxt,uint32_t val) {
  set_dword(nxt->ptr,val);
  nxt->ptr += 4;
}

void nxt_pack_start(nxt_t *nxt,nxt_cmd_t cmd) {
  nxt->buffer[0] = cmd<0x80?NXT_TYPE_DIRECT_RESP:NXT_TYPE_SYSTEM_RESP;
  nxt->buffer[1] = cmd;
  nxt->ptr = nxt->buffer+2;
}

void nxt_pack_mem(nxt_t *nxt,void *buf,size_t len) {
  memcpy(nxt->ptr,buf,len);
  nxt->ptr += len;
}

void nxt_pack_str(nxt_t *nxt,const char *str,size_t maxlen) {
  strncpy(nxt->ptr,str,maxlen);
  nxt->ptr += maxlen;
}

/// Functions for unpacking packages
uint8_t nxt_unpack_byte(nxt_t *nxt) {
  return *(nxt->ptr)++;
}

uint16_t nxt_unpack_word(nxt_t *nxt) {
  uint16_t ret = get_word(nxt->ptr);
  nxt->ptr += 2;
  return ret;
}

uint32_t nxt_unpack_dword(nxt_t *nxt) {
  uint32_t ret = get_dword(nxt->ptr);
  nxt->ptr += 4;
  return ret;
}

int nxt_unpack_start(nxt_t *nxt,nxt_cmd_t cmd) {
  if ((nxt->buffer[0]&0xFF)!=NXT_TYPE_REPLY || (nxt->buffer[1]&0xFF)!=cmd) return NXT_FAIL;
  else {
    nxt->ptr = nxt->buffer+2;
    return NXT_SUCC;
  }
}

int nxt_unpack_error(nxt_t *nxt) {
  int error = (*(nxt->ptr)++)&0xFF;
  if (error!=0) nxt->error = error;
  return error;
}

void *nxt_unpack_mem(nxt_t *nxt,size_t len) {
  void *ret = nxt->ptr;
  nxt->ptr += len;
  return ret;
}

void *nxt_unpack_str(nxt_t *nxt,size_t len) {
  char *ret = nxt->ptr;
  ret[len-1] = 0;
  nxt->ptr += len;
  return ret;
}
