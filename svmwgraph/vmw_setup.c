/* Routines for setting up structures for the Super VMW graphics library */

#include "svmwgraph.h"
#include "sdl_svmwgraph.h"  /* Make this dynamic somehow? */
#include "null_svmwgraph.h"
#include "curses_svmwgraph.h"
#include "opengl_svmwgraph.h"

#include <stdio.h>  /* For printf */
#include <stdlib.h> /* For Memory Allocation */

void *(*vmwSetupGraphics)(int *xsize,int *ysize, int *bpp, 
			  int fullscreen,int verbose);
void (*vmwBlitMemToDisplay)(vmwSVMWGraphState *display, vmwVisual *source);
void (*vmwFlushPalette)(vmwSVMWGraphState *state);

void (*vmwClearKeyboardBuffer)(void);
int (*vmwGetInput)(void);
void (*vmwCloseGraphics)(void);

vmwSVMWGraphState *vmwSetupSVMWGraph(int display_type,int xsize,int ysize,
				     int bpp,int scale,int fullscreen,
				     int verbose) {
   
    vmwSVMWGraphState *temp_state;
   
    if  ( (temp_state=calloc(1,sizeof(vmwSVMWGraphState)) )==NULL) {
       printf("Error allocating memory!\n");
       return NULL;
    }
       /* Setup Setup routines */
    switch (display_type) {
        case VMW_NULLTARGET:   
                               vmwSetupGraphics=null_setupGraphics;
                               break;
        case VMW_CURSESTARGET: 
#ifdef CURSES_TARGET
                               vmwSetupGraphics=curses_setupGraphics;
#endif
                               break;
        case VMW_OPENGLTARGET: 
#ifdef OPENGL_TARGET
                               vmwSetupGraphics=openGL_setupGraphics;
#endif
                               break;
        case VMW_SDLTARGET:    
#ifdef SDL_TARGET
                               vmwSetupGraphics=SDL_setupGraphics;
#endif
                               break;
        default: printf("ERROR! Unknown Display Target %i.\n",display_type);
                 return NULL;
    }
   
    temp_state->bpp=bpp;
    temp_state->xsize=xsize;
    temp_state->ysize=ysize;
    temp_state->scale=scale;
    temp_state->default_font=NULL;
    temp_state->palette_size=256;
   
    if ( (temp_state->palette=calloc(temp_state->palette_size,sizeof(int)) )==NULL) {
       printf("Error allocating palette of size %i!\n",temp_state->palette_size);
       return NULL;
    }

    if ( (temp_state->actual_pal=calloc(temp_state->palette_size,sizeof(vmw24BitPal)) )==NULL) {
       printf("Error allocating actual palette of size %i!\n",temp_state->palette_size);
       return NULL;
    }


       /* Attempt to get desired graphics state */
    temp_state->output_screen=vmwSetupGraphics(&temp_state->xsize,
					       &temp_state->ysize,
					       &temp_state->bpp,
					       fullscreen,verbose);
   if (temp_state->output_screen==NULL) return NULL;
       /* Setup proper blitter and others*/   
    switch (display_type) {
       case VMW_NULLTARGET: 
               vmwFlushPalette=null_flushPalette;
               vmwClearKeyboardBuffer=null_clearKeyboardBuffer;
               vmwBlitMemToDisplay=null_BlitMem;
               vmwGetInput=null_getInput;
               vmwCloseGraphics=null_closeGraphics;
               break;
       case VMW_OPENGLTARGET:
#ifdef OPENGL_TARGET
               vmwFlushPalette=openGL_flushPalette;
               vmwClearKeyboardBuffer=SDL_clearKeyboardBuffer;
               vmwBlitMemToDisplay=openGL_BlitMem;
               vmwGetInput=SDL_getInput;
               vmwCloseGraphics=openGL_closeGraphics;
#endif
               break;
       case VMW_CURSESTARGET:
#ifdef CURSES_TARGET
               vmwFlushPalette=curses_flushPalette;
               vmwClearKeyboardBuffer=curses_clearKeyboardBuffer;
               vmwBlitMemToDisplay=curses_BlitMem;
               vmwGetInput=curses_getInput;
               vmwCloseGraphics=curses_closeGraphics;
#endif
               break;
       case VMW_SDLTARGET: 
#ifdef SDL_TARGET
               if (temp_state->bpp==8) {
		  if (scale==1) {
		     vmwBlitMemToDisplay=SDL_NoScale8bpp_BlitMem;
		  } else {
		     vmwBlitMemToDisplay=SDL_Double8bpp_BlitMem;
		  }
	       }
               if (temp_state->bpp==16) {
		  if (scale==1) {
                     vmwBlitMemToDisplay=SDL_NoScale16bpp_BlitMem;
                  } else { 
	            vmwBlitMemToDisplay=SDL_Double16bpp_BlitMem;
		  }
	       }
               if (temp_state->bpp==24) {
		  printf("ERROR! 24bpp not supported!\n");
		  if (scale==1) {
		     vmwBlitMemToDisplay=SDL_NoScale16bpp_BlitMem;
		  } else { 
		     vmwBlitMemToDisplay=SDL_Double16bpp_BlitMem;
		  }
	       }
               if (temp_state->bpp>=32) {
	          if (scale==1) {
	             vmwBlitMemToDisplay=SDL_NoScale32bpp_BlitMem;
		  } else { 
	             vmwBlitMemToDisplay=SDL_Double32bpp_BlitMem;
		  }
	       }
       
               vmwFlushPalette=SDL_flushPalette;
               vmwClearKeyboardBuffer=SDL_clearKeyboardBuffer;
               vmwGetInput=SDL_getInput;
               vmwCloseGraphics=SDL_closeGraphics;
#endif
               break;
    default: printf("ERROR! Unknown Display Target %i.\n",display_type);
             return NULL;
   }
      
   return temp_state;
}

vmwVisual *vmwSetupVisual(int xsize,int ysize) {
   
    vmwVisual *temp_visual;
   
    if ( (temp_visual=calloc(1,sizeof(vmwVisual)) )==NULL) {
       printf("Error allocating memory for visual!\n");
       return NULL;
    }
    temp_visual->xsize=xsize;
    temp_visual->ysize=ysize;
    
    if ( (temp_visual->memory=
	               calloc(xsize*ysize,sizeof(unsigned char)) )==NULL) {
       printf("Error allocating %i x %i visual memory!\n",xsize,ysize);
       return NULL;
    }
    return temp_visual;
   
}
