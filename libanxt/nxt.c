/*
    libanxt/nxt.c
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

#include <anxt/nxt.h>
#include "private.h"

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
 *  @param name Name, Bluetooth address or ID of NXT
 *  @return NXT handle
 *  @note You can pass a NULL pointer as name if you wish to use the first NXT found
 */
nxt_t *nxt_open_net(const char *name,const char *hostname,int port,const char *password) {
  size_t i,num_nxts;
  nxtnet_cli_t *cli;
  nxt_t *nxt;
  nxt_id_t id;

  // Connect to nxtd
  cli = nxtnet_cli_connect(hostname,port,password);
  if (cli==NULL) {
    fprintf(stderr,"Could not connect to nxtd, make sure nxtd is running\n");
    return NULL;
  }

  // Convert name to bluetooth address or ID (if possible)
  if (name==NULL ||
     (sscanf(name,"%02x:%02x:%02x:%02x:%02x:%02x",&id[0],&id[1],&id[2],&id[3],&id[4],&id[5])!=6
   && sscanf(name,"%02x%02x%02x%02x%02x%02x",&id[0],&id[1],&id[2],&id[3],&id[4],&id[5])!=6)) {
    memset(id,0,6);
  }

  // List NXTs
  struct nxtnet_proto_list_sc *list = nxtnet_cli_list(cli);
  if (list!=NULL) {
    for (i=0;i<list->num_items;i++) {
      if (name==NULL || strcmp(name,list->nxts[i].name)==0 || memcmp(id, list->nxts[i].id, 6)==0) {
        nxt = malloc(sizeof(nxt_t));
        nxt->cli = cli;
        nxt->name = strdup(list->nxts[i].name);
        nxt->buffer = malloc(NXT_CON_BUFFERSIZE);
        nxt->error = 0;
        nxt->contype = list->nxts[i].is_bt?NXT_CON_BT:NXT_CON_USB;
        nxt->handle = list->nxts[i].handle;
        memcpy(nxt->id, list->nxts[i].id, 6);
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
 * Sends message to NXT
 *  @param nxt NXT handle
 *  @param mailbox Mailbox
 *  @param data Message
 *  @return Whether message could be sent
 */
int nxt_sendmsg(nxt_t *nxt,int mailbox,char *data) {
  if (!VALID_MAILBOX(mailbox)) return NXT_FAIL;
  size_t len = strlen(data)+1;
  nxt_pack_start(nxt,0x09);
  nxt_pack_byte(nxt,mailbox);
  nxt_pack_byte(nxt,len);
  nxt_pack_str(nxt,data,len);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,3));
  test(nxt_unpack_start(nxt,0x09));
  return nxt_unpack_error(nxt)==0?NXT_SUCC:NXT_FAIL;
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
  nxt_pack_start(nxt,0x13);
  nxt_pack_byte(nxt,mailbox);
  nxt_pack_byte(nxt,0);
  nxt_pack_byte(nxt,clear?1:0);
  if (nxt_con_send(nxt)==NXT_FAIL) return NULL;
  if (nxt_con_recv(nxt,64)==NXT_FAIL) return NULL;
  if (nxt_unpack_start(nxt,0x13)==NXT_FAIL) return NULL;
  nxt_unpack_error(nxt);
  nxt_unpack_byte(nxt);
  size_t size = nxt_unpack_byte(nxt);
  return strdup(nxt_unpack_mem(nxt,size));
}

/**
 * Sets name of NXT
 *  @param nxt NXT handle
 *  @param name New NXT name
 *  @return Whether name could be set
 */
int nxt_setname(nxt_t *nxt,char *name) {
  nxt_pack_start(nxt,0x98);
  nxt_pack_str(nxt,name,15);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,3));
  test(nxt_unpack_start(nxt,0x98));
  if (nxt_unpack_error(nxt)==0) {
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
  nxt_pack_start(nxt,0x88);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,7));
  test(nxt_unpack_start(nxt,0x88));
  if (nxt_unpack_error(nxt)==0) {
    if (protomin!=NULL) *protomin = nxt_unpack_byte(nxt);
    else nxt_unpack_byte(nxt);
    if (protomaj!=NULL) *protomaj = nxt_unpack_byte(nxt);
    else nxt_unpack_byte(nxt);
    if (firmmin!=NULL) *firmmin = nxt_unpack_byte(nxt);
    else nxt_unpack_byte(nxt);
    if (firmmaj!=NULL) *firmmaj = nxt_unpack_byte(nxt);
    else nxt_unpack_byte(nxt);
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
  nxt_pack_start(nxt,0x05);
  nxt_pack_byte(nxt,sensor);
  nxt_pack_byte(nxt,type);
  nxt_pack_byte(nxt,mode);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,3));
  test(nxt_unpack_start(nxt,0x05));
  return nxt_unpack_error(nxt)==0?NXT_SUCC:NXT_FAIL;
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
  nxt_pack_start(nxt,0x07);
  nxt_pack_byte(nxt,sensor);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,16));
  test(nxt_unpack_start(nxt,0x07));
  if (nxt_unpack_error(nxt)==0) {
    nxt_unpack_byte(nxt); // sensor port
    if (nxt_unpack_byte(nxt)) {
      if (values!=NULL) {
        values->is_calibrated = nxt_unpack_byte(nxt);
        values->type = (int)nxt_unpack_byte(nxt); // type
        values->mode = (int)nxt_unpack_byte(nxt); // mode
        values->raw = (int)nxt_unpack_word(nxt); // raw
        values->normalized = (int)nxt_unpack_word(nxt); // type dependent
        values->scaled = (int)nxt_unpack_word(nxt); // mode dependent
        values->calibrated = (int)nxt_unpack_word(nxt); // calibrated
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
  nxt_pack_start(nxt,0x08);
  nxt_pack_byte(nxt,sensor);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,3));
  test(nxt_unpack_start(nxt,0x08));
  return nxt_unpack_error(nxt)==0?NXT_SUCC:NXT_FAIL;
}

/**
 * Gets battery level of NXT
 *  @param nxt NXT handle
 *  @return battery level in millivolt
 */
int nxt_getbattery(nxt_t *nxt) {
  nxt_pack_start(nxt,0x0B);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,5));
  test(nxt_unpack_start(nxt,0x0B));
  if (nxt_unpack_error(nxt)==0) return nxt_unpack_word(nxt);
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
  nxt_pack_start(nxt,0x04);
  nxt_pack_byte(nxt,motor);
  nxt_pack_byte(nxt,power);
  nxt_pack_byte(nxt,mode);
  if ((power == 0) && ((mode & NXT_BRAKE) == 0))
    nxt_pack_byte(nxt,NXT_REGMODE_IDLE);
  else
    nxt_pack_byte(nxt,regmode);
  nxt_pack_byte(nxt,turnratio);
  if ((power == 0) && ((mode & NXT_BRAKE) == 0))
    nxt_pack_byte(nxt,NXT_RUN_STATE_IDLE);
  else
    nxt_pack_byte(nxt,NXT_RUN_STATE_RUNNING);
  nxt_pack_dword(nxt,rotation);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,3));
  test(nxt_unpack_start(nxt,0x04));
  return nxt_unpack_error(nxt)==0?NXT_SUCC:NXT_FAIL;
}

/**
 * Get tacho value (gets motor rotation)
 *  @param nxt NXT handle
 *  @param motor Motor
 *  @return Rotation count
 */
int nxt_tacho(nxt_t *nxt,int motor) {
  if (!VALID_MOTOR(motor)) return NXT_FAIL;
  nxt_pack_start(nxt,0x06);
  nxt_pack_byte(nxt,motor);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,25));
  test(nxt_unpack_start(nxt,0x06));
  if (nxt_unpack_error(nxt)==0) {
    nxt_unpack_byte(nxt); // Motor
    nxt_unpack_byte(nxt); // Power
    nxt_unpack_byte(nxt); // Mode
    nxt_unpack_byte(nxt); // Regulation Mode
    nxt_unpack_byte(nxt); // Turn Ratio
    nxt_unpack_byte(nxt); // RunState
    nxt_unpack_dword(nxt); // Tacho Limit
    nxt_unpack_dword(nxt); // Tacho Count
    nxt_unpack_dword(nxt); // Block Tacho Count
    return nxt_unpack_dword(nxt);
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
  nxt_pack_start(nxt,0x0A);
  nxt_pack_byte(nxt,motor);
  nxt_pack_byte(nxt,relative?1:0);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,3));
  test(nxt_unpack_start(nxt,0x0A));
  return nxt_unpack_error(nxt)==0?NXT_SUCC:NXT_FAIL;
}

/**
 * Stops program
 *  @param nxt NXT handle
 */
int nxt_stopprogram(nxt_t *nxt) {
  nxt_pack_start(nxt,0x01);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,3));
  test(nxt_unpack_start(nxt,0x01));
  return nxt_unpack_error(nxt)==0?NXT_SUCC:NXT_FAIL;
}

/**
 * Gets current program
 *  @param nxt NXT handle
 *  @return Name of current programm
 *  @note The return pointer can and should be passed to free()
 */
char *nxt_getcurprog(nxt_t *nxt) {
  nxt_pack_start(nxt,0x11);
  if (nxt_con_send(nxt)==NXT_FAIL) return NULL;
  usleep(60000); // latency of direct output commands: up to 60 ms
  if (nxt_con_recv(nxt,23)==NXT_FAIL) return NULL;
  if (nxt_unpack_start(nxt,0x11)==NXT_FAIL) return NULL;
  return nxt_unpack_error(nxt)==0?strdup(nxt_unpack_str(nxt,20)):NULL;
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
  nxt_pack_start(nxt,0x9B);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,33));
  test(nxt_unpack_start(nxt,0x9B));
  if (nxt_unpack_error(nxt)==0) {
    if (nxt_name!=NULL) *nxt_name = strdup(nxt_unpack_str(nxt,15));
    else nxt_unpack_str(nxt,15);
    if (bt_addr!=NULL) memcpy(bt_addr,nxt_unpack_mem(nxt,6),6);
    else nxt_unpack_mem(nxt,6);
    if (bt_strength!=NULL) *bt_strength = nxt_unpack_dword(nxt);
    else nxt_unpack_dword(nxt);
    if (free_flash!=NULL) *free_flash = nxt_unpack_dword(nxt);
    else nxt_unpack_dword(nxt);
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
  nxt_pack_start(nxt,0x0D);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,7));
  test(nxt_unpack_start(nxt,0x0D));
  return nxt_unpack_error(nxt)==0?nxt_unpack_dword(nxt):NXT_FAIL;
}

/**
 * Beep (plays a tone)
 *  @param nxt NXT handle
 *  @param freq Frequency (200..14000)
 *  @param dur Duration
 *  @return Success
 */
int nxt_beep(nxt_t *nxt,unsigned int freq,unsigned int dur) {
  nxt_pack_start(nxt,0x03);
  nxt_pack_word(nxt,freq);
  nxt_pack_word(nxt,dur);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,3));
  test(nxt_unpack_start(nxt,0x03));
  return nxt_unpack_error(nxt)==0?NXT_SUCC:NXT_FAIL;
}

/**
 * Plays a sound file
 *  @param nxt NXT handle
 *  @param filename Sound file
 *  @param loop Whether to loop sound playback
 *  @return Success
 */
int nxt_playsoundfile(nxt_t *nxt,char *filename,int loop) {
  nxt_pack_start(nxt,0x02);
  nxt_pack_byte(nxt,loop?1:0);
  nxt_pack_str(nxt,filename,20);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,3));
  test(nxt_unpack_start(nxt,0x02));
  return nxt_unpack_error(nxt)==0?NXT_SUCC:NXT_FAIL;
}

/**
 * Stops sound playback
 *  @param nxt NXT handle
 *  @return Success
 */
int nxt_stopsound(nxt_t *nxt) {
  nxt_pack_start(nxt,0x0C);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,3));
  test(nxt_unpack_start(nxt,0x0C));
  return nxt_unpack_error(nxt)==0?NXT_SUCC:NXT_FAIL;
}

/**
 * Starts a program
 *  @param nxt NXT handle
 *  @param filename Program file
 *  @return Success
 */
int nxt_startprogram(nxt_t *nxt,char *filename) {
  nxt_pack_start(nxt,0x00);
  nxt_pack_str(nxt,filename,20);
  test(nxt_con_send(nxt));
  usleep(60000);
  test(nxt_con_recv(nxt,3));
  test(nxt_unpack_start(nxt,0x00));
  return nxt_unpack_error(nxt)==0?NXT_SUCC:NXT_FAIL;
}

/**
 * Resets Bluetooth
 *  @param nxt NXT handle
 *  @return Success
 *  @note Can only be done via USB
 */
int nxt_resetbt(nxt_t *nxt) {
  if (nxt->contype==NXT_CON_BT) return NXT_FAIL;
  nxt_pack_start(nxt,0xA4);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,28));
  test(nxt_unpack_start(nxt,0xA4));
  if (nxt_unpack_error(nxt)==0) {
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
  nxt_pack_start(nxt,0xA1);
  nxt_pack_byte(nxt,buffer);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,5));
  test(nxt_unpack_start(nxt,0xA1));
  if (nxt_unpack_error(nxt)==0) {
    size_t size = nxt_unpack_byte(nxt);
    if (size>0) {
      // get command
      nxt_pack_start(nxt,0xA2);
      nxt_pack_byte(nxt,buffer);
      nxt_pack_byte(nxt,size);
      test(nxt_con_send(nxt));
      test(nxt_con_recv(nxt,64));
      test(nxt_unpack_start(nxt,0xA2));
      if (nxt_unpack_error(nxt)==0) {
        size = nxt_unpack_byte(nxt);
        if (size>0) {
          if (ptr!=NULL) *ptr = memcpy(malloc(size),nxt_unpack_mem(nxt,size),size);
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
  nxt_pack_start(nxt,0xA0);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,3));
  test(nxt_unpack_start(nxt,0xA0));
  return nxt_unpack_error(nxt)==0?NXT_SUCC:NXT_FAIL;
}
