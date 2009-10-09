/*
    accel.c
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

#include <stdint.h>

#include <nxt.h>
#include <nxt_i2c/accel.h>

/// I2C Address
int nxt_accel_i2c_addr = 0x02;

/**
 * Returns sensor sensity
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @return Sensity
 */
float nxt_accel_getsensity(nxt_t *nxt,int port) {
  float t[] = {2.5, 3.3, 6.7, 10.0};
  char c;

  nxt_i2c_regr(nxt,port,nxt_accel_i2c_addr,NXT_ACCEL_REG_SENSITY,1,&c);

  if (c>='1' && c<='4') {
    return t[c-'1'];
  }
  else {
    return 0.;
  }
}

/**
 * Sets sensor sensity
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @param sensity Sensity
 *  @return 0 = Success
 *         -1 = Failure
 */
int nxt_accel_setsensity(nxt_t *nxt,int port,float sensity) {
  char c;

  if (sensity==2.5) {
    c = 0;
  }
  else if (sensity==3.3) {
    c = 1;
  }
  else if (sensity==6.7) {
    c = 2;
  }
  else if (sensity==10.0) {
    c = 3;
  }
  else {
    return -1;
  }

  return nxt_accel_cmd(nxt,port,c);
}

/**
 * Returns tilt data
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @param tilt Reference to vector for tilt data
 *  @return 0 = Success
 *         -1 = Failure
 */
int nxt_accel_gettilt(nxt_t *nxt,int port,struct nxt_accel_vector *tilt) {
  char buf[3];

  if (nxt_i2c_regr(nxt,port,nxt_accel_i2c_addr,NXT_ACCEL_REG_TILT,3,buf)!=3) {
    return -1;
  }

  if (tilt!=NULL) {
    tilt->x = buf[0];
    tilt->y = buf[1];
    tilt->z = buf[2];
  }

  return 0;
}

/**
 * Returns acceleration data
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @param accel Reference to vector for accel data
 *  @return Tilt data
 */
int nxt_accel_getaccel(nxt_t *nxt,int port,struct nxt_accel_vector *accel) {
  int16_t buf[3];

  if (nxt_i2c_regr(nxt,port,nxt_accel_i2c_addr,NXT_ACCEL_REG_ACCEL,6,buf)!=6) {
    return -1;
  }

  if (accel!=NULL) {
    accel->x = buf[0];
    accel->y = buf[1];
    accel->z = buf[2];
  }

  return 0;
}
