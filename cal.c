/*
    cal.c
    libnxt - A C library for using LEGO Mindstorms NXT
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

#include <sys/types.h>
#include <stdint.h>
#include <stdlib.h>

#include "nxtfile/cal.h"

#define BIGENDIAN_SET_WORD(buf,val) { ((uint8_t*)(buf))[0] = (val)/0x100; ((uint8_t*)(buf))[1] = (val)%0x100; }
#define BIGENDIAN_GET_WORD(buf)     (((uint16_t)((buf)[0]))*0x100+(buf)[1])

#define LITTLEENDIAN_SET_WORD(buf,val) { ((uint8_t*)(buf))[1] = (val)/0x100; ((uint8_t*)(buf))[0] = (val)%0x100; }
#define LITTLEENDIAN_GET_WORD(buf)     (((uint16_t)((buf)[1]))*0x100+(buf)[0])

size_t cal_encode(void **ptr,int min,int max) {
  struct cal_data *data = malloc(sizeof(struct cal_data));
  LITTLEENDIAN_SET_WORD(&(data->min),min);
  LITTLEENDIAN_SET_WORD(&(data->max),max);
  *ptr = data;
  return sizeof(struct cal_data);
}

void cal_decode(void *ptr,int *min,int *max) {
  struct cal_data *data = (struct cal_data*)ptr;
  if (min!=NULL) *min = LITTLEENDIAN_GET_WORD(&(data->min));
  if (max!=NULL) *max = LITTLEENDIAN_GET_WORD(&(data->max));
}
