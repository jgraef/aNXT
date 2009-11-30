/*
    nxttools.c - A C library of often used aNXT support functions
    aNXT - a NXt Toolkit
    Libraries and tools for LEGO Mindstorms NXT robots
    Copyright (C) 2008  Janosch Gräf <janosch.graef@gmx.net>
    Copyright (C) 2008  J. "MUFTI" Scheurich (IITS Universitaet Stuttgart)

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

#include "nxttools.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define NXT_BUFSIZE 4096

typedef struct {
  FILE *out;
  int show_filesizes;
  int show_hidden;
} nxt_list_file_data;

void nxt_print_file(char *filename,size_t filesize,void *data) {
  nxt_list_file_data *list_data = data;
  if (!list_data->show_hidden && *filename=='!') return;
  fprintf(list_data->out,"%s",filename);
  if (list_data->show_filesizes) fprintf(list_data->out,"\t%d",filesize);
  putc('\n',list_data->out);
}

int nxt_list(nxt_t *nxt,char *wildcard,nxt_list_one_file_callback callback,void *data) {
  char *filename;
  size_t filesize;
  int fh,valid_fh;
  char *wild = wildcard;

  if (wildcard==NULL)
    wild = "*.*";

  if ((fh = nxt_findfirst(nxt,wild,&filename,&filesize))!=NXT_FAIL) {
    do {
      valid_fh = fh;
      callback(filename,filesize,data);
    }
    while ((fh = nxt_findnext(nxt,fh,&filename,&filesize))!=NXT_FAIL);
    nxt_file_close(nxt,valid_fh);
  }

  if (nxt_error(nxt)==NXT_ERR_FILE_NOT_FOUND) {
    nxt_reset_error(nxt);
    return 0;
  } else {
    fprintf(stderr,"Error: %s\n",nxt_strerror(nxt_error(nxt)));
    return -1;
  }
}

int nxt_list_files(nxt_t *nxt,FILE *out,char *wildcard,int filesizes,int hidden) {
  nxt_list_file_data file_data;
  file_data.out = out;
  file_data.show_filesizes = filesizes;
  file_data.show_hidden = hidden;

  return nxt_list(nxt,wildcard,nxt_print_file,&file_data);
}

void nxt_print_mod(char *modname,int modid,size_t modsz,size_t iomapsz,void *data) {
  fprintf((FILE *)data,"%s\t",modname);
  if (strlen(modname) < 8)
  fprintf((FILE *)data,"\t");
  fprintf((FILE *)data,"%08x\t%d\t%d\n",modid,modsz,iomapsz);
}

int nxt_lsmod(nxt_t *nxt,char *wildcard,nxt_list_one_module_callback callback,void *data) {
  char *modname;
  size_t modsz,iomapsz;
  int handle,modid,last;
  char *wild;

  wild = wildcard;
  if (wildcard == NULL)
    wild = "*.mod";
  handle = nxt_mod_first(nxt,wild,&modname,&modid,&modsz,&iomapsz);
  if (handle!=NXT_FAIL) {
    callback(modname,modid,modsz,iomapsz,data);
    free(modname);
    last = handle;
    while ((handle = nxt_mod_next(nxt,last,&modname,&modid,&modsz,&iomapsz))!=NXT_FAIL) {
      nxt_mod_close(nxt,last);
      callback(modname,modid,modsz,iomapsz,data);
      free(modname);
      last = handle;
    }
  }
  if (nxt_error(nxt)==NXT_ERR_MODULE_NOT_FOUND) {
    nxt_reset_error(nxt);
    nxt_mod_close(nxt,last);
    return 0;
  } else {
    fprintf(stderr,"Error: %s\n",nxt_strerror(nxt_error(nxt)));
    return -1;
  }
}

int nxt_list_modules(nxt_t *nxt,FILE *out,char *wildcard) {
  return nxt_lsmod(nxt,wildcard,nxt_print_mod,out);
}

/**
 * Record motor tacho and send it to a function together with time for each value
 *  @param nxt      NXT handle
 *  @param motor    (0,1,2)
 *  @param t        how much seconds are values recorded
 *  @param callback function of type void func(double t,int tacho, void *data); to be called for each recorded value
 *  @param data     void* data pointer for use in callback
 */

void nxt_motor_record(nxt_t *nxt,int motor,double t,nxt_motor_record_callback callback,void *data) {
  struct timeval tv, starttv;
  int rot;
  int err;
  double time = 0;
  int capacity = 256;
  int fill_counter = 0;
  nxt_motor(nxt,motor,0,0,NXT_MOTORON,NXT_REGMODE_MOTOR_SPEED,0);
  gettimeofday(&starttv,NULL);
  while (1) {
    rot = nxt_tacho(nxt,motor);
    err = nxt_error(nxt);
    if (err != 0) 
      break;
    gettimeofday(&tv,NULL);
    time = tv.tv_sec - starttv.tv_sec + (tv.tv_usec - starttv.tv_usec) * 1e-6;
    if (time > t) 
      break;
    callback(time,rot,data);
  }
}

/**
 * Play back recorded motor tacho values
 *  @param nxt       NXT handle
 *  @param motor     (0,1,2)
 *  @param power     (-100..100)
 *  @param numvalues number of values to play back
 *  @param times     array of doubles with target times
 *  @param rotations array of integer with target tacho values
 *  @param stop      if != 0 coast after last value. Default: brake (block) motor after last value
 *  @param verbose   if != 0 print target and real tacho values to standard out
 */

void nxt_motor_playback(nxt_t *nxt,int motor,int power,int numvalues,double *times,int *rotations,int stop,int verbose) {
  int newtacho,oldtacho,sign;
  struct timeval tv,starttv;
  int i;
  int timeout = 0;

  if (numvalues == 0)
    return;

  oldtacho = nxt_tacho(nxt,motor);
  if (nxt_error(nxt) != 0)
    return;

  /* play back values */
  gettimeofday(&starttv,NULL);
  i = 0;
  while (1) {
    int oldi = i;
    double time;
    int tacho;
    if (timeout == 0) {
      gettimeofday(&tv,NULL);
      time = tv.tv_sec - starttv.tv_sec + (tv.tv_usec - starttv.tv_usec) * 1e-6;
      while (time > *(times + i - 1))
        if (i < (numvalues - 1))
          i++;
        else {
          timeout = 1;
          break;
        }
    }    
    // new value or fix tacho at end
    if ((i > oldi) || (timeout == 1)) {
      tacho = nxt_tacho(nxt,motor);
      newtacho = *(rotations + i - 1) - *rotations + oldtacho;
      if (nxt_error(nxt) != 0)
        break;
      if (newtacho > tacho)
        sign = 1;
      else if (newtacho < tacho)
        sign = -1;
      else {
        sign = 0;
        if (timeout == 1) {
          nxt_setmotorbrake(nxt,motor);
          break;
        }
      }
      if (verbose)
        printf("target tacho:\t%d\tcurrent tacho:\t%d\n",newtacho,tacho);
      nxt_setmotorrotation(nxt,motor,newtacho,sign * power);
      if (nxt_error(nxt) != 0)
        break;
    }
  }

  if (stop == 1)
    nxt_setmotorcoast(nxt,motor);
}

/**
 * Read time/tacho values from file into memory
 *  @param numvalues pointer to number of values to play back
 *  @param times     pointer to array of doubles with target times
 *  @param rotations pointer to array of integer with target tacho values
 *  @param file      file handle for input file
 *  @return != 0 on error
 */

int nxt_read_recorded_file(int *numvalues,double **times,int **rotations,FILE *file) {
  size_t size = 0;
  char *buf = malloc(NXT_BUFSIZE);
  char *readbuffer = NULL;
  *numvalues = 0;
  *times = (double *)malloc(sizeof(double));
  *rotations = (int *)malloc(sizeof(int));

  /* read values to play back from file */
  while (!feof(file)) {
    size += fread(buf+size,1,NXT_BUFSIZE,file);
    buf = realloc(buf,size+NXT_BUFSIZE);
  }

  /* add newline at end of data if needed */
  if (buf[size - 1] != '\n') {
    buf = realloc(buf,size + 1);
    buf[size] = '\n';
    size++;
  }

  /* parse values */
  readbuffer = buf;
  while (readbuffer < (buf + size)) {
    char *nextspace;
    char *nextnewline;
    nextspace = strchr(readbuffer,' ');
    nextnewline = strchr(readbuffer,'\n');
    if ((nextspace == NULL) || (nextnewline == NULL) ||
        (nextnewline < nextspace)) {
      fprintf(stderr, "syntax error in line %d\n", *numvalues + 1);
      return 1;
    }
    *times = realloc(*times,(*numvalues + 1) * sizeof(double));
    *rotations = (int *)realloc(*rotations,(*numvalues + 1) * sizeof(int));
    *nextspace = (char)0;
    sscanf(readbuffer,"%lf",*times + *numvalues);
    readbuffer = nextspace + 1;
    while ((*readbuffer) == ' ')
      readbuffer++;
    *nextnewline = (char)0;
    sscanf(readbuffer,"%d",*rotations + *numvalues);
    *numvalues = *numvalues + 1;
    readbuffer = nextnewline + 1;
  }
  return 0;
}

static char *types[] = {
  "none",
  "switch",
  "temperature",
  "reflection",
  "angle",
  "light_active",
  "light_inactive",
  "sound_db",
  "sound_dba",
  "custom",
  "lowspeed",
  "lowspeed_9v"
};

/**
 * Gets number of type of sensor from a string like "angle"
 *  @param str string with sensorname
 *  @return number of type of sensor or -1 on failure
 */

int nxt_str2type(char *str) {
  int i;
  for (i=0;i<NXT_NUM_TYPES;i++) {
    if (strcasecmp(str,types[i])==0) return i;
  }
  return -1;
}

/**
 * Gets string like "angle" from list of mode of sensor 
 *  @param i number of mode of sensor 
 *  @return string with type of sensor or NULL on failure
 */

char *nxt_getType(int i) {
  if (i < NXT_NUM_TYPES)
    return types[i];
  else
    return NULL;
}

static char *modes[] = {
  "raw",
  "boolean",
  "transition_count",
  "period_count",
  "percent",
  "celsius",
  "fahrenheit",
  "angle_step"
};

/**
 * Gets number of mode of sensor from a string like "raw"
 *  @param str string with sensorname
 *  @return number of mode of sensor or -1 on failure
 */

int nxt_str2mode(char *str) {
  int i;
  for (i=0;i<NXT_NUM_MODES;i++) {
    if (strcasecmp(str,modes[i])==0) return i*0x20;
  }
  return -1;
}

/**
 * Gets string like "raw" from list of mode of sensor 
 *  @param i number of mode of sensor in list from 0 to NXT_NUM_MODES
 *  @return string with mode of sensor or NULL on failure
 */

char *nxt_getMode(int i) {
  if (i < NXT_NUM_MODES)
    return modes[i];
  else
    return NULL;
}

/**
 * Gets number of motor port from a string like "A"
 *  @param str string of motor
 *  @return number of motor port or -1 on failure
 */
int nxt_str2motorport(char *str) {
  int motor;
  if (strcasecmp(str,"abc")==0) motor = NXT_MOTORABC;
  else if (strlen(str)==1) {
    motor = tolower(*str)-'a';
    if (motor<0 || motor>2) motor = -1;
  }
  else motor = -1;
  return motor;
}

/**
 * Gets number of motor port from a string like "BC"
 *  @param str string of motor
 *  @param index number of motor
 *  @return number of first motor port or -1 on failure
 */
int nxt_str2motorport_index(char *str, int index) {
  int motor;
  char string[2];
  if ((index<strlen(str)) && (index > 0)) {
    string[0] = str[index];
    string[1] = 0;
    return nxt_str2motorport(string);
  }
  else motor = -1;
  return motor;
}

/**
 * Gets number of first motor port from a string like "BC"
 *  @param str string of motor
 *  @return number of first motor port or -1 on failure
 */
int nxt_str2motorport1(char *str) {
  return nxt_str2motorport_index(str, 1);
}

/**
 * Gets number of second motor port from a string like "BC"
 *  @param str string of motor
 *  @return number of second motor port or -1 on failure
 */
int nxt_str2motorport2(char *str) {
  return nxt_str2motorport_index(str, 2);
}

/**
 * Gets number of button from a string like "enter"
 *  @param str string of button
 *  @return number of button or 0 on failure
 */
int nxt_str2btn(char *str) {
  if (strcasecmp(str,"enter")==0) return NXT_UI_BUTTON_ENTER;
  else if (strcasecmp(str,"left")==0) return NXT_UI_BUTTON_LEFT;
  else if (strcasecmp(str,"right")==0) return NXT_UI_BUTTON_RIGHT;
  else if (strcasecmp(str,"exit")==0) return NXT_UI_BUTTON_EXIT;
  else return 0;
}

/**
 * Gets number of bitmap fileformat from a string like "png"
 *  @param str string of bitmap fileformat
 *  @return number of fileformat or 0 on failure
 */
int nxt_str2fmt(char *str) {
  if (strcasecmp(str,"jpeg")==0) return NXT_JPEG;
  else if (strcasecmp(str,"png")==0) return NXT_PNG;
  else return 0;
}

#define BUFSIZE 4096

/**
 * Download a file src from NXT brick to file dest on host filesystem
 *  @param nxt NXT handle
 *  @param src file on NXT
 *  @param dest filename on host
 *  @return Success?
 */
int nxt_download(nxt_t *nxt,char *src,char *dest) {
  size_t size = 0;
  int fh;
  int ret = 0;
  char *buffer = NULL;

  fh = nxt_file_open(nxt,src,NXT_OREAD,&size);
  if (fh>=0) {
    FILE *output = NULL;
    if (strcmp(dest,"-")==0) output = stdout;
    else {
      output = fopen(dest,"w");
      if (output==NULL) {
        fprintf(stderr,"Error: %s\n",strerror(errno));
        return -1;
      }
    }
    if (size>0) {
      buffer = malloc(size);
      size = nxt_file_read(nxt,fh,buffer,size);
      if (size>=0) {
        int rest;
        rest = size;
        do {
          int written;
          written = fwrite(buffer,1,rest,output);
          if (written<=0) {
            perror(dest);
            ret = -1;
          }
          rest -= written;
        } while (rest>0);
      } else {
        fprintf(stderr,"Error: %s\n",nxt_strerror(nxt_error(nxt)));
        ret = -1;
      }
    }
    if (output!=stdout)
      if (fclose(output)!=0) {
        perror(dest);
        ret = -1;
      }
  } else {
    fprintf(stderr,"Error: %s\n",nxt_strerror(nxt_error(nxt)));
    ret = -1;
  }

  if (buffer!=NULL) free(buffer);
  if (fh>=0) nxt_file_close(nxt,fh);
  return ret;
}

/**
 * Download a file src from NXT brick to file dest on host filesystem
 *  @param nxt NXT handle
 *  @param src filename on host
 *  @param dest file on NXT
 *  @param oflag open flags for nxt_file_open
 *  @return Success?
 */

int nxt_upload(nxt_t *nxt,char *src,char *dest,int oflag) {
  char *buf = malloc(BUFSIZE);
  size_t size = 0;
  int ret = 0;
  FILE *input;

  if (strcmp(src,"-")==0) input = stdin;
  else {
    input = fopen(src,"r");
    if (input==NULL) {
      fprintf(stderr,"Error: %s\n",strerror(errno));
      return -1;
    }
  }

  while (!feof(input)) {
    size += fread(buf+size,1,BUFSIZE,input);
    buf = realloc(buf,size+BUFSIZE);
  }

  int handle = nxt_file_open(nxt,dest,oflag,size);
  if (handle>=0) {
    nxt_file_write(nxt,handle,buf,size);
    nxt_file_close(nxt,handle);
  }
  else {
    fprintf(stderr,"Error: %s\n",nxt_strerror(nxt_error(nxt)));
    ret = -1;
  }

  free(buf);
  return ret;
}
