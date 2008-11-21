/*
    libanxttools - A library of often used aNXT support functions 
    aNXT - a NXt Toolkit
    Libraries and tools for LEGO Mindstorms NXT robots
    Copyright (C) 2008  Janosch Gräf <janosch.graef@gmx.net>
    Copyright (C) 2008  J. "MUFTI" Scheurich (IITS Universitaet Stuttgart)

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

#ifndef _NXT_TOOLS_H_
#define _NXT_TOOLS_H_

#include "nxt.h"

typedef void (*nxt_list_one_file_callback)(char *filename,size_t filesize,void *data);
int nxt_list(nxt_t *nxt,char* wildcard,nxt_list_one_file_callback callback,void *data);
int nxt_list_files(nxt_t *nxt,FILE *out,char *wildcard,int filesizes,int hidden);

typedef void (*nxt_list_one_module_callback)(char *modname,int modid,size_t modsz,size_t iomapsz,void *data);
int nxt_list_modules(nxt_t *nxt,FILE *out,char *wild);

typedef void (*nxt_motor_record_callback)(double t,int tacho,void *data);
void nxt_motor_record(nxt_t *nxt,int motor,double t,nxt_motor_record_callback callback,void* data);

void nxt_motor_playback(nxt_t *nxt,int motor,int power,int numvalues,double *times,int *rotations,int stop,int verbose);
int nxt_read_recorded_file(int *numvalues,double **times,int **rotations,FILE *file);

int nxt_str2motorport(char *str);
int nxt_str2btn(char *str);
int nxt_str2fmt(char *str);
int nxt_str2type(char *str);
int nxt_str2mode(char *str);

char *nxt_getType(int i);
char *nxt_getMode(int i);
#endif /* _NXT_TOOLS_H_ */


