/* Palette handling part of the Super VMW Graphics Library */

#include "svmwgraph.h"

void vmwLoadPalette(vmwSVMWGraphState *state,unsigned char r,
		                             unsigned char g,
		                             unsigned char b,int color) {
   
   if (state->bpp==8) {
   
   }
   if (state->bpp==16) {
         /* 565 color cube */
         /* which can be LESS colorful than 8bpp.  Imagine that */
      state->palette[color]=  ( ((int)(r>>3)) <<11)+
	                  ( ((int)(g>>2)) <<5)+
	                  (b>>3);
      
   }
   vmwWritePaletteColor(state,r,g,b,color);
   
}

   
void vmwFadeToBlack(vmwVisual *source) {
//    int temp_pal[256];
    // temp_pal=real_pal
    // for (i=0;i<256;i++) {
    //    temp_pal[i]--;
    //    vmwBlitMemToDisplay()
    //    pauseawhile()
    // 
}

void vmwUnFade(vmwVisual *source) {
}
