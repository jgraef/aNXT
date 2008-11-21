#include <sys/types.h>

#include "nxtd.h"

// Bluetooth Dummy driver

int nxtd_bt_init() {
  return 0;
}
void nxtd_bt_shutdown() {
}
void nxtd_bt_close(struct nxtd_nxt_bt *nxt) {
}
void nxtd_bt_scan() {
}
int nxtd_bt_connect(struct nxtd_nxt_bt *nxt) {
  return 0;
}
int nxtd_bt_disconnect(struct nxtd_nxt_bt *nxt) {
  return 0;
}
ssize_t nxtd_bt_send(struct nxtd_nxt_bt *nxt,const void *data,size_t size) {
  return 0;
}
ssize_t nxtd_bt_recv(struct nxtd_nxt_bt *nxt,void *data,size_t size) {
  return 0;
}
