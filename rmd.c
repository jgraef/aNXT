/*
    rmd.c
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
#include <string.h>
#include <stdlib.h>

#define BIGENDIAN_SET_WORD(buf,val) { ((uint8_t*)(buf))[0] = (val)/0x100; ((uint8_t*)(buf))[1] = (val)%0x100; }
#define BIGENDIAN_GET_WORD(buf)     (((uint16_t)((buf)[0]))*0x100+(buf)[1])

#define LITTLEENDIAN_SET_WORD(buf,val) { ((uint8_t*)(buf))[1] = (val)/0x100; ((uint8_t*)(buf))[0] = (val)%0x100; }
#define LITTLEENDIAN_GET_WORD(buf)     (((uint16_t)((buf)[1]))*0x100+(buf)[0])

struct rmd_note {
  uint16_t freq;
  uint16_t dur;
} __attribute__ ((packed));

struct rmd_data {
  uint16_t format;  // 6
  uint16_t datalen; // sizeof(struct rmd_note)*notes
  uint32_t noidea;  // 0
  struct rmd_note data[1];
} __attribute__ ((packed));

/**
 * Converts note data to RMD data
 *  @param ptr Reference for RMD data
 *  @param notes Amount of notes
 *  @param src Note data (2*16bit; freq,dur)
 *  @return Size of RMD data
 *  @note Pointer 'ptr' can and should be passed to free()
 */
size_t rmd_encode(void **ptr,size_t notes,void *src) {
  size_t i;
  size_t bufsize = sizeof(struct rmd_data)+sizeof(struct rmd_note)*(notes-1);
  struct rmd_data *rmd_data = malloc(bufsize);

  LITTLEENDIAN_SET_WORD(&(rmd_data->format),6);
  BIGENDIAN_SET_WORD(&(rmd_data->datalen),sizeof(struct rmd_note)*notes);
  rmd_data->noidea = 0;

  memcpy(rmd_data->data,src,notes*4);

  *ptr = rmd_data;
  return bufsize;
}

/**
 * Converts RSO data to wave data
 *  @param ptr Reference for note data (2*16bit; freq,dur)
 *  @param src RMD data
 *  @return Amount of notes
 *  @note After calling this function 'ptr' points into 'rsodata'
 */
size_t rmd_decode(void **ptr,void *src) {
  struct rmd_data *rmd_data = src;
  if (LITTLEENDIAN_GET_WORD(&(rmd_data->format))==6) {
    *ptr = rmd_data->data;
    return rmd_data->datalen/sizeof(struct rmd_note);
  }
  else return 0;
}
