/* The SDL hooks for the Super VMW graphics library */

#include <SDL.h>
#include "svmwgraph.h"
#include <stdlib.h>  /* For atexit() */

    /* Setup the Graphics */
    /* Pass '0' to auto-detect bpp */
void *SDL_setupGraphics(int *xsize,int *ysize,int *bpp,
			int fullscreen,int verbose)
{
    SDL_Surface *sdl_screen=NULL;
    int mode;
   
    mode=SDL_SWSURFACE|SDL_HWPALETTE|SDL_HWSURFACE;
    if (fullscreen) mode|=SDL_FULLSCREEN;
      
       /* Initialize the SDL library */
    if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
       fprintf(stderr,
	       "Couldn't initialize SDL: %s\n", SDL_GetError());
       return NULL;
    }
   
       /* Clean up on exit */
    atexit(SDL_Quit);
   
    if (*bpp!=0) {
   
          /* Initialize the display */
       sdl_screen = SDL_SetVideoMode(*xsize, *ysize, *bpp, mode);
    }
    else {
       sdl_screen=SDL_SetVideoMode(*xsize,*ysize,0,mode);
    }
    
    
    if ( sdl_screen == NULL ) {
       fprintf(stderr, "ERROR!  Couldn't set %dx%d video mode: %s\n",
	       *xsize,*ysize,SDL_GetError());
       return NULL;
    }
   
    if (*bpp==0) *bpp=sdl_screen->format->BytesPerPixel*8;
    if (verbose) {
       printf("  + SDL Graphics Initialization successful...\n");
       printf("  + Using %dx%dx%dbpp Visual...\n",*xsize,*ysize,*bpp);
    }
    return sdl_screen;
}


void SDL_flushPalette(vmwSVMWGraphState *state) {
 
   SDL_Surface *target;
   SDL_Color temp_col[256];
   int i;
   
   if (state->bpp==8) {
      for(i=0;i<256;i++) {
         temp_col[i].r=(state->palette[i]>>11)<<3;
         temp_col[i].g=((state->palette[i]>>5)&0x3f)<<2;
         temp_col[i].b=(state->palette[i]&0x1f)<<3;
      }
   
      target=(SDL_Surface *)state->output_screen;
   
      SDL_SetColors(target,temp_col,0,256);
   }
}


void SDL_NoScale16bpp_BlitMem(vmwSVMWGraphState *target_p, vmwVisual *source) {
   
   int x,y;
   
   unsigned char *s_pointer,*t_pointer;
   
   SDL_Surface *target;
   
   target=(SDL_Surface *)target_p->output_screen;
   
   if ( SDL_MUSTLOCK(target) ) {
      if ( SDL_LockSurface(target) < 0 )
      return;
   }
   
   s_pointer=source->memory;
   t_pointer=((Uint8 *)target->pixels);

//   printf("%i %i\n",source->xsize,source->ysize);
   
   for (x=0;x<source->xsize;x++)
       for (y=0;y<source->ysize;y++) {
           *((Uint16 *)(t_pointer))=target_p->palette[*(s_pointer)];
           s_pointer++; t_pointer+=2;
       }
   
   
     /* Update the display */
      if ( SDL_MUSTLOCK(target) ) {
	 SDL_UnlockSurface(target);
      }
   
      /* Write this out to the screen */
   SDL_UpdateRect(target, 0, 0, source->xsize, source->ysize);
   
}

   /* I should make this generic, but it makes it really slow */
void SDL_Double16bpp_BlitMem(vmwSVMWGraphState *target_p, vmwVisual *source) {
   
   int x,y,scale;
   
   unsigned char *s_pointer,*t_pointer;
   
   SDL_Surface *target;
   
   target=(SDL_Surface *)target_p->output_screen;
   
   scale=target_p->scale;
   
   if ( SDL_MUSTLOCK(target) ) {
      if ( SDL_LockSurface(target) < 0 )
      return;
   }
   
   s_pointer=source->memory;
   t_pointer=((Uint8 *)target->pixels);

   for (y=0;y<source->ysize;y++) {
       for (x=0;x<source->xsize;x++) {
	   
	   /* i=0, j=0 */
	   *((Uint16 *) ( (t_pointer)))=
	                                  target_p->palette[*(s_pointer)];
	  
	   /* i=1, j=0 */
	   *((Uint16 *) ( (t_pointer+(2*target_p->xsize)  )))=
	                                  target_p->palette[*(s_pointer)];
	   /* i=0, j=1 */
	   *((Uint16 *) ( (t_pointer+2) ))=
	                                  target_p->palette[*(s_pointer)];
	  
           /* i=1 j=1 */
	    *((Uint16 *) ( (t_pointer+2+(2*target_p->xsize)  )))=
	                                  target_p->palette[*(s_pointer)];
	  
	  
           s_pointer++; t_pointer+=4;
       }
       t_pointer+=2*target_p->xsize;
   }
   
   
     /* Update the display */
      if ( SDL_MUSTLOCK(target) ) {
	 SDL_UnlockSurface(target);
      }
   
      /* Write this out to the screen */
   SDL_UpdateRect(target, 0, 0, target_p->xsize, target_p->ysize);
   
}

void SDL_NoScale8bpp_BlitMem(vmwSVMWGraphState *target_p, vmwVisual *source) {
   
   int x,y;
   
   unsigned char *s_pointer,*t_pointer;
   
   SDL_Surface *target;
   
   target=(SDL_Surface *)target_p->output_screen;
   
   if ( SDL_MUSTLOCK(target) ) {
      if ( SDL_LockSurface(target) < 0 )
      return;
   }
   
   s_pointer=source->memory;
   t_pointer=((Uint8 *)target->pixels);
   
   for (x=0;x<source->xsize;x++)
       for (y=0;y<source->ysize;y++) {
           *((Uint8 *)(t_pointer))=*(s_pointer);
           s_pointer++; t_pointer++;
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
	      default: if ((keypressed>='a') && (keypressed<='z')  
		          && (event.key.keysym.mod & KMOD_SHIFT) )
		          return keypressed-32;
		       else return keypressed;
	     }
	default: break;
       }
   }
   return 0;
}

void SDL_closeGraphics() {
}
