/* Page Flipping Routines for the Super VMW Graphics Library */

#include "svmwgraph.h"

#include <string.h> /* For memcpy() */

void vmwFlipVirtual(vmwVisual *destination, 
		    vmwVisual *source) {
 
   memcpy(destination,source,source->xsize*source->ysize);
   
}


int vmwArbitraryCrossBlit(vmwVisual *src,int x1,int y1,int w,int h,
			  vmwVisual *dest,int x2,int y2)
{
   
   int y;
   unsigned char *source,*destination;
   
   source=src->memory+(src->xsize*y1);
   destination=dest->memory+(dest->xsize*y2);
   
   for(y=0;y<h;y++) {
      memcpy ((destination+x2),(source+x1),w);
      source+=src->xsize;
      destination+=dest->ysize;
   }
    
   return 0;   
    
}

