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
