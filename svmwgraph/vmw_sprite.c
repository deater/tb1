/* Sprite implementation for the Super VMW graphics library */

#include "svmwgraph.h"
#include <stdlib.h> /* for calloc/malloc */


    /* Doesn't matter if this is semi-slow */
    /* It is used both to get size of the sprite, then to actually get it */
int fakeGetSprite(int x,int y,int xsize,int ysize,
			 vmwVisual *screen,int capture,
		         char *spritedata) {
   
    int oldcolor,xtemp,ytemp=0,color,numacross,pointer=0;

    oldcolor=vmwGetPixel(x,y,screen);
    xtemp=0;
    numacross=0;   
   
    while (ytemp<ysize) {
       color=vmwGetPixel(x+xtemp,y+ytemp,screen);
       if ( (color==oldcolor) && (numacross<126) && (xtemp<xsize)) numacross++;
       else { /* We aren't a run */
	  if ((numacross==1) && (oldcolor<128)) {
	         /* We can fit in a single byte */
	     if (capture) spritedata[pointer]=oldcolor;
	     pointer++;
	  }
	  else {  /* We fit in 2 bytes */
	     if (capture) spritedata[pointer]=numacross+128;
	     pointer++;
	     if (capture) spritedata[pointer]=oldcolor;
	     pointer++;
	     numacross=1;  /* Because the new color already is 1 across */
	  }
       }
       oldcolor=color;
       xtemp++;
       if (xtemp>xsize) {
          xtemp=0;
          numacross=0;
          ytemp++;
	  oldcolor=vmwGetPixel(x,y+ytemp,screen);
       }
   }
   return pointer;
}


    /* Sprites are now RLE encoded for speed and size reasons */
vmwSprite *vmwGetSprite(int x, int y,
			       int xsize, int ysize,vmwVisual *screen) {
   
    vmwSprite *temp_sprite;
    
    int sprite_size=0;
   
    sprite_size=fakeGetSprite(x,y,xsize,ysize,
		             screen,0,NULL);
      
    temp_sprite=calloc(1,sizeof(vmwSprite));
    temp_sprite->spritedata=calloc(1,sprite_size);
    temp_sprite->xsize=xsize;
    temp_sprite->ysize=ysize;
   
    fakeGetSprite(x,y,xsize,ysize,screen,1,temp_sprite->spritedata);

       /* Original non-RLE sprite code */
/*   for(ytemp=0;ytemp<ysize;ytemp++)
         for(xtemp=0;xtemp<xsize;xtemp++)
             *((temp_sprite->spritedata)+ytemp*xsize+xtemp)= 
  	   vmwGetPixel(xtemp+x,ytemp+y,screen);
*/   
    return temp_sprite;
}

void vmwFreeSprite(vmwSprite *sprite) {
   
    free(sprite->spritedata);
    free(sprite);
   
}

    /* Sprites are now RLE-encoded for size and speed reasons */
void vmwPutSprite(vmwSprite *sprite,int x,int y,
		  vmwVisual *screen) {
      
    unsigned char *temp_source,*temp_destination;
    int xx=0,yy=0,run_length,i;
   
    temp_source=sprite->spritedata;
    temp_destination= (unsigned char *)(screen->memory)+(y*screen->xsize+x);
     
    while (yy<sprite->ysize) {
       if ( (*temp_source) <128) {  /* Single Pixel */
	  if (*temp_source!=0) {
	     *(temp_destination)=*(temp_source);
	  }
	  xx++;
	  temp_source++;
	  temp_destination++;
       }
       
       else {   /* Run */
	  run_length=(*temp_source)-128;
	  temp_source++;
	     /* Non-transparent */
	  if (*temp_source) {
	     for (i=0;i<run_length;i++) {
	        *(temp_destination)=*(temp_source);
		temp_destination++;
	     }
	  }
	    
	  else { /* transparent */
	     temp_destination+=run_length;
	  }
	  
	  xx+=run_length;
	  temp_source++;
       }
        
          /* See if we are done this line */
       if (xx>=sprite->xsize) {
          temp_destination+=((screen->xsize-sprite->xsize));
          yy++;
	  xx=0;
       }
    }
   
    /* The old non-RLE sprite code */   
/*   for (yy=0;yy<sprite->ysize;yy++) {
       for (xx=0;xx<sprite->xsize;xx++) {	  
	   if (*temp_source!=0) {
	      *(temp_destination)=*(temp_source);
	   }
	   temp_source++;
	   temp_destination++;
       }
       temp_destination+=((screen->xsize-sprite->xsize));
*/
}

void vmwPutSpriteNonTransparent(vmwSprite *sprite,int x,int y,
		  vmwVisual *screen) {
      
    unsigned char *temp_source,*temp_destination;
    int xx,yy,run_length,i;
   
    temp_source=sprite->spritedata;
    temp_destination= (unsigned char *)(screen->memory)+(y*screen->xsize+x);
      
    yy=0;
    xx=0;
    
    while (yy<sprite->ysize) {
    
       if ( (*temp_source) <128) {  /* Single Pixel */
	  *(temp_destination)=*(temp_source);
	  xx++;
	  temp_source++;
	  temp_destination++;
       }
       
       else {   /* Run */
	  run_length=(*temp_source)-128;
	  temp_source++;
	   
	  for (i=0;i<run_length;i++) {
	      *(temp_destination)=*(temp_source);
	      temp_destination++;
	  }
	  
	  xx+=run_length;
	  temp_source++;
       }
        
          /* See if we are done this line */
       if (xx>=sprite->xsize) {
          temp_destination+=((screen->xsize-sprite->xsize));
          yy++;
	  xx=0;
       }
    }
  
}

void vmwPutPartialSprite(vmwVisual *destination,
			 vmwSprite *sprite,int x,int y,
			 int x_start,int x_stop,
		         int y_start, int y_stop)
 {                     
    
    unsigned char *temp_source,*temp_destination;
    int xx,yy,run_length,i;
    
    temp_source=sprite->spritedata;
    temp_destination=(unsigned char *)(destination->memory)+(y*destination->xsize+x);

    yy=0;
    xx=0;
    
    while (yy<sprite->ysize) {
    
       if ( (*temp_source) <128) {  /* Single Pixel */
	  if (*temp_source!=0) {
	     if ( (yy+y>=y_start) && (yy+y<=y_stop) &&
		                 (xx+x>=x_start) && (xx+x<=x_stop) )
	       *(temp_destination)=*(temp_source);
	  }
	  xx++;
	  temp_source++;
	  temp_destination++;
       }
       
       else {   /* Run */
	  run_length=(*temp_source)-128;
	  temp_source++;
	     /* Non-transparent */
	  if (*temp_source) {
	     for (i=0;i<run_length;i++) {
		if ( (yy+y>=y_start) && (yy+y<=y_stop) &&
		                    (xx+x>=x_start) && (xx+x<=x_stop) )
	           *(temp_destination)=*(temp_source);
		temp_destination++;
	     }
	  }
	    
	  else { /* transparent */
	     temp_destination+=run_length;
	  }
	  
	  xx+=run_length;
	  temp_source++;
       }
        
          /* See if we are done this line */
       if (xx>=sprite->xsize) {
          temp_destination+=((destination->xsize-sprite->xsize));
          yy++;
	  xx=0;
       }
    }
    
    return;
   
}
