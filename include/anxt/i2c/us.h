/*
    include/i2c/us.h
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

#define NXT_US_REG_FACTORY_ZERO          0x11
#define NXT_US_REG_FACTORY_SCALE_FACTOR  0x12
#define NXT_US_REG_FACTORY_SCALE_DIVISOR 0x13
#define NXT_US_REG_MEASUREMENT_UNITS     0x14
#define NXT_US_REG_MEASUREMENT_INTERVAL  0x40
#define NXT_US_REG_MEASUREMENT_DATA      0x42
#define NXT_US_REG_ACTUAL_ZERO           0x50
#define NXT_US_REG_ACTUAL_SCALE_FACTOR   0x51
#define NXT_US_REG_ACTUAL_SCALE_DIVISOR  0x52

#define NXT_US_CMD_TURN_OFF         0x00
#define NXT_US_CMD_SINGLE_SHOT      0x01
#define NXT_US_CMD_CONTINUOUS_SHOTS 0x02
#define NXT_US_CMD_EVENT_CAPTURE    0x03
#define NXT_US_CMD_WARM_RESET       0x04

#define NXT_US_MODE_OFF         NXT_US_CMD_TURN_OFF
#define NXT_US_MODE_SINGLE      NXT_US_CMD_SINGLE_SHOT
#define NXT_US_MODE_CONTINUOUS  NXT_US_CMD_CONTINUOUS_SHOTS
#define NXT_US_MODE_EVENT       NXT_US_CMD_EVENT_CAPTURE

#define nxt_us_get_version(nxt,port)  nxt_i2c_get_version(nxt,port,nxt_us_i2c_addr)
#define nxt_us_get_vendorid(nxt,port) nxt_i2c_get_vendorid(nxt,port,nxt_us_i2c_addr)
#define nxt_us_get_deviceid(nxt,port) nxt_i2c_get_deviceid(nxt,port,nxt_us_i2c_addr)
#define nxt_us_cmd(nxt,port,cmd)      nxt_i2c_cmd(nxt,port,nxt_us_i2c_addr,cmd)

int nxt_us_i2c_addr;

int nxt_us_get_measdata(nxt_t *nxt,int port,size_t m1,size_t nm,int *mbuf);
int nxt_us_get_dist(nxt_t *nxt,int port);
int nxt_us_set_meas_interval(nxt_t *nxt, int port, int interval);
int nxt_us_get_meas_interval(nxt_t *nxt, int port);
int nxt_us_set_actual_zero(nxt_t *nxt, int port, int zero);
int nxt_us_get_actual_zero(nxt_t *nxt, int port);
int nxt_us_set_actual_scale_factor(nxt_t *nxt, int port, int scale_factor);
int nxt_us_get_actual_scale_factor(nxt_t *nxt, int port);
int nxt_us_set_actual_scale_divisor(nxt_t *nxt, int port, int scale_divisor);
int nxt_us_get_actual_scale_divisor(nxt_t *nxt, int port);
int nxt_us_get_factory_zero(nxt_t *nxt, int port);
int nxt_us_get_factory_scale_factor(nxt_t *nxt, int port);
int nxt_us_get_factory_scale_divisor(nxt_t *nxt, int port);
const char *nxt_us_get_meas_units(nxt_t *nxt, int port);


static __inline__ int nxt_us_set_mode(nxt_t *nxt, int port, int mode) {
  return nxt_us_cmd(nxt, port, mode);
}

static __inline__ int nxt_us_reset(nxt_t *nxt, int port) {
  return nxt_us_cmd(nxt, port, NXT_US_CMD_WARM_RESET);
}

#endif /* _NXT_I2C_US_H_ */
