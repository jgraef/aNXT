/*
    ric.c
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

#include <sys/types.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "nxtfile/ric.h"

#define MAKE_EVEN(x) ((x)+((x)%2))

/**
 * Converts a bitmap into a RIC file
 *  @param width Width of bitmap
 *  @param height Height of bitmap
 *  @param bitmap Bitmap (one byte per pixel)
 *  @return RIC data
 *  @note Return value can and should be passed to free()
 */
size_t ric_encode(void **ptr,unsigned int width,unsigned int height,void *vdbitmap) {
  uint8_t *bitmap = vdbitmap;
  unsigned int rowbytes = (width-1)/8+1;
  size_t bitmap_size = MAKE_EVEN(rowbytes*height);
  unsigned int x,y;
  size_t bufsize = sizeof(struct ric_description)+sizeof(struct ric_sprite)+bitmap_size+sizeof(struct ric_copybits);

  // Prepare buffer
  void *buffer = malloc(bufsize);
  memset(buffer,0,bufsize);

  // Write description
  struct ric_description *desc = buffer;
  desc->header.len = sizeof(struct ric_description)-2;
  desc->header.opcode = RIC_OPCODE_DESCRIPTION;
  desc->width = width;
  desc->height = height;

  // Write sprite (bitmap)
  struct ric_sprite *sprite = buffer+sizeof(struct ric_description);
  sprite->header.len = sizeof(struct ric_sprite)-2+bitmap_size;
  sprite->header.opcode = RIC_OPCODE_SPRITE;
  sprite->dataaddr = 1; // like NXT-G does
  sprite->rows = height;
  sprite->rowbytes = rowbytes;
  // Write bitmap part of sprite
  uint8_t *data = (uint8_t*)sprite->data;
  for (y=0;y<height;y++) {
    for (x=0;x<width;x++) {
      if (bitmap[x+y*width]<0x80) data[x/8] |= 1<<(7-(x%8));
    }
    data += rowbytes;
  }

  // Write Copy command
  struct ric_copybits *copy = buffer+sizeof(struct ric_description)+sizeof(struct ric_sprite)+bitmap_size;
  copy->header.len = sizeof(struct ric_copybits)-2;
  copy->header.opcode = RIC_OPCODE_COPYBITS;
  copy->options = RIC_COPY;
  copy->dataaddr = 1;
  copy->src.point.x = 0;
  copy->src.point.y = 0;
  copy->src.width = width;
  copy->src.height = height;
  copy->dest.x = 0;
  copy->dest.y = 0;

  *ptr = buffer;
  return bufsize;
}
#include <stdio.h>
ssize_t ric_decode(void **ptr,void *data,size_t data_size,unsigned int *width,unsigned int *height) {
  struct ric_sprite *sprite = NULL;
  struct ric_copybits *copybits = NULL;
  size_t i = 0;

  // Get Sprite command and Copybits command
  while (i<data_size) {
    struct ric_opcode *op = data+i;
    if (op->opcode==RIC_OPCODE_SPRITE) sprite = (struct ric_sprite*)op;
    else if (op->opcode==RIC_OPCODE_COPYBITS) copybits = (struct ric_copybits*)op;
    if (sprite!=NULL && copybits!=NULL) break;
    i += op->len+2;
  }
  if (sprite==NULL || copybits==NULL) return -1;

  // Prepare bitmap
  uint8_t *bitmap = malloc(copybits->src.width*copybits->src.height);
  *width = copybits->src.width;
  *height = copybits->src.height;

  // Copy bitmap
  size_t x,y;
  uint8_t *sprite_data = sprite->data;
  for (y=0;y<copybits->src.height;y++) {
    for (x=0;x<copybits->src.width;x++) {
      if (sprite_data[x/8]&(1<<(7-(x%8)))) bitmap[x+y*copybits->src.width] = 0;
      else bitmap[x+y*copybits->src.width] = 0xFF;
    }
    sprite_data += sprite->rowbytes;
  }

  *ptr = bitmap;
  return copybits->src.width*copybits->src.height;
}
