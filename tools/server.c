/*
    server.c
    aNXT - a NXt Toolkit
    Libraries and tools for LEGO Mindstorms NXT robots
    Copyright (C) 2008  Janosch Gräf <janosch.graef@gmx.net>
    Copyright (C) 2008  J. "MUFTI" Scheurich (IITS Universitaet Stuttgart)

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

/*
   A server program to start nxt_ commandline commands via named pipes.
   This avoids the startup time, especially for bluetooth commands

   The named pipes for communication are named /tmp/nxt_server_in_"key" and
   /tmp/nxt_server_out__"key" (key is either gotten as commandlineargument 
   or the own processid)
*/

#include <nxt.h>
#include <nxt_i2c/us.h>
#include <nxttools.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <gd.h>

static nxt_t *nxt;
static FILE *out = NULL;
static FILE *err = NULL;
static int vflag = 0;
static FILE *tty = NULL;
static int open_each_command = 0;

static char fifo_in[PATH_MAX];
static char fifo_out[PATH_MAX];
static char fifo_err[PATH_MAX];

static void *open_output(char *fifo_out)
{
  if (vflag)
    fprintf(tty,"open %s as output fifo\n",fifo_out);
  out = fopen(fifo_out,"wb");
  if (out==NULL) {
    perror(fifo_out);
    exit(1);
  }
}

static void *open_error(char *fifo_err)
{
  if (vflag)
    fprintf(tty,"open %s as error output fifo\n",fifo_err);
  err = freopen(fifo_err,"wb",stderr);
  if (err==NULL) {
    perror(fifo_err);
    exit(1);
  }
}

void error()
{
}

/* common options and arguments */

/* nxt_beep arguments */
static unsigned int frequency; /* argument for option f */
static unsigned int duration;  /* argument for option d */

/* nxt_button arguments */
static int button;             /* argument for option b */

/* nxt_download arguments */
static int force;              /*              option f */
static int oflag;              /* argument for option o */

/* nxt_motor arguments */
static int motor;              /* argument for option m */
static int power;              /* argument for option p */
static int rot;                /* argument for option r */
static int brake;              /*              option b */
static int stop;               /*              option s */
static int syncro;             /*              option y */
static int idle;               /*              option i */

/* nxt_motor_record arguments */
static double time;            /* argument for option t */

/* nxt_motor_travel arguments */
static int motor1;             /* argument for option m */
static int motor2;             /* argument for option m */
static int turnratio;          /* argument for option t */

/* nxt_pollcmd arguments */
static int buffer = NXT_BUFFER_POLL; /* argument for option b */

/* nxt_recv arguments */
static int mailbox = 0;          /* argument for option m */
static int clear_mailbox = 0;    /*              option c */
static int verbose = 0;          /*              option v */

/* nxt_sensor arguments */
static int type = NXT_SENSOR_TYPE_NONE; /* argument for option t */
static int mode = NXT_SENSOR_MODE_RAW;  /* argument for option m */
static int sensor = -1;                  /* argument for option s */
static int reset = 0;                   /*              option r */

/* nxt_list arguments */
static int size;              /*              option s */
static int all;               /*              option a */

/* nxt_screenshot arguments */
static int format;                     /* argument for option f */
static char *output_screenshot = NULL; /* argument for option o */

/* 1. and 2. argument */
static char *arg1 = NULL;
static char *arg2 = NULL;

static void reset_options(void)
{
  frequency = 440;
  duration = 200;

  button = NXT_UI_BUTTON_ENTER;

  motor = 0;
  power = INT_MAX;
  rot = 0;  
  brake = 0;
  syncro = 0;
  idle = 0;

  time = 10;

  motor1 = 1;
  motor2 = 2;
  turnratio = 0;

  force = 0;
  oflag = 0;

  size = 0;
  all = 0;

  format = NXT_PNG;
  if (output_screenshot != NULL)
    free(output_screenshot);
  output_screenshot = NULL;

  buffer = NXT_BUFFER_POLL;

  mailbox = 0;
  clear_mailbox = 0;
  verbose = 0;

  type = NXT_SENSOR_TYPE_NONE; 
  mode = NXT_SENSOR_MODE_RAW;  
  sensor = -1;                  
  reset = 0;                   

  if (arg1 != NULL)
    free(arg1);
  arg1 = NULL;
  if (arg2 != NULL)
    free(arg2);
  arg2 = NULL;
}

static void handle_option(char command)
{
   switch(command) {
     case 'a':
       all = 1; 
       break;
     case 'b':
       brake = 1;
       break;
     case 'c':
       clear_mailbox = 1;
       break;
     case 'f':
       force = 1; 
       break;
     case 'i':
       idle = 1; 
       break;
     case 'r': 
       reset = 1;
       break;
     case 's': 
       power = 0;
       size = 0;
       break;
     case 'v':
       verbose = 1; 
       break;
     case 'y':
       syncro = 1; 
       break;
   }
}

static void handle_argument(char option, char* argument)
{
   switch(option) {
     case 'f':
       frequency = atoi(argument);
       format = nxt_str2fmt(argument);
       break;
     case 'd':
       duration = atoi(argument);
       break;

     case 'b':
       button = nxt_str2btn(argument);
       if (strcasecmp(argument,"highspeed")==0) 
         buffer = NXT_BUFFER_HIGHSPEED;
       else if (strcasecmp(argument,"poll")==0)       
         buffer = NXT_BUFFER_POLL;     
       break;

     case 'm':
       motor = nxt_str2motorport(argument);
       motor1 = nxt_str2motorport1(argument);
       motor2 = nxt_str2motorport2(argument);
       mailbox = atoi(argument)-1;
       mode = nxt_str2mode(argument);
       break;
     case 'p': 
       power = atoi(argument);
       break;
     case 'r': 
       rot = atoi(argument);
       break;

     case 'o':
       if (strcasecmp(argument,"fragment")==0 || strcasecmp(argument,"f")==0) oflag = NXT_OWFRAG;
       else if (strcasecmp(argument,"linear")==0 || strcasecmp(argument,"l")==0) oflag = NXT_OWLINE;
       else if (strcasecmp(argument,"append")==0 || strcasecmp(argument,"a")==0) oflag = NXT_OAPPND;
       output_screenshot = strdup(argument);       
       break;     

     case 's': 
       sensor = atoi(argument) - 1;
       break;
     case 't': 
       sscanf(argument,"%lf",&time);
       type = nxt_str2type(argument);
       turnratio = atoi(argument);
       break;


     /* usefull for download/upload commands */
     case '/':
       chdir(argument);
       break;

     case '1':
       arg1 = strdup(argument);
       break;
     case '2':
       arg2 = strdup(argument);
       break;
      
   }
}

struct nxt_server_output_data {
  char *outptr;
  int rest;
  int show_filesize;
  int show_hidden;
};

#define OUTPUT_SIZE 8000

/* 
    callback for nxt_list to list file into output memory
 */

void nxt_server_print_file(char *filename,size_t filesize,void *data) {
  struct nxt_server_output_data *list_data = data;
  if (!list_data->show_hidden && *filename=='.') return;
  if (list_data->show_filesize) 
    snprintf(list_data->outptr,list_data->rest,"%s\t%d\n",filename,filesize);
  else
    snprintf(list_data->outptr,list_data->rest,"%s\n",filename);
  int size = strlen(list_data->outptr);
  list_data->outptr += size;
  list_data->rest -= size;      
}

/* 
    callback for nxt_lsmod to list modules into output memory
 */

void nxt_server_print_module(char *modname,int modid,size_t modsz,size_t iomapsz,void *data) {
  struct nxt_server_output_data *list_data = data;
  if (strlen(modname) < 8)
    snprintf(list_data->outptr,list_data->rest,"%s\t\t%08x\t%d\t%d\n",modname,modid,modsz,iomapsz);
  else
    snprintf(list_data->outptr,list_data->rest,"%s\t%08x\t%d\t%d\n",modname,modid,modsz,iomapsz);
  int size = strlen(list_data->outptr);
  list_data->outptr += size;
  list_data->rest -= size;      
}

/* 
   callback for nxt_motor_record to 
 */
void nxt_motor_record_print_time_tacho(double time,int tacho, void *data) {
  FILE *file = (FILE *) data; 
  fprintf(file, "%lf %d\n",time,tacho);
}

#define UPDATE_OUTPUT_PTR() \
  { \
  outptr = output + strlen(output); \
  rest = OUTPUT_SIZE - strlen(output); \
  }

static void handle_commmands(char *command, char *fifo_out, char *fifo_err)
{
  char output[OUTPUT_SIZE];
  int rest = OUTPUT_SIZE;
  char *outptr = output;
  if (strlen(command) == 1) {
    /* single option */
    handle_option(command[0]);
  } else if ((command[1]==' ') && (strlen(command) > 2)) {
    /* option with argument */
    handle_argument(command[0],command + 2);
  } else if (strcmp(command,"exit")==0) {
    return;
  } else {
    if (open_each_command)
      output[0] = 0;
    else {
      output[0]='\n';
      output[1]=0;
    }
    if (strcmp(command,"beep")==0) {
      nxt_beep(nxt,frequency,duration);
    } else if (strcmp(command,"delflash")==0) {
      nxt_deluserflash(nxt);
    } else if (strcmp(command,"download")==0) {
      if (arg1 != NULL) {
         if (arg2 == NULL)
             arg2 = strdup(arg1);
         nxt_download(nxt,arg1,arg2);  
      }
    } else if (strcmp(command,"getprog")==0) {
      char *prog;
      prog = nxt_getcurprog(nxt);
      if (prog != NULL)
        snprintf(output,OUTPUT_SIZE,"%s\n",prog);
    } else if (strcmp(command,"info")==0) {
      char *nxt_name;
      unsigned char bt_addr[6];
      unsigned int bt_strength,free_flash;
      int firmmaj,firmmin,protomaj,protomin,battery,devinfo,ver,volume;
  
      volume = nxt_getvolume(nxt);
      battery = nxt_getbattery(nxt);
      devinfo = nxt_getdevinfo(nxt,&nxt_name,bt_addr,&bt_strength,&free_flash);
      ver = nxt_getver(nxt,&firmmaj,&firmmin,&protomaj,&protomin);
  
      snprintf(outptr,rest,"Name:               %s\n",nxt_name);
      UPDATE_OUTPUT_PTR()

      if (battery>=0) {
        snprintf(outptr,rest,"Battery:            %dmV\n",battery);
        UPDATE_OUTPUT_PTR()
      }
      if (volume>=0) {
        snprintf(outptr,rest,"Volume:             %d\n",volume);
        UPDATE_OUTPUT_PTR()
      }
      if (devinfo>=0) {
        snprintf(outptr,rest,"Bluetooth Address:  %02x:%02x:%02x:%02x:%02x:%02x\n",bt_addr[0],bt_addr[1],bt_addr[2],bt_addr[3],bt_addr[4],bt_addr[5]);
        UPDATE_OUTPUT_PTR()
        snprintf(outptr,rest,"Bluetooth Strength: %u%%\n",bt_strength);
        UPDATE_OUTPUT_PTR()
        snprintf(outptr,rest,"Free flash:         %u bytes\n",free_flash);
        UPDATE_OUTPUT_PTR()
      }
      if (ver>=0) {
        snprintf(outptr,rest,"Firmware version:   %u.%u\n",firmmaj,firmmin);
        UPDATE_OUTPUT_PTR()
        snprintf(outptr,rest,"Protocol version:   %u.%u\n",protomaj,protomin);
        UPDATE_OUTPUT_PTR()
      }
    } else if (strcmp(command,"list")==0) {
      struct nxt_server_output_data list_data;
      list_data.show_filesize = size;
      list_data.show_hidden = all;
      list_data.outptr = output;
      list_data.rest = OUTPUT_SIZE;
      nxt_list(nxt,arg1,nxt_server_print_file,&list_data);
    } else if (strcmp(command,"lsmod")==0) {
      struct nxt_server_output_data list_data;
      list_data.outptr = output;
      list_data.rest = OUTPUT_SIZE;
      nxt_lsmod(nxt,arg1,nxt_server_print_module,&list_data);
    } else if (strcmp(command,"motor")==0) {
      if (power == INT_MAX)
        power = 50;
        if (idle)
          nxt_motor(nxt,motor,rot,brake?0:power,NXT_MOTORON|(brake?NXT_BRAKE|NXT_REGULATED:0),NXT_REGMODE_IDLE,turnratio);
        else
          nxt_motor(nxt,motor,rot,brake?0:power,NXT_MOTORON|(brake||syncro?NXT_BRAKE|NXT_REGULATED:0),syncro?NXT_REGMODE_MOTOR_SYNC:NXT_REGMODE_MOTOR_SPEED,turnratio);
    } else if (strcmp(command,"motor_playback")==0) {
      int numvalues;
      double *times;
      int *rotations;
      if (arg1 != NULL) {
        FILE *file = fopen(arg1,"r");
        if (file != NULL) {
          if (nxt_read_recorded_file(&numvalues,&times,&rotations,file) == 0) {
            if (power == INT_MAX)
              power = 50;
            nxt_motor_playback(nxt,motor,power,numvalues,times,rotations,stop,0);
            if (verbose)
              fprintf(tty,"nxt_motor_playback verbose not supported by server.c\n");
            free(times);
            free(rotations);
          }
          fclose(file);
        }
      }
    } else if (strcmp(command,"motor_record")==0) {
      if (arg1 != NULL) {
        FILE *file = fopen(arg1,"w");
        if (file != NULL) {
          nxt_motor_record(nxt,motor,time,nxt_motor_record_print_time_tacho,file);
          if (verbose)
            fprintf(tty,"nxt_motor_record verbose not supported by server.c\n");
          fclose(file);
        }
      } else
        fprintf(tty,"Error: motor_record need result file as first argument\n");

    } else if (strcmp(command,"motor_travel")==0) {
      int rotation1 = 0;
      int rotation2 = 0;
      int regmode = 0;
      int synchronise = 1;

      nxt_motor(nxt,motor1,rotation1,0,brake?NXT_MOTORON|NXT_BRAKE|NXT_REGULATED:0,brake?:NXT_REGMODE_IDLE,turnratio);
      nxt_motor(nxt,motor2,rotation2,0,brake?NXT_MOTORON|NXT_BRAKE|NXT_REGULATED:0,brake?:NXT_REGMODE_IDLE,turnratio);

      nxt_resettacho(nxt,motor1,1);
      nxt_resettacho(nxt,motor2,1);

      if (idle) {
        mode = brake?0:power,NXT_MOTORON|(brake?NXT_BRAKE|NXT_REGULATED:0);
        regmode = NXT_REGMODE_IDLE;
      } else {
        mode = NXT_MOTORON|(brake||(synchronise && (turnratio == 0))?NXT_BRAKE:0)|NXT_REGULATED;
       regmode = synchronise && (power != 0)?NXT_REGMODE_MOTOR_SYNC:NXT_REGMODE_MOTOR_SPEED;
      }
 
      nxt_motor(nxt,motor1,rotation1,brake?0:power,mode,regmode,turnratio);
      nxt_motor(nxt,motor2,rotation2,brake?0:power,mode,regmode,turnratio);
    } else if (strcmp(command,"pollcmd")==0) {
      void *cmd;
      ssize_t size;
      size = nxt_pollcmd(nxt,&cmd,buffer);
      if (size>0) {
        fwrite(cmd,1,size,out);
        free(cmd);
      }
    } else if (strcmp(command,"recv")==0) {
      char *msg;
      if ((msg = nxt_recvmsg(nxt,mailbox,clear_mailbox))!=NULL) {
        if (verbose) snprintf(output,OUTPUT_SIZE,"Mailbox %d: ",mailbox+1);
        snprintf(output,OUTPUT_SIZE,"%s%c",msg,verbose?'\n':0);
        free(msg);
      }
    } else if (strcmp(command,"remove")==0) {
      if (arg1 != NULL)
        nxt_file_remove(nxt,arg1);
    } else if (strcmp(command,"resetbt")==0) {
      nxt_resetbt(nxt);
    } else if (strcmp(command,"run")==0) {
      if (arg1 != NULL)
        nxt_startprogram(nxt,arg1);
    } else if (strcmp(command,"screenshot")==0) {
      char screen[64][100];
      int transparency = 0;
      int x,y;
      if (nxt_screenshot(nxt,screen)==0) {
        gdImagePtr im;
        int black,white;
        im = gdImageCreate(100,64);
        black = gdImageColorAllocate(im,0,0,0);
        white = gdImageColorAllocate(im,255,255,255);
        if (transparency) gdImageColorTransparent(im,white);
        for (y=0;y<64;y++) {
          for (x=0;x<100;x++) gdImageSetPixel(im,x,y,screen[y][x]?black:white);
        }
        FILE *bitmap = fopen(output_screenshot!=NULL?output_screenshot:(format==NXT_JPEG?"display.jpg":"display.png"),"w");
        if (format==NXT_JPEG) gdImageJpeg(im,bitmap,-1);
        else if (format==NXT_PNG) gdImagePng(im,bitmap);
        fclose(bitmap);
        gdImageDestroy(im);
      }
    } else if (strcmp(command,"send")==0) {
      if (arg1 != NULL)
        nxt_sendmsg(nxt,mailbox,arg1);
    } else if (strcmp(command,"sensor")==0) {
      char *unit = NULL;
      if (sensor==-1) {
        if (type==3 || type==5 || type==6) sensor = 2;
        else if (type==7 || type==8) sensor = 1;
        else sensor = 0;
      }
      if (unit==NULL) {
        if (mode==0xA0) unit = "°C";
        else if (mode==0xC0) unit = "°F";
        else if (mode==0x80) unit = "%";
        else unit = "";
      }
      nxt_setsensormode(nxt,sensor,type,mode);
      usleep(100000);
      int val;
      val = nxt_getsensorval(nxt,sensor);
      if (reset) 
        nxt_setsensormode(nxt,sensor,NXT_SENSOR_TYPE_NONE,NXT_SENSOR_MODE_RAW);
      if (val >= 0) {
        if (verbose) snprintf(output,OUTPUT_SIZE,"Sensor %d: ",sensor+1,val);
        snprintf(output,OUTPUT_SIZE,"%d%s\n",val,verbose?unit:"");
      }
    } else if (strcmp(command,"sensorus")==0) {
      int dist;
      nxt_setsensormode(nxt,sensor,NXT_SENSOR_TYPE_LOWSPEED,NXT_SENSOR_MODE_RAW);
      usleep(60000);
      dist = nxt_us_get_dist(nxt,sensor);
      if (reset)
        nxt_setsensormode(nxt,sensor,NXT_SENSOR_TYPE_NONE,NXT_SENSOR_MODE_RAW);
      if (dist>=0) {
        if (verbose) snprintf(output,OUTPUT_SIZE,"Sensor %d: ",sensor+1);
        if (dist==0xFF) snprintf(output,OUTPUT_SIZE,"?\n");
        else snprintf(output,OUTPUT_SIZE,"%d%s\n",dist,verbose?"cm":"");
      }
    } else if (strcmp(command,"setbutton")==0) {
      nxt_setbutton(nxt,button);
    } else if (strcmp(command,"setname")==0) {
      if (arg1 != NULL)
        nxt_setname(nxt,arg1);
    } else if (strcmp(command,"stop")==0) {
      nxt_stopprogram(nxt);
    } else if (strcmp(command,"tacho")==0) {
      int rot;
      rot = nxt_tacho(nxt,motor);
      if (reset)
        nxt_resettacho(nxt,motor,0);
      if (verbose) 
        snprintf(output,OUTPUT_SIZE,"Motor %c: ",motor+'A');
      snprintf(output,OUTPUT_SIZE,"%d%s\n",rot,verbose?"°":"");
    } else if (strcmp(command,"turnoff")==0) {
      nxt_turnoff(nxt);
    } else if (strcmp(command,"up_run")==0) {
      if (arg1 != NULL) {
        int ret;
        int oflag = NXT_OWLINE;

        nxt_stopprogram(nxt);

        nxt_wait_after_direct_command();

        if (arg2 == NULL)
           arg2 = strdup(arg1);
        oflag |= NXT_OWOVER;
        nxt_upload(nxt,arg1,arg2,oflag);

        nxt_wait_extra_long_after_communication_command();

        ret = nxt_error(nxt);
        if (ret == 0)
          nxt_startprogram(nxt,arg2);
      }
    } else if (strcmp(command,"upload")==0) {
      if (arg1 != NULL) {
        if (arg2 == NULL)
          arg2 = strdup(arg1);
        nxt_upload(nxt,arg1,arg2,(force && oflag!=NXT_OAPPND)?NXT_OWOVER:0);
      }
    } else if (strcmp(command,"dummy")==0) {
    } else
      fprintf(tty,"command %s not understood\n",command);

    if (open_each_command)
      open_error(fifo_err);  
    if (nxt_error(nxt) != 0)
      fprintf(err,"Error: %s\n",nxt_strerror(nxt_error(nxt)));
    else if (open_each_command)
      fprintf(err,"");
    else
      fprintf(err,"\n");
    fflush(err);
    if (open_each_command)
      fclose(err);

    if (open_each_command)
      open_output(fifo_out);
    else {
      outptr = output + strlen(output);
      rest = rest - strlen(output); 
      snprintf(outptr,rest,"\n");
      UPDATE_OUTPUT_PTR()
    }
    fprintf(out,"%s",output);
    fflush(out);
    if (open_each_command)
      fclose(out);

    reset_options();
  }
}

static void usage(char *cmd,int r) {
  FILE *out = r==0?stdout:stderr;
  fprintf(out,"Usage: %s [OPTIONS] [KEY]\n",cmd);
  fprintf(out,"Opens a NXT brick for use via named pipes, KEY is used for generating pipe names\n");
  fprintf(out,"Options:\n");
  fprintf(out,"\t-h          Show help\n");
  fprintf(out,"\t-n NXTNAME  Name of NXT (Default: first found) or bluetooth address\n");
  fprintf(out,"\t-o          Open/close error/output pipes on each command (for scripts)\n");
  fprintf(out,"\t-v          verbose mode: show internals\n");
  exit(r);
}

#define MAX_COMMAND_LEN 1024

int main(int argc,char *argv[]) {
  char *name = NULL;
  int c;
  FILE *in = NULL;
  int pid;
  int verbose = 0;

  char key[1024];
  char command[MAX_COMMAND_LEN];
  key[0] = 0;
  command[0] = 0;

  reset_options();

  while ((c = getopt(argc,argv,":hvon:"))!=-1) {
    switch(c) {
      case 'h':
        usage(argv[0],0);
        break;
      case 'n':
        name = strdup(optarg);
        break;
      case 'o':
        open_each_command = 1;
        break;
      case 'v':
        vflag = 1;
        verbose = 1;
        break;
      case ':':
        fprintf(stderr,"Option -%c requires an operand\n",optopt);
        usage(argv[0],1);
        break;
      case '?':
        fprintf(stderr,"Unrecognized option: -%c\n",optopt);
        usage(argv[0],1);
        break;
    }
  }

  if (key[0] == 0) 
    if (optind < argc)
       snprintf(key,sizeof(key) - 1,"%s",argv[optind]);
    else
       snprintf(key,sizeof(key) - 1,"%d", getpid());

  if (vflag)
    tty = fopen("/dev/tty","w");

  nxt = nxt_open(name);
  if (nxt==NULL) {
    fprintf(stderr,"Could not find NXT\n");
    return 1;
  }

  if (strlen(key) == 0) {
    snprintf(key,sizeof(key) - 1,"%d",getpid());
    vflag = 1;
  }

  snprintf(fifo_in,PATH_MAX - 1,"/tmp/nxt_server_in_%s",key);
  if (mkfifo(fifo_in,0666) != 0) {
    perror(fifo_in);
  }

  snprintf(fifo_out,PATH_MAX - 1,"/tmp/nxt_server_out_%s",key);
  if (mkfifo(fifo_out,0666) != 0) {
    perror(fifo_out);
  }

  snprintf(fifo_err,PATH_MAX - 1,"/tmp/nxt_server_err_%s",key);
  if (mkfifo(fifo_err,0666) != 0) {
    perror(fifo_err);
  }

  if (verbose || vflag) {
    printf("%s\n",fifo_in);  
    printf("%s\n",fifo_err);  
    printf("%s\n",fifo_out);  
  }

  if (vflag)
    fprintf(tty,"open %s as input fifo\n",fifo_in);
  in = fopen(fifo_in,"rb");
  if (in==NULL) {
    perror("open input fifo");
    return 1;
  }

  if (!open_each_command) {
    open_error(fifo_err);
    open_output(fifo_out);
  }
  
  while (strcmp(command,"exit")!=0) {
    char *newline;
    static int got_command = 1;
    command[0] = 0;
    if (vflag && got_command)
      fprintf(tty,"read input from %s\n",fifo_in);
    if (fgets(command,MAX_COMMAND_LEN-1,in) == NULL) {
      got_command = 0;
      continue;
    } else
      got_command = 1;
    if (strlen(command)==MAX_COMMAND_LEN-1)
      continue;
    if (strlen(command) == 0)
      continue;
    newline = strchr(command,'\n');
    if (newline != NULL)
      *newline = 0;
    if (vflag)
      fprintf(tty,"got command \"%s\"\n",command);
    handle_commmands(command,fifo_out,fifo_err);
  }

  if (!open_each_command) {
    if (out != NULL)
      fclose(out);
    if (err != NULL)
      fclose(err);
  }
  fclose(in);
  unlink(fifo_out);
  unlink(fifo_in);
  unlink(fifo_err);
  if (name!=NULL) free(name);
  nxt_close(nxt);

  return 0;
}
