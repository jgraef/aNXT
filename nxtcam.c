/*
    nxtcam.c
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

#include <nxt.h>
#include <nxt_i2c/nxtcam.h>

#include <stdlib.h>
#include <string.h>

/**
 * Reads version
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @return Version string
 */
char *nxt_nxtcam_getversion(nxt_t *nxt,int port) {
  static char buf[9] = {0x02,0x00,0x03};
  nxt_lsxchg(nxt,port,3,8,buf);
  buf[8] = 0;
  return buf;
}

/**
 * Reads product ID
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @return Product ID
 */
char *nxt_nxtcam_getproductid(nxt_t *nxt,int port) {
  static char buf[9] = {0x02,0x08,0x03};
  nxt_lsxchg(nxt,port,3,8,buf);
  buf[8] = 0;
  return buf;
}

/**
 * Reads sensor type
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @return Sensor Type
 */
char *nxt_nxtcam_getsensortype(nxt_t *nxt,int port) {
  static char buf[9] = {0x02,0x10,0x03};
  nxt_lsxchg(nxt,port,3,8,buf);
  buf[8] = 0;
  return buf;
}

/**
 * Sends a command
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @param cmd Command
 */
void nxt_nxtcam_command(nxt_t *nxt,int port,int cmd) {
  char buf[3] = {0x02,0x41,cmd};
  nxt_lswrite(nxt,port,3,0,buf);
}

/**
 * Checks if cam can see objects
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @return If cam can see objects
 */
int nxt_nxtcam_noobjects(nxt_t *nxt,int port) {
  static char buf[3] = {0x02,0x42,0x03};
  nxt_lsxchg(nxt,port,3,1,buf);
  return buf[0];
}

/**
 * Gets object data
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @param obj Object number
 *  @param object Reference for object data
 *  @return Success
 */
int nxt_nxtcam_getobject(nxt_t *nxt,int port,int obj,nxt_nxtcam_object_t *object) {
  if (!VALID_OBJ(obj)) return NXT_FAIL;
  char sbuf[3] = {0x02,0x43,0x03};
  sbuf[1] += obj*5;
  static char buf[3];
  // color
  memcpy(buf,sbuf,3);
  nxt_lsxchg(nxt,port,3,1,buf);
  object->color = buf[0];
  // ul_x
  sbuf[1]++;
  memcpy(buf,sbuf,3);
  nxt_lsxchg(nxt,port,3,1,buf);
  object->ul_x = buf[0];
  // ul_y
  sbuf[1]++;
  memcpy(buf,sbuf,3);
  nxt_lsxchg(nxt,port,3,1,buf);
  object->ul_y = buf[0];
  // lr_x
  sbuf[1]++;
  memcpy(buf,sbuf,3);
  nxt_lsxchg(nxt,port,3,1,buf);
  object->lr_x = buf[0];
  // lr_y
  sbuf[1]++;
  memcpy(buf,sbuf,3);
  nxt_lsxchg(nxt,port,3,1,buf);
  object->lr_y = buf[0];

  return NXT_SUCC;
}
