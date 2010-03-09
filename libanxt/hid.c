/*
    libanxt/hid.c
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

#include <anxt/nxt.h>
#include <anxt/i2c/hid.h>

/// I2C Address
int nxt_hid_i2c_addr = 0x04;

int nxt_hid_set_modifier(nxt_t *nxt, int port, int modifier) {
  char m = modifier;
  return nxt_i2c_write(nxt, port, nxt_hid_i2c_addr, NXT_HID_REG_MODIFIER, 1, &m)==1?0:-1;
}

int nxt_hid_set_key(nxt_t *nxt, int port, int key) {
  char k = key;
  return nxt_i2c_write(nxt,port, nxt_hid_i2c_addr, NXT_HID_REG_KEYDATA, 1, &k)==1?0:-1;
}

int nxt_hid_send_str(nxt_t *nxt, int port, const char *str) {
  int i;

  nxt_hid_set_mode(nxt, port, NXT_HID_MODE_ASCII);
  nxt_wait_after_communication_command();

  for (i=0; str[i]; i++) {
    nxt_hid_set_key(nxt, port, str[i]);
    nxt_wait_after_communication_command();

    nxt_hid_transmit(nxt, port);
    nxt_wait_after_communication_command();
  }
}
