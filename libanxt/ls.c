/*
    libanxt/ls.c
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
#include <string.h>

#include <anxt/nxt.h>
#include <anxt/ls.h>
#include "private.h"

/**
 * Gets LS status
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @return Bytes ready
 */
ssize_t nxt_ls_status(nxt_t *nxt,int port) {
  if (!NXT_VALID_SENSOR(port)) return NXT_FAIL;
  nxt_pack_start(nxt,0x0E);
  nxt_pack_byte(nxt,port);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,4));
  test(nxt_unpack_start(nxt,0x0E));
  return nxt_unpack_error(nxt)==0?nxt_unpack_byte(nxt):NXT_FAIL;
}

/**
 * Writes LS data
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @param tx How many bytes to send
 *  @param rx How many bytes to receive by next nxt_lsread()
 *  @param data Data to send
 *  @return Success
 */
int nxt_ls_write(nxt_t *nxt,int port,size_t tx,size_t rx,void *data) {
  if (!NXT_VALID_SENSOR(port)) return NXT_FAIL;
  if (tx>16 || rx>16) return NXT_FAIL;
  nxt_pack_start(nxt,0x0F);
  nxt_pack_byte(nxt,port);
  nxt_pack_byte(nxt,tx);
  nxt_pack_byte(nxt,rx);
  nxt_pack_mem(nxt,data,tx);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,3));
  test(nxt_unpack_start(nxt,0x0F));
  return nxt_unpack_error(nxt)==0?NXT_SUCC:NXT_FAIL;
}

/**
 * Reads LS data
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @param bufsize Size of buffer
 *  @param buf Buffer for received data
 *  @return How many bytes read
 */
ssize_t nxt_ls_read(nxt_t *nxt,int port,size_t bufsize,void *buf) {
  if (!NXT_VALID_SENSOR(port)) return NXT_FAIL;
  if (buf==NULL) return NXT_FAIL;
  nxt_pack_start(nxt,0x10);
  nxt_pack_byte(nxt,port);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,20));
  test(nxt_unpack_start(nxt,0x10));
  if (nxt_unpack_error(nxt)==0) {
    ssize_t size = nxt_unpack_byte(nxt);
    void *data = nxt_unpack_mem(nxt,size);
    memcpy(buf,data,bufsize);
    return size<bufsize?size:bufsize;
  }
  else return NXT_FAIL;
}
