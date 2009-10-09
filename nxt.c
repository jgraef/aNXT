/*
    nxt.c
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
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdio.h>
#include <nxtnet.h>
#include <unistd.h>

#include "nxt.h"

#define test(func)             { if ((func)==NXT_FAIL) return NXT_FAIL; }

#define set_word(buf,word)     { ((unsigned char*)(buf))[0] = ((uint16_t)(word))%0x100; ((unsigned char*)(buf))[1] = ((uint16_t)(word))/0x100; }
#define set_dword(buf,dword)   { set_word((buf),((uint32_t)(dword))%0x10000); set_word((buf)+2,((uint32_t)(dword))/0x10000); }
#define get_word(buf)          (((uint16_t)(((unsigned char*)(buf))[0]))+((unsigned char*)(buf))[1]*0x100)
#define get_dword(buf)         (((uint32_t)get_word(buf))+get_word(buf+2)*0x10000)

#define FLAGSET(val,flag) (((val)&(flag))==(flag))

/// Packet type
typedef enum {
  /// Invalid packet type
  NXT_TYPE_NONE = -1,
  /// Direct command telegram, response required
  NXT_TYPE_DIRECT_RESP = 0x00,
  /// System command telegram, response required
  NXT_TYPE_SYSTEM_RESP = 0x01,
  /// Reply telegram
  NXT_TYPE_REPLY       = 0x02,
  /// Direct command telegram, no response
  NXT_TYPE_DIRECT_NORE = 0x80,
  /// System command telegram, no response
  NXT_TYPE_SYSTEM_NORE = 0x81
} nxt_type_t;

/// Packet command
typedef enum {
  NXT_CMD_NONE = -1
} nxt_cmd_t;

/**
 * Send data
 *  @param nxt NXT handle
 *  @return How many bytes sent
 */
static __inline__ ssize_t nxt_con_send(nxt_t *nxt) {
  ssize_t ret = nxtnet_cli_send(nxt->cli,nxt->nxtid,nxt->buffer,nxt->ptr-nxt->buffer);
  if (ret==-1) nxt->error = NXT_ERR_CONNECTION;
  return ret;
}

/**
 * Receive data
 *  @param nxt NXT handle
 *  @param size How many bytes to receive
 *  @return How many bytes received
 */
static __inline__ ssize_t nxt_con_recv(nxt_t *nxt,size_t size) {
  ssize_t ret = nxtnet_cli_recv(nxt->cli,nxt->nxtid,nxt->buffer,size);
  if (ret==-1) nxt->error = NXT_ERR_CONNECTION;
  return ret;
}

/// Functions for packing packages
static void nxt_packbyte(nxt_t *nxt,uint8_t val) {
  *(nxt->ptr)++ = val;
}
static void nxt_packword(nxt_t *nxt,uint16_t val) {
  set_word(nxt->ptr,val);
  nxt->ptr += 2;
}
static void nxt_packdword(nxt_t *nxt,uint32_t val) {
  set_dword(nxt->ptr,val);
  nxt->ptr += 4;
}
static void nxt_packstart(nxt_t *nxt,nxt_cmd_t cmd) {
  nxt->buffer[0] = cmd<0x80?NXT_TYPE_DIRECT_RESP:NXT_TYPE_SYSTEM_RESP;
  nxt->buffer[1] = cmd;
  nxt->ptr = nxt->buffer+2;
}
static void nxt_packmem(nxt_t *nxt,void *buf,size_t len) {
  memcpy(nxt->ptr,buf,len);
  nxt->ptr += len;
}
static void nxt_packstr(nxt_t *nxt,const char *str,size_t maxlen) {
  strncpy(nxt->ptr,str,maxlen);
  nxt->ptr += maxlen;
}

/// Functions for unpacking packages
static uint8_t nxt_unpackbyte(nxt_t *nxt) {
  return *(nxt->ptr)++;
}
static uint16_t nxt_unpackword(nxt_t *nxt) {
  uint16_t ret = get_word(nxt->ptr);
  nxt->ptr += 2;
  return ret;
}
static uint32_t nxt_unpackdword(nxt_t *nxt) {
  uint32_t ret = get_dword(nxt->ptr);
  nxt->ptr += 4;
  return ret;
}
static int nxt_unpackstart(nxt_t *nxt,nxt_cmd_t cmd) {
  if ((nxt->buffer[0]&0xFF)!=NXT_TYPE_REPLY || (nxt->buffer[1]&0xFF)!=cmd) return NXT_FAIL;
  else {
    nxt->ptr = nxt->buffer+2;
    return NXT_SUCC;
  }
}
static int nxt_unpackerror(nxt_t *nxt) {
  int error = (*(nxt->ptr)++)&0xFF;
  if (error!=0) nxt->error = error;
  return error;
}
static void *nxt_unpackmem(nxt_t *nxt,size_t len) {
  void *ret = nxt->ptr;
  nxt->ptr += len;
  return ret;
}
static void *nxt_unpackstr(nxt_t *nxt,size_t len) {
  char *ret = nxt->ptr;
  ret[len-1] = 0;
  nxt->ptr += len;
  return ret;
}

/**
 * Wait after a direct command
 */

void nxt_wait_after_direct_command(void)
{
  usleep(NXT_DIRECT_COMMAND_LATENCY);
}

/**
 * Wait after a communication command
 */

void nxt_wait_after_communication_command(void)
{
  usleep(NXT_COMMUNICATION_COMMAND_LATENCY);
}

/**
 * Extra long wait (after a communication command)
 * reason unknown
 */

void nxt_wait_extra_long_after_communication_command(void)
{
  usleep(10 * NXT_COMMUNICATION_COMMAND_LATENCY);
}

/**
 * Opens a NXT
 *  @param name Name of NXT
 *  @return NXT handle
 *  @note You can pass a NULL pointer as name if you wish to use the first NXT found
 */
nxt_t *nxt_open_net(const char *name,const char *hostname,int port,const char *password) {
  size_t i,num_nxts;
  nxtnet_cli_t *cli;
  nxt_t *nxt;

  // Connect to nxtd
  cli = nxtnet_cli_connect(hostname,port,password);
  if (cli==NULL) {
    fprintf(stderr,"Could not connect to nxtd, make sure nxtd is running\n");
    return NULL;
  }

  // Search USB
  struct nxtnet_proto_listusb_sc *listusb = nxtnet_cli_listusb(cli);
  if (listusb!=NULL) {
    for (i=0;i<listusb->num_items;i++) {
      if (name==NULL || strcmp(name,listusb->nxts[i].name)==0) {
        nxt = malloc(sizeof(nxt_t));
        nxt->cli = cli;
        nxt->name = strdup(listusb->nxts[i].name);
        nxt->buffer = malloc(NXT_CON_BUFFERSIZE);
        nxt->error = 0;
        nxt->contype = NXT_CON_USB;
        nxt->nxtid = listusb->nxts[i].nxtid;
        return nxt;
      }
    }
  }

  // Search Bluetooth

  struct nxtnet_proto_listbt_sc *listbt = nxtnet_cli_listbt(cli);
  if (listbt!=NULL) {
    for (i=0;i<listbt->num_items;i++) {
      if (name==NULL || strcmp(name,listbt->nxts[i].name)==0) {
        nxt = malloc(sizeof(nxt_t));
        nxt->cli = cli;
        nxt->name = strdup(listbt->nxts[i].name);
        nxt->buffer = malloc(NXT_CON_BUFFERSIZE);
        nxt->error = 0;
        nxt->contype = NXT_CON_BT;
        nxt->nxtid = listbt->nxts[i].nxtid;
        return nxt;
      }
    }
  }


  nxtnet_cli_disconnect(cli);

  return NULL;
}

/**
 * Closes a NXT
 *  @param nxt NXT handle
 */
void nxt_close(nxt_t *nxt) {
  nxtnet_cli_disconnect(nxt->cli);
  free(nxt->name);
  free(nxt->buffer);
  free(nxt);
}

/**
 * Gets error number
 *  @param nxt NXT handle
 *  @return Error number
 */
int nxt_error(nxt_t *nxt) {
  return nxt->error;
}

/**
 * Gets error string
 *  @param error Error number
 *  @return Error string
 */
char *nxt_strerror(unsigned int error) {
  // NXT error strings
  char *errstrings1[] = {
    "No more handles",
    "No space",
    "No more files",
    "End of file expected",
    "End of file",
    "Not a linear file",
    "File not found",
    "Handle already closed",
    "No linear space",
    "Undefined error",
    "File is busy",
    "No write buffers",
    "Append not possible",
    "File is full",
    "File exists",
    "Module not found",
    "Out of boundary",
    "Illegal file name",
    "Illegal handle"
  };
  char *errstrings2[] = {
    "Request failed (i.e. specified file not found)",
    "Unknown command opcode",
    "Insane packet",
    "Data contains out-of-range values"
  };
  char *errstrings3[] = {
    "Communication bus error",
    "No free memory in communication buffer",
    "Specified channel/connection is not valid",
    "Specified channel/connection not configured or busy"
  };
  char *errstrings4[] = {
    "No active program",
    "Illegal size specified",
    "Illegal mailbox queue ID specified",
    "Attempted to access invalid field of a structure",
    "Bad input or output specified"
  };
  // aNXT error strings
  char *errstrings5[] = {
    "Connection error",
  };

  if (error==0) return "Success";
  else if (error==0x20) return "Pending communication transaction in progress";
  else if (error==0x40) return "Specified mailbox queue is empty";
  else if (error>0x80 && error<0x94) return errstrings1[error-0x81];
  else if (error>0xBC && error<0xC1) return errstrings2[error-0xBD];
  else if (error>0xDC && error<0xE1) return errstrings3[error-0xDD];
  else if (error>0xEB && error<0xF1) return errstrings4[error-0xEC];
  else if (error==0xFB) return "Insufficient memory available";
  else if (error==0xFF) return "Bad arguments";
  else if (error>=0x100 && error<0x101) return errstrings5[error-0x100];
  else return "Unknown Error";
}

/**
 * Reset error number to zero
 *  @param nxt NXT handle
 */
void nxt_reset_error(nxt_t *nxt) {
  nxt->error = 0;
}

/**
 * Returns connection type of NXT
 *  @param nxt NXT handle
 *  @return Connection type
 */
nxt_contype_t nxt_getcontype(nxt_t *nxt) {
  return nxt->contype;
}

/**
 * Opens a file for writing in fragmented mode
 *  @param nxt NXT handle
 *  @param file Filename
 *  @param size How big the file should be
 *  @return File handle
 *  @note Use nxt_file_open
 */
static int nxt_file_open_write(nxt_t *nxt,const char *file,size_t size) {
  nxt_packstart(nxt,0x81);
  nxt_packstr(nxt,file,20);
  nxt_packdword(nxt,size);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,4));
  test(nxt_unpackstart(nxt,0x81));
  if (nxt_unpackerror(nxt)==0) return nxt_unpackbyte(nxt);
  else return NXT_FAIL;
}

/**
 * Opens a file for writing in linear mode
 *  @param nxt NXT handle
 *  @param file Filename
 *  @param size How big the file should be
 *  @return File handle
 *  @note Use nxt_file_open
 */
static int nxt_file_open_write_linear(nxt_t *nxt,const char *file,size_t size) {
  nxt_packstart(nxt,0x89);
  nxt_packstr(nxt,file,20);
  nxt_packdword(nxt,size);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,4));
  test(nxt_unpackstart(nxt,0x89));
  if (nxt_unpackerror(nxt)==0) return nxt_unpackbyte(nxt);
  else return NXT_FAIL;
}

/**
 * Opens a file for appending data
 *  @param nxt NXT handle
 *  @param file Filename
 *  @return File handle
 *  @note Use nxt_file_open
 */
static int nxt_file_open_append(nxt_t *nxt,const char *file,size_t *avail) {
  nxt_packstart(nxt,0x8C);
  nxt_packstr(nxt,file,20);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,4));
  test(nxt_unpackstart(nxt,0x8C));
  if (nxt_unpackerror(nxt)==0) {
    int handle = nxt_unpackbyte(nxt);
    if (avail!=NULL) *avail = nxt_unpackdword(nxt);
    return handle;
  }
  else return NXT_FAIL;
}

/**
 * Opens a file on NXT for reading
 *  @param nxt NXT handle
 *  @param file Filename
 *  @param filesize Reference for filesize
 *  @return File handle
 *  @note Use nxt_file_open
 */
static int nxt_file_open_read(nxt_t *nxt,const char *file,size_t *filesize) {
  nxt_packstart(nxt,0x80);
  nxt_packstr(nxt,file,20);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,8));
  test(nxt_unpackstart(nxt,0x80));
  if (nxt_unpackerror(nxt)==0) {
    int handle = (int)nxt_unpackbyte(nxt);
    if (filesize!=NULL) *filesize = nxt_unpackdword(nxt);
    return handle;
  }
  else return NXT_FAIL;
}

/**
 * Opens a file on NXT for writing
 *  @param nxt NXT handle
 *  @param file Filename
 *  @param oflag Open flags
 *  @param [size] How big the file should be
 *  @return File handle
 *  @note For fragmented and linear mode 'size' is the requested file size
 *  @note For appending mode 'size' is a refernce for the available file size
 *  @note For reading mode 'size' is a reference for the file size
 */
int nxt_file_open(nxt_t *nxt,const char *file,int oflag,...) {
  va_list args;
  int handle = -1;

  va_start(args,oflag);

  if (FLAGSET(oflag,NXT_OREAD)) handle = nxt_file_open_read(nxt,file,va_arg(args,size_t*));
  else if (FLAGSET(oflag,NXT_OAPPND)) handle = nxt_file_open_append(nxt,file,va_arg(args,size_t*));
  else {
    if (FLAGSET(oflag,NXT_OWOVER) && (handle = nxt_findfirst(nxt,file,NULL,NULL))>=0) {
      nxt_file_close(nxt,handle);
      nxt_file_remove(nxt,file);
      handle = -1;
    }
    if (FLAGSET(oflag,NXT_OWLINE)) handle = nxt_file_open_write_linear(nxt,file,va_arg(args,size_t));
    else if (FLAGSET(oflag,NXT_OWFRAG)) handle = nxt_file_open_write(nxt,file,va_arg(args,size_t));
  }

  va_end(args);

  return handle;
}

/**
 * Reads data from file from NXT (Lowlevel)
 *  @param nxt NXT handle
 *  @param handle File handle
 *  @param dest Buffer where to store data
 *  @param count How many bytes to read (max: 59 bytes)
 *  @return How many bytes read
 */
static ssize_t nxt_file_read_low(nxt_t *nxt,int handle,void *dest,size_t count) {
  nxt_packstart(nxt,0x82);
  nxt_packbyte(nxt,handle);
  nxt_packword(nxt,count);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,6+count));
  test(nxt_unpackstart(nxt,0x82));
  if (nxt_unpackerror(nxt)==0) {
    nxt_unpackbyte(nxt);
    count = nxt_unpackword(nxt);
    memcpy(dest,nxt_unpackmem(nxt,count),count);
    return count;
  }
  else return NXT_FAIL;
}

/**
 * Reads data from file from NXT
 *  @param nxt NXT handle
 *  @param handle File handle
 *  @param dest Buffer to store red data in
 *  @param count How many bytes to read
 *  @return How many bytes red
 */
ssize_t nxt_file_read(nxt_t *nxt,int handle,void *dest,size_t count) {
  int i;
  ssize_t partlen;
  int len = 0;
  for (i=0;i<count/50;i++) {
    partlen = nxt_file_read_low(nxt,handle,dest,50);
    if (partlen==-1) return NXT_FAIL;
    dest += partlen;
    len += partlen;
  }
  if ((partlen = nxt_file_read_low(nxt,handle,dest,count%50))==-1) return NXT_FAIL;
  len += partlen;
  return len;
}

/**
 * Writes data to file from NXT (Lowlevel)
 *  @param nxt NXT handle
 *  @param handle File handle
 *  @param src Data to write in file
 *  @param count How many bytes to write (max: 59 bytes)
 *  @return How many bytes written
 */
static ssize_t nxt_file_write_low(nxt_t *nxt,int handle,void *src,size_t count) {
  nxt_packstart(nxt,0x83);
  nxt_packbyte(nxt,handle);
  nxt_packmem(nxt,src,count);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,6));
  test(nxt_unpackstart(nxt,0x83));
  if (nxt_unpackerror(nxt)==0) {
    nxt_unpackbyte(nxt);
    return nxt_unpackword(nxt);
  }
  else return NXT_FAIL;
}

/**
 * Writes data to file from NXT
 *  @param nxt NXT handle
 *  @param handle File handle
 *  @param src Data to write in file
 *  @param count How many bytes to write
 *  @return How many bytes written
 */
ssize_t nxt_file_write(nxt_t *nxt,int handle,void *src,size_t count) {
  int i,partlen;
  int len = 0;
  for (i=0;i<count/50;i++) {
    partlen = nxt_file_write_low(nxt,handle,src,50);
    if (partlen==-1) return -1;
    src += partlen;
    len += partlen;
  }
  if ((partlen = nxt_file_write_low(nxt,handle,src,count%50))==-1) return NXT_FAIL;
  len += partlen;
  return len;
}

/**
 * Closes file from NXT
 *  @param nxt NXT handle
 *  @param handle File handle
 *  @return Success?
 */
int nxt_file_close(nxt_t *nxt,int handle) {
  nxt_packstart(nxt,0x84);
  nxt_packbyte(nxt,handle);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,4));
  test(nxt_unpackstart(nxt,0x84));
  return nxt_unpackerror(nxt)==0?NXT_SUCC:NXT_FAIL;
}

/**
 * Removes file from NXT
 *  @param nxt NXT handle
 *  @param file Filename
 *  @return Whether file could be removed
 */
int nxt_file_remove(nxt_t *nxt,const char *file) {
  nxt_packstart(nxt,0x85);
  nxt_packstr(nxt,file,20);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,23));
  test(nxt_unpackstart(nxt,0x85));
  return nxt_unpackerror(nxt)==0?NXT_SUCC:NXT_FAIL;
}

/**
 * Sends message to NXT
 *  @param nxt NXT handle
 *  @param mailbox Mailbox
 *  @param data Message
 *  @return Whether message could be sent
 */
int nxt_sendmsg(nxt_t *nxt,int mailbox,char *data) {
  if (!VALID_MAILBOX(mailbox)) return NXT_FAIL;
  size_t len = strlen(data)+1;
  nxt_packstart(nxt,0x09);
  nxt_packbyte(nxt,mailbox);
  nxt_packbyte(nxt,len);
  nxt_packstr(nxt,data,len);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,3));
  test(nxt_unpackstart(nxt,0x09));
  return nxt_unpackerror(nxt)==0?NXT_SUCC:NXT_FAIL;
}

/**
 * Receives message from NXT
 *  @param nxt NXT handle
 *  @param mailbox Mailbox
 *  @param clear Whether to clear message box
 *  @return Message as string
 *  @note The return pointer can and should be passed to free()
 */
char *nxt_recvmsg(nxt_t *nxt,int mailbox,int clear) {
  if (!VALID_MAILBOX(mailbox)) return NULL;
  nxt_packstart(nxt,0x13);
  nxt_packbyte(nxt,mailbox);
  nxt_packbyte(nxt,0);
  nxt_packbyte(nxt,clear?1:0);
  if (nxt_con_send(nxt)==NXT_FAIL) return NULL;
  if (nxt_con_recv(nxt,64)==NXT_FAIL) return NULL;
  if (nxt_unpackstart(nxt,0x13)==NXT_FAIL) return NULL;
  nxt_unpackerror(nxt);
  nxt_unpackbyte(nxt);
  size_t size = nxt_unpackbyte(nxt);
  return strdup(nxt_unpackmem(nxt,size));
}

/**
 * Sets name of NXT
 *  @param nxt NXT handle
 *  @param name New NXT name
 *  @return Whether name could be set
 */
int nxt_setname(nxt_t *nxt,char *name) {
  nxt_packstart(nxt,0x98);
  nxt_packstr(nxt,name,15);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,3));
  test(nxt_unpackstart(nxt,0x98));
  if (nxt_unpackerror(nxt)==0) {
    free(nxt->name);
    nxt->name = strdup(name);
    return NXT_SUCC;
  }
  else return NXT_FAIL;
}

/**
 * Gets firmware and protocol version from NXT
 *  @param nxt NXT handle
 *  @param firmmaj Pointer where to store major firmware version
 *  @param firmmin Pointer where to store minor firmware version
 *  @param protomaj Pointer where to store major protocol version
 *  @param protomin Pointer where to store minor protocol version
 *  @return Whether function was successful
 */
int nxt_getver(nxt_t *nxt,int *firmmaj,int *firmmin,int *protomaj,int *protomin) {
  nxt_packstart(nxt,0x88);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,7));
  test(nxt_unpackstart(nxt,0x88));
  if (nxt_unpackerror(nxt)==0) {
    if (protomin!=NULL) *protomin = nxt_unpackbyte(nxt);
    else nxt_unpackbyte(nxt);
    if (protomaj!=NULL) *protomaj = nxt_unpackbyte(nxt);
    else nxt_unpackbyte(nxt);
    if (firmmin!=NULL) *firmmin = nxt_unpackbyte(nxt);
    else nxt_unpackbyte(nxt);
    if (firmmaj!=NULL) *firmmaj = nxt_unpackbyte(nxt);
    else nxt_unpackbyte(nxt);
    return NXT_SUCC;
  }
  else return NXT_FAIL;
}

/**
 * Sets mode of sensor
 *  @param nxt NXT handle
 *  @param sensor Sensor (0,1,2,3)
 *  @param type Sensor type
 *  @param mode Sensor mode
 *  @return Success?
 */
int nxt_setsensormode(nxt_t *nxt,int sensor,int type,int mode) {
  if (!VALID_SENSOR(sensor)) return NXT_FAIL;
  nxt_packstart(nxt,0x05);
  nxt_packbyte(nxt,sensor);
  nxt_packbyte(nxt,type);
  nxt_packbyte(nxt,mode);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,3));
  test(nxt_unpackstart(nxt,0x05));
  return nxt_unpackerror(nxt)==0?NXT_SUCC:NXT_FAIL;
}

/**
 * Gets values of sensor
 *  @param nxt NXT handle
 *  @param sensor Sensor (0,1,2,3)
 *  @return Sensor value
 */
int nxt_getsensorval(nxt_t *nxt,int sensor) {
  struct nxt_sensor_values values;
  if (nxt_getsensorvals(nxt,sensor,&values)==0) {
    return values.is_calibrated?values.calibrated:values.scaled;
  }
  else {
    return -1;
  }
}

/**
 * Gets values of sensor
 *  @param nxt NXT handle
 *  @param sensor Sensor (0,1,2,3)
 *  @param values Pointer to structure for storing sensor values
 *  @return 0 = Success
 *         -1 = Failure
 */
int nxt_getsensorvals(nxt_t *nxt,int sensor,struct nxt_sensor_values *values) {
  if (!VALID_SENSOR(sensor)) return NXT_FAIL;
  nxt_packstart(nxt,0x07);
  nxt_packbyte(nxt,sensor);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,16));
  test(nxt_unpackstart(nxt,0x07));
  if (nxt_unpackerror(nxt)==0) {
    nxt_unpackbyte(nxt); // sensor port
    if (nxt_unpackbyte(nxt)) {
      if (values!=NULL) {
        values->is_calibrated = nxt_unpackbyte(nxt);
        values->type = (int)nxt_unpackbyte(nxt); // type
        values->mode = (int)nxt_unpackbyte(nxt); // mode
        values->raw = (int)nxt_unpackword(nxt); // raw
        values->normalized = (int)nxt_unpackword(nxt); // type dependent
        values->scaled = (int)nxt_unpackword(nxt); // mode dependent
        values->calibrated = (int)nxt_unpackword(nxt); // calibrated
        return 0;
      }
    }
    else {
      nxt_wait_after_direct_command();
      return nxt_getsensorvals(nxt,sensor,values);
    }
  }
  return NXT_FAIL;
}

/**
 * Resets a sensor
 *  @param nxt NXT handle
 *  @param sensor Sensor (0,1,2,3)
 *  @return Success?
 */
int nxt_resetsensor(nxt_t *nxt,int sensor) {
  if (!VALID_SENSOR(sensor)) return NXT_FAIL;
  nxt_packstart(nxt,0x08);
  nxt_packbyte(nxt,sensor);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,3));
  test(nxt_unpackstart(nxt,0x08));
  return nxt_unpackerror(nxt)==0?NXT_SUCC:NXT_FAIL;
}

/**
 * Gets battery level of NXT
 *  @param nxt NXT handle
 *  @return battery level in millivolt
 */
int nxt_getbattery(nxt_t *nxt) {
  nxt_packstart(nxt,0x0B);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,5));
  test(nxt_unpackstart(nxt,0x0B));
  if (nxt_unpackerror(nxt)==0) return nxt_unpackword(nxt);
  else return NXT_FAIL;
}

/**
 * Sets rotation of a motor
 *  @param nxt NXT handle
 *  @param motor (0,1,2)
 *  @param rotation (0..360)
 *  @param power (-100..100)
 *  @param mode (0..7)
 *  @param regmode (0..3)
 *  @param turnratio (-100..100)
 *  @return Success?
 *  @note If rotation is 0 motor will work until it's turned off
 */
int nxt_motor(nxt_t *nxt,int motor,unsigned int rotation,int power,int mode,int regmode,int turnratio) {
  if (!VALID_MOTOR(motor)) return NXT_FAIL;
  nxt_packstart(nxt,0x04);
  nxt_packbyte(nxt,motor);
  nxt_packbyte(nxt,power);
  nxt_packbyte(nxt,mode);
  if ((power == 0) && ((mode & NXT_BRAKE) == 0))
    nxt_packbyte(nxt,NXT_REGMODE_IDLE);
  else
    nxt_packbyte(nxt,regmode);
  nxt_packbyte(nxt,turnratio);
  if ((power == 0) && ((mode & NXT_BRAKE) == 0))
    nxt_packbyte(nxt,NXT_RUN_STATE_IDLE);
  else
    nxt_packbyte(nxt,NXT_RUN_STATE_RUNNING);
  nxt_packdword(nxt,rotation);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,3));
  test(nxt_unpackstart(nxt,0x04));
  return nxt_unpackerror(nxt)==0?NXT_SUCC:NXT_FAIL;
}

/**
 * Get tacho value (gets motor rotation)
 *  @param nxt NXT handle
 *  @param motor Motor
 *  @return Rotation count
 */
int nxt_tacho(nxt_t *nxt,int motor) {
  if (!VALID_MOTOR(motor)) return NXT_FAIL;
  nxt_packstart(nxt,0x06);
  nxt_packbyte(nxt,motor);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,25));
  test(nxt_unpackstart(nxt,0x06));
  if (nxt_unpackerror(nxt)==0) {
    nxt_unpackbyte(nxt); // Motor
    nxt_unpackbyte(nxt); // Power
    nxt_unpackbyte(nxt); // Mode
    nxt_unpackbyte(nxt); // Regulation Mode
    nxt_unpackbyte(nxt); // Turn Ratio
    nxt_unpackbyte(nxt); // RunState
    nxt_unpackdword(nxt); // Tacho Limit
    nxt_unpackdword(nxt); // Tacho Count
    nxt_unpackdword(nxt); // Block Tacho Count
    return nxt_unpackdword(nxt);
  }
  else return NXT_FAIL;
}

/**
 * Resets current tacho value
 *  @param nxt NXT handle
 *  @param motor Motor
 *  @param relative Relativ (or Absolute)
 *  @return Whether resetting was successful
 */
int nxt_resettacho(nxt_t *nxt,int motor,int relative) {
  if (!VALID_MOTOR(motor)) return NXT_FAIL;
  nxt_packstart(nxt,0x0A);
  nxt_packbyte(nxt,motor);
  nxt_packbyte(nxt,relative?1:0);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,3));
  test(nxt_unpackstart(nxt,0x0A));
  return nxt_unpackerror(nxt)==0?NXT_SUCC:NXT_FAIL;
}

/**
 * Stops program
 *  @param nxt NXT handle
 */
int nxt_stopprogram(nxt_t *nxt) {
  nxt_packstart(nxt,0x01);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,3));
  test(nxt_unpackstart(nxt,0x01));
  return nxt_unpackerror(nxt)==0?NXT_SUCC:NXT_FAIL;
}

/**
 * Gets current program
 *  @param nxt NXT handle
 *  @return Name of current programm
 *  @note The return pointer can and should be passed to free()
 */
char *nxt_getcurprog(nxt_t *nxt) {
  nxt_packstart(nxt,0x11);
  if (nxt_con_send(nxt)==NXT_FAIL) return NULL;
  usleep(60000); // latency of direct output commands: up to 60 ms
  if (nxt_con_recv(nxt,23)==NXT_FAIL) return NULL;
  if (nxt_unpackstart(nxt,0x11)==NXT_FAIL) return NULL;
  return nxt_unpackerror(nxt)==0?strdup(nxt_unpackstr(nxt,20)):NULL;
}

/**
 * Gets device information
 *  @param nxt NXT handle
 *  @param nxt_name Reference to store NXT name as string
 *  @param bt_addr Reference to store Bluetooth address
 *  @param bt_strength Reference to store Bluetooth strength
 *  @param free_flash Reference to store amount of free flash
 *  @return Whether infos could be get
 *  @note Set a reference to NULL if you do not need this value
 *  @note The pointer 'nxt_name' can and should be passed to free()
 */
int nxt_getdevinfo(nxt_t *nxt,char **nxt_name,char bt_addr[6],unsigned int *bt_strength,unsigned int *free_flash) {
  nxt_packstart(nxt,0x9B);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,33));
  test(nxt_unpackstart(nxt,0x9B));
  if (nxt_unpackerror(nxt)==0) {
    if (nxt_name!=NULL) *nxt_name = strdup(nxt_unpackstr(nxt,15));
    else nxt_unpackstr(nxt,15);
    if (bt_addr!=NULL) memcpy(bt_addr,nxt_unpackmem(nxt,6),6);
    else nxt_unpackmem(nxt,6);
    if (bt_strength!=NULL) *bt_strength = nxt_unpackdword(nxt);
    else nxt_unpackdword(nxt);
    if (free_flash!=NULL) *free_flash = nxt_unpackdword(nxt);
    else nxt_unpackdword(nxt);
    return NXT_SUCC;
  }
  else return NXT_FAIL;
}

/**
 * Gets name of NXT
 *  @param nxt NXT handle
 *  @return NXT name as string
 */
char *nxt_getname(nxt_t *nxt) {
  return nxt->name;
}

/**
 * Keeps NXT alive
 *  @param nxt NXT handle
 *  @return Current sleep time limit (in milliseconds)
 */
int nxt_keepalive(nxt_t *nxt) {
  nxt_packstart(nxt,0x0D);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,7));
  test(nxt_unpackstart(nxt,0x0D));
  return nxt_unpackerror(nxt)==0?nxt_unpackdword(nxt):NXT_FAIL;
}

/**
 * Beep (plays a tone)
 *  @param nxt NXT handle
 *  @param freq Frequency (200..14000)
 *  @param dur Duration
 *  @return Success
 */
int nxt_beep(nxt_t *nxt,unsigned int freq,unsigned int dur) {
  nxt_packstart(nxt,0x03);
  nxt_packword(nxt,freq);
  nxt_packword(nxt,dur);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,3));
  test(nxt_unpackstart(nxt,0x03));
  return nxt_unpackerror(nxt)==0?NXT_SUCC:NXT_FAIL;
}

/**
 * Plays a sound file
 *  @param nxt NXT handle
 *  @param filename Sound file
 *  @param loop Whether to loop sound playback
 *  @return Success
 */
int nxt_playsoundfile(nxt_t *nxt,char *filename,int loop) {
  nxt_packstart(nxt,0x02);
  nxt_packbyte(nxt,loop?1:0);
  nxt_packstr(nxt,filename,20);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,3));
  test(nxt_unpackstart(nxt,0x02));
  return nxt_unpackerror(nxt)==0?NXT_SUCC:NXT_FAIL;
}

/**
 * Stops sound playback
 *  @param nxt NXT handle
 *  @return Success
 */
int nxt_stopsound(nxt_t *nxt) {
  nxt_packstart(nxt,0x0C);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,3));
  test(nxt_unpackstart(nxt,0x0C));
  return nxt_unpackerror(nxt)==0?NXT_SUCC:NXT_FAIL;
}

/**
 * Starts a program
 *  @param nxt NXT handle
 *  @param filename Program file
 *  @return Success
 */
int nxt_startprogram(nxt_t *nxt,char *filename) {
  nxt_packstart(nxt,0x00);
  nxt_packstr(nxt,filename,20);
  test(nxt_con_send(nxt));
  usleep(60000);
  test(nxt_con_recv(nxt,3));
  test(nxt_unpackstart(nxt,0x00));
  return nxt_unpackerror(nxt)==0?NXT_SUCC:NXT_FAIL;
}

/**
 * Finds a file
 *  @param nxt NXT handle
 *  @param wildcard Wildcard to search for
 *  @param filename Reference to store filename of found file as string
 *  @param filesize Reference to store filesize of found file
 *  @return File handle of found file
 *  @note The pointer 'filename' can and should be passed to free()
 *  @note If the file is not needed it should be passed to nxt_file_close()
 */
int nxt_findfirst(nxt_t *nxt,const char *wildcard,char **filename,size_t *filesize) {
  nxt_packstart(nxt,0x86);
  nxt_packstr(nxt,wildcard,20);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,28));
  test(nxt_unpackstart(nxt,0x86));
  if (nxt_unpackerror(nxt)==0) {
    int handle = nxt_unpackbyte(nxt);
    if (filename!=NULL) *filename = strdup(nxt_unpackstr(nxt,20));
    if (filesize!=NULL) *filesize = nxt_unpackdword(nxt);
    return handle;
  }
  else return NXT_FAIL;
}

/**
 * Finds a file
 *  @param nxt NXT handle
 *  @param handle Filehandle of previous found file
 *  @param filename Reference to store filename of found file as string
 *  @param filesize Reference to store filesize of found file
 *  @return File handle of found file
 *  @note The pointer 'filename' can and should be passed to free()
 *  @note If the file is not needed it should be passed to nxt_file_close()
 */
int nxt_findnext(nxt_t *nxt,int handle,char **filename,size_t *filesize) {
  nxt_packstart(nxt,0x87);
  nxt_packbyte(nxt,handle);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,28));
  test(nxt_unpackstart(nxt,0x87));
  if (nxt_unpackerror(nxt)==0) {
    int handle = nxt_unpackbyte(nxt);
    if (filename!=NULL) *filename = strdup(nxt_unpackstr(nxt,20));
    if (filesize!=NULL) *filesize = nxt_unpackdword(nxt);
    return handle;
  }
  else return NXT_FAIL;
}

/**
 * Resets Bluetooth
 *  @param nxt NXT handle
 *  @return Success
 *  @note Can only be done via USB
 */
int nxt_resetbt(nxt_t *nxt) {
  if (nxt->contype==NXT_CON_BT) return NXT_FAIL;
  nxt_packstart(nxt,0xA4);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,28));
  test(nxt_unpackstart(nxt,0xA4));
  if (nxt_unpackerror(nxt)==0) {
    free(nxt->name);
    nxt->name = strdup("NXT");
    return NXT_SUCC;
  }
  else return NXT_FAIL;
}

/**
 * Polls a command
 *  @param nxt NXT handle
 *  @param ptr Reference for command
 *  @param buffer Buffer number
 *  @note The pointer 'ptr' can and should be passed to free()
 *  @todo What does this function do?
 */
ssize_t nxt_pollcmd(nxt_t *nxt,void **ptr,int buffer) {
  if (ptr==NULL) return NXT_FAIL;
  // get size
  nxt_packstart(nxt,0xA1);
  nxt_packbyte(nxt,buffer);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,5));
  test(nxt_unpackstart(nxt,0xA1));
  if (nxt_unpackerror(nxt)==0) {
    size_t size = nxt_unpackbyte(nxt);
    if (size>0) {
      // get command
      nxt_packstart(nxt,0xA2);
      nxt_packbyte(nxt,buffer);
      nxt_packbyte(nxt,size);
      test(nxt_con_send(nxt));
      test(nxt_con_recv(nxt,64));
      test(nxt_unpackstart(nxt,0xA2));
      if (nxt_unpackerror(nxt)==0) {
        size = nxt_unpackbyte(nxt);
        if (size>0) {
          if (ptr!=NULL) *ptr = memcpy(malloc(size),nxt_unpackmem(nxt,size),size);
        }
        return size;
      }
      else return NXT_FAIL;
    }
    return size;
  }
  return NXT_FAIL;
}

/**
 * Removes user flash
 *  @param nxt NXT handle
 *  @note Can take upto 3 seconds
 */
int nxt_deluserflash(nxt_t *nxt) {
  nxt_packstart(nxt,0xA0);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,3));
  test(nxt_unpackstart(nxt,0xA0));
  return nxt_unpackerror(nxt)==0?NXT_SUCC:NXT_FAIL;
}

/**
 * Gets LS status
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @return Bytes ready
 */
ssize_t nxt_lsstatus(nxt_t *nxt,int port) {
  if (!VALID_SENSOR(port)) return NXT_FAIL;
  nxt_packstart(nxt,0x0E);
  nxt_packbyte(nxt,port);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,4));
  test(nxt_unpackstart(nxt,0x0E));
  return nxt_unpackerror(nxt)==0?nxt_unpackbyte(nxt):NXT_FAIL;
}

/**
 * Writes LS data
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @param tx How many bytes to send
 *  @param rx How many bytes to receive by next nxt_lsread()
 *  @param data Data to send
 *  @return Success
 */
int nxt_lswrite(nxt_t *nxt,int port,size_t tx,size_t rx,void *data) {
  if (!VALID_SENSOR(port)) return NXT_FAIL;
  if (tx>16 || rx>16) return NXT_FAIL;
  nxt_packstart(nxt,0x0F);
  nxt_packbyte(nxt,port);
  nxt_packbyte(nxt,tx);
  nxt_packbyte(nxt,rx);
  nxt_packmem(nxt,data,tx);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,3));
  test(nxt_unpackstart(nxt,0x0F));
  return nxt_unpackerror(nxt)==0?NXT_SUCC:NXT_FAIL;
}

/**
 * Reads LS data
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @param bufsize Size of buffer
 *  @param buf Buffer for received data
 *  @return How many bytes read
 */
ssize_t nxt_lsread(nxt_t *nxt,int port,size_t bufsize,void *buf) {
  if (!VALID_SENSOR(port)) return NXT_FAIL;
  if (buf==NULL) return NXT_FAIL;
  nxt_packstart(nxt,0x10);
  nxt_packbyte(nxt,port);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,20));
  test(nxt_unpackstart(nxt,0x10));
  if (nxt_unpackerror(nxt)==0) {
    ssize_t size = nxt_unpackbyte(nxt);
    void *data = nxt_unpackmem(nxt,size);
    memcpy(buf,data,bufsize);
    return size<bufsize?size:bufsize;
  }
  else return NXT_FAIL;
}

/**
 * Read I2C register
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @param addr I2C address
 *  @param reg1 First register
 *  @param nreg Number of registers
 *  @param buf Buffer for read data
 *  @return How many bytes read
 */
ssize_t nxt_i2c_regr(nxt_t *nxt,int port,int addr,size_t reg1,size_t nreg,void *buf) {
  time_t timeout;
  char cmd[3] = {
    addr,
    reg1
  };

  if (nreg>16) {
    nreg = 16;
  }

  nxt_lswrite(nxt,port,2,nreg,cmd);

  // wait for data
  timeout = time(NULL)+2;
  while (nxt_lsstatus(nxt,port)<((ssize_t)nreg) && time(NULL)<timeout);

  return nxt_lsread(nxt,port,nreg,buf);
}

/**
 * Write I2C register
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @param addr I2C address
 *  @param reg1 First register
 *  @param nreg Number of registers
 *  @param buf Data to write to registers
 *  @return How many bytes written
 */
ssize_t nxt_i2c_regw(nxt_t *nxt,int port,int addr,size_t reg1,size_t nreg,void *buf) {
  char cmd[16] = {
    addr,
    reg1
  };

  if (nreg>14) {
    nreg = 14;
  }

  memcpy(cmd+2,buf,nreg);
  return nxt_lswrite(nxt,port,nreg+2,0,cmd);
}

/**
 * Sends command to I2C sensor
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @param addr I2C address
 *  @param cmd Command
 */
int nxt_i2c_cmd(nxt_t *nxt,int port,int addr,int cmd) {
  char buf[3] = {
    addr,
    NXT_I2C_REG_CMD,
    cmd
  };

  return nxt_lswrite(nxt,port,3,0,buf)==3?0:-1;
}

/**
 * Gets version from I2C sensor
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @param addr I2C address
 *  @return Version
 */
const char *nxt_i2c_get_version(nxt_t *nxt,int port,int addr) {
  static char buf[9];

  if (nxt_i2c_regr(nxt,port,addr,NXT_I2C_REG_VERSION,8,buf)==8) {
    buf[8] = 0;
    return buf;
  }
  else {
    return NULL;
  }
}

/**
 * Gets vendor ID from I2C sensor
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @param addr I2C address
 *  @return Vendor ID
 */
const char *nxt_i2c_get_vendorid(nxt_t *nxt,int port,int addr) {
  static char buf[9];

  if (nxt_i2c_regr(nxt,port,addr,NXT_I2C_REG_VENDORID,8,buf)==8) {
    buf[8] = 0;
    return buf;
  }
  else {
    return NULL;
  }
}

/**
 * Gets device ID from I2C sensor
 *  @param nxt NXT handle
 *  @param port Sensor port
 *  @param addr I2C address
 *  @return Device ID
 */
const char *nxt_i2c_get_deviceid(nxt_t *nxt,int port,int addr) {
  static char buf[9];

  if (nxt_i2c_regr(nxt,port,addr,NXT_I2C_REG_DEVICEID,8,buf)==8) {
    buf[8] = 0;
    return buf;
  }
  else {
    return NULL;
  }
}

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
  nxt_packstart(nxt,0x90);
  nxt_packstr(nxt,wildcard,20);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,34));
  test(nxt_unpackstart(nxt,0x90));
  if (nxt_unpackerror(nxt)==0) {
    int handle = nxt_unpackbyte(nxt);
    if (modname!=NULL) *modname = strdup(nxt_unpackstr(nxt,20));
    else nxt_unpackstr(nxt,20);
    if (modid!=NULL) *modid = nxt_unpackdword(nxt);
    else nxt_unpackdword(nxt);
    if (modsz!=NULL) *modsz = nxt_unpackdword(nxt);
    else nxt_unpackdword(nxt);
    if (iomapsz!=NULL) *iomapsz = nxt_unpackword(nxt);
    else nxt_unpackword(nxt);
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
  nxt_packstart(nxt,0x91);
  nxt_packbyte(nxt,handle);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,34));
  test(nxt_unpackstart(nxt,0x91));
  if (nxt_unpackerror(nxt)==0) {
    handle = nxt_unpackbyte(nxt);
    if (modname!=NULL) *modname = strdup(nxt_unpackstr(nxt,20));
    else nxt_unpackstr(nxt,20);
    if (modid!=NULL) *modid = nxt_unpackdword(nxt);
    else nxt_unpackdword(nxt);
    if (modsz!=NULL) *modsz = nxt_unpackdword(nxt);
    else nxt_unpackdword(nxt);
    if (iomapsz!=NULL) *iomapsz = nxt_unpackword(nxt);
    else nxt_unpackword(nxt);
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
  nxt_packstart(nxt,0x92);
  nxt_packbyte(nxt,handle);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,4));
  test(nxt_unpackstart(nxt,0x92));
  return nxt_unpackerror(nxt)==0?NXT_SUCC:NXT_FAIL;
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
  nxt_packstart(nxt,0x94);
  nxt_packdword(nxt,modid);
  nxt_packword(nxt,offset);
  nxt_packword(nxt,size);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,9+size));
  test(nxt_unpackstart(nxt,0x94));
  if (nxt_unpackerror(nxt)==0) {
    nxt_unpackdword(nxt); // modid
    size = nxt_unpackword(nxt);
    memcpy(buf,nxt_unpackmem(nxt,size),size);
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
  nxt_packstart(nxt,0x95);
  nxt_packdword(nxt,modid);
  nxt_packword(nxt,offset);
  nxt_packword(nxt,size);
  nxt_packmem(nxt,(void*)buf,size);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,9));
  test(nxt_unpackstart(nxt,0x95));
  if (nxt_unpackerror(nxt)==0) {
    nxt_unpackdword(nxt); // modid
    return nxt_unpackword(nxt);
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
int nxt_mod_getid(nxt_t *nxt,char *file) {
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
int nxt_getvolume(nxt_t *nxt) {
  int modid;
  if ((modid = nxt_mod_getid(nxt,NXT_UI_MODFILE))!=-1) {
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
int nxt_setvolume(nxt_t *nxt,int volume) {
  int modid;
  char vol = volume;
  if (VALID_VOLUME(volume) && (modid = nxt_mod_getid(nxt,NXT_UI_MODFILE))!=-1) {
    if (nxt_mod_write(nxt,modid,&vol,NXT_UI_VOLUME,1)==1) return 0;
  }
  else return -1;
}

/**
 * Turns NXT off
 *  @param nxt NXT handle
 *  @note NXT handle must be closed after this
 */
int nxt_turnoff(nxt_t *nxt) {
  int modid;
  char off = 1;
  if ((modid = nxt_mod_getid(nxt,NXT_UI_MODFILE))!=-1) {
    if (nxt_mod_write(nxt,modid,&off,NXT_UI_TURNOFF,1)==1) return 0;
  }
  else return -1;
}

/**
 * "Press" button on NXT
 *  @param nxt NXT handle
 *  @param button Button
 */
int nxt_setbutton(nxt_t *nxt,int button) {
  int modid;
  char btn = button;
  if ((modid = nxt_mod_getid(nxt,NXT_UI_MODFILE))!=-1) {
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
  if ((modid = nxt_mod_getid(nxt,NXT_DISPLAY_MODFILE))!=-1) {
    nxt_mod_read(nxt,modid,screen,NXT_DISPLAY_BITMAP,800);
    for (y=0;y<64;y++) {
      for (x=0;x<100;x++) buf[y][x] = screen[y/8][x]&(1<<(y%8));
    }
    return 0;
  }
  else return -1;
}
