/* Routines for setting up structures for the Super VMW graphics library */

#include "svmwgraph.h"
#include "sdl_svmwgraph.h"  /* Make this dynamic somehow? */

#include <stdio.h>  /* For printf */
#include <stdlib.h> /* For Memory Allocation */

void *(*vmwSetupGraphics)(int *xsize,int *ysize, int *bpp, 
			  int fullscreen,int verbose);
void (*vmwBlitMemToDisplay)(vmwSVMWGraphState *display, vmwVisual *source);
void (*vmwWritePaletteColor)(vmwSVMWGraphState *state,
	        unsigned char r,unsigned char g,unsigned char b,int color);

void (*vmwClearKeyboardBuffer)(void);
int (*vmwGetInput)(void);

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
        case VMW_SDLTARGET: vmwSetupGraphics=SDL_setupGraphics;
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


       /* Attempt to get desired graphics state */
    temp_state->output_screen=vmwSetupGraphics(&temp_state->xsize,
					      &temp_state->ysize,
					      &temp_state->bpp,
					      fullscreen,verbose);
       /* Setup proper blitter and others*/   
    switch (display_type) {
       case VMW_SDLTARGET: 
               if (temp_state->bpp==8) {
		  vmwBlitMemToDisplay=SDL_NoScale8bpp_BlitMem;
	       }
               if (temp_state->bpp>=16) {
		  if (scale==1) {
                     vmwBlitMemToDisplay=SDL_NoScale16bpp_BlitMem;
                  } else { 
	            vmwBlitMemToDisplay=SDL_Double16bpp_BlitMem;
		  }
	       }
               vmwWritePaletteColor=SDL_WritePaletteColor;
               vmwClearKeyboardBuffer=SDL_clearKeyboardBuffer;
               vmwGetInput=SDL_getInput;
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
