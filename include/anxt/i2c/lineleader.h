/*
    lineleader.h
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

  NOTE: Not tested yet! I don't have the sensor
*/

#ifndef _NXT_I2C_LINELEADER_H_
#define _NXT_I2C_LINELEADER_H_

#include <anxt/nxt.h>

#define NXT_LINE_CMD_CALIBRATE_WHITE  'W'
#define NXT_LINE_CMD_CALIBRATE_BLACK  'B'
#define NXT_LINE_CMD_SLEEP            'D'
#define NXT_LINE_CMD_WAKEUP           'P'
#define NXT_LINE_CMD_SET_INVERSION    'I'
#define NXT_LINE_CMD_RESET_INVERSION  'R'
#define NXT_LINE_CMD_SNAPSHOT         'S'
#define NXT_LINE_CMD_CONFIG_US        'A'
#define NXT_LINE_CMD_CONFIG_EU        'E'
#define NXT_LINE_CMD_CONFIG_UNIVERSAL 'U'

#define NXT_LINE_REG_READING          0x49
#define NXT_LINE_REG_STEERING         0x42
#define NXT_LINE_REG_AVERAGE          0x43
#define NXT_LINE_REG_RESULT           0x44
#define NXT_LINE_REG_SETPOINT         0x45
#define NXT_LINE_REG_KP               0x46
#define NXT_LINE_REG_KI               0x47
#define NXT_LINE_REG_KD               0x48
#define NXT_LINE_REG_KP_DIVISOR       0x61
#define NXT_LINE_REG_KI_DIVISOR       0x62
#define NXT_LINE_REG_KD_DIVISOR       0x63

#define NXT_LINE_CALIBRATE_WHITE NXT_LINE_CMD_CALIBRATE_WHITE
#define NXT_LINE_CALIBRATE_BLACK NXT_LINE_CMD_CALIBRATE_BLACK

#define NXT_LINE_CONFIG_US        NXT_LINE_CMD_CONFIG_US
#define NXT_LINE_CONFIG_EU        NXT_LINE_CMD_CONFIG_EU
#define NXT_LINE_CONFIG_UNIVERSAL NXT_LINE_CMD_CONFIG_UNIVERSAL

#define nxt_line_get_version(nxt, port)  nxt_i2c_get_version(nxt, port, nxt_line_i2c_addr)
#define nxt_line_get_vendorid(nxt, port) nxt_i2c_get_vendorid(nxt, port, nxt_line_i2c_addr)
#define nxt_line_get_deviceid(nxt, port) nxt_i2c_get_deviceid(nxt, port, nxt_line_i2c_addr)
#define nxt_line_cmd(nxt, port, cmd)      nxt_i2c_cmd(nxt, port, nxt_line_i2c_addr, cmd)

int nxt_line_i2c_addr;

int nxt_line_get_reading(nxt_t *nxt, int port, int *values);
int nxt_line_get_steering(nxt_t *nxt, int port);
int nxt_line_get_average(nxt_t *nxt, int port);
int nxt_line_get_result(nxt_t *nxt, int port);
int nxt_line_get_setpoint(nxt_t *nxt, int port);
int nxt_line_get_pid(nxt_t *nxt, int port, int *Kp, int *Ki, int *Kd);
int nxt_line_get_divisor(nxt_t *nxt, int port, int *Kpd, int *Kid, int *Kdd);
void nxt_line_calibrate(nxt_t *nxt, int port, int white_black);
void nxt_line_sleep(nxt_t *nxt, int port);
void nxt_line_wakeup(nxt_t *nxt, int port);
void nxt_line_invert(nxt_t *nxt, int port, int invert);
void nxt_line_snapshot(nxt_t *nxt, int port);
void nxt_line_config(nxt_t *nxt, int port, int mode);

#endif /* _NXT_I2C_LINELEADER_H_ */
