#include "svmwgraph.h"
#include <unistd.h>  /* usleep */


  /* MAKE SURE YOU DO A COOLBOX TO 319,199, not 320,200!!! */
  /* This will cause HARD TO DEBUG BUGS!!! */
  /* I should add a check here I suppose */

void vmwCoolBox(int x1,int y1,int x2,int y2,int fill,vmwVisual *target)
{
    int i;
   
    for(i=0;i<5;i++) {
       vmwDrawHLine(x1+i,y1+i,(x2-x1-i-i),31-i,target);
       vmwDrawHLine(x1+i,y2-i,(x2-x1-i-i),31-i,target);
       vmwDrawVLine(x1+i,y1+i,(y2-y1-i-i)+1,31-i,target);
       vmwDrawVLine(x2-i,y1+i,(y2-y1-i-i)+1,31-i,target);
    }
    if (fill) {
       for(i=y1+5;i<y2-4;i++) vmwDrawHLine(x1+5,i,(x2-x1-9),7,target);
     }  
}


int vmwAreYouSure(vmwSVMWGraphState *graph_state,
		  vmwFont *tb1_font,
		  vmwVisual *target,
		  char *warning_1,
		  char *warning_2,
		  char *yes_option,
		  char *no_option) {


    int barpos=0,ch=0;
       
    vmwCoolBox(90,75,230,125,1,target);
    vmwTextXY(warning_1,97,82,9,7,0,tb1_font,target);
    vmwTextXY(warning_2,97,90,9,7,0,tb1_font,target);
   
    while (ch!=VMW_ENTER){
       if (barpos==0) vmwTextXY(yes_option,97,98,150,0,1,tb1_font,target);
       else vmwTextXY(yes_option,97,98,150,7,1,tb1_font,target);
       if (barpos==1) vmwTextXY(no_option,97,106,150,0,1,tb1_font,target);
       else vmwTextXY(no_option,97,106,150,7,1,tb1_font,target);
       vmwBlitMemToDisplay(graph_state,target);
       
       while ( !(ch=vmwGetInput()) ) {
	  usleep(30);
       }
       if ((ch==VMW_UP)||(ch==VMW_DOWN)||(ch==VMW_LEFT)||(ch==VMW_RIGHT)) barpos++;
       if (ch=='y') barpos=0;
       if (ch=='n') barpos=1;
       if (barpos==2) barpos=0;
    }
    return !barpos;
}
