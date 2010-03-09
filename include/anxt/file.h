/*
    include/anxt/file.h
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

#ifndef _ANXT_FILE_H_
#define _ANXT_FILE_H_

#include <sys/types.h>

#include <anxt/nxt.h>

// File open flags
#define NXT_OWFRAG 0
#define NXT_OWLINE 1
#define NXT_OAPPND 2
#define NXT_OREAD  4
#define NXT_OWOVER 8 // can be or'd with write flags

int nxt_file_open(nxt_t *nxt,const char *file,int oflag,...);
ssize_t nxt_file_read(nxt_t *nxt,int handle,void *dest,size_t count);
ssize_t nxt_file_write(nxt_t *nxt,int handle,void *src,size_t count);
int nxt_file_close(nxt_t *nxt,int handle);
int nxt_file_remove(nxt_t *nxt,const char *file);
// TODO rename
int nxt_findfirst(nxt_t *nxt,const char *wildcard,char **filename,size_t *filesize);
int nxt_findnext(nxt_t *nxt,int handle,char **filename,size_t *filesize);

#endif /* _ANXT_FILE_H_ */
