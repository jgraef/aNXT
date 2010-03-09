/*
    include/anxt/i2c.h
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

#ifndef _ANXT_I2C_H_
#define _ANXT_I2C_H_

#include <sys/types.h>

#include <anxt/nxt.h>

// Common I2C registers
#define NXT_I2C_REG_VERSION  0x00
#define NXT_I2C_REG_VENDORID 0x08
#define NXT_I2C_REG_DEVICEID 0x10
#define NXT_I2C_REG_CMD      0x41

ssize_t nxt_i2c_read(nxt_t *nxt,int port,int addr,size_t reg1,size_t nreg,void *buf);
ssize_t nxt_i2c_write(nxt_t *nxt,int port,int addr,size_t reg1,size_t nreg,void *buf);
int nxt_i2c_cmd(nxt_t *nxt,int port,int addr,int cmd);
int nxt_i2c_set_i2caddr(nxt_t *nxt, int port, int addr, int newaddr);
const char *nxt_i2c_get_version(nxt_t *nxt,int port,int addr);
const char *nxt_i2c_get_vendorid(nxt_t *nxt,int port,int addr);
const char *nxt_i2c_get_deviceid(nxt_t *nxt,int port,int addr);


#endif /* _ANXT_I2C_H_ */
