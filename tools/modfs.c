/*
    modfs.c
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
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <errno.h>
#include <fcntl.h>

#ifdef FUSE_VERSION_2_5
# define FUSE_USE_VERSION 25
#else
# define FUSE_USE_VERSION 27
#endif
#include <fuse.h>
#include <fuse_opt.h>

#include <nxt.h>

typedef struct {
  char *name;
  int modid;
  size_t mod_size;
  size_t iomap_size;
} module_t;

struct options {
  char* name;
} options;

/** macro to define options */
#define NXTFS_OPT_KEY(t, p, v) { t, offsetof(struct options, p), v }

/** keys for FUSE_OPT_ options */
enum {
  KEY_VERSION,
  KEY_HELP,
};

static struct fuse_opt nxtfs_opts[] = {
  NXTFS_OPT_KEY("-n %s",name,0),
  // #define FUSE_OPT_KEY(templ, key) { templ, -1U, key }
  FUSE_OPT_KEY("-V",KEY_VERSION),
  FUSE_OPT_KEY("--version",KEY_VERSION),
  FUSE_OPT_KEY("-h",KEY_HELP),
  FUSE_OPT_KEY("--help",KEY_HELP),
  FUSE_OPT_END
};

static nxt_t *nxt;
static size_t num_modules = 0;
static module_t modules[256];

#include <stdarg.h>
#include <stdio.h>
static void debug(const char *fmt,...) {
  //char *path;
  va_list args;

  va_start(args,fmt);
  //asprintf(&path,"/proc/%d/fd/0",fuse_get_context()->pid);
  FILE *fd = fopen("/home/janosch/Desktop/libnxt/modfs.log","a");
  if (fd!=NULL) {
    vfprintf(fd,fmt,args);
    fclose(fd);
  }
  //free(path);
  va_end(args);
}

// NXT operations
static int get_modules() {
  int handle,modid,last;
  handle = nxt_mod_first(nxt,"*.mod",&(modules[num_modules].name),&(modules[num_modules].modid),&(modules[num_modules].mod_size),&(modules[num_modules].iomap_size));
  if (handle!=NXT_FAIL) {
    last = handle;
    num_modules++;
    while ((handle = nxt_mod_next(nxt,last,&(modules[num_modules].name),&(modules[num_modules].modid),&(modules[num_modules].mod_size),&(modules[num_modules].iomap_size)))!=NXT_FAIL) {
      nxt_mod_close(nxt,last);
      last = handle;
      num_modules++;
    }
  }

  /*int modid,valid_modid;

  if ((modid = nxt_mod_first(nxt,"*.mod",&(modules[num_modules].name)
                                     ,&(modules[num_modules].modid)
                                     ,&(modules[num_modules].mod_size)
                                     ,&(modules[num_modules].iomap_size)))!=NXT_FAIL) {
    do {
      valid_modid = modid;
    }
    while ((modid = nxt_mod_next(nxt,modid,&(modules[num_modules].name)
                                    ,&(modules[num_modules].modid)
                                    ,&(modules[num_modules].mod_size)
                                    ,&(modules[num_modules].iomap_size)))!=NXT_FAIL);
    nxt_file_close(nxt,valid_modid);
  }*/

  if (nxt_error(nxt)==NXT_ERR_MODULE_NOT_FOUND) {
    nxt_reset_error(nxt);
    //nxt_mod_close(nxt,last);
    return 0;
  }
  else return -1;
}

// Filesystem operations ////

static int nxtfs_error() {
  int err = nxt_error(nxt);
  if (err==NXT_ERR_NO_MORE_HANDLES) return -ENFILE;
  else if (err==NXT_ERR_NO_SPACE) return -ENOSPC;
  else if (err==NXT_ERR_NO_MORE_FILES) return -EMFILE;
  else if (err==NXT_ERR_FILE_NOT_FOUND) return -ENOENT;
  else if (err==NXT_ERR_HANDLE_ALREADY_CLOSED) return -EBADF;
  else if (err==NXT_ERR_FILE_IS_BUSY) return -EBUSY;
  else if (err==NXT_ERR_FILE_IS_FULL) return -ENOMEM;
  else if (err==NXT_ERR_FILE_EXISTS) return -EEXIST;
  else if (err==NXT_ERR_MODULE_NOT_FOUND) return -ENOENT;
  else if (err==NXT_ERR_OUT_OF_BOUNDARY) return -ERANGE;
  else if (err==NXT_ERR_ILLEGAL_FILE_NAME) return -EINVAL;
  else if (err==NXT_ERR_ILLEGAL_HANDLE) return -EBADF;
  else if (err==NXT_ERR_OUT_OF_RANGE) return -ERANGE;
  else if (err==NXT_ERR_NO_FREE_MEMORY_IN_BUFFER) return -ENOMEM;
  else if (err==NXT_ERR_ILLEGAL_SIZE) return -EINVAL;
  else if (err==NXT_ERR_ILLEGAL_MAILBOX_QUEUE_ID) return -EINVAL;
  else if (err==NXT_ERR_BAD_INPUT_OUTPUT) return -EIO;
  else if (err==NXT_ERR_BAD_ARGUMENTS) return -EINVAL;
  else return 0;
}

/**
 * Opens a file on NXT
 *  @param filename Filename
 *  @param fi File info
 *  @return Success?
 */
static int nxtfs_open(const char *filename,struct fuse_file_info *fi) {
  debug("nxtfs_open(%s,0x%x)\n",filename,fi);
  size_t i;

  for (i=0;i<num_modules;i++) {
    if (strcmp(filename+1,modules[i].name)==0) {
      fi->fh = i;
      return 0;
    }
  }

  return -ENOENT;
}

/**
 * Closes a file
 *  @param filename Filename
 *  @param fi File info
 *  @return Success?
 */
static int nxtfs_close(const char *filename,struct fuse_file_info *fi) {
  return 0;
}

/**
 * Reads from file
 *  @param filename Filename
 *  @param buf Buffer
 *  @param count How many bytes to read
 *  @param offset Offset in file
 *  @param fi File info
 *  @return How many bytes read
 *  @todo something does not work here
 */
static int nxtfs_read(const char *filename,char *buf,size_t count,off_t offset,struct fuse_file_info *fi) {
  debug("nxtfs_read(%s,0x%x,%d,%d,0x%x)\n",filename,buf,count,(int)offset,fi);
  if (count>modules[fi->fh].iomap_size) count = modules[fi->fh].iomap_size;
  ssize_t size = nxt_mod_read(nxt,modules[fi->fh].modid,buf,offset,count);
  if (size<0) return nxtfs_error();
  else return size;
}

/**
 * Writes to file
 *  @param filename Filename
 *  @param buf Buffer
 *  @param count How many bytes to write
 *  @param offset Offset in file
 *  @param fi File info
 *  @return How many bytes written
 */
static int nxtfs_write(const char *filename,const char *buf,size_t count,off_t offset,struct fuse_file_info *fi) {
  debug("nxtfs_write(%s,0x%x,%d,%d,0x%x)\n",filename,buf,count,offset,fi);
  if (count>modules[fi->fh].iomap_size) count = modules[fi->fh].iomap_size;
  ssize_t size = nxt_mod_write(nxt,modules[fi->fh].modid,buf,offset,count);
  if (size<0) return nxtfs_error(size);
  else return size;
}

/**
 * Reads directory
 *  @param path Path (should always be /)
 *  @param buf Buffer
 *  @param filler Filler function
 *  @param offset Offset (ignored)
 *  @param fi File info
 *  @return Success?
 */
static int nxtfs_readdir(const char *path,void *buf,fuse_fill_dir_t filler,off_t offset,struct fuse_file_info *fi) {
  if (strcmp(path,"/")==0) {
    size_t i;
    struct stat stbuf;

    memset(&stbuf,0,sizeof(stbuf));
    stbuf.st_mode = S_IFREG|0777;

    for (i=0;i<num_modules;i++) {
      stbuf.st_size = modules[i].iomap_size;
      stbuf.st_nlink = 1;
      filler(buf,modules[i].name,&stbuf,0);
    }
    return 0;
  }
  else return -ENOENT;
}

/**
 * Gets attributes of a file
 *  @param filename Filename
 *  @param stbuf Stat buffer
 *  @return Success?
 */
static int nxtfs_getattr(const char *filename,struct stat *stbuf) {
  size_t i;
  memset(stbuf,0,sizeof(struct stat));
  if (strcmp(filename,"/")==0) {
    stbuf->st_mode = S_IFDIR|0777;
    stbuf->st_size = 0;
    stbuf->st_nlink = 2;
    return 0;
  }
  else {
    for (i=0;i<num_modules;i++) {
      if (strcmp(filename+1,modules[i].name)==0) {
        stbuf->st_mode = S_IFREG|0777;
        stbuf->st_size = modules[i].iomap_size;
        stbuf->st_nlink = 1;
        return 0;
      }
    }
    return -ENOENT;
  }
}

static int nxtfs_chown(const char *filename,uid_t uid,gid_t gid) {
  return 0;
}

static int nxtfs_chmod(const char *filename,mode_t mode) {
  return 0;
}

// Run filesystem ////

static struct fuse_operations nxtfs_oper = {
  .open      = nxtfs_open,
  .release   = nxtfs_close,
  .read      = nxtfs_read,
  .write     = nxtfs_write,
  .readdir   = nxtfs_readdir,
  .getattr   = nxtfs_getattr,
  // dummy functions
  .chown     = nxtfs_chown,
  .chmod     = nxtfs_chmod,
};

int main(int argc,char *argv[]) {
  int ret = 0;
  struct fuse_args args = FUSE_ARGS_INIT(argc,argv);
  memset(&options,0,sizeof(struct options));
  if (fuse_opt_parse(&args,&options,nxtfs_opts,NULL)==-1) return 1;

  nxt = nxt_open(options.name);
  if (nxt!=NULL) {
    get_modules();
#ifdef FUSE_VERSION_2_5
    ret = fuse_main(args.argc,args.argv,&nxtfs_oper);
#else
    ret = fuse_main(args.argc,args.argv,&nxtfs_oper,NULL);
#endif
    nxt_close(nxt);
  }
  else ret = 1;

  fuse_opt_free_args(&args);
  return ret;
}
