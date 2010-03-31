/*
    libanxt/nxtcam.c
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
#include <anxt/i2c/nxtcam.h>

/// I2C Address
int nxt_cam_i2c_addr = 0x02;

int nxt_cam_num_objects(nxt_t *nxt,int port) {
  char buf;

  if (nxt_i2c_read(nxt,port,nxt_cam_i2c_addr,NXT_CAM_REG_OBJCOUNT,1,&buf)==-1) {
    return -1;
  }

  return buf;
}

int nxt_cam_get_objects(nxt_t *nxt,int port,size_t obj1,size_t nobj,nxt_cam_object_t *objbuf) {
  char buf[40];
  int i;

  if (obj1>7) {
    return -1;
  }
  if (obj1+nobj>8) {
    nobj = 8-obj1;
  }

  if (nxt_i2c_read(nxt,port,nxt_cam_i2c_addr,NXT_CAM_REG_OBJDATA+obj1*5,nobj*5,buf)==-1) {
    return -1;
  }

  for (i=0;i<nobj;i++) {
    objbuf[i].id = i;
    objbuf[i].color = buf[i*5+0]&0xFF;
    objbuf[i].x = buf[i*5+1]&0xFF;
    objbuf[i].y = buf[i*5+2]&0xFF;
    objbuf[i].x2 = buf[i*5+3]&0xFF;
    objbuf[i].y2 = buf[i*5+4]&0xFF;
    objbuf[i].w = objbuf[i].x2-objbuf[i].x;
    objbuf[i].h = objbuf[i].y2-objbuf[i].y;
  }

  return i;
}

int nxt_cam_get_colormap(nxt_t *nxt,int port,nxt_cam_colormap_t *colormap) {
  int i;

  nxt_cam_cmd(nxt,port,NXT_CAM_CMD_COLORMAP_READ);

  for (i=0;i<3;i++) {
    if (nxt_i2c_read(nxt,port,nxt_cam_i2c_addr,NXT_CAM_REG_COLORMAP+i*0x10,0x10,((void*)colormap)+i*0x10)==-1) {
      return -1;
    }
  }

  return 0;
}
