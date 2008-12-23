/*
    nxtd_bt_dummy.c
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

#ifndef _NXTD_USB_LIBUSB_H_
#define _NXTD_USB_LIBUSB_H_

#include <sys/types.h>
#include <usb.h>

#include "nxtd.h"

#define NXT_USB_VENDORID      0x0694
#define NXT_USB_PRODUCTID     0x0002
#define NXT_USB_OUT_ENDPOINT  0x01
#define NXT_USB_IN_ENDPOINT   0x82
#define NXT_USB_INTERFACE     0
#define NXT_USB_CONFIG        1
#define NXT_USB_WAIT_TIMEOUT  2000 /* milliseconds */
#define NXT_USB_IDLE_TIMEOUT  30   /* seconds */

struct nxtd_nxt_usb {
  struct nxtd_nxt nxt;
  struct usb_dev_handle *usb_handle;
  struct usb_device *usb_dev;
};

int nxtd_usb_init();
void nxtd_usb_shutdown();
void nxtd_usb_close(struct nxtd_nxt_usb *nxt);
int nxtd_usb_scan();
int nxtd_usb_connect(struct nxtd_nxt_usb *nxt);
int nxtd_usb_disconnect(struct nxtd_nxt_usb *nxt);
ssize_t nxtd_usb_send(struct nxtd_nxt_usb *nxt,const void *data,size_t size);
ssize_t nxtd_usb_recv(struct nxtd_nxt_usb  *nxt,void *data,size_t size);

#endif /* _NXTD_USB_LIBUSB_H_ */
