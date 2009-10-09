/*
    psp.c
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

#include <nxt.h>
#include <nxt_i2c/psp.h>

/// I2C Address
int nxt_psp_i2c_addr = 0x02;

/**
 * Returns mode
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @return Controller mode
 *  @todo Doesn't work yet
 */
int nxt_psp_get_mode(nxt_t *nxt,int port) {
  char mode;

  if (nxt_i2c_regr(nxt,port,nxt_psp_i2c_addr,NXT_PSP_REG_MODE,1,&mode)==1) {
    return mode;
  }
  else {
    return -1;
  }
}

/**
 * Returns button states
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @param buttons Reference to structure for button states
 *  @return 0 = Success
 *         -1 = Failure
 */
int nxt_psp_getbuttons(nxt_t *nxt,int port,struct nxt_psp_buttons *buttons) {
  uint16_t buf;

  int ret = nxt_i2c_regr(nxt,port,nxt_psp_i2c_addr,NXT_PSP_REG_BUTTONS,2,&buf);
  if (ret==2) {
    buf = ~buf;

    if (buttons!=NULL) {
      buttons->left = buf&NXT_PSP_BTN_LEFT;
      buttons->down = buf&NXT_PSP_BTN_DOWN;
      buttons->up = buf&NXT_PSP_BTN_UP;
      buttons->right = buf&NXT_PSP_BTN_RIGHT;
      buttons->r3 = buf&NXT_PSP_BTN_R3;
      buttons->l3 = buf&NXT_PSP_BTN_L3;
      buttons->square = buf&NXT_PSP_BTN_SQUARE;
      buttons->x = buf&NXT_PSP_BTN_X;
      buttons->o = buf&NXT_PSP_BTN_O;
      buttons->triangle = buf&NXT_PSP_BTN_TRIANGLE;
      buttons->r1 = buf&NXT_PSP_BTN_R1;
      buttons->l1 = buf&NXT_PSP_BTN_L1;
      buttons->r2 = buf&NXT_PSP_BTN_R2;
      buttons->l2 = buf&NXT_PSP_BTN_L2;
    }

    return 0;
  }
  else {
    return -1;
  }
}

/**
 * Gets analog joystick axis' values
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @param joy Joystick
 *  @param rx Reference for X axis' value
 *  @param ry Reference for Y axis' value
 *  @return 0 = Success
 *         -1 = Failure
 */
int nxt_psp_get_joystick(nxt_t *nxt,int port,int joy,struct nxt_psp_joystick *jbuf) {
  char buf[4];
  int j1,nj,i;

  if (joy==NXT_PSP_JOY_LEFT|NXT_PSP_JOY_RIGHT) {
    j1 = 0;
    nj = 2;
  }
  else if (joy==NXT_PSP_JOY_LEFT) {
    j1 = 0;
    nj = 1;
  }
  else if (joy==NXT_PSP_JOY_RIGHT) {
    j1 = 1;
    nj = 1;
  }

  if (nxt_i2c_regr(nxt,port,nxt_psp_i2c_addr,NXT_PSP_REG_JOYSTICKS+j1*2,nj*2,buf)==nj*2) {
    if (jbuf!=NULL) {
      for (i=0;i<nj;i++) {
        jbuf[i].x = (buf[i*2]&0xFF)-128;
        jbuf[i].y = (buf[i*2+1]&0xFF)-128;
      }
    }

    return 0;
  }
  else {
    return -1;
  }
}
