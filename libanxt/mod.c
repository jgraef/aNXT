/*
    libanxt/mod.c
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

#include <anxt/nxt.h>
#include <anxt/mod.h>
#include "private.h"

/**
 * Finds first module
 *  @param nxt NXT handle
 *  @param wildcard Module name wildcard
 *  @param modname Reference for module name
 *  @param modid Reference for module ID
 *  @param modsz Reference for module size
 *  @param iomapsz Reference for module size in IOMAP
 *  @return Handle
 */
int nxt_mod_first(nxt_t *nxt,char *wildcard,char **modname,int *modid,size_t *modsz,size_t *iomapsz) {
  nxt_pack_start(nxt,0x90);
  nxt_pack_str(nxt,wildcard,20);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,34));
  test(nxt_unpack_start(nxt,0x90));
  if (nxt_unpack_error(nxt)==0) {
    int handle = nxt_unpack_byte(nxt);
    if (modname!=NULL) *modname = strdup(nxt_unpack_str(nxt,20));
    else nxt_unpack_str(nxt,20);
    if (modid!=NULL) *modid = nxt_unpack_dword(nxt);
    else nxt_unpack_dword(nxt);
    if (modsz!=NULL) *modsz = nxt_unpack_dword(nxt);
    else nxt_unpack_dword(nxt);
    if (iomapsz!=NULL) *iomapsz = nxt_unpack_word(nxt);
    else nxt_unpack_word(nxt);
    return handle;
  }
  else return NXT_FAIL;
}

/**
 * Finds next module
 *  @param nxt NXT handle
 *  @param handle Handle of previous found module
 *  @param modname Reference for module name
 *  @param modid Reference for module ID
 *  @param modsz Reference for module size
 *  @param iomapsz Reference for module size in IOMAP
 *  @return Handle
 */
int nxt_mod_next(nxt_t *nxt,int handle,char **modname,int *modid,size_t *modsz,size_t *iomapsz) {
  nxt_pack_start(nxt,0x91);
  nxt_pack_byte(nxt,handle);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,34));
  test(nxt_unpack_start(nxt,0x91));
  if (nxt_unpack_error(nxt)==0) {
    handle = nxt_unpack_byte(nxt);
    if (modname!=NULL) *modname = strdup(nxt_unpack_str(nxt,20));
    else nxt_unpack_str(nxt,20);
    if (modid!=NULL) *modid = nxt_unpack_dword(nxt);
    else nxt_unpack_dword(nxt);
    if (modsz!=NULL) *modsz = nxt_unpack_dword(nxt);
    else nxt_unpack_dword(nxt);
    if (iomapsz!=NULL) *iomapsz = nxt_unpack_word(nxt);
    else nxt_unpack_word(nxt);
    return handle;
  }
  else return NXT_FAIL;
}

/**
 * Closes module handle
 *  @param nxt NXT handle
 *  @param handle Module handle
 *  @return Success?
 */
int nxt_mod_close(nxt_t *nxt,int handle) {
  nxt_pack_start(nxt,0x92);
  nxt_pack_byte(nxt,handle);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,4));
  test(nxt_unpack_start(nxt,0x92));
  return nxt_unpack_error(nxt)==0?NXT_SUCC:NXT_FAIL;
}

/**
 * Reads from IOMap (Lowlevel)
 *  @param nxt NXT handle
 *  @param modid Module ID
 *  @param buf Buffer
 *  @param offset Data offset
 *  @param size Data size
 *  @return How many bytes read
 */
static ssize_t nxt_mod_read_low(nxt_t *nxt,int modid,void *buf,size_t offset,size_t size) {
  nxt_pack_start(nxt,0x94);
  nxt_pack_dword(nxt,modid);
  nxt_pack_word(nxt,offset);
  nxt_pack_word(nxt,size);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,9+size));
  test(nxt_unpack_start(nxt,0x94));
  if (nxt_unpack_error(nxt)==0) {
    nxt_unpack_dword(nxt); // modid
    size = nxt_unpack_word(nxt);
    memcpy(buf,nxt_unpack_mem(nxt,size),size);
    return size;
  }
  else return NXT_FAIL;
}

/**
 * Reads from IOMap
 *  @param nxt NXT handle
 *  @param modid Module ID
 *  @param buf Buffer
 *  @param offset Data offset
 *  @param size Data size
 *  @return How many bytes read
 */
ssize_t nxt_mod_read(nxt_t *nxt,int modid,void *buf,size_t offset,size_t size) {
  int i;
  ssize_t partlen;
  int len = 0;
  for (i=0;i<size/50;i++) {
    partlen = nxt_mod_read_low(nxt,modid,buf,offset,50);
    if (partlen==-1) return -1;
    buf += partlen;
    len += partlen;
    offset += partlen;
  }
  if ((partlen = nxt_mod_read_low(nxt,modid,buf,offset,size%50))==-1) return NXT_FAIL;
  len += partlen;
  return len;
}

/**
 * Writes to IOMap (Lowlevel)
 *  @param nxt NXT handle
 *  @param modid Module ID
 *  @param buf Buffer
 *  @param offset Data offset
 *  @param size Data size
 *  @return How many bytes written
 */
static ssize_t nxt_mod_write_low(nxt_t *nxt,int modid,const void *buf,off_t offset,size_t size) {
  nxt_pack_start(nxt,0x95);
  nxt_pack_dword(nxt,modid);
  nxt_pack_word(nxt,offset);
  nxt_pack_word(nxt,size);
  nxt_pack_mem(nxt,(void*)buf,size);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,9));
  test(nxt_unpack_start(nxt,0x95));
  if (nxt_unpack_error(nxt)==0) {
    nxt_unpack_dword(nxt); // modid
    return nxt_unpack_word(nxt);
  }
  else return NXT_FAIL;
}

/**
 * Writes to IOMap
 *  @param nxt NXT handle
 *  @param modid Module ID
 *  @param buf Buffer
 *  @param offset Data offset
 *  @param size Data size
 *  @return How many bytes written
 */
ssize_t nxt_mod_write(nxt_t *nxt,int modid,const void *buf,off_t offset,size_t size) {
  int i;
  ssize_t partlen;
  int len = 0;
  for (i=0;i<size/50;i++) {
    partlen = nxt_mod_write_low(nxt,modid,buf,offset,50);
    if (partlen==-1) return NXT_FAIL;
    buf += partlen;
    len += partlen;
    offset += partlen;
  }
  if ((partlen = nxt_mod_write_low(nxt,modid,buf,offset,size%50))==-1) return NXT_FAIL;
  len += partlen;
  return len;
}

/**
 * Gets module ID
 *  @param nxt NXT handle
 *  @param file Module file
 *  @return Module ID
 */
int nxt_mod_get_id(nxt_t *nxt,char *file) {
  int handle,modid;
  if ((handle = nxt_mod_first(nxt,file,NULL,&modid,NULL,NULL))==0) {
    nxt_mod_close(nxt,handle);
    return modid;
  }
  else return -1;
}

/**
 * Gets volume from NXT
 *  @param nxt NXT handle
 *  @return Volume
 */
int nxt_get_volume(nxt_t *nxt) {
  int modid;
  if ((modid = nxt_mod_get_id(nxt,NXT_UI_MODFILE))!=-1) {
    char volume;
    if (nxt_mod_read(nxt,modid,&volume,NXT_UI_VOLUME,1)==1) return volume;
  }
  else return -1;
}

/**
 * Sets volume on NXT
 *  @param nxt NXT handle
 *  @param volume Volume
 */
int nxt_set_volume(nxt_t *nxt,int volume) {
  int modid;
  char vol = volume;
  if (NXT_VALID_VOLUME(volume) && (modid = nxt_mod_get_id(nxt,NXT_UI_MODFILE))!=-1) {
    if (nxt_mod_write(nxt,modid,&vol,NXT_UI_VOLUME,1)==1) return 0;
  }
  else return -1;
}

/**
 * Turns NXT off
 *  @param nxt NXT handle
 *  @note NXT handle must be closed after this
 */
int nxt_turn_off(nxt_t *nxt) {
  int modid;
  char off = 1;
  if ((modid = nxt_mod_get_id(nxt,NXT_UI_MODFILE))!=-1) {
    if (nxt_mod_write(nxt,modid,&off,NXT_UI_TURNOFF,1)==1) return 0;
  }
  else return -1;
}

/**
 * "Press" button on NXT
 *  @param nxt NXT handle
 *  @param button Button
 */
int nxt_set_button(nxt_t *nxt,int button) {
  int modid;
  char btn = button;
  if ((modid = nxt_mod_get_id(nxt,NXT_UI_MODFILE))!=-1) {
    if (nxt_mod_write(nxt,modid,&btn,NXT_UI_BUTTON,1)==1) return 0;
  }
  else return -1;
}

/**
 * Gets screenshot of NXT
 *  @param nxt NXT handle
 *  @param buf Buffer
 *  @return Success?
 */
int nxt_screenshot(nxt_t *nxt,char buf[64][100]) {
  char screen[8][100];
  int modid,x,y;
  if ((modid = nxt_mod_get_id(nxt,NXT_DISPLAY_MODFILE))!=-1) {
    nxt_mod_read(nxt,modid,screen,NXT_DISPLAY_BITMAP,800);
    for (y=0;y<64;y++) {
      for (x=0;x<100;x++) buf[y][x] = screen[y/8][x]&(1<<(y%8));
    }
    return 0;
  }
  else return -1;
}
