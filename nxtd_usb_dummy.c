/*
    nxtd_usb_dummy.c
    aNXT - a NXt Toolkit
    Libraries and tools for LEGO Mindstorms NXT robots
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

#include <sys/types.h>

#include "nxtd.h"

int nxtd_usb_init() {
  return 0;
}
void nxtd_usb_shutdown() {
}
void nxtd_usb_close(struct nxtd_nxt_usb *nxt) {
}
void nxtd_usb_scan() {
}
int nxtd_usb_connect(struct nxtd_nxt_usb *nxt) {
  return 0;
}
int nxtd_usb_disconnect(struct nxtd_nxt_usb *nxt) {
  return 0;
}
ssize_t nxtd_usb_send(struct nxtd_nxt_usb *nxt,const void *data,size_t size) {
  return 0;
}
ssize_t nxtd_usb_recv(struct nxtd_nxt_usb *nxt,void *data,size_t size) {
  return 0;
}
