#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "svmwgraph/svmwgraph.h"
#include "tb1_state.h"
#include "graphic_tools.h"

void pauseawhile(int howlong) {
   
       struct timeval bob;
       struct timezone mree;
       long begin_s,begin_u;
   
       vmwClearKeyboardBuffer();
       gettimeofday(&bob,&mree);
       begin_s=bob.tv_sec; begin_u=bob.tv_usec;
       while ((bob.tv_sec-begin_s)<howlong) {
	         if (vmwGetInput()) return;
	         usleep(30);
	         gettimeofday(&bob,&mree);
       }
}


    /* The collision detection routine.  Optimize? */
    /* Detects if x1,y1 is within x2,y2            */
int collision(int x1,int y1,int xsize,int ysize,
	      int x2,int y2,int x2size,int y2size)
{
       if (abs((y1+ysize)-(y2+y2size))<(ysize+y2size)){
	         if (abs((x1+xsize)-(x2+x2size))<(xsize+x2size)) return 1;
       }
       return 0;
}

    
    /* Now memory-leak friendly */
char *tb1_data_file(char *name,char *path)
{   
    static int initialized=0;
    static char *tempst;
   
   if (!initialized) {
       tempst=(char *)calloc(BUFSIZ,sizeof(char));
       initialized=1;
    }
    snprintf(tempst,BUFSIZ,"%s/%s",path,name);
    return tempst;
}

int are_you_sure(tb1_state *game_state,
		 char *warning_1,
		 char *warning_2,
		 char *yes_option,
		 char *no_option) {


    int barpos=0,ch=0;
    
    vmwFont *tb1_font;
    vmwVisual *target;
   
    tb1_font=game_state->graph_state->default_font;
    target=game_state->virtual_1;
   
    coolbox(90,75,230,125,1,target);
    vmwTextXY(warning_1,97,82,9,7,0,tb1_font,target);
    vmwTextXY(warning_2,97,90,9,7,0,tb1_font,target);
   
    while (ch!=VMW_ENTER){
       if (barpos==0) vmwTextXY(yes_option,97,98,150,0,1,tb1_font,target);
       else vmwTextXY(yes_option,97,98,150,7,1,tb1_font,target);
       if (barpos==1) vmwTextXY(no_option,97,106,150,0,1,tb1_font,target);
       else vmwTextXY(no_option,97,106,150,7,1,tb1_font,target);
       vmwBlitMemToDisplay(game_state->graph_state,target);
       
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
