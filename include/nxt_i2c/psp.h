/*
    psp.h
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

#ifndef _NXT_I2C_PSP_H_
#define _NXT_I2C_PSP_H_

#include <nxt.h>

#define NXT_PSP_REG_MODE      0x41
#define NXT_PSP_REG_BUTTONS   0x42
#define NXT_PSP_REG_JOYSTICKS 0x44

#define NXT_PSP_BTN_LEFT     0x0080
#define NXT_PSP_BTN_DOWN     0x0040
#define NXT_PSP_BTN_RIGHT    0x0020
#define NXT_PSP_BTN_UP       0x0010
#define NXT_PSP_BTN_R3       0x0004
#define NXT_PSP_BTN_L3       0x0002
#define NXT_PSP_BTN_SQUARE   0x8000
#define NXT_PSP_BTN_X        0x4000
#define NXT_PSP_BTN_O        0x2000
#define NXT_PSP_BTN_TRIANGLE 0x1000
#define NXT_PSP_BTN_R1       0x0800
#define NXT_PSP_BTN_L1       0x0400
#define NXT_PSP_BTN_R2       0x0200
#define NXT_PSP_BTN_L2       0x0100

#define NXT_PSP_CMD_SETANALOG  NXT_PSP_MODE_ANALOG
#define NXT_PSP_CMD_SETDIGITAL NXT_PSP_MODE_DIGITAL
#define NXT_PSP_CMD_POWEROFF   0x44
#define NXT_PSP_CMD_POWERON    0x45

#define NXT_PSP_MODE_DIGITAL  0x41
#define NXT_PSP_MODE_ANALOG   0x73

#define NXT_PSP_JOY_LEFT  1
#define NXT_PSP_JOY_RIGHT 2

#define nxt_psp_get_version(nxt,port)  nxt_i2c_get_version(nxt,port,nxt_psp_i2c_addr)
#define nxt_psp_get_vendorid(nxt,port) nxt_i2c_get_vendorid(nxt,port,nxt_psp_i2c_addr)
#define nxt_psp_get_deviceid(nxt,port) nxt_i2c_get_deviceid(nxt,port,nxt_psp_i2c_addr)
#define nxt_psp_cmd(nxt,port,cmd)     nxt_i2c_cmd(nxt,port,nxt_psp_i2c_addr,cmd)

struct nxt_psp_buttons {
  int left,down,up,right;
  int r1,l1,r2,l2,r3,l3;
  int square,x,o,triangle;
};

struct nxt_psp_joystick {
  int x;
  int y;
};

int nxt_psp_i2c_addr;

int nxt_psp_command(nxt_t *nxt,int port,int cmd);
int nxt_psp_get_mode(nxt_t *nxt,int port);
int nxt_psp_get_buttons(nxt_t *nxt,int port,struct nxt_psp_buttons *buttons);
int nxt_psp_get_joystick(nxt_t *nxt,int port,int joy,struct nxt_psp_joystick *jbuf);

#endif /* _NXT_I2C_PSP_H_ */