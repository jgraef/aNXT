/*
    rso.c
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

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <nxtfile/rso.h>

#define BIGENDIAN_SET_WORD(buf,val) { ((uint8_t*)(buf))[0] = (val)/0x100; ((uint8_t*)(buf))[1] = (val)%0x100; }
#define BIGENDIAN_GET_WORD(buf)     ((uint16_t)(((uint8_t*)(buf))[0])*0x100+((uint8_t*)(buf))[1])

#define LITTLEENDIAN_SET_WORD(buf,val) { ((uint8_t*)(buf))[1] = (val)/0x100; ((uint8_t*)(buf))[0] = (val)%0x100; }
#define LITTLEENDIAN_GET_WORD(buf)     ((uint16_t)(((uint8_t*)(buf))[1])*0x100+((uint8_t*)(buf))[0])

/**
 * Converts wave data to RSO data
 *  @param ptr Reference for RSO data
 *  @param samplerate Samplerate
 *  @param len Length of data
 *  @param data Wave data (8bit)
 *  @return Size of RSO data
 *  @note Pointer 'ptr' can and should be passed to free()
 */
size_t rso_encode(void **ptr,unsigned int samplerate,size_t len,void *data) {
  size_t buflen = sizeof(struct rso_sound)+len;
  struct rso_sound *sound = malloc(buflen);

  if (samplerate>RSO_SAMPLERATE_MAX || samplerate<RSO_SAMPLERATE_MIN) return 0;

  LITTLEENDIAN_SET_WORD(&(sound->format),1);
  BIGENDIAN_SET_WORD(&(sound->databytes),len);
  BIGENDIAN_SET_WORD(&(sound->samplerate),samplerate);
  memcpy(sound->data,data,len);

  *ptr = sound;
  return buflen;
}

/**
 * Converts RSO data to wave data
 *  @param ptr Reference for wave data (8bit)
 *  @param rsodata RSO data
 *  @return Size of wave data
 *  @note After calling this function 'ptr' points in 'rsodata'
 */
size_t rso_decode(void **ptr,void *data,unsigned int *samplerate) {
  struct rso_sound *rsodata = data;
  if (LITTLEENDIAN_GET_WORD(&(rsodata->format))==1) {
    *ptr = rsodata->data;
    if (samplerate!=NULL) *samplerate = BIGENDIAN_GET_WORD(&(rsodata->samplerate));
    return BIGENDIAN_GET_WORD(&(rsodata->databytes));
  }
  else return 0;
}
