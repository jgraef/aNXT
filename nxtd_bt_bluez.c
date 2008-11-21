#include <sys/types.h>
#include <stdint.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/rfcomm.h>

#include "nxtd.h"

#define NXT_BT_DEVCLASS_BYTE0 4
#define NXT_BT_DEVCLASS_BYTE1 8
#define NXT_BT_DEVCLASS_BYTE2 0

/**
 * Identifies a NXT over Bluetooth
 *  @param class Device class
 *  @param sock Socket
 *  @return Whether device is a NXT
 */
static int nxt_bt_identify(uint8_t class[3],int sock) {
  return (class[0]==NXT_BT_DEVCLASS_BYTE0) && 
         (class[1]==NXT_BT_DEVCLASS_BYTE1) &&
         (class[2]==NXT_BT_DEVCLASS_BYTE2);
}

/**
 * Initializes Bluetooth
 *  @return Success
 */
int nxtd_bt_init() {
  return 0;
}

/**
 * Shuts down Bluetooth
 */
void nxtd_bt_shutdown() {
}

/**
 * Closes a NXT over BT
 *  @param nxt NXT handle
 */
void nxtd_bt_close(struct nxtd_nxt_bt *nxt) {
  nxtd_bt_disconnect(nxt);
  free(nxt->nxt.nxt_name);
  free(nxt);
}

/**
 * Scans for NXTs on BT
 */
void nxtd_bt_scan() {
  inquiry_info *ii;
  char bt_name[248]= { 0 };
  int dev_id,sock,max_rsp,num_rsp,len,flags,i;
  bdaddr_t bt_addr;

  bacpy(&bt_addr,BDADDR_ANY);

  if ((dev_id = hci_get_route(NULL))<0) return;
  if ((sock = hci_open_dev(dev_id))<0) return;

  len  = 8;
  max_rsp = 255;
  flags = IREQ_CACHE_FLUSH;
  ii = malloc(max_rsp*sizeof(inquiry_info));
  num_rsp = hci_inquiry(dev_id,len,max_rsp,NULL,&ii,flags);

  for (i=0;i<num_rsp;i++) {
    memset(bt_name,0,sizeof(bt_name));
    hci_read_remote_name(sock,&(ii+i)->bdaddr,sizeof(bt_name),bt_name,0);
    if (nxt_bt_identify((ii+i)->dev_class,sock)) {
      struct nxtd_nxt_bt *nxt = malloc(sizeof(struct nxtd_nxt_bt));
      memset(nxt,0,sizeof(struct nxtd_nxt_bt));
      nxt->nxt.conn_type = CONN_BT;
      nxt->nxt.nxt_name = strdup(bt_name);
      bacpy(&(nxt->bt_addr),&(ii+i)->bdaddr);
      nxtd_nxt_reg((struct nxtd_nxt*)nxt);
    }
  }

  close(sock);
  free(ii);
}

/**
 * Connect to NXT over Bluetooth
 *  @param nxt NXT to connect to
 *  @return Success?
 */
int nxtd_bt_connect(struct nxtd_nxt_bt *nxt) {
  if (!nxt->connected) {
    struct sockaddr_rc addr = {0};
    int sock;
    size_t i;

    sock = socket(AF_BLUETOOTH,SOCK_STREAM,BTPROTO_RFCOMM);
    addr.rc_family = AF_BLUETOOTH;
    bacpy(&addr.rc_bdaddr,&(nxt->bt_addr));

    for (i=1;i<31;i++) {
      addr.rc_channel = i;
      if (connect(sock, (struct sockaddr *)&addr, sizeof(addr))==0) {
        nxt->bt_sock = sock;
        nxt->connected = 1;
        return 0;
      }
    }

    return -1;
  }
  else return 0;
}

/**
 * Disconnect from NXT over Bluetooth
 *  @param nxt NXT to disconnect from
 *  @return Success?
 */
int nxtd_bt_disconnect(struct nxtd_nxt_bt *nxt) {
  if (nxt->connected) close(nxt->bt_sock);
  return 0;
}

/**
 * Sends data over BT
 *  @param nxt NXT handle
 *  @param data Data to send
 *  @param size How many bytes to send
 *  @return How many bytes sent
 */
ssize_t nxtd_bt_send(struct nxtd_nxt_bt *nxt,const void *data,size_t size) {
  nxtd_bt_connect(nxt);
  uint16_t sz = size;
  if (write(nxt->bt_sock,&sz,sizeof(sz))<0) return 0;
  return write(nxt->bt_sock,data,size);
}

/**
 * Receives data over BT
 *  @param nxt NXT handle
 *  @param data Buffer for data
 *  @param size How many bytes to receive
 *  @return How many bytes received
 */
ssize_t nxtd_bt_recv(struct nxtd_nxt_bt *nxt,void *data,size_t size) {
  nxtd_bt_connect(nxt);
  uint16_t sz;
  if (read(nxt->bt_sock,&sz,sizeof(sz))<0) return 0;
  return read(nxt->bt_sock,data,sz);
}
