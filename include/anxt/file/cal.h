/*
    cal.h
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

#ifndef _NXTFILE_CAL_H_
#define _NXTFILE_CAL_H_

#include <sys/types.h>

struct cal_data {
  uint16_t min;
  uint16_t max;
} __attribute__ ((packed));

size_t cal_encode(void **ptr,int min,int max);
void cal_decode(void *ptr,int *min,int *max);

#endif /* _NXTFILE_CAL_H_ */
