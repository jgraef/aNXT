/*
    us.c
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

#include <nxt.h>
#include <nxt_i2c/us.h>

int nxt_us_i2c_addr = 0x02;

int nxt_us_get_measdata(nxt_t *nxt,int port,size_t m1,size_t nm,int *mbuf) {
  char buf[8];
  int i;

  if (m1+nm>8) {
    return -1;
  }

  if (nxt_i2c_read(nxt,port,nxt_us_i2c_addr,NXT_US_REG_MEASUREMENT_DATA+m1,nm,buf)==nm) {
    for (i=0;i<nm;i++) {
      mbuf[i] = buf[i];
    }

    return nm;
  }
  else {
    return -1;
  }
}

int nxt_us_get_dist(nxt_t *nxt,int port) {
  int dist;

  if (nxt_us_get_measdata(nxt,port,0,1,&dist)==1) {
    return dist;
  }
  else {
    return -1;
  }
}
