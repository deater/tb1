/* The NULL target, for when we don't have an actual output */

#include <stdio.h>
#include "svmwgraph.h"

void *null_setupGraphics(int *xsize,int *ysize,int *bpp,
			int fullscreen,int verbose)
{
    /* Returning NULL is considered an error */
  return (void *)1;
}


void null_flushPalette(vmwSVMWGraphState *state) {

}


void null_BlitMem(vmwSVMWGraphState *target_p, vmwVisual *source) {
   
}

void null_clearKeyboardBuffer() {

}

int null_getInput() {
   return 0;
}

void null_closeGraphics() {
   
}
