/*
    libanxt/us.c
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

#include <stddef.h>

#include <anxt/nxt.h>
#include <anxt/i2c.h>
#include <anxt/i2c/us.h>

int nxt_us_i2c_addr = 0x02;

int nxt_us_get_meas_data(nxt_t *nxt,int port,size_t m1,size_t nm,int *mbuf) {
  char buf[8];
  int i;

  if (m1+nm>8) {
    return -1;
  }

  if (nxt_i2c_read(nxt,port,nxt_us_i2c_addr,NXT_US_REG_MEASUREMENT_DATA+m1,nm,buf)==nm) {
    for (i=0;i<nm;i++) {
      mbuf[i] = buf[i]&0xFF;
    }

    return nm;
  }
  else {
    return -1;
  }
}

int nxt_us_get_dist(nxt_t *nxt,int port) {
  int dist;

  if (nxt_us_get_meas_data(nxt, port, 0, 1, &dist)==1) {
    return dist;
  }
  else {
    return -1;
  }
}

int nxt_us_set_meas_interval(nxt_t *nxt, int port, int interval) {
  char i = interval;
  return nxt_i2c_write(nxt, port, nxt_us_i2c_addr, NXT_US_REG_MEASUREMENT_INTERVAL, 1, &i)==1?NXT_SUCC:NXT_FAIL;
}

int nxt_us_get_meas_interval(nxt_t *nxt, int port) {
  char i;

  if (nxt_i2c_read(nxt, port, nxt_us_i2c_addr, NXT_US_REG_MEASUREMENT_INTERVAL, 1, &i)==-1) {
    return -1;
  }

  return i;
}

int nxt_us_set_actual_zero(nxt_t *nxt, int port, int zero) {
  char z = zero;
  return nxt_i2c_write(nxt, port, nxt_us_i2c_addr, NXT_US_REG_ACTUAL_ZERO, 1, &z)==1?NXT_SUCC:NXT_FAIL;
}

int nxt_us_get_actual_zero(nxt_t *nxt, int port) {
  char i;

  if (nxt_i2c_read(nxt, port, nxt_us_i2c_addr, NXT_US_REG_ACTUAL_ZERO, 1, &i)==-1) {
    return -1;
  }

  return i;
}

int nxt_us_set_actual_scale_factor(nxt_t *nxt, int port, int scale_factor) {
  char sf = scale_factor;
  return nxt_i2c_write(nxt, port, nxt_us_i2c_addr, NXT_US_REG_ACTUAL_SCALE_FACTOR, 1, &sf)==1?NXT_SUCC:NXT_FAIL;
}

int nxt_us_get_actual_scale_factor(nxt_t *nxt, int port) {
  char sf;

  if (nxt_i2c_read(nxt, port, nxt_us_i2c_addr, NXT_US_REG_ACTUAL_SCALE_FACTOR, 1, &sf)==-1) {
    return -1;
  }

  return sf;
}

int nxt_us_set_actual_scale_divisor(nxt_t *nxt, int port, int scale_divisor) {
  char sd = scale_divisor;
  return nxt_i2c_write(nxt, port, nxt_us_i2c_addr, NXT_US_REG_MEASUREMENT_INTERVAL, 1, &sd)==1?NXT_SUCC:NXT_FAIL;
}

int nxt_us_get_actual_scale_divisor(nxt_t *nxt, int port) {
  char sd;

  if (nxt_i2c_read(nxt, port, nxt_us_i2c_addr, NXT_US_REG_ACTUAL_SCALE_DIVISOR, 1, &sd)==-1) {
    return -1;
  }

  return sd;
}

int nxt_us_get_factory_zero(nxt_t *nxt, int port) {
  char z;

  if (nxt_i2c_read(nxt, port, nxt_us_i2c_addr, NXT_US_REG_FACTORY_ZERO, 1, &z)==-1) {
    return -1;
  }

  return z;
}

int nxt_us_get_factory_scale_factor(nxt_t *nxt, int port) {
  char sf;

  if (nxt_i2c_read(nxt, port, nxt_us_i2c_addr, NXT_US_REG_FACTORY_SCALE_FACTOR, 1, &sf)==-1) {
    return -1;
  }

  return sf;
}

int nxt_us_get_factory_scale_divisor(nxt_t *nxt, int port) {
  char sd;

  if (nxt_i2c_read(nxt, port, nxt_us_i2c_addr, NXT_US_REG_FACTORY_SCALE_DIVISOR, 1, &sd)==-1) {
    return -1;
  }

  return sd;
}

const char *nxt_us_get_meas_units(nxt_t *nxt, int port) {
  static char mu[7];

  if (nxt_i2c_read(nxt, port, nxt_us_i2c_addr, NXT_US_REG_MEASUREMENT_UNITS, 7, mu)==-1) {
    return NULL;
  }

  return mu;
}
