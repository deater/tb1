#include <stdio.h>
#include <unistd.h>

#include "svmwgraph/svmwgraph.h"
#include "tb1_state.h"
#include "menu_tools.h"
#include "sound.h"

int quit(struct tb1_state *game_state)
{

    int barpos=0,ch=0;
    vmwFont *tb1_font;
    vmwVisual *target;
   
    tb1_font=game_state->graph_state->default_font;
    target=game_state->virtual_1;
   
    coolbox(90,75,230,125,1,target);
    vmwTextXY("QUIT??? ARE YOU",97,82,9,7,0,tb1_font,target);
    vmwTextXY("ABSOLUTELY SURE?",97,90,9,7,0,tb1_font,target);
   
    while (ch!=VMW_ENTER){
       if (barpos==0) vmwTextXY("YES-RIGHT NOW!",97,98,150,0,1,tb1_font,target);
       else vmwTextXY("YES-RIGHT NOW!",97,98,150,7,1,tb1_font,target);
       if (barpos==1) vmwTextXY("NO--NOT YET.",97,106,150,0,1,tb1_font,target);
       else vmwTextXY("NO--NOT YET.",97,106,150,7,1,tb1_font,target);
       vmwBlitMemToDisplay(game_state->graph_state,target);
       
       while ( !(ch=vmwGetInput()) ) {
	  usleep(30);
       }
       if ((ch==VMW_UP)||(ch==VMW_DOWN)||(ch==VMW_LEFT)||(ch==VMW_RIGHT)) barpos++;
       if (ch=='y') barpos=0;
       if (ch=='n') barpos=1;
       if (barpos==2) barpos=0;
    }
    if (barpos==0){ 
       shutdownSound();
       exit(1);
    }
    else return 6;

}
