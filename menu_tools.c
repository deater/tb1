#include <stdio.h>

#include "svmwgraph/svmwgraph.h"

void coolbox(int x1,int y1,int x2,int y2,int fill,vmwVisual *target)
{
    int i;
   
    for(i=0;i<5;i++) {
       vmwDrawHLine(x1+i,y1+i,(x2-x1-i-i),31-i,target);
       vmwDrawHLine(x1+i,y2-i,(x2-x1-i-i),31-i,target);
       vmwDrawVLine(x1+i,y1+i,(y2-y1-i-i),31-i,target);
       vmwDrawVLine(x2-i,y1+i,(y2-y1-i-i),31-i,target);
    }
    if (fill) {
       for(i=y1+5;i<y2-4;i++) vmwDrawHLine(x1+5,i,(x2-x1-9),7,target);
     }  
}

void shadowrite(char *st,int x5,int y5,int forecol,int backcol,
		vmwFont *tb1_font,vmwVisual *target)
{
    vmwTextXY(st,x5+1,y5+1,backcol,0,0,tb1_font,target);
    vmwTextXY(st,x5,y5,forecol,0,0,tb1_font,target);
}
