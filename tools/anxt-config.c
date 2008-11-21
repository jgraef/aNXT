#include <stdio.h>

void usage(char *prog) {
  printf("Usage: %s [--version] [--cflags] [--libs] [--static-libs]\n",prog);
}

int main(int argc,char *argv[]) {
  unsigned int i;

  if (argc>1) {
    for (i=1;i<argc;i++) {
      if (strcmp(argv[i],"--version")==0) puts(VERSION);
      else if (strcmp(argv[i],"--cflags")==0) puts(CFLAGS);
      else if (strcmp(argv[i],"--libs")==0) puts(LIBS);
      else if (strcmp(argv[i],"--static-libs")==0) puts(STATIC_LIBS);
      else usage(argv[0]);
    }
  }
  else usage(argv[0]);
  return 0;
}
