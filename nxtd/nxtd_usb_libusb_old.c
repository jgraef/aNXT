/*
    nxtd_usb_libusb.c
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
#include <usb.h>
#include <time.h>

#include "nxtd.h"
#include "nxtd_usb_libusb_old.h"

#ifdef NXT_USB_IDLE_TIMEOUT
  #define nxtd_usb_timeout(n) (n)->nxt.conn_timeout = time(NULL)+NXT_USB_IDLE_TIMEOUT;
#else
  #define nxtd_usb_timeout(n) (n)->nxt.conn_timeout = 0;
#endif

int nxtd_usb_init() {
  usb_init();
  usb_set_debug(0);
  return 0;
}

void nxtd_usb_shutdown() {
}

/**
 * Gets name of NXT device
 *  @param handle USB handle
 *  @param id Reference for pointer to id
 *  @return Name of NXT
 */
static char *nxtd_usb_getname(struct usb_dev_handle *handle, char **id) {
  static char buffer[33];
  // send
  buffer[0] = 0x01;
  buffer[1] = 0x9B;
  usb_bulk_write(handle,NXT_USB_OUT_ENDPOINT,buffer,3,NXT_USB_WAIT_TIMEOUT);

  // receive
  usb_bulk_read(handle,NXT_USB_IN_ENDPOINT,buffer,33,NXT_USB_WAIT_TIMEOUT);
  if (buffer[2]!=0) {
     return NULL;
  }
  else {
    if (id!=NULL) {
      *id = buffer+18;
    }
    return buffer+3;
  }
}

/**
 * Opens a USB handle for NXT
 *  @param dev USB device
 *  @return USB handle
 */
static struct usb_dev_handle *nxtd_usb_opendev(struct usb_device *dev) {
  struct usb_dev_handle *handle = usb_open(dev);
  if (handle==NULL) return NULL;
  if (usb_reset(handle)) return NULL;
  if (usb_set_configuration(handle,NXT_USB_CONFIG)) return NULL;
  if (usb_claim_interface(handle,NXT_USB_INTERFACE)) return NULL;
  if (usb_set_altinterface(handle,NXT_USB_INTERFACE)) return NULL;
  return handle;
}

/**
 * Closes a NXT over USB
 *  @param nxt NXT handle
 */
void nxtd_usb_close(struct nxtd_nxt_usb *nxt) {
  nxtd_usb_disconnect(nxt);
  free(nxt->nxt.name);
  free(nxt);
}

/**
 * Find NXT by USB Device
 *  @param dev USB Device
 *  @return NXT
 */
struct nxtd_nxt_usb *nxtd_usb_finddev(struct usb_device *dev) {
  struct nxtd_nxt_usb *nxt;
  size_t i;

  for (i=0;i<NXTD_MAXNUM;i++) {
    if (nxts.list[i]!=NULL && nxts.list[i]->conn_type==NXTD_USB) {
      nxt = (struct nxtd_nxt_usb*)nxts.list[i];
      if (nxt->usb_dev==dev) return nxt;
    }
  }

  return NULL;
}

/**
 * Scans for NXTs on USB
 *  @return Success?
 */
int nxtd_usb_scan() {
  struct usb_bus *bus;
  struct usb_device *dev;
  struct usb_dev_handle *handle;
  struct nxtd_nxt_usb *nxt;
  char *id;

  usb_find_busses();
  usb_find_devices();
  for (bus = usb_busses; bus; bus = bus->next) {
    for (dev = bus->devices; dev; dev = dev->next) {
      if (nxtd_usb_finddev(dev)==NULL && dev->descriptor.idVendor==NXT_USB_VENDORID && dev->descriptor.idProduct==NXT_USB_PRODUCTID) {
        handle = nxtd_usb_opendev(dev);
        if (handle!=NULL) {
          char *name = nxtd_usb_getname(handle, &id);
          if (name!=NULL) {
            nxt = malloc(sizeof(struct nxtd_nxt_usb));
            memcpy(&nxt->nxt.id, id, sizeof(nxtd_id_t));
            nxt->nxt.name = strdup(name);
            nxt->nxt.conn_type = NXTD_USB;
            nxt->nxt.conn_timeout = 0;
            nxt->usb_handle = NULL;
            nxt->usb_dev = dev;
            nxtd_nxt_reg((struct nxtd_nxt*)nxt);
          }
          usb_close(handle);
        }
      }
    }
  }

  return 0;
}

/**
 * Connect to NXT over USB
 *  @param nxt NXT to connect to
 *  @return Success?
 */
int nxtd_usb_connect(struct nxtd_nxt_usb *nxt) {
  if (nxt->usb_handle==NULL) {
    nxt->usb_handle = nxtd_usb_opendev(nxt->usb_dev);
    if (nxt->usb_handle!=NULL) {
      nxtd_usb_timeout(nxt);
      return 0;
    }
    else return -1;
  }
  else return 0;
}

/**
 * Disconnect from NXT over USB
 *  @param nxt NXT to disconnect from
 *  @return Success?
 */
int nxtd_usb_disconnect(struct nxtd_nxt_usb *nxt) {
  if (nxt->usb_handle!=NULL) {
    usb_release_interface(nxt->usb_handle,NXT_USB_INTERFACE);
    usb_close(nxt->usb_handle);
    nxt->usb_handle = NULL;
    nxt->nxt.conn_timeout = 0;
  }
  return 0;
}

/**
 * Sends data over USB
 *  @param nxt NXT handle
 *  @param data Data to send
 *  @param size How many bytes to send
 *  @return How many bytes sent
 */
ssize_t nxtd_usb_send(struct nxtd_nxt_usb *nxt,const void *data,size_t size) {
  nxtd_usb_connect(nxt);

  ssize_t send;
  size_t absolute = 0;
#ifdef DEBUG
  char *data_chr = data;
  int i;
  fprintf(stderr, "nxt_usb_send to %x\n", nxt->usb_handle);
  for (i = 0; i < size; i++) {
    fprintf(stderr, "nxt_usb_send buffer[%d]: %#2.2hhx", i, data_chr[i]);
    if (isascii(data_chr[i]) && (data_chr[i] > ' '))
      fprintf(stderr, ": %c", data_chr[i]);
    fprintf(stderr,"\n");
  }
#endif
  do {
    if ((send = usb_bulk_write(nxt->usb_handle,NXT_USB_OUT_ENDPOINT,(void*)data+absolute,size-absolute,NXT_USB_WAIT_TIMEOUT))<0) return -1;
#ifdef DEBUG
    fprintf(stderr, "usb_bulk_write send %d bytes\n", send);
#endif
    absolute += send;
  }
  while (absolute<size);
#ifdef DEBUG
  fprintf(stderr, "usb_bulk_write returned %d\n", absolute);
#endif
  nxtd_usb_timeout(nxt);
  return absolute;
}

/**
 * Receives data over USB
 *  @param nxt NXT handle
 *  @param data Buffer for data
 *  @param size How many bytes to receive
 *  @return How many bytes received
 */
ssize_t nxtd_usb_recv(struct nxtd_nxt_usb *nxt,void *data,size_t size) {
  nxtd_usb_connect(nxt);

#ifdef DEBUG
  int i;
  char *data_chr = data;
#endif
  ssize_t recv;
  size_t absolute = 0;
  do {
    if ((recv = usb_bulk_read(nxt->usb_handle,NXT_USB_IN_ENDPOINT,data+absolute,size-absolute,NXT_USB_WAIT_TIMEOUT))<0) return -1;
#ifdef DEBUG
    fprintf(stderr, "nxt_usb_read received %d bytes\n", recv);
#endif
    absolute += recv;
  }
  while (absolute<size);
#ifdef DEBUG
  fprintf(stderr, "nxt_usb_read from %x\n", nxt->usb_handle);
  for (i = 0; i < size; i++) {
    fprintf(stderr, "nxt_usb_recv buffer[%d]: %#2.2hhx", i, nxt->buffer[i]);
    if (isascii(data_chr[i]) && (data_chr[i] > ' '))
      fprintf(stderr, ": %c", data_chr[i]);
    fprintf(stderr,"\n");
  }
  fprintf(stderr, "usb_bulk_read returned %d\n", absolute);
#endif
  nxtd_usb_timeout(nxt);
  return absolute;
}
