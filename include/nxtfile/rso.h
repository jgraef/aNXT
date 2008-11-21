/*
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

#ifndef _NXTFILE_RSO_H_
#define _NXTFILE_RSO_H_

#include <stdint.h>

#define RSO_FREQUENCY_MIN      220   // [Hz]
#define RSO_FREQUENCY_MAX      14080 // [Hz]

#define RSO_SAMPLERATE_MIN     2000  // Min sample rate [sps]
#define RSO_SAMPLERATE_DEFAULT 8000  // Default sample rate [sps]
#define RSO_SAMPLERATE_MAX     16000 // Max sample rate [sps]

struct rso_sound {
  uint16_t format;
  uint16_t databytes;
  uint16_t samplerate;
  uint16_t playmode;
  uint8_t data[0];
} __attribute__ ((packed));

size_t rso_encode(void **ptr,unsigned int samplerate,size_t len,void *data);
size_t rso_decode(void **ptr,void*rsodata,unsigned int *samplerate);

#endif
