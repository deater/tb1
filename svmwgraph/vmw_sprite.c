/* Sprite implementation for the Super VMW graphics library */

#include "svmwgraph.h"
#include <stdlib.h> /* for calloc/malloc */

vmwSprite *vmwGetSprite(int x, int y,
			       int xsize, int ysize,vmwVisual *screen) {
   
   vmwSprite *temp_sprite;
   int xtemp,ytemp;
   
   temp_sprite=calloc(1,sizeof(vmwSprite));
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

void vmwFreeSprite(vmwSprite *sprite) {
   
   free(sprite->spritedata);
   free(sprite);
   
}

void vmwPutSprite(vmwSprite *sprite,int x,int y,
		  vmwVisual *screen) {
      
   unsigned char *temp_source,*temp_destination;
   int xx,yy;
   
   temp_source=sprite->spritedata;
   temp_destination= (unsigned char *)(screen->memory)+(y*screen->xsize+x);
  
   for (yy=0;yy<sprite->ysize;yy++) {
       for (xx=0;xx<sprite->xsize;xx++) {
	  
	   if (*temp_source!=0) {
	      *(temp_destination)=*(temp_source);
	   }
	   temp_source++;
	   temp_destination++;
       }
       temp_destination+=((screen->xsize-sprite->xsize));
   }
}

void vmwPutSpriteNonTransparent(vmwSprite *sprite,int x,int y,
		  vmwVisual *screen) {
      
   unsigned char *temp_source,*temp_destination;
   int xx,yy;
   
   temp_source=sprite->spritedata;
   temp_destination= (unsigned char *)(screen->memory)+(y*screen->xsize+x);
  
   for (yy=0;yy<sprite->ysize;yy++) {
       for (xx=0;xx<sprite->xsize;xx++) {
	      *(temp_destination)=*(temp_source);
	   temp_source++;
	   temp_destination++;
       }
       temp_destination+=((screen->xsize-sprite->xsize));
   }
}

void vmwPutPartialSprite(vmwVisual *destination,
			 vmwSprite *sprite,int x,int y,
			 int x_start,int x_stop,
		         int y_start, int y_stop)
 {                     
    
    unsigned char *temp_source,*temp_destination;
    int xx,yy;
    
    temp_source=sprite->spritedata;
    temp_destination=(unsigned char *)(destination->memory)+(y*destination->xsize+x);

    for (yy=0;yy<sprite->ysize;yy++) {
       for (xx=0;xx<sprite->xsize;xx++) {
	      /* Only draw if in bounds */
	   if ( (yy+y>=y_start) && (yy+y<=y_stop) &&
	        (xx+x>=x_start) && (xx+x<=x_stop) ) {
	      
	      if (*temp_source!=0) {
	         *(temp_destination)=*(temp_source);
	      }
           }
	   temp_source++;
	   temp_destination++;
       }
       temp_destination+=((destination->xsize-sprite->xsize));
   }
   return;
   
}
