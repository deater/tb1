/* Palette handling part of the Super VMW Graphics Library */

#include "svmwgraph.h"


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
