/*
    cam.h
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

#ifndef _NXT_I2C_NXTCAM_H_
#define _NXT_I2C_NXTCAM_H_

#include <nxt.h>

#define NXT_CAM_VALID_OBJ(o) ((o)>=0 && (o)<=8)

#define NXT_CAM_REG_VERSION    0x00
#define NXT_CAM_REG_PRODUCTID  0x08
#define NXT_CAM_REG_SENSORTYPE 0x10
#define NXT_CAM_REG_COMMAND    0x41
#define NXT_CAM_REG_OBJCOUNT   0x42
#define NXT_CAM_REG_OBJDATA    0x43

#define NXT_CAM_CMD_TRACKING_DISABLE    0x44
#define NXT_CAM_CMD_TRACKING_ENABLE     0x45
#define NXT_CAM_CMD_LINETRACKING_ENABLE 0x4C
#define NXT_CAM_CMD_RESET               0x52
#define NXT_CAM_CMD_COLORSORT_ENABLE    0x55
#define NXT_CAM_CMD_COLORSORT_DISABLE   0x58

typedef struct {
  int id;
  int color;
  int ul_x;
  int ul_y;
  int lr_x;
  int lr_y;
} nxt_cam_object_t;

int nxt_cam_i2c_addr;

char *nxt_cam_getversion(nxt_t *nxt,int port);
char *nxt_cam_getproductid(nxt_t *nxt,int port);
char *nxt_cam_getsensortype(nxt_t *nxt,int port);
void nxt_cam_command(nxt_t *nxt,int port,int cmd);
int nxt_cam_noobjects(nxt_t *nxt,int port);
int nxt_cam_getobject(nxt_t *nxt,int port,int obj,nxt_cam_object_t *object);

/**
 * Enable/Disable tracking
 *  @param enable Enable or Disable
 */
static __inline__ void nxt_cam_tracking_enable(nxt_t *nxt,int port,int enable) {
  nxt_cam_command(nxt,port,enable?NXT_CAM_CMD_TRACKING_ENABLE:NXT_CAM_CMD_TRACKING_DISABLE);
}

/**
 * Select line tracking mode
 */
static __inline__ void nxt_cam_linetracking(nxt_t *nxt,int port) {
  nxt_cam_command(nxt,port,NXT_CAM_CMD_LINETRACKING_ENABLE);
}

/**
 * Reset camera
 */
static __inline__ void nxt_cam_reset(nxt_t *nxt,int port) {
  nxt_cam_command(nxt,port,NXT_CAM_CMD_RESET);
}

/**
 * Enable/Disable sorting by color
 *  @param x Enable or Disable
 */
static __inline__ void nxt_cam_colorsort_enable(nxt_t *nxt,int port,int enable) {
  nxt_cam_command(nxt,port,enable?NXT_CAM_CMD_COLORSORT_ENABLE:NXT_CAM_CMD_COLORSORT_DISABLE);
}

#endif /* _NXT_I2C_NXTCAM_H_ */
