/* I will finish this game!!! */
/* This is the SDL implementation of the Super VMW graphics library */
/* Which was originally a lot of Turbo-Pascal w inline assembly     */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include "SDL.h"
#include "tb_keypress.h"


void vmwBlitMemToSDL(SDL_Surface *target, unsigned char *source) {
   
   int x,y,Bpp;
   
   unsigned char *s_pointer,*t_pointer;
   

   if ( SDL_MUSTLOCK(target) ) {
      if ( SDL_LockSurface(target) < 0 )
      return;
   }
   
   Bpp= target->format->BytesPerPixel;
   
   s_pointer=source;
   t_pointer=((Uint8 *)target->pixels);
   
   for (x=0;x<320;x++)
       for (y=0;y<200;y++) {
           *((Uint16 *)(t_pointer))=*((Uint16 *)(s_pointer));
           s_pointer+=2; t_pointer+=2;
       }
   
   
     /* Update the display */
      if ( SDL_MUSTLOCK(target) ) {
	 SDL_UnlockSurface(target);
      }
   
      /* Write this out to the screen */
   SDL_UpdateRect(target, 0, 0, 320, 200);
   
}


void vmwClearKeyboardBuffer() {
   SDL_Event event;
   while (SDL_PollEvent(&event)) {
      
   }
}

int vmwGetInput() {
   
   SDL_Event event;
   int keypressed;
   
   while ( SDL_PollEvent(&event)) {
      
       switch(event.type) {
	case SDL_KEYDOWN:
	     keypressed=event.key.keysym.sym;
	     switch (keypressed) {
	      case SDLK_BACKSPACE: return TB_BACKSPACE;
	      case SDLK_ESCAPE   : return TB_ESCAPE;
	      case SDLK_RETURN   : return TB_ENTER;
	      case SDLK_UP       : return TB_UP;
	      case SDLK_DOWN     : return TB_DOWN;
	      case SDLK_RIGHT    : return TB_RIGHT;
	      case SDLK_LEFT     : return TB_LEFT;
	      case SDLK_F1       : return TB_F1;
	      case SDLK_F2       : return TB_F2;
	      case SDLK_PAGEUP   : return TB_PGUP;
	      case SDLK_PAGEDOWN : return TB_PGDN;
	      default:             return keypressed;
	     }
	default: break;
       }
   }
   return 0;
}
