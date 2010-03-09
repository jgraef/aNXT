/*
    us.h
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

#ifndef _NXT_I2C_US_H_
#define _NXT_I2C_US_H_

#include <anxt/nxt.h>

#define NXT_US_REG_MEASUREMENT_DATA 0x42

#define nxt_us_get_version(nxt,port)  nxt_i2c_get_version(nxt,port,nxt_us_i2c_addr)
#define nxt_us_get_vendorid(nxt,port) nxt_i2c_get_vendorid(nxt,port,nxt_us_i2c_addr)
#define nxt_us_get_deviceid(nxt,port) nxt_i2c_get_deviceid(nxt,port,nxt_us_i2c_addr)
#define nxt_us_cmd(nxt,port,cmd)     nxt_i2c_cmd(nxt,port,nxt_us_i2c_addr,cmd)

int nxt_us_i2c_addr;

int nxt_us_get_measdata(nxt_t *nxt,int port,size_t m1,size_t nm,int *mbuf);
int nxt_us_get_dist(nxt_t *nxt,int port);

#endif /* _NXT_I2C_US_H_ */
