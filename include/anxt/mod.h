/*
    include/anxt/mod.h
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

#ifndef _ANXT_MOD_H_
#define _ANXT_MOD_H_

#include <sys/types.h>

#include <anxt/nxt.h>

// Module programmers
#define NXT_MOD_LEGO 0x01

// Module types
#define NXT_MOD_CMD      0x01
#define NXT_MOD_OUTPUT   0x02
#define NXT_MOD_INPUT    0x03
#define NXT_MOD_BUTTON   0x04
#define NXT_MOD_COMM     0x05
#define NXT_MOD_IOCTRL   0x06
#define NXT_MOD_LED      0x07
#define NXT_MOD_SOUND    0x08
#define NXT_MOD_LOADER   0x09
#define NXT_MOD_DISPLAY  0x0A
#define NXT_MOD_LOWSPEED 0x0B
#define NXT_MOD_UI       0x0C

// Display module
#define NXT_DISPLAY_MODFILE "Display.mod"
#define NXT_DISPLAY_BITMAP  119

// UI Module
#define NXT_UI_MODFILE "Ui.mod"
#define NXT_UI_BUTTON  28
#define NXT_UI_VOLUME  36
#define NXT_UI_TURNOFF 39

// Output Module
#define NXT_OUTPUT_MODFILE  "Output.mod"
#define NXT_OUTPUT_SPEED(m) ((m)*32+20)
#define NXT_OUTPUT_PID(m)   ((m)*32+22)

// Buttons
#define NXT_UI_BUTTON_LEFT  1 // Left arrow button
#define NXT_UI_BUTTON_ENTER 2 // Enter button
#define NXT_UI_BUTTON_RIGHT 3 // Right arrow button
#define NXT_UI_BUTTON_EXIT  4 // Exit button

int nxt_mod_first(nxt_t *nxt,char *wildcard,char **modname,int *modid,size_t *modsz,size_t *iomapsz);
int nxt_mod_next(nxt_t *nxt,int handle,char **modname,int *modid,size_t *modsz,size_t *iomapsz);
int nxt_mod_close(nxt_t *nxt,int handle);
ssize_t nxt_mod_read(nxt_t *nxt,int modid,void *buf,size_t offset,size_t size);
ssize_t nxt_mod_write(nxt_t *nxt,int modid,const void *buf,off_t offset,size_t size);
int nxt_mod_get_id(nxt_t *nxt,char *file);

int nxt_get_volume(nxt_t *nxt);
int nxt_set_volume(nxt_t *nxt,int volume);
int nxt_turn_off(nxt_t *nxt);
int nxt_set_button(nxt_t *nxt,int button);
int nxt_screenshot(nxt_t *nxt,char buf[64][100]);

#endif /* _ANXT_MOD_H_ */
