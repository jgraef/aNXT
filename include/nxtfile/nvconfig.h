/*
    nvconfig.h
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

#ifndef _NXTFILE_NVCONFIG_H_
#define _NXTFILE_NVCONFIG_H_

#include <stddef.h>

#define NVCONFIG_SLEEP_NEVER 0
#define NVCONFIG_SLEEP_2MIN  1
#define NVCONFIG_SLEEP_5MIN  2
#define NVCONFIG_SLEEP_10MIN 3
#define NVCONFIG_SLEEP_30MIN 4
#define NVCONFIG_SLEEP_60MIN 5

struct nvconfig {
  unsigned sleep:3;
  unsigned one1:1;
  unsigned volume:3;
  unsigned one2:1;
};

uint8_t nvconfig_set(unsigned int sleep,unsigned int volume);
void nvconfig_get(uint8_t nvconfig,unsigned int *sleep,unsigned int *volume);

#endif /* _NXTFILE_NVCONFIG_H_ */
