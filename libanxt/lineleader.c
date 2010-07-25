/*
    libanxt/lineleader.c
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

#include <anxt/nxt.h>
#include <anxt/i2c/lineleader.h>

/// I2C Address
int nxt_line_i2c_addr = 0x02;

int nxt_line_get_reading(nxt_t *nxt, int port, int *values) {
  char v[8];
  int i;

  if (nxt_i2c_read(nxt, port, nxt_line_i2c_addr, NXT_LINE_REG_READING, 8, &v)==8) {
    for (i=0; i<8; i++) {
      values[i] = v[i];
    }
  }
  else {
    return -1;
  }
}

int nxt_line_get_steering(nxt_t *nxt, int port) {
  char s;

  if (nxt_i2c_read(nxt, port, nxt_line_i2c_addr, NXT_LINE_REG_STEERING, 1, &s)==1) {
    return s;
  }
  else {
    return -1;
  }
}

int nxt_line_get_average(nxt_t *nxt, int port) {
  char a;

  if (nxt_i2c_read(nxt, port, nxt_line_i2c_addr, NXT_LINE_REG_AVERAGE, 1, &a)==1) {
    return a;
  }
  else {
    return -1;
  }
}

int nxt_line_get_result(nxt_t *nxt, int port) {
  char r;

  if (nxt_i2c_read(nxt, port, nxt_line_i2c_addr, NXT_LINE_REG_RESULT, 1, &r)==1) {
    return r;
  }
  else {
    return -1;
  }
}

int nxt_line_get_setpoint(nxt_t *nxt, int port) {
  char s;

  if (nxt_i2c_read(nxt, port, nxt_line_i2c_addr, NXT_LINE_REG_SETPOINT, 1, &s)==1) {
    return s;
  }
  else {
    return -1;
  }
}

int nxt_line_get_pid(nxt_t *nxt, int port, int *Kp, int *Ki, int *Kd) {
  char pid[3];

  if (nxt_i2c_read(nxt, port, nxt_line_i2c_addr, NXT_LINE_REG_KP, 3, &pid)==3) {
    if (Kp!=NULL) {
      *Kp = pid[0];
    }
    if (Ki!=NULL) {
      *Ki = pid[1];
    }
    if (Kd!=NULL) {
      *Kd = pid[2];
    }
    return 0;
  }
  else {
    return -1;
  }
}

int nxt_line_get_divisor(nxt_t *nxt, int port, int *Kpd, int *Kid, int *Kdd) {
  char divisors[3];

  if (nxt_i2c_read(nxt, port, nxt_line_i2c_addr, NXT_LINE_REG_KP_DIVISOR, 3, &divisors)==3) {
    if (Kpd!=NULL) {
      *Kpd = divisors[0];
    }
    if (Kid!=NULL) {
      *Kid = divisors[1];
    }
    if (Kdd!=NULL) {
      *Kdd = divisors[2];
    }
    return 0;
  }
  else {
    return -1;
  }
}

void nxt_line_calibrate(nxt_t *nxt, int port, int white_black) {
  nxt_line_cmd(nxt, port, white_black);
}

void nxt_line_sleep(nxt_t *nxt, int port) {
  nxt_line_cmd(nxt, port, NXT_LINE_CMD_SLEEP);
}

void nxt_line_wakeup(nxt_t *nxt, int port) {
  nxt_line_cmd(nxt, port, NXT_LINE_CMD_WAKEUP);
}

void nxt_line_invert(nxt_t *nxt, int port, int invert) {
  nxt_line_cmd(nxt, port, invert?NXT_LINE_CMD_SET_INVERSION:NXT_LINE_CMD_RESET_INVERSION);
}

void nxt_line_snapshot(nxt_t *nxt, int port) {
  nxt_line_cmd(nxt, port, NXT_LINE_CMD_SNAPSHOT);
}

void nxt_line_config(nxt_t *nxt, int port, int mode) {
  nxt_line_cmd(nxt, port, mode);
}
