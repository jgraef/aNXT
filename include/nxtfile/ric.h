/*
    ric.h
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

#ifndef _NXTFILE_RIC_H_
#define _NXTFILE_RIC_H_

#include <sys/types.h>
#include <stdint.h>

#define RIC_OPCODE_DESCRIPTION 0
#define RIC_OPCODE_SPRITE      1
#define RIC_OPCODE_COPYBITS    3

struct ric_opcode {
  uint16_t len;
  uint16_t opcode;
} __attribute__ ((packed));

struct ric_description {
  struct ric_opcode header;
  uint16_t options;
  uint16_t width;
  uint16_t height;
} __attribute__ ((packed));

struct ric_sprite {
  struct ric_opcode header;
  uint16_t dataaddr;
  uint16_t rows;
  uint16_t rowbytes;
  uint8_t data[0];
} __attribute__ ((packed));

struct ric_point {
  int16_t x;
  int16_t y;
} __attribute__ ((packed));

struct ric_rect {
  struct ric_point point;
  int16_t width;
  int16_t height;
} __attribute__ ((packed));

struct ric_copybits {
  struct ric_opcode header;
  enum {
    RIC_COPY = 0,
    RIC_COPYNOT = 1,
    RIC_OR = 2,
    RIC_BITCLEAR = 3
  } options:16;
  uint16_t dataaddr;
  struct ric_rect src;
  struct ric_point dest;
} __attribute__ ((packed));

size_t ric_encode(void **ptr,unsigned int width,unsigned int height,void *bitmap);
ssize_t ric_decode(void **ptr,void *data,size_t data_size,unsigned int *width,unsigned int *height);

#endif
