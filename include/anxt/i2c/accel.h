/*
    accel.h
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

#ifndef _NXT_I2C_ACCEL_H_
#define _NXT_I2C_ACCEL_H_

#include <anxt/nxt.h>

#define NXT_ACCEL_REG_SENSITY 0x19
#define NXT_ACCEL_REG_TILT    0x42
#define NXT_ACCEL_REG_ACCEL   0x45

#define NXT_ACCEL_CMD_ACQUIRE_X    0x58
#define NXT_ACCEL_CMD_ACQUIRE_Y    0x59
#define NXT_ACCEL_CMD_ACQUIRE_Z    0x5A
#define NXT_ACCEL_CMD_ACQUIREEND_X 0x78
#define NXT_ACCEL_CMD_ACQUIREEND_Y 0x79
#define NXT_ACCEL_CMD_ACQUIREEND_Z 0x7A

#define nxt_accel_getversion(nxt,port)  nxt_i2c_get_version(nxt,port,nxt_accel_i2c_addr)
#define nxt_accel_getvendorid(nxt,port) nxt_i2c_get_vendorid(nxt,port,nxt_accel_i2c_addr)
#define nxt_accel_getdeviceid(nxt,port) nxt_i2c_get_deviceid(nxt,port,nxt_accel_i2c_addr)
#define nxt_accel_cmd(nxt,port,cmd)     nxt_i2c_cmd(nxt,port,nxt_accel_i2c_addr,cmd)

struct nxt_accel_vector {
  int x,y,z;
};

int nxt_accel_i2c_addr;

float nxt_accel_get_sensity(nxt_t *nxt,int port);
int nxt_accel_set_sensity(nxt_t *nxt,int port,float sensity);
int nxt_accel_get_tilt(nxt_t *nxt,int port,struct nxt_accel_vector *tilt);
int nxt_accel_get_accel(nxt_t *nxt,int port,struct nxt_accel_vector *accel);

#endif /* _NXT_I2C_ACCEL_H_ */ 
