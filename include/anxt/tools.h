/*
    libanxttools - A library of often used aNXT support functions 
    aNXT - a NXt Toolkit
    Libraries and tools for LEGO Mindstorms NXT robots
    Copyright (C) 2008  Janosch Gräf <janosch.graef@gmx.net>
    Copyright (C) 2008  J. "MUFTI" Scheurich (IITS Universitaet Stuttgart)

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

#ifndef _NXT_TOOLS_H_
#define _NXT_TOOLS_H_

#include <anxt/nxt.h>

#define NXT_BUTTON_STATUS_STR(s) ((s)?"pressed":"released")
#define NXT_BUTTON_STATUS_INT(s) ((s)?1:0)

typedef void (*nxt_list_one_file_callback)(char *filename,size_t filesize,void *data);
int nxt_list(nxt_t *nxt,char* wildcard,nxt_list_one_file_callback callback,void *data);
int nxt_list_files(nxt_t *nxt,FILE *out,char *wildcard,int filesizes,int hidden);

typedef void (*nxt_list_one_module_callback)(char *modname,int modid,size_t modsz,size_t iomapsz,void *data);
int nxt_lsmod(nxt_t *nxt,char *wildcard,nxt_list_one_module_callback callback,void *data);
int nxt_list_modules(nxt_t *nxt,FILE *out,char *wild);

typedef void (*nxt_motor_record_callback)(double t,int tacho,void *data);
void nxt_motor_record(nxt_t *nxt,int motor,double t,nxt_motor_record_callback callback,void* data);

void nxt_motor_playback(nxt_t *nxt,int motor,int power,int numvalues,double *times,int *rotations,int stop,int verbose);
int nxt_read_recorded_file(int *numvalues,double **times,int **rotations,FILE *file);

int nxt_chr2motor(char chr);
int nxt_str2motor(const char *str);
int nxt_str2button(char *str);
int nxt_str2format(char *str);
int nxt_str2type(char *str);
int nxt_str2mode(char *str);

char *nxt_get_type(int i);
char *nxt_get_mode(int i);

int nxt_download(nxt_t *nxt,char *src,char *dest);
int nxt_upload(nxt_t *nxt,char *src,char *dest,int oflag);

#endif /* _NXT_TOOLS_H_ */


