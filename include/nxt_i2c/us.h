/*
    us.h
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

#ifndef _NXT_I2C_US_H_
#define _NXT_I2C_US_H_

#include <nxt.h>

/**
 * Gets distance from US Sensor (default mode)
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @return Distance
 */
#define nxt_us_getdist(nxt,port) nxt_us_getmeasbyte(nxt,port,0)

char *nxt_us_getversion(nxt_t *nxt,int port);
char *nxt_us_getproductid(nxt_t *nxt,int port);
char *nxt_us_getsensortype(nxt_t *nxt,int port);
int nxt_us_getfactoryzero(nxt_t *nxt,int port);
int nxt_us_getfactoryscalefactor(nxt_t *nxt,int port);
int nxt_us_getfactoryscaledivisor(nxt_t *nxt,int port);
char *nxt_us_getmeasunits(nxt_t *nxt,int port);
int nxt_us_getcontmeasinterval(nxt_t *nxt,int port);
int nxt_us_getcmdstate(nxt_t *nxt,int port);
int nxt_us_getmeasbyte(nxt_t *nxt,int port,int byte);
int nxt_us_getactualzero(nxt_t *nxt,int port);
int nxt_us_getactualscalefactor(nxt_t *nxt,int port);
int nxt_us_getactualscaledivisor(nxt_t *nxt,int port);
void nxt_us_off(nxt_t *nxt,int port);
void nxt_us_singleshot(nxt_t *nxt,int port);
void nxt_us_contmeas(nxt_t *nxt,int port);
void nxt_us_eventmeas(nxt_t *nxt,int port);
void nxt_us_warmreset(nxt_t *nxt,int port);
void nxt_us_setcontmeasinterval(nxt_t *nxt,int port,int interval);
void nxt_us_setactualzero(nxt_t *nxt,int port,int zero);
void nxt_us_setactualscalefactor(nxt_t *nxt,int port,int factor);
void nxt_us_setactualscaledivisor(nxt_t *nxt,int port,int divisor);

#endif /* _NXT_I2C_US_H_ */
