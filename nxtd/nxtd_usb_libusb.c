/*
    nxtd_usb_libusb.c
    aNXT - a NXt Toolkit
    Libraries and tools for LEGO Mindstorms NXT robots
    Copyright (C) 2010  Janosch Gräf <janosch.graef@gmx.net>

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
#include <libusb.h>
#include <time.h>
#include <stdlib.h>

#include "nxtd.h"
#include "nxtd_usb_libusb.h"

static libusb_context *nxtd_usb_context;


int nxtd_usb_init() {
  if (libusb_init(&nxtd_usb_context)!=0) {
    return -1;
  }
  libusb_set_debug(nxtd_usb_context, 0);
  return 0;
}

void nxtd_usb_shutdown() {
  libusb_exit(nxtd_usb_context);
}

static void nxtd_usb_timeout(struct nxtd_nxt_usb *nxt) {
#ifdef NXT_USB_IDLE_TIMEOUT
  nxt->nxt.conn_timeout = time(NULL)+NXT_USB_IDLE_TIMEOUT;
#else
  nxt->nxt.conn_timeout = 0;
#endif
}

/**
 * Gets name of NXT device
 *  @param handle USB handle
 *  @param id Reference for pointer to id
 *  @return Name of NXT
 */
static char *nxtd_usb_getname(libusb_device_handle *handle, char **id) {
  static char buffer[33];
  int transferred;

  // send
  buffer[0] = 0x01;
  buffer[1] = 0x9B;
  if (libusb_bulk_transfer(handle, NXT_USB_OUT_ENDPOINT, buffer, 3, &transferred, NXT_USB_WAIT_TIMEOUT)!=0 || transferred!=3) {
    return NULL;
  }

  // receive
  if (libusb_bulk_transfer(handle, NXT_USB_IN_ENDPOINT, buffer, 33, &transferred, NXT_USB_WAIT_TIMEOUT)!=0 || transferred!=33) {
    return NULL;
  }
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
static libusb_device_handle *nxtd_usb_opendev(libusb_device *dev) {
  libusb_device_handle *handle;

  if (libusb_open(dev, &handle)!=0) {
    return NULL;
  }
  if (libusb_reset_device(handle)!=0) {
    return NULL;
  }
  if (libusb_set_configuration(handle, NXT_USB_CONFIG)!=0) {
    return NULL;
  }
  if (libusb_claim_interface(handle, NXT_USB_INTERFACE)!=0) {
    return NULL;
  }
  /*if (libusb_set_interface_alt_setting(handle, NXT_USB_INTERFACE)) {
    return NULL;
  }*/

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
struct nxtd_nxt_usb *nxtd_usb_finddev(libusb_device *dev) {
  struct nxtd_nxt_usb *nxt;
  size_t i;

  for (i=0;i<NXTD_MAXNUM;i++) {
    if (nxts.list[i]!=NULL && nxts.list[i]->conn_type==NXTD_USB) {
      nxt = (struct nxtd_nxt_usb*)nxts.list[i];
      if (nxt->usb_dev==dev) {
        return nxt;
      }
    }
  }

  return NULL;
}

/**
 * Scans for NXTs on USB
 *  @return Success?
 */
int nxtd_usb_scan() {
  libusb_device **devlist;
  int i;
  struct libusb_device_descriptor devdesc;
  libusb_device_handle *handle;
  char *id;
  char *name;
  struct nxtd_nxt_usb *nxt;

  libusb_get_device_list(nxtd_usb_context, &devlist);
  for (i=0; devlist[i]!=NULL; i++) {
    // check if we don't already have this device listed
    if (nxtd_usb_finddev(devlist[i])==NULL) {
      // get USB device descriptor
      if (libusb_get_device_descriptor(devlist[i], &devdesc)==0) {
        // check against vendor ID and product ID
        if (devdesc.idVendor==NXT_USB_VENDORID && devdesc.idProduct==NXT_USB_PRODUCTID) {
          // open device
          handle = nxtd_usb_opendev(devlist[i]);
          if (handle!=NULL) {
            // get NXT name
            name = nxtd_usb_getname(handle, &id);
            if (name!=NULL) {
              // put NXT into list
              nxt = malloc(sizeof(struct nxtd_nxt_usb));
              memcpy(&nxt->nxt.id, id, sizeof(nxtd_id_t));
              nxt->nxt.name = strdup(name);
              nxt->nxt.conn_type = NXTD_USB;
              nxt->nxt.conn_timeout = 0;
              nxt->usb_handle = NULL; // TODO why set this NULL and close handle?
              nxt->usb_dev = devlist[i];
              libusb_ref_device(devlist[i]);
              nxtd_nxt_reg((struct nxtd_nxt*)nxt);
              nxtd_usb_timeout(nxt);
            }
            libusb_close(handle); // TODO why do we close this, even if NXT is added
          }
        }
      }
    }
  }

  libusb_free_device_list(devlist, 1);

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
    else {
      return -1;
    }
  }
  else {
    return 0;
  }
}

/**
 * Disconnect from NXT over USB
 *  @param nxt NXT to disconnect from
 *  @return Success?
 */
int nxtd_usb_disconnect(struct nxtd_nxt_usb *nxt) {
  if (nxt->usb_handle!=NULL) {
    libusb_release_interface(nxt->usb_handle, NXT_USB_INTERFACE);
    libusb_close(nxt->usb_handle);
    libusb_unref_device(nxt->usb_dev);
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
ssize_t nxtd_usb_send(struct nxtd_nxt_usb *nxt, const void *data, size_t size) {
  ssize_t transferred;
  size_t absolute = 0;

  nxtd_usb_connect(nxt);

  while (absolute<size) {
    if (libusb_bulk_transfer(nxt->usb_handle, NXT_USB_OUT_ENDPOINT, (void*)data+absolute, size-absolute, &transferred, NXT_USB_WAIT_TIMEOUT)!=0) {
      return -1;
    }
    absolute += transferred;
  }

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
  ssize_t transferred;
  size_t absolute = 0;

  nxtd_usb_connect(nxt);

  do {
    if (libusb_bulk_transfer(nxt->usb_handle, NXT_USB_IN_ENDPOINT, data+absolute, size-absolute, &transferred, NXT_USB_WAIT_TIMEOUT)!=0) {
      return -1;
    }
    absolute += transferred;
  }
  while (absolute<size);

  nxtd_usb_timeout(nxt);

  return absolute;
}
