/*
    libanxt/i2c.c
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
#include <anxt/i2c.h>
#include <anxt/ls.h>

/**
 * Read I2C register (lowlevel)
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @param addr I2C address
 *  @param reg1 First register
 *  @param nreg Number of registers (max 16)
 *  @param buf Buffer for read data
 *  @return How many bytes read
 */
static ssize_t nxt_i2c_read_low(nxt_t *nxt,int port,int addr,size_t reg1,size_t nreg,void *buf) {
  time_t timeout;
  char cmd[3] = {
    addr,
    reg1
  };

  // NOTE Don't know why it works with 15 bytes at max. From spec. it should work with 16 bytes.
  if (nreg>15) {
    nreg = 15;
  }

  nxt_ls_write(nxt,port,2,nreg,cmd);

  // wait for data
  timeout = time(NULL)+2;
  while (nxt_ls_status(nxt,port)<((ssize_t)nreg) && time(NULL)<timeout);

  if (nxt_ls_read(nxt,port,nreg,buf)==-1) {
    return -1;
  }
  else {
    return nreg;
  }
}

/**
 * Read I2C register
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @param addr I2C address
 *  @param reg1 First register
 *  @param nreg Number of registers
 *  @param buf Buffer for read data
 *  @return How many bytes read
 */
ssize_t nxt_i2c_read(nxt_t *nxt, int port, int addr, size_t reg1, size_t nreg, void *buf) {
  size_t n, c;

  n = nreg;

  while (n>0) {
    c = nxt_i2c_read_low(nxt, port, addr, reg1, n, buf);

    if (c==-1) {
      return -1;
    }

    n -= c;
    reg1 += c;
    buf += c;
  }

  return nreg;
}

/**
 * Write I2C register (lowlevel)
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @param addr I2C address
 *  @param reg1 First register
 *  @param nreg Number of registers (max 14)
 *  @param buf Data to write to registers
 *  @return How many bytes written
 */
static ssize_t nxt_i2c_write_low(nxt_t *nxt,int port,int addr,size_t reg1,size_t nreg,void *buf) {
  char cmd[16] = {
    addr,
    reg1
  };

  if (nreg>14) {
    nreg = 14;
  }

  memcpy(cmd+2,buf,nreg);
  if (nxt_ls_write(nxt,port,nreg+2,0,cmd)==-1) {
    return -1;
  }
  else {
    return nreg;
  }
}

/**
 * Write I2C register
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @param addr I2C address
 *  @param reg1 First register
 *  @param nreg Number of registers
 *  @param buf Data to write to registers
 *  @return How many bytes written
 */
ssize_t nxt_i2c_write(nxt_t *nxt, int port, int addr, size_t reg1, size_t nreg, void *buf) {
  size_t n, c;

  n = nreg;

  while (n>0) {
    c = nxt_i2c_write_low(nxt, port, addr, reg1, n, buf);
    if (c==-1) {
      return -1;
    }

    n -= c;
    reg1 += c;
    buf += c;
  }

  return nreg;
}

/**
 * Sends command to I2C sensor
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @param addr I2C address
 *  @param cmd Command
 */
int nxt_i2c_cmd(nxt_t *nxt,int port,int addr,int cmd) {
  char buf[3] = {
    addr,
    NXT_I2C_REG_CMD,
    cmd
  };

  return nxt_ls_write(nxt,port,3,0,buf);
}

/**
 * Changes I2C address
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @param addr I2C address
 *  @param newaddr New I2C address
 *  @note Only changes hardware I2C address.
 */
int nxt_i2c_set_i2caddr(nxt_t *nxt, int port, int addr, int newaddr) {
  char buf[] = { 0xA0, 0xAA, 0xA5, newaddr };
  int i;

  for (i=0;i<4;i++) {
    if (nxt_i2c_cmd(nxt, port, addr, buf[i])==-1) {
      return -1;
    }
    usleep(NXT_COMMUNICATION_COMMAND_LATENCY);
  }

  return 0;
}

/**
 * Gets version from I2C sensor
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @param addr I2C address
 *  @return Version
 */
const char *nxt_i2c_get_version(nxt_t *nxt,int port,int addr) {
  static char buf[8];

  if (nxt_i2c_read(nxt,port,addr,NXT_I2C_REG_VERSION,8,buf)==8) {
    buf[7] = 0;
    return buf;
  }
  else {
    return NULL;
  }
}

/**
 * Gets vendor ID from I2C sensor
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @param addr I2C address
 *  @return Vendor ID
 */
const char *nxt_i2c_get_vendorid(nxt_t *nxt,int port,int addr) {
  static char buf[8];

  if (nxt_i2c_read(nxt,port,addr,NXT_I2C_REG_VENDORID,8,buf)==8) {
    buf[7] = 0;
    return buf;
  }
  else {
    return NULL;
  }
}

/**
 * Gets device ID from I2C sensor
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @param addr I2C address
 *  @return Device ID
 */
const char *nxt_i2c_get_deviceid(nxt_t *nxt,int port,int addr) {
  static char buf[8];

  if (nxt_i2c_read(nxt,port,addr,NXT_I2C_REG_DEVICEID,8,buf)==8) {
    buf[7] = 0;
    return buf;
  }
  else {
    return NULL;
  }
}

