/* Palette handling part of the Super VMW Graphics Library */

#include "svmwgraph.h"


   /* The "packing" method used by the 16 bit palette */
int vmwPack3Bytes(int r, int g, int b) {
   
         return  ( ( ( (r>>3)&0x1f) <<11)+
		   ( ( (g>>2)&0x3f) <<5)+
		   ( ( (b>>3)&0x1f) ) );
}


void vmwLoadCustomPalette(vmwVisual *source, int pal[256]) {
   int i;
   for (i=0;i<256;i++) {
       source->palette[i]=pal[i];
   }
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
