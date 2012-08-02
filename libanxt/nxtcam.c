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


/**
 * Get number of objects (blobs)
 *  @param nxt  NXT handle
 *  @param port Sensor port
 *  @return Number of currently seen objects (blobs)
 */
int nxt_cam_num_objects(nxt_t *nxt, int port) {
  char buf;

  if (nxt_i2c_read(nxt, port, nxt_cam_i2c_addr, NXT_CAM_REG_OBJCOUNT, 1, &buf)==-1) {
    return -1;
  }

  if (buf<0 || buf>8) {
    return -1;
  }
  else {
    return buf;
  }
}

/**
 * Get posisitions of currently seen objects
 *  @param nxt    NXT handle
 *  @param port   Sensor port
 *  @param obj1   Index of first object that should be returned
 *  @param nobj   Number of objects that should be returned
 *  @param objbuf Object position buffer
 *  @return Number of objects actually read
 */
int nxt_cam_get_objects(nxt_t *nxt, int port, unsigned int obj1, unsigned int nobj, nxt_cam_object_t *objbuf) {
  unsigned char buf[40];
  int i;

  if (!NXT_CAM_VALID_OBJ(obj1)) {
    return -1;
  }
  if (obj1+nobj>8) {
    nobj = 8-obj1;
  }

  if (nxt_i2c_read(nxt, port, nxt_cam_i2c_addr, NXT_CAM_REG_OBJDATA+obj1*5, nobj*5, buf)==-1) {
    return -1;
  }

  for (i=0;i<nobj;i++) {
    objbuf[i].id = i;
    objbuf[i].color = buf[i*5+0];
    objbuf[i].x = buf[i*5+1];
    objbuf[i].y = buf[i*5+2];
    objbuf[i].x2 = buf[i*5+3];
    objbuf[i].y2 = buf[i*5+4];
    objbuf[i].w = objbuf[i].x2-objbuf[i].x;
    objbuf[i].h = objbuf[i].y2-objbuf[i].y;
  }

  return i;
}

/**
 * Get colormap from camera
 *  @param nxt      NXT handle
 *  @param port     Sensor port
 *  @param colormap Colormap destination buffer
 *  @return 0 on success, else -1
 */
int nxt_cam_get_colormap(nxt_t *nxt, int port, nxt_cam_colormap_t *colormap) {
  nxt_cam_cmd(nxt, port, NXT_CAM_CMD_COLORMAP_READ);

  if (nxt_i2c_read(nxt, port, nxt_cam_i2c_addr, NXT_CAM_REG_COLORMAP, sizeof(colormap), colormap)==-1) {
    return -1;
  }

  return 0;
}

/**
 * Send colormap to camera
 *  @param nxt      NXT handle
 *  @param port     Sensor port
 *  @param colormap Colormap data
 *  @return 0 on success, else -1
 */
int nxt_cam_set_colormap(nxt_t *nxt, int port, const nxt_cam_colormap_t *colormap) {
  if (nxt_i2c_read(nxt, port, nxt_cam_i2c_addr, NXT_CAM_REG_COLORMAP, sizeof(colormap), colormap)==-1) {
    return -1;
  }

  nxt_cam_cmd(nxt, port, NXT_CAM_CMD_COLORMAP_WRITE);

  return 0;
}

/* Load colormap from file
 *  @param filename Path to colormap file
 *  @param colormap Colormap destination buffer
 *  @return 0 on success, else -1
 */
int nxt_cam_load_colormap(const char *filename, nxt_cam_colormap_t *colormap) {
  FILE *fd;
  char sig[32];
  int ret = -1;

  fd = fopen(filename, "rb");
  if (fd!=NULL) {
    return -1;
  }

  // read signature
  fgets(sig, 32, fd);
  if (strcmp(NXT_CAM_COLORMAP_FILESIG, sig)!=0) {
    fclose(fd);
    return -1;
  }

  // read colormap
  if (fread(colormap, sizeof(colormap), 1, fd)!=1) {
    fclose(fd);
    return -1;
  }

  fclose(fd);
  return 0;
}

/* Save colormap to file
 *  @param filename Path to colormap file
 *  @param colormap Colormap data
 *  @return 0 on success, else -1
 */
int nxt_cam_save_colormap(const char *filename, const nxt_cam_colormap_t *colormap) {
  FILE *fd;
  int ret = -1;

  fd = fopen(filename, "rb");
  if (fd!=NULL) {
    return -1;
  }

  // write signature
  fputs(NXT_CAM_COLORMAP_FILESIG, fd);

  // write colormap
  if (fwrite(colormap, sizeof(colormap), 1, fd)!=1) {
    return -1;
    fclose(fd);
  }

  fclose(fd);
  return 0;
}


/**
 * Enable/Disable tracking
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @param enable Enable or Disable
 */
void nxt_cam_enable_tracking(nxt_t *nxt, int port, int enable) {
  nxt_cam_cmd(nxt, port, enable?NXT_CAM_CMD_TRACKING_ENABLE:NXT_CAM_CMD_TRACKING_DISABLE);
}

/**
 * Set tracking mode
 *  @param nxt  NXT handle
 *  @param port Sensor port
 *  @param mode Which mode to set
 */
void nxt_cam_set_trackingmode(nxt_t *nxt, int port, int mode) {
  nxt_cam_cmd(nxt, port, mode);
}

/**
 * Reset camera
 *  @param nxt  NXT handle
 *  @param port Sensor port
 */
void nxt_cam_reset(nxt_t *nxt, int port) {
  nxt_cam_cmd(nxt, port, NXT_CAM_CMD_RESET);
}

/**
 * Enable/Disable sorting by color
 *  @param nxt    NXT handle
 *  @param port   Sensor port
 *  @param enable Enable or Disable
 */
void nxt_cam_enable_colorsort(nxt_t *nxt, int port, int enable) {
  nxt_cam_cmd(nxt, port, enable?NXT_CAM_CMD_COLORSORT_ENABLE:NXT_CAM_CMD_COLORSORT_DISABLE);
}

