/*
    include/anxt/ls.h
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

#ifndef _ANXT_LS_H_
#define _ANXT_LS_H_

#include <sys/types.h>

#include <anxt/nxt.h>

ssize_t nxt_ls_status(nxt_t *nxt,int port);
int nxt_ls_write(nxt_t *nxt,int port,size_t tx,size_t rx,void *data);
ssize_t nxt_ls_read(nxt_t *nxt,int port,size_t bufsize,void *buf);

#endif /* _ANXT_LS_H_ */
