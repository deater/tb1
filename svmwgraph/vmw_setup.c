/* Routines for setting up structures for the Super VMW graphics library */

#include "svmwgraph.h"
#include "sdl_svmwgraph.h"  /* Make this dynamic somehow? */

#include <stdio.h>  /* For printf */
#include <stdlib.h> /* For Memory Allocation */

void *(*vmwSetupGraphics)(int xsize,int ysize, int bpp, int verbose);
void (*vmwBlitMemToDisplay)(vmwSVMWGraphState *display, vmwVisual *source);
void (*vmwClearKeyboardBuffer)(void);
int (*vmwGetInput)(void);

vmwSVMWGraphState *vmwSetupSVMWGraph(int display_type,int xsize,int ysize,
				     int bpp,int verbose) {
   
   vmwSVMWGraphState *temp_state;
   
   if ( (temp_state=calloc(1,sizeof(vmwSVMWGraphState)) )==NULL) {
      printf("Error allocating memory!\n");
      return NULL;
   }
   
   switch (display_type) {
    case VMW_SDLTARGET: vmwSetupGraphics=SDL_setupGraphics;
                        vmwBlitMemToDisplay=SDL_NoScale16bpp_BlitMem;
                        vmwClearKeyboardBuffer=SDL_clearKeyboardBuffer;
                        vmwGetInput=SDL_getInput;
                        temp_state->output_screen=
	                          vmwSetupGraphics(xsize,ysize,bpp,verbose);
                        break;
    default: printf("ERROR! Unknown Display Target %i.\n",display_type);
             return NULL;
   }
   
   temp_state->xsize=xsize;
   temp_state->ysize=ysize;
   temp_state->bpp=bpp;
   temp_state->default_font=NULL;
   
   return temp_state;
}

vmwVisual *vmwSetupVisual(int xsize,int ysize,int palette_size) {
   
    vmwVisual *temp_visual;
   
    if ( (temp_visual=calloc(1,sizeof(vmwVisual)) )==NULL) {
       printf("Error allocating memory for visual!\n");
       return NULL;
    }
    temp_visual->xsize=xsize;
    temp_visual->ysize=ysize;
    temp_visual->palette_size=palette_size;
    
    if ( (temp_visual->memory=
	               calloc(xsize*ysize,sizeof(unsigned char)) )==NULL) {
       printf("Error allocating %i x %i visual memory!\n",xsize,ysize);
       return NULL;
    }
    if ( (temp_visual->palette=
	               calloc(palette_size,sizeof(int)) )==NULL) {
       printf("Error allocating palette of size %i!\n",palette_size);
       return NULL;
    }
    return temp_visual;
   
}
