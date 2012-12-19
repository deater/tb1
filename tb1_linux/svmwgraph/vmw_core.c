/* Core Graphics routines for the Super VMW Graphics Library */

#include "svmwgraph.h"
#include <stdio.h>
#include <string.h> /* memset */
#include <math.h>

#define DEBUG 1

    /*---------------------------------------------------------------*\
    |          vmwPutPixel                                            |
    \*---------------------------------------------------------------*/
     
void vmwPutPixel(int x,int y,int color,vmwVisual *target) {
   
#ifdef DEBUG   
    if ( (y> (target->ysize-1) ) || (x> (target->xsize-1) ) ) {
       printf("Out of bounds with PutPixel %i,%i\n",x,y);
       return;
    }
#endif

    target->memory[(y*target->xsize)+x]=color;
      
}

unsigned char vmwGetPixel(int x,int y,vmwVisual *source) {
    
    return source->memory[(y*source->xsize)+x];
   
}


void vmwClearScreen(vmwVisual *target, int color) {
   
    memset(target->memory,color,target->xsize*target->ysize);
    
}

    /*---------------------------------------------------------------*\
    |     vmwDrawHLine                                                |
    |  This is really begging to be optimized.  I am sure some        |
    |       targets can do this in hardware                           |
    \*---------------------------------------------------------------*/
   
void vmwDrawHLine(int xstart,int ystart,int how_long,int color, 
		  vmwVisual *target) {
   
    unsigned char *temp_pointer;
    int i;
   
    temp_pointer=target->memory+(ystart*target->xsize)+xstart;
   
    for(i=0;i<how_long;i++) {
       *temp_pointer=color;
       temp_pointer++;
#ifdef DEBUG
       if (xstart+i>target->xsize-1) {
	  printf("Out of bounds!\n");
	  return;  
       }
#endif
    }
}


void vmwDrawVLine(int xstart,int ystart,int how_long,int color, 
		  vmwVisual *target) {
   
    unsigned char *temp_pointer;
    int i;
   
    temp_pointer=target->memory+(ystart*target->xsize)+xstart;
   
    for(i=0;i<how_long;i++) {
       *temp_pointer=color;
       temp_pointer+=target->xsize;
    }
}


int sgn(float a) {
     if (a>0)  return 1;
     if (a<0)   return -1;
     return 0;
}


    /* I don't pretend to understand the following yet.  Taken
     * from PCGPE.  Might re-write by self when get around to it */
void vmwLine(int x1,int y1,int x2,int y2,int color,vmwVisual *destination) {
   
    float dx,dy,d1x,d1y,d2x,d2y,m,n,s;
    int i;
   
    dx = x2 - x1;
    dy = y2 - y1;
   
    d1x=sgn(dx);
    d1y=sgn(dy);
    d2x=sgn(dx);
    d2y= 0.0;
   
    m=fabs(dx);
    n=fabs(dy);
    if (!(m>n)) {
       d2x=0;
       d2y=sgn(dy);
       m=fabs(dy);
       n=fabs(dx);
    }
    s=rint(m / 2);
    for (i= 0;i< (int)m;i++) {
        vmwPutPixel(x1,y1,color,destination);
        s+=n;
        if (!(s<m)) {
           s-=m;
           x1+=rint(d1x);
           y1+=rint(d1y);
	}
        else {
           x1+=rint(d2x);
           y1+=rint(d2y);
	}
    }
}

void vmwDrawBox(int x1,int y1,int xsize,int ysize,int col, vmwVisual *where) {
    int i;
   
    for(i=0;i<ysize;i++) {
       vmwDrawHLine(x1,y1+i,xsize,col,where);
    }
}
