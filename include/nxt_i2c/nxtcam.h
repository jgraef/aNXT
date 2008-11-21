/*
    nxtcam.h
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

#ifndef _NXT_I2C_CAM_H_
#define _NXT_I2C_CAM_H_

#include <nxt.h>

#define VALID_OBJ(o) ((o)>=0 && (o)<=8)

/**
 * Enable/Disable tracking
 *  @param x Enable or Disable
 */
#define nxt_nxtcam_tracking(x) nxt_nxtcam_command((x)?0x45:0x44)

/**
 * Select line tracking mode
 */
#define nxt_nxtcam_linetracking() nxt_nxtcam_command(0x4C)

/**
 * Enable/Disable ADPA mode
 *  @param x Enable or Disable
 */
#define nxt_nxtcam_adpa(x) nxt_nxtcam_command((x)?0x4E:0x4F)

/**
 * Ping camera
 */
#define nxt_nxtcam_ping() nxt_nxtcam_command(0x50)

/**
 * Reset camera
 */
#define nxt_nxtcam_reset() nxt_nxtcam_command(0x52)

/**
 * Enable/Disable sorting by color
 *  @param x Enable or Disable
 */
#define nxt_nxtcam_sort(x) nxt_nxtcam_command((x)?0x55:0x58)

typedef struct {
  int color;
  int ul_x;
  int ul_y;
  int lr_x;
  int lr_y;
} nxt_nxtcam_object_t;

char *nxt_nxtcam_getversion(nxt_t *nxt,int port);
char *nxt_nxtcam_getproductid(nxt_t *nxt,int port);
char *nxt_nxtcam_getsensortype(nxt_t *nxt,int port);
void nxt_nxtcam_command(nxt_t *nxt,int port,int cmd);
int nxt_nxtcam_noobjects(nxt_t *nxt,int port);
int nxt_nxtcam_getobject(nxt_t *nxt,int port,int obj,nxt_nxtcam_object_t *object);

#endif /* _NXT_I2C_CAM_H_ */
