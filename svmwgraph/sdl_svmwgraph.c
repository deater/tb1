/* The SDL hooks for the Super VMW graphics library */

#include <SDL.h>
#include "svmwgraph.h"
#include <stdlib.h>  /* For atexit() */

    /* Setup the Graphics */
void *SDL_setupGraphics(int xsize,int ysize,int bpp,int verbose)
{
    SDL_Surface *sdl_screen;
   
       /* Initialize the SDL library */
    if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
       fprintf(stderr,
	       "Couldn't initialize SDL: %s\n", SDL_GetError());
       return NULL;
    }
   
       /* Clean up on exit */
    atexit(SDL_Quit);
   
       /* Initialize the display */
    sdl_screen = SDL_SetVideoMode(xsize, ysize, 16, SDL_SWSURFACE);
    if ( sdl_screen == NULL ) {
       fprintf(stderr, "Couldn't set %dx%dx%d video mode: %s\n",
	       xsize,ysize,bpp,SDL_GetError());
       exit(1);
    }
    if (verbose) {
       printf("  + SDL Graphics Initialization successful...\n");
       printf("  + Using %dx%dx%dbpp Visual...\n",xsize,ysize,bpp);
    }
    return sdl_screen;
}



void SDL_NoScale16bpp_BlitMem(vmwSVMWGraphState *target_p, vmwVisual *source) {
   
   int x,y,Bpp;
   
   unsigned char *s_pointer,*t_pointer;
   
   SDL_Surface *target;
   
   target=(SDL_Surface *)target_p->output_screen;
   
   if ( SDL_MUSTLOCK(target) ) {
      if ( SDL_LockSurface(target) < 0 )
      return;
   }
   
   Bpp=target->format->BytesPerPixel;
   
   s_pointer=source->memory;
   t_pointer=((Uint8 *)target->pixels);

//   printf("%i %i\n",source->xsize,source->ysize);
   
   for (x=0;x<source->xsize;x++)
       for (y=0;y<source->ysize;y++) {
           *((Uint16 *)(t_pointer))=source->palette[*(s_pointer)];
           s_pointer++; t_pointer+=2;
       }
   
   
     /* Update the display */
      if ( SDL_MUSTLOCK(target) ) {
	 SDL_UnlockSurface(target);
      }
   
      /* Write this out to the screen */
   SDL_UpdateRect(target, 0, 0, source->xsize, source->ysize);
   
}


void SDL_clearKeyboardBuffer() {
   SDL_Event event;
   while (SDL_PollEvent(&event)) {
      
   }
}

int SDL_getInput() {
   
   SDL_Event event;
   int keypressed;
   
   while ( SDL_PollEvent(&event)) {
      
       switch(event.type) {
	case SDL_KEYDOWN:
	     keypressed=event.key.keysym.sym;
	     switch (keypressed) {
	      case SDLK_BACKSPACE: return VMW_BACKSPACE;
	      case SDLK_ESCAPE   : return VMW_ESCAPE;
	      case SDLK_RETURN   : return VMW_ENTER;
	      case SDLK_UP       : return VMW_UP;
	      case SDLK_DOWN     : return VMW_DOWN;
	      case SDLK_RIGHT    : return VMW_RIGHT;
	      case SDLK_LEFT     : return VMW_LEFT;
	      case SDLK_F1       : return VMW_F1;
	      case SDLK_F2       : return VMW_F2;
	      case SDLK_PAGEUP   : return VMW_PGUP;
	      case SDLK_PAGEDOWN : return VMW_PGDN;
	      default:             return keypressed;
	     }
	default: break;
       }
   }
   return 0;
}
