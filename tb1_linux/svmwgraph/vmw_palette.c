/* Palette handling part of the Super VMW Graphics Library */

#include "svmwgraph.h"
#include <stdio.h>  /* for printf */
#include <unistd.h> /* for usleep */

void vmwLoadPalette(vmwSVMWGraphState *state,unsigned char r,
		                             unsigned char g,
		                             unsigned char b,int color) {
   

   
   state->actual_pal[color].r=r;
   state->actual_pal[color].g=g;
   state->actual_pal[color].b=b;
   
         /* 565 color cube */
         /* which can be LESS colorful than 8bpp.  Imagine that */
      
   state->palette[color]=  ( ((int)(r>>3)) <<11)+
	                  ( ((int)(g>>2)) <<5)+
	                  (b>>3);
      


}

int faded_pal[256];
int already_faded=0;

   
void vmwFadeToBlack(vmwSVMWGraphState *state, vmwVisual *source, 
		    int justLoadPal) {

    int i,j;
    int r[256],g[256],b[256];
   
    for (i=0;i<256;i++) faded_pal[i]=state->palette[i];
   
    if (justLoadPal) {
       already_faded=1;
       return;
    }
   
    if (already_faded) printf("Warning!  Fading already faded screen!\n");
   
    for(i=0;i<256;i++) {
       r[i]=(state->palette[i])>>11;
       g[i]=(state->palette[i]>>5)&0x3f;
       b[i]=(state->palette[i]&0x1f);
    }
   
    for(i=0;i<32;i++) {
       for(j=0;j<256;j++) {
	  r[j]-=2;  
	  if (r[j]<0) r[j]=0;
	  b[j]-=2;
	  if (b[j]<0) b[j]=0;
	  g[j]-=4; /* g has an extra bit */
	  if (g[j]<0) g[j]=0;
	  
	  state->palette[j]=(r[j]<<11)+(g[j]<<5)+b[j];
       }
       vmwFlushPalette(state);
       vmwBlitMemToDisplay(state,source);
       usleep(10000);
    }
    already_faded=1;
}

void vmwUnFade(vmwSVMWGraphState *state,vmwVisual *source) {
       
    int i,j;
   
    unsigned char r_max[256],g_max[256],b_max[256],
                  r[256],g[256],b[256];
   
    if (!already_faded) printf("Warning!  Unfading random palette!\n");
   
    for(i=0;i<256;i++) {
       r_max[i]=(faded_pal[i])>>11;
       g_max[i]=(faded_pal[i]>>5)&0x3f;
       b_max[i]=(faded_pal[i]&0x1f);
       r[i]=g[i]=b[i]=0;
    }
   
    for(i=0;i<32;i++) {
       for(j=0;j<256;j++) {
	  r[j]+=2;  
	  if (r[j]>r_max[j]) r[j]=r_max[j];
	  b[j]+=2;
	  if (b[j]>b_max[j]) b[j]=b_max[j];
	  g[j]+=4; /* g has an extra bit */
	  if (g[j]>g_max[j]) g[j]=g_max[j];
	  
	  state->palette[j]=(r[j]<<11)+(g[j]<<5)+b[j];
       }
       vmwFlushPalette(state);
       vmwBlitMemToDisplay(state,source);
       usleep(10000);
    }
}
