#include <sys/types.h>
#include <stdint.h>
#include <unistd.h>
#include <nxt.h>
#include <SDL.h>
#include <SDL_image.h>

// Images
#include "pilot_bg.xpm"
#include "pilot_enter.xpm"
#include "pilot_exit.xpm"
#include "pilot_left.xpm"
#include "pilot_right.xpm"

static void usage(char *cmd,int r) {
  FILE *out = r==0?stdout:stderr;
  fprintf(out,"Usage: %s [OPTIONS]\n",cmd);
  fprintf(out,"Control the NXT UI without touching it\n");
  fprintf(out,"Options:\n");
  fprintf(out,"\t-h           Show help\n");
  fprintf(out,"\t-n NXTNAME   Name of NXT (Default: first found) or bluetooth address\n");
  fprintf(out,"\t-f           Force. Don't ask anything\n");
  fprintf(out,"\t-d DURATION  Duration (Default: 200ms)\n");
  exit(r);
}

static SDL_Surface *load_display(nxt_t *nxt) {
  static char screen[64][100];
  uint32_t image[70][100];
  size_t x,y;

  for (y=0;y<64;y++) {
    for (x=0;x<100;x++) image[y][x] = screen[y][x]?0x000000FF:0xFFFFFFFF;
  }

  if (nxt_screenshot(nxt,screen)==0) {
    return SDL_CreateRGBSurfaceFrom(image,100,64,32,400,0xFF000000,0x00FF0000,0x0000FF00,0x000000FF);
  }
  else return NULL;
}

#define in_rect(rect,mouse) ((mouse).x>=(rect).x && (mouse).x<(rect).x+(rect).w && (mouse).y>=(rect).y && (mouse).y<(rect).y+(rect).h)

int main(int argc,char *argv[]) {
  SDL_Event event;
  SDL_Surface *screen;
  SDL_Surface *bg,*left,*enter,*right,*exit,*display;
  SDL_Rect rect_left = {
    .x = 51,
    .y = 172,
    .w = 27,
    .h = 31,
  };
  SDL_Rect rect_enter = {
    .x = 91,
    .y = 173,
    .w = 32,
    .h = 31,
  };
  SDL_Rect rect_right = {
    .x = 136,
    .y = 172,
    .w = 27,
    .h = 31,
  };
  SDL_Rect rect_exit = {
    .x = 91,
    .y = 216,
    .w = 31,
    .h = 20,
  };
  SDL_Rect rect_display = {
    .x = 57,
    .y = 62,
    .w = 100,
    .h = 64,
  };
  int c;
  char *name = NULL;
  int force = 0;

  while ((c = getopt(argc,argv,":hn:f"))!=-1) {
    switch (c) {
      case 'h':
        usage(argv[0],0);
        break;
      case 'f':
        force = 1;
        break;
      case 'n':
        name = optarg;
        break;
      case ':':
        fprintf(stderr,"Option -%c requires an operand\n",optopt);
        usage(argv[0],1);
        break;
      case '?':
        fprintf(stderr,"Unrecognized option: -%c\n", optopt);
        usage(argv[0],1);
        break;
    }
  }

  // init NXT
  nxt_t *nxt = nxt_open(name);
  if (nxt==NULL) {
    fprintf(stderr,"Could not find NXT\n");
    return 1;
  }
  if (nxt_getcontype(nxt)==NXT_CON_BT && !force) {
    fprintf(stderr,"Warning! Using NXT Pilot over Bluetooth can make trouble. Are you sure to continue (y/n)[n]: ");
    if (fgetc(stdin)!='y') {
      nxt_close(nxt);
      return 0;
    }
  }

  // init window
  if (SDL_Init(SDL_INIT_VIDEO)==-1) {
    fprintf(stderr,"Can't init SDL:  %s\n",SDL_GetError());
    return 1;
  }
  atexit(SDL_Quit);
  screen = SDL_SetVideoMode(215,322,32,SDL_HWSURFACE);
  if (screen==NULL) {
    fprintf(stderr,"Can't open window: %s\n",SDL_GetError());
    return 1;
  }
  SDL_WM_SetCaption("NXT Pilot","NXT Pilot");

  // load images
  bg = IMG_ReadXPMFromArray(pilot_bg_xpm);
  left = IMG_ReadXPMFromArray(pilot_left_xpm);
  enter = IMG_ReadXPMFromArray(pilot_enter_xpm);
  right = IMG_ReadXPMFromArray(pilot_right_xpm);
  exit = IMG_ReadXPMFromArray(pilot_exit_xpm);

  // display NXT
  SDL_BlitSurface(bg,NULL,screen,NULL);
  SDL_BlitSurface(left,NULL,screen,&rect_left);
  SDL_BlitSurface(enter,NULL,screen,&rect_enter);
  SDL_BlitSurface(right,NULL,screen,&rect_right);
  SDL_BlitSurface(exit,NULL,screen,&rect_exit);

  int done = 0;
  unsigned int pause = nxt_getcontype(nxt)==NXT_CON_BT?500:100;
  while (!done) {
    while (SDL_PollEvent(&event)) {
      switch(event.type) {
        case SDL_QUIT:
          done = 1;
          break;
        case SDL_KEYUP:
          if (event.key.keysym.sym==SDLK_ESCAPE) done = 1;
          else if (event.key.keysym.sym==SDLK_RETURN) nxt_setbutton(nxt,NXT_UI_BUTTON_ENTER);
          else if (event.key.keysym.sym==SDLK_BACKSPACE) nxt_setbutton(nxt,NXT_UI_BUTTON_EXIT);
          else if (event.key.keysym.sym==SDLK_LEFT) nxt_setbutton(nxt,NXT_UI_BUTTON_LEFT);
          else if (event.key.keysym.sym==SDLK_RIGHT) nxt_setbutton(nxt,NXT_UI_BUTTON_RIGHT);
          break;
        case SDL_MOUSEBUTTONUP:
          if (in_rect(rect_left,event.button)) nxt_setbutton(nxt,NXT_UI_BUTTON_LEFT);
          else if (in_rect(rect_enter,event.button)) nxt_setbutton(nxt,NXT_UI_BUTTON_ENTER);
          else if (in_rect(rect_right,event.button)) nxt_setbutton(nxt,NXT_UI_BUTTON_RIGHT);
          else if (in_rect(rect_exit,event.button)) nxt_setbutton(nxt,NXT_UI_BUTTON_EXIT);
          break;
      }
    }

    display = load_display(nxt);
    if (display!=NULL) {
      SDL_BlitSurface(display,NULL,screen,&rect_display);
      SDL_UpdateRect(screen,0,0,0,0);
      SDL_FreeSurface(display);
    }

    SDL_Delay(pause);
  }

  // free images
  SDL_FreeSurface(bg);
  SDL_FreeSurface(left);
  SDL_FreeSurface(enter);
  SDL_FreeSurface(right);
  SDL_FreeSurface(exit);

  // close nxt
  nxt_close(nxt);
}

