/*
    tools/mount.c
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

#include <anxt/nxt.h>
#include <anxt/file.h>
#include <anxt/tools.h>

/// Data needed to add a file to a directory
struct nxtfs_dir_file {
  /// Fuse filler function
  fuse_fill_dir_t filler;
  /// Directory buffer
  void *buf;
};

/// Opened file
typedef struct {
  char *filename;
  char *buf;
  size_t size;
  int dirty;
} nxt_file_t;

/// Filesystem options
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
static int nxtfs_open(const char *fn,struct fuse_file_info *fi) {
  char *filename = ((char*)fn)+1;
  int fh;
  size_t filesize;
  if ((fh = nxt_file_open(nxt,filename,NXT_OREAD,&filesize))!=-1) {
    nxt_file_t *new = malloc(sizeof(nxt_file_t));
    fi->fh = (intptr_t)new;
    new->filename = strdup(filename);
    new->buf = malloc(filesize);
    new->size = filesize;
    new->dirty = 0;
    nxt_file_read(nxt,fh,new->buf,filesize);
    nxt_file_close(nxt,fh);
    return 0;
  }
  else return -errno;
}

/**
 * Flushs buffered data to NXT
 *  @param filename Filename
 *  @param fi File info
 *  @return Success?
 */
static int nxtfs_flush(const char *filename,struct fuse_file_info *fi) {
  nxt_file_t *file = (nxt_file_t*)((intptr_t)(fi->fh));
  if (file->dirty==0) return 0;
  int fh;
  if ((fh = nxt_file_open(nxt,file->filename,NXT_OWLINE|NXT_OWOVER,file->size))!=-1) {
    nxt_file_write(nxt,fh,file->buf,file->size);
    nxt_file_close(nxt,fh);
    file->dirty = 0;
    return 0;
  }
  else return -1;
}

/**
 * Closes a file (inclusive flushing)
 *  @param filename Filename
 *  @param fi File info
 *  @return Success?
 */
static int nxtfs_close(const char *filename,struct fuse_file_info *fi) {
  if (nxtfs_flush(filename,fi)!=-1) {
    nxt_file_t *file = (nxt_file_t*)((intptr_t)(fi->fh));
    free(file->filename);
    free(file->buf);
    free(file);
    return 0;
  }
  else return -1;
}

/**
 * Truncates file
 *  @param filename Filename
 *  @param newsize New filesize
 *  @param fi File info
 *  @return Success?
 */
static int nxtfs_ftruncate(const char *filename,off_t newsize,struct fuse_file_info *fi) {
  nxt_file_t *file = (nxt_file_t*)((intptr_t)(fi->fh));
  file->buf = realloc(file->buf,newsize);
  //if (newsize>file->size) memset(file->buf+file->size,0,newsize-file->size);
  file->size = newsize;
  file->dirty = 1;
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
 */
static int nxtfs_read(const char *filename,char *buf,size_t count,off_t offset,struct fuse_file_info *fi) {
  nxt_file_t *file = (nxt_file_t*)((intptr_t)(fi->fh));
  if (offset>file->size) return -EINVAL;
  if (offset+count>file->size) count = file->size-offset;
  memcpy(buf,file->buf+offset,count);
  return count;
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
  nxt_file_t *file = (nxt_file_t*)((intptr_t)(fi->fh));
  if (offset>file->size) return -EINVAL;
  if (offset+count>file->size) nxtfs_ftruncate(filename,offset+count,fi);
  memcpy(file->buf+offset,buf,count);
  file->dirty = 1;
  return count;
}

/**
 * Adds file to directory
 *  @param filename Filename
 *  @param filesize Filesize
 *  @param vdata Void pointer to dir_file data
 */
static void nxtfs_dir_file_add(char *filename,size_t filesize,void *vdata) {
  struct nxtfs_dir_file *data = (struct nxtfs_dir_file*)vdata;
  struct stat stbuf;

  memset(&stbuf,0,sizeof(stbuf));
  stbuf.st_mode = S_IFREG|0777;
  stbuf.st_size = filesize;
  stbuf.st_nlink = 1;
  data->filler(data->buf,filename,&stbuf,0);
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
    struct nxtfs_dir_file data = {
      .filler = filler,
      .buf = buf
    };

    filler(buf,".",NULL,0);
    filler(buf,"..",NULL,0);

    return nxt_list(nxt,"*.*",nxtfs_dir_file_add,&data)==0?0:nxtfs_error();
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
  if (strcmp(filename,"/")==0) {
    stbuf->st_mode = S_IFDIR|0777;
    stbuf->st_nlink = 1;
    return 0;
  }
  else {
    int fh;
    if ((fh = nxt_file_open(nxt,((char*)filename)+1,NXT_OREAD,&(stbuf->st_size)))!=-1) {
      nxt_file_close(nxt,fh);
      stbuf->st_mode = S_IFREG|0777;
      stbuf->st_nlink = 1;
      return 0;
    }
    else return nxtfs_error();
  }
}

/**
 * Makes a new node (only regular files)
 *  @param filename Filename
 *  @param mode Mode
 *  @param dev Device (ignored)
 *  @return Success?
 */
static int nxtfs_mknod(const char *filename,mode_t mode,dev_t dev) {
  if (mode&S_IFREG) {
    int fh;
    if ((fh = nxt_file_open(nxt,((char*)filename)+1,NXT_OWLINE,1))!=-1) {
      nxt_file_write(nxt,fh,"\0",1);
      nxt_file_close(nxt,fh);
      return 0;
    }
    else return nxtfs_error();
  }
  return -EINVAL;
}

/**
 * Creates a file
 *  @param filename Filename
 *  @param mode Mode
 *  @param fi File info
 *  @return Success?
 */
static int nxtfs_create(const char *filename,mode_t mode,struct fuse_file_info *fi) {
  int err;
  if ((err = nxtfs_mknod(filename,mode|S_IFREG,0))!=-1) {
    fi->flags |= O_WRONLY;
    return nxtfs_open(filename,fi);
  }
  else return err;
}

/**
 * Removes a file
 *  @param filename Filename
 *  @return Success?
 */
static int nxtfs_unlink(const char *filename) {
  if (nxt_file_remove(nxt,((char*)filename)+1)==0) return 0;
  else return nxtfs_error();
}

/**
 * Renames a file
 *  @param _old Old filename
 *  @param _new New filename
 *  @return Success?
 */
static int nxtfs_rename(const char *_old,const char *_new) {
  char *old = ((char*)_old)+1;
  char *new = ((char*)_new)+1;
  size_t filesize;
  int fh_old,fh_new,ret;

  if ((fh_old = nxt_file_open(nxt,old,NXT_OREAD,&filesize))!=-1) {
    void *buf = malloc(filesize);
    nxt_file_read(nxt,fh_old,buf,filesize);
    if ((fh_new = nxt_file_open(nxt,new,NXT_OWLINE,filesize))!=-1) {
      nxt_file_write(nxt,fh_new,buf,filesize);
      nxt_file_close(nxt,fh_new);
      ret = 0;
    }
    else ret = nxtfs_error();
    free(buf);
    nxt_file_close(nxt,fh_old);
    if (ret==0) nxt_file_remove(nxt,old);
  }
  else ret = nxtfs_error();
  return ret;
}

/**
 * Truncates a file
 *  @param fn Filename
 *  @param newsize New size
 *  @return Success?
 */
static int nxtfs_truncate(const char *fn,off_t newsize) {
  char *filename = ((char*)fn)+1;
  size_t filesize;
  int fh,ret;

  if (newsize==0) newsize = 1;
  if ((fh = nxt_file_open(nxt,filename,NXT_OREAD,&filesize))!=-1) {
    void *buf = malloc(newsize);
    nxt_file_read(nxt,fh,buf,filesize<newsize?filesize:newsize);
    //if (newsize>filesize) memset(buf+filesize,0,newsize-filesize);
    nxt_file_close(nxt,fh);
    if ((fh = nxt_file_open(nxt,filename,NXT_OWLINE|NXT_OWOVER,newsize))!=-1) {
      nxt_file_write(nxt,fh,buf,newsize);
      nxt_file_close(nxt,fh);
      ret = 0;
    }
    else ret = nxtfs_error();
    free(buf);
  }
  else ret = nxtfs_error();
  return ret;
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
  .flush     = nxtfs_flush,
  .release   = nxtfs_close,
  .ftruncate = nxtfs_ftruncate,
  .read      = nxtfs_read,
  .write     = nxtfs_write,
  .readdir   = nxtfs_readdir,
  .getattr   = nxtfs_getattr,
  .mknod     = nxtfs_mknod,
  .create    = nxtfs_create,
  .unlink    = nxtfs_unlink,
  .rename    = nxtfs_rename,
  .truncate  = nxtfs_truncate,
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
