/* Core Graphics routines for the Super VMW Graphics Library */

#include "svmwgraph.h"

// #define DEBUG 1

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

void vmwLine(int x1, int y1, int x2, int y2, int color, 
	     vmwVisual *destination) {

/*function sgn(a:real):integer;
begin
     if a>0 then sgn:=+1;
     if a<0 then sgn:=-1;
     if a=0 then sgn:=0;
end;

procedure line(a,b,c,d,col:integer;where:word);
var u,s,v,d1x,d1y,d2x,d2y,m,n:real;
    i:integer;
begin
     u:= c - a;
     v:= d - b;
     d1x:= SGN(u);
     d1y:= SGN(v);
     d2x:= SGN(u);
     d2y:= 0;
     m:= ABS(u);
     n := ABS(v);
     IF NOT (M>N) then
     BEGIN
          d2x := 0 ;
          d2y := SGN(v);
          m := ABS(v);
          n := ABS(u);
     END;
     s := INT(m / 2);
     FOR i := 0 TO round(m) DO
     BEGIN
          putpixel(a,b,col,where);
          s := s + n;
          IF not (s<m) THEN
          BEGIN
               s := s - m;
               a:= a +round(d1x);
               b := b + round(d1y);
          END
          ELSE
          BEGIN
               a := a + round(d2x);
               b := b + round(d2y);
          END;
     end;
END;
*/

}

void vmwDrawBox(int x1,int y1,int xsize,int ysize,int col, vmwVisual *where) {
    int i;
   
    for(i=0;i<ysize;i++) {
       vmwDrawHLine(x1,y1+i,xsize,col,where);
    }
}
