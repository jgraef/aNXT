/*
    libanxt/file.c
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

#include <sys/types.h>
#include <string.h>
#include <stdarg.h>

#include <anxt/nxt.h>
#include <anxt/file.h>
#include "private.h"

/**
 * Opens a file for writing in fragmented mode
 *  @param nxt NXT handle
 *  @param file Filename
 *  @param size How big the file should be
 *  @return File handle
 *  @note Use nxt_file_open
 */
static int nxt_file_open_write(nxt_t *nxt,const char *file,size_t size) {
  nxt_pack_start(nxt,0x81);
  nxt_pack_str(nxt,file,20);
  nxt_pack_dword(nxt,size);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,4));
  test(nxt_unpack_start(nxt,0x81));
  if (nxt_unpack_error(nxt)==0) return nxt_unpack_byte(nxt);
  else return NXT_FAIL;
}

/**
 * Opens a file for writing in linear mode
 *  @param nxt NXT handle
 *  @param file Filename
 *  @param size How big the file should be
 *  @return File handle
 *  @note Use nxt_file_open
 */
static int nxt_file_open_write_linear(nxt_t *nxt,const char *file,size_t size) {
  nxt_pack_start(nxt,0x89);
  nxt_pack_str(nxt,file,20);
  nxt_pack_dword(nxt,size);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,4));
  test(nxt_unpack_start(nxt,0x89));
  if (nxt_unpack_error(nxt)==0) return nxt_unpack_byte(nxt);
  else return NXT_FAIL;
}

/**
 * Opens a file for appending data
 *  @param nxt NXT handle
 *  @param file Filename
 *  @return File handle
 *  @note Use nxt_file_open
 */
static int nxt_file_open_append(nxt_t *nxt,const char *file,size_t *avail) {
  nxt_pack_start(nxt,0x8C);
  nxt_pack_str(nxt,file,20);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,4));
  test(nxt_unpack_start(nxt,0x8C));
  if (nxt_unpack_error(nxt)==0) {
    int handle = nxt_unpack_byte(nxt);
    if (avail!=NULL) *avail = nxt_unpack_dword(nxt);
    return handle;
  }
  else return NXT_FAIL;
}

/**
 * Opens a file on NXT for reading
 *  @param nxt NXT handle
 *  @param file Filename
 *  @param filesize Reference for filesize
 *  @return File handle
 *  @note Use nxt_file_open
 */
static int nxt_file_open_read(nxt_t *nxt,const char *file,size_t *filesize) {
  nxt_pack_start(nxt,0x80);
  nxt_pack_str(nxt,file,20);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,8));
  test(nxt_unpack_start(nxt,0x80));
  if (nxt_unpack_error(nxt)==0) {
    int handle = (int)nxt_unpack_byte(nxt);
    if (filesize!=NULL) *filesize = nxt_unpack_dword(nxt);
    return handle;
  }
  else return NXT_FAIL;
}

/**
 * Opens a file on NXT for writing
 *  @param nxt NXT handle
 *  @param file Filename
 *  @param oflag Open flags
 *  @param [size] How big the file should be
 *  @return File handle
 *  @note For fragmented and linear mode 'size' is the requested file size
 *  @note For appending mode 'size' is a refernce for the available file size
 *  @note For reading mode 'size' is a reference for the file size
 */
int nxt_file_open(nxt_t *nxt,const char *file,int oflag,...) {
  va_list args;
  int handle = -1;

  va_start(args,oflag);

  if (FLAG_ISSET(oflag,NXT_OREAD)) handle = nxt_file_open_read(nxt,file,va_arg(args,size_t*));
  else if (FLAG_ISSET(oflag,NXT_OAPPND)) handle = nxt_file_open_append(nxt,file,va_arg(args,size_t*));
  else {
    if (FLAG_ISSET(oflag,NXT_OWOVER) && (handle = nxt_file_find_first(nxt,file,NULL,NULL))>=0) {
      nxt_file_close(nxt,handle);
      nxt_file_remove(nxt,file);
      handle = -1;
    }
    if (FLAG_ISSET(oflag,NXT_OWLINE)) handle = nxt_file_open_write_linear(nxt,file,va_arg(args,size_t));
    else if (FLAG_ISSET(oflag,NXT_OWFRAG)) handle = nxt_file_open_write(nxt,file,va_arg(args,size_t));
  }

  va_end(args);

  return handle;
}

/**
 * Reads data from file from NXT (Lowlevel)
 *  @param nxt NXT handle
 *  @param handle File handle
 *  @param dest Buffer where to store data
 *  @param count How many bytes to read (max: 59 bytes)
 *  @return How many bytes read
 */
static ssize_t nxt_file_read_low(nxt_t *nxt,int handle,void *dest,size_t count) {
  nxt_pack_start(nxt,0x82);
  nxt_pack_byte(nxt,handle);
  nxt_pack_word(nxt,count);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,6+count));
  test(nxt_unpack_start(nxt,0x82));
  if (nxt_unpack_error(nxt)==0) {
    nxt_unpack_byte(nxt);
    count = nxt_unpack_word(nxt);
    memcpy(dest,nxt_unpack_mem(nxt,count),count);
    return count;
  }
  else return NXT_FAIL;
}

/**
 * Reads data from file from NXT
 *  @param nxt NXT handle
 *  @param handle File handle
 *  @param dest Buffer to store red data in
 *  @param count How many bytes to read
 *  @return How many bytes red
 */
ssize_t nxt_file_read(nxt_t *nxt,int handle,void *dest,size_t count) {
  int i;
  ssize_t partlen;
  int len = 0;
  for (i=0;i<count/50;i++) {
    partlen = nxt_file_read_low(nxt,handle,dest,50);
    if (partlen==-1) return NXT_FAIL;
    dest += partlen;
    len += partlen;
  }
  if ((partlen = nxt_file_read_low(nxt,handle,dest,count%50))==-1) return NXT_FAIL;
  len += partlen;
  return len;
}

/**
 * Writes data to file from NXT (Lowlevel)
 *  @param nxt NXT handle
 *  @param handle File handle
 *  @param src Data to write in file
 *  @param count How many bytes to write (max: 59 bytes)
 *  @return How many bytes written
 */
static ssize_t nxt_file_write_low(nxt_t *nxt,int handle,void *src,size_t count) {
  nxt_pack_start(nxt,0x83);
  nxt_pack_byte(nxt,handle);
  nxt_pack_mem(nxt,src,count);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,6));
  test(nxt_unpack_start(nxt,0x83));
  if (nxt_unpack_error(nxt)==0) {
    nxt_unpack_byte(nxt);
    return nxt_unpack_word(nxt);
  }
  else return NXT_FAIL;
}

/**
 * Writes data to file from NXT
 *  @param nxt NXT handle
 *  @param handle File handle
 *  @param src Data to write in file
 *  @param count How many bytes to write
 *  @return How many bytes written
 */
ssize_t nxt_file_write(nxt_t *nxt,int handle,void *src,size_t count) {
  int i,partlen;
  int len = 0;
  for (i=0;i<count/50;i++) {
    partlen = nxt_file_write_low(nxt,handle,src,50);
    if (partlen==-1) return -1;
    src += partlen;
    len += partlen;
  }
  if ((partlen = nxt_file_write_low(nxt,handle,src,count%50))==-1) return NXT_FAIL;
  len += partlen;
  return len;
}

/**
 * Closes file from NXT
 *  @param nxt NXT handle
 *  @param handle File handle
 *  @return Success?
 */
int nxt_file_close(nxt_t *nxt,int handle) {
  nxt_pack_start(nxt,0x84);
  nxt_pack_byte(nxt,handle);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,4));
  test(nxt_unpack_start(nxt,0x84));
  return nxt_unpack_error(nxt)==0?NXT_SUCC:NXT_FAIL;
}

/**
 * Removes file from NXT
 *  @param nxt NXT handle
 *  @param file Filename
 *  @return Whether file could be removed
 */
int nxt_file_remove(nxt_t *nxt,const char *file) {
  nxt_pack_start(nxt,0x85);
  nxt_pack_str(nxt,file,20);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,23));
  test(nxt_unpack_start(nxt,0x85));
  return nxt_unpack_error(nxt)==0?NXT_SUCC:NXT_FAIL;
}

/**
 * Finds a file
 *  @param nxt NXT handle
 *  @param wildcard Wildcard to search for
 *  @param filename Reference to store filename of found file as string
 *  @param filesize Reference to store filesize of found file
 *  @return File handle of found file
 *  @note The pointer 'filename' can and should be passed to free()
 *  @note If the file is not needed it should be passed to nxt_file_close()
 */
int nxt_file_find_first(nxt_t *nxt,const char *wildcard,char **filename,size_t *filesize) {
  nxt_pack_start(nxt,0x86);
  nxt_pack_str(nxt,wildcard,20);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,28));
  test(nxt_unpack_start(nxt,0x86));
  if (nxt_unpack_error(nxt)==0) {
    int handle = nxt_unpack_byte(nxt);
    if (filename!=NULL) *filename = strdup(nxt_unpack_str(nxt,20));
    if (filesize!=NULL) *filesize = nxt_unpack_dword(nxt);
    return handle;
  }
  else return NXT_FAIL;
}

/**
 * Finds a file
 *  @param nxt NXT handle
 *  @param handle Filehandle of previous found file
 *  @param filename Reference to store filename of found file as string
 *  @param filesize Reference to store filesize of found file
 *  @return File handle of found file
 *  @note The pointer 'filename' can and should be passed to free()
 *  @note If the file is not needed it should be passed to nxt_file_close()
 */
int nxt_file_find_next(nxt_t *nxt,int handle,char **filename,size_t *filesize) {
  nxt_pack_start(nxt,0x87);
  nxt_pack_byte(nxt,handle);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,28));
  test(nxt_unpack_start(nxt,0x87));
  if (nxt_unpack_error(nxt)==0) {
    int handle = nxt_unpack_byte(nxt);
    if (filename!=NULL) *filename = strdup(nxt_unpack_str(nxt,20));
    if (filesize!=NULL) *filesize = nxt_unpack_dword(nxt);
    return handle;
  }
  else return NXT_FAIL;
}
