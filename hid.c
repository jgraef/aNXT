/*
    hid.c
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
#include <nxt_i2c/hid.h>

/// I2C Address
int nxt_hid_i2c_addr = 0x02;

int nxt_hid_transmit(nxt_t *nxt,int port) {
  return nxt_i2c_cmd(nxt,port,nxt_hid_i2c_addr,NXT_HID_CMD_TRANSMIT);
}

int nxt_hid_setmode(nxt_t *nxt,int port,int mode) {
  return nxt_i2c_cmd(nxt,port,nxt_hid_i2c_addr,mode);
}

int nxt_hid_setmodifier(nxt_t *nxt,int port,int modifier) {
  char m = modifier;
  return nxt_i2c_regw(nxt,port,nxt_hid_i2c_addr,NXT_HID_REG_MODIFIER,1,&m);
}

int nxt_hid_setkey(nxt_t *nxt,int port,int key) {
  char k = key;
  return nxt_i2c_regw(nxt,port,nxt_hid_i2c_addr,NXT_HID_REG_KEYDATA,1,&k);
}
