/*
    nxtcam.h
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

#include <anxt/nxt.h>

#define NXT_CAM_COLORMAP_FILESIG "#aNXTCam COLORMAP\n"

#define NXT_CAM_VALID_OBJ(o) ((o)>=0 && (o)<8)

#define NXT_CAM_REG_OBJCOUNT   0x42
#define NXT_CAM_REG_OBJDATA    0x43
#define NXT_CAM_REG_COLORMAP   0x80

#define NXT_CAM_CMD_OBJECTTRACKING      0x42
#define NXT_CAM_CMD_TRACKING_DISABLE    0x44
#define NXT_CAM_CMD_TRACKING_ENABLE     0x45
#define NXT_CAM_CMD_COLORMAP_READ       0x47
#define NXT_CAM_CMD_LINETRACKING        0x4C
#define NXT_CAM_CMD_RESET               0x52
#define NXT_CAM_CMD_COLORMAP_WRITE      0x53
#define NXT_CAM_CMD_COLORSORT_ENABLE    0x55
#define NXT_CAM_CMD_COLORSORT_DISABLE   0x58

#define NXT_CAM_TRACKING_OBJECT NXT_CAM_CMD_OBJECTTRACKING
#define NXT_CAM_TRACKING_LINE   NXT_CAM_CMD_LINETRACKING

#define nxt_cam_get_version(nxt,port)  nxt_i2c_get_version(nxt,port,nxt_cam_i2c_addr)
#define nxt_cam_get_vendorid(nxt,port) nxt_i2c_get_vendorid(nxt,port,nxt_cam_i2c_addr)
#define nxt_cam_get_deviceid(nxt,port) nxt_i2c_get_deviceid(nxt,port,nxt_cam_i2c_addr)
#define nxt_cam_cmd(nxt,port,cmd)      nxt_i2c_cmd(nxt,port,nxt_cam_i2c_addr,cmd)

typedef struct {
  int id;
  int color;
  int x,y;
  int x2,y2;
  int w,h;
} nxt_cam_object_t;

// Colormap
// NOTE: exactly same format as on camera and in file
typedef struct {
  uint8_t r[16];
  uint8_t g[16];
  uint8_t b[16];
}  __attribute__ ((packed)) nxt_cam_colormap_t;

int nxt_cam_i2c_addr;

int nxt_cam_num_objects(nxt_t *nxt,int port);
int nxt_cam_get_objects(nxt_t *nxt, int port, unsigned int obj1, unsigned int nobj, nxt_cam_object_t *objbuf);
int nxt_cam_get_colormap(nxt_t *nxt, int port, nxt_cam_colormap_t *colormap);
int nxt_cam_set_colormap(nxt_t *nxt, int port, const nxt_cam_colormap_t *colormap);
int nxt_cam_load_colormap(const char *filename, nxt_cam_colormap_t *colormap);
int nxt_cam_save_colormap(const char *filename, const nxt_cam_colormap_t *colormap);
void nxt_cam_enable_tracking(nxt_t *nxt,int port,int enable);
void nxt_cam_set_trackingmode(nxt_t *nxt,int port,int mode);
void nxt_cam_reset(nxt_t *nxt,int port);
void nxt_cam_enable_colorsort(nxt_t *nxt,int port,int enable);

#endif /* _NXT_I2C_NXTCAM_H_ */
