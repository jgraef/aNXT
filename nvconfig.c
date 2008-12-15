/*
    nvconfig.c
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

#include <stddef.h>
#include <stdint.h>

#include "nxtfile/nvconfig.h"

uint8_t nvconfig_set(unsigned int sleep,unsigned int volume) {
  struct nvconfig config = {
    .sleep = sleep,
    .one1 = 1,
    .volume = volume,
    .one2 = 1
  };
  return *((uint8_t*)&config);
}

void nvconfig_get(uint8_t nvconfig,unsigned int *sleep,unsigned int *volume) {
  struct nvconfig *config = (struct nvconfig*)&nvconfig;
  if (sleep!=NULL) *sleep = config->sleep;
  if (volume!=NULL) *volume = config->volume;
}
