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

void vmwPutPartialSprite(vmwVisual *destination,
			                          vmwSprite *sprite,
			                          int x_start,int y_start,
			                          int x_stop, int y_stop)
 {                          /* x_start/stop not implemented yet */
//       int xx,yy;
      /*
       *     dest+=(dest_stride*y);
       *     for(yy=0;yy<h;yy++){
       *        for(xx=0;xx<w;xx++)
       *           if ((*(src+xx)) && ((yy>=y_start) && (yy<=y_stop)) )
       *              memcpy(dest+(stride_factor*(xx+x)),(src+xx),stride_factor);
       *          // **(dest+xx+x)=15;
       *    src+=w;
       *    dest+=dest_stride;
       *     }
       *     */
     return;
   
}
