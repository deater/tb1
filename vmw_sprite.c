#include "SDL.h"
#include "sdl_svmwgraph.h"
#include "vmw_sprite.h"
#include <stdlib.h>

extern unsigned int global_palette[256];


struct vmwSprite *vmwGetSprite(int x, int y,
			int xsize, int ysize,unsigned char *screen) {
   
   struct vmwSprite *temp_sprite;
   int xtemp,ytemp;
   
   temp_sprite=calloc(1,sizeof(struct vmwSprite));
   temp_sprite->spritedata=calloc(1,xsize*ysize);
   temp_sprite->xsize=xsize;
   temp_sprite->ysize=ysize;
   
   /* The following is very slow and can be easily optimized */
   
   for(ytemp=0;ytemp<ysize;ytemp++)
       for(xtemp=0;xtemp<xsize;xtemp++)
           *((temp_sprite->spritedata)+ytemp*xsize+xtemp)= 
	   vmwGetPixel(xtemp+x,ytemp+y,screen);
   
   return temp_sprite;
}

void vmwPutSprite(struct vmwSprite *sprite,int x,int y,int shield_color,
		  unsigned char *screen) {
      
   unsigned char *temp_source,*temp_destination;
   int i=0,xx,yy;
   
   temp_source=sprite->spritedata;
   temp_destination= ((Uint8 *)screen)+(y*320+x)*2;
  
   for (yy=0;yy<sprite->ysize;yy++) {
       for (xx=0;xx<sprite->xsize;xx++) {
	  
	   if (*temp_source!=0) {
	      *((Uint16 *)(temp_destination))=global_palette[*(temp_source)];
	   }
	   temp_source++;
	   temp_destination+=2;
       }
       temp_destination+=((320-sprite->xsize)*2);
      
   }
}

