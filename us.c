/*
    us.c
    aNXT - a NXt Toolkit
    Libraries and tools for LEGO Mindstorms NXT robots
    Copyright (C) 2008  Janosch Gräf <janosch.graef@gmx.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <nxt.h>
#include <nxt_i2c/us.h>

/**
 * Reads version
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @return Version string
 */
char *nxt_us_getversion(nxt_t *nxt,int port) {
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
char *nxt_us_getproductid(nxt_t *nxt,int port) {
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
char *nxt_us_getsensortype(nxt_t *nxt,int port) {
  static char buf[9] = {0x02,0x10,0x03};
  nxt_lsxchg(nxt,port,3,8,buf);
  buf[8] = 0;
  return buf;
}

/**
 * Reads factory zero (Cal 1)
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @return Factory zero
 */
int nxt_us_getfactoryzero(nxt_t *nxt,int port) {
  static char buf[3] = {0x02,0x11,0x03};
  nxt_lsxchg(nxt,port,3,1,buf);
  return buf[0];
}

/**
 * Reads factory scale factor (Cal 2)
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @return Factory scale factor
 */
int nxt_us_getfactoryscalefactor(nxt_t *nxt,int port) {
  static char buf[3] = {0x02,0x12,0x03};
  nxt_lsxchg(nxt,port,3,1,buf);
  return buf[0];
}

/**
 * Reads factory scale divisor
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @return Factory Scale devisor
 */
int nxt_us_getfactoryscaledivisor(nxt_t *nxt,int port) {
  static char buf[3] = {0x02,0x13,0x03};
  nxt_lsxchg(nxt,port,3,1,buf);
  return buf[0];
}

/**
 * Reads measurement units
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @return Measurement Units
 */
char *nxt_us_getmeasunits(nxt_t *nxt,int port) {
  static char buf[8] = {0x02,0x14,0x03};
  nxt_lsxchg(nxt,port,3,7,buf);
  buf[7] = 0;
  return buf;
}

/**
 * Reads continuous measurements interval
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @return Interval
 */
int nxt_us_getcontmeasinterval(nxt_t *nxt,int port) {
  static char buf[3] = {0x02,0x40,0x03};
  nxt_lsxchg(nxt,port,3,1,buf);
  return buf[0];
}

/**
 * Reads command status
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @return Command status
 */
int nxt_us_getcmdstate(nxt_t *nxt,int port) {
  static char buf[3] = {0x02,0x41,0x03};
  nxt_lsxchg(nxt,port,3,1,buf);
  return buf[0];
}

/**
 * Reads Measurement byte
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @param byte Byte number
 *  @return Value
 */
int nxt_us_getmeasbyte(nxt_t *nxt,int port,int byte) {
  if (byte<0 || byte>7) return 0;
  static char buf[3] = {0x02,0x42,0x03};
  buf[2] += byte;
  nxt_lsxchg(nxt,port,2,1,buf);
  return (unsigned char)buf[0];
}

/**
 * Reads actual zero
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @return Actual zero
 */
int nxt_us_getactualzero(nxt_t *nxt,int port) {
  static char buf[3] = {0x02,0x50,0x03};
  nxt_lsxchg(nxt,port,3,1,buf);
  return buf[0];
}

/**
 * Reads actual scale factor
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @return Actual scale factor
 */
int nxt_us_getactualscalefactor(nxt_t *nxt,int port) {
  static char buf[3] = {0x02,0x51,0x03};
  nxt_lsxchg(nxt,port,3,1,buf);
  return buf[0];
}

/**
 * Reads actual scale divisor
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @return Actual scale divisor
 */
int nxt_us_getactualscaledivisor(nxt_t *nxt,int port) {
  static char buf[3] = {0x02,0x52,0x03};
  nxt_lsxchg(nxt,port,3,1,buf);
  return buf[0];
}

/**
 * Turns US sensor off
 *  @param nxt NXT handle
 *  @param port Sensor port
 */
void nxt_us_off(nxt_t *nxt,int port) {
  char buf[3] = {0x02,0x41,0x00};
  nxt_lswrite(nxt,port,3,0,buf);
}

/**
 * Makes a single measurement
 *  @param nxt NXT handle
 *  @param port Sensor port
 */
void nxt_us_singleshot(nxt_t *nxt,int port) {
  char buf[3] = {0x02,0x41,0x01};
  nxt_lswrite(nxt,port,3,0,buf);
}

/**
 * Make continnuous measurements (default)
 *  @param nxt NXT handle
 *  @param port Sensor port
 */
void nxt_us_contmeas(nxt_t *nxt,int port) {
  char buf[3] = {0x02,0x41,0x02};
  nxt_lswrite(nxt,port,3,0,buf);
}

/**
 * Makes an event measurement
 *  @param nxt NXT handle
 *  @param port Sensor port
 */
void nxt_us_eventmeas(nxt_t *nxt,int port) {
  char buf[3] = {0x02,0x41,0x03};
  nxt_lswrite(nxt,port,3,0,buf);
}

/**
 * Does a warm reset
 *  @param nxt NXT handle
 *  @param port Sensor port
 */
void nxt_us_warmreset(nxt_t *nxt,int port) {
  char buf[3] = {0x02,0x41,0x04};
  nxt_lswrite(nxt,port,3,0,buf);
}

/**
 * Sets continuous measurement inreval
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @param interval Interval
 */
void nxt_us_setcontmeasinterval(nxt_t *nxt,int port,int interval) {
  char buf[3] = {0x02,0x40,interval};
  nxt_lswrite(nxt,port,3,0,buf);
}

/**
 * Sets actual zero
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @param zero Actual zero
 */
void nxt_us_setactualzero(nxt_t *nxt,int port,int zero) {
  char buf[3] = {0x02,0x50,zero};
  nxt_lswrite(nxt,port,3,0,buf);
}

/**
 * Sets actual scale factor
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @param factor Actual scale factor
 */
void nxt_us_setactualscalefactor(nxt_t *nxt,int port,int factor) {
  char buf[3] = {0x02,0x51,factor};
  nxt_lswrite(nxt,port,3,0,buf);
}

/**
 * Sets actual scale divisor
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @param divisor Actual scale divisor
 */
void nxt_us_setactualscaledivisor(nxt_t *nxt,int port,int divisor) {
  char buf[3] = {0x02,0x51,divisor};
  nxt_lswrite(nxt,port,3,0,buf);
}
