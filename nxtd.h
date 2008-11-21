/*
    nxtd.h
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

#ifndef _NXTD_H_
#define _NXTD_H_

#include <sys/types.h>
#include <usb.h>
#include <bluetooth/bluetooth.h>

#define NXT_MAXNUM 256

struct nxtd_nxt {
  char *nxt_name;
  enum {
    CONN_USB,
    CONN_BT
  } conn_type;
};

struct nxtd_nxt_usb {
  struct nxtd_nxt nxt;
  struct usb_dev_handle *usb_handle;
  struct usb_device *usb_dev;
};

struct nxtd_nxt_bt {
  struct nxtd_nxt nxt;
  int bt_sock;
  bdaddr_t bt_addr;
  int connected;
};

struct nxtd_nxt *nxts[NXT_MAXNUM];

int nxtd_nxt_reg(struct nxtd_nxt *nxt);

// USB
int nxtd_usb_init();
void nxtd_usb_shutdown();
void nxtd_usb_close(struct nxtd_nxt_usb *nxt);
void nxtd_usb_scan();
int nxtd_usb_connect(struct nxtd_nxt_usb *nxt);
int nxtd_usb_disconnect(struct nxtd_nxt_usb *nxt);
ssize_t nxtd_usb_send(struct nxtd_nxt_usb *nxt,const void *data,size_t size);
ssize_t nxtd_usb_recv(struct nxtd_nxt_usb  *nxt,void *data,size_t size);

// Bluetooth
int nxtd_bt_init();
void nxtd_bt_shutdown();
void nxtd_bt_close(struct nxtd_nxt_bt *nxt);
void nxtd_bt_scan();
int nxtd_bt_connect(struct nxtd_nxt_bt *nxt);
int nxtd_bt_disconnect(struct nxtd_nxt_bt *nxt);
ssize_t nxtd_bt_send(struct nxtd_nxt_bt *nxt,const void *data,size_t size);
ssize_t nxtd_bt_recv(struct nxtd_nxt_bt *nxt,void *data,size_t size);

#endif
