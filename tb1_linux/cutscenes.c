/*
 *  Level 2 Engine Code for Tom Bombem
 *  */

    /* The Includes */
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include "svmwgraph/svmwgraph.h"
#include "tb1_state.h"
#include "levels.h"
#include "tblib.h"
#include "sound.h"

#include "sidebar.h"
#include "help.h"
#include "loadsave.h"
#include "graphic_tools.h"


void level2_end(tb1_state *game_state) {
  
       vmwClearKeyboardBuffer();
       pauseawhile(5);
       vmwLoadPicPacked(0,0,game_state->virtual_3,0,1,
			tb1_data_file("level1/viewscr.tb1",game_state->path_to_data),
			game_state->graph_state);
       vmwClearScreen(game_state->virtual_1,0);
       vmwArbitraryCrossBlit(game_state->virtual_3,0,79,58,37,
			     game_state->virtual_1,10,10);
       vmwClearKeyboardBuffer();
       vmwSmallTextXY("UNIDENTIFIED SPACECRAFT!",70,10,2,0,1,tb1_font,game_state->virtual_1);
       vmwSmallTextXY("DO YOU WISH TO DEACTIVATE ",70,20,2,0,1,tb1_font,game_state->virtual_1);
       vmwSmallTextXY("THIS SHIP'S SECURITY SYSTEMS? (Y/N)",70,30,2,0,1,tb1_font,game_state->virtual_1);
       vmwBlitMemToDisplay(game_state->graph_state,virtual_1);
       vmwClearKeyboardBuffer();
      
       ch='!';
       while ((ch!='Y') && (ch!='y') && (ch!='N') && (ch!='n')) {
          while(!(ch=vmwGetInput())) usleep(1000);
       }
       
       if ((ch=='N') || (ch=='n')) {
	  vmwArbitraryCrossBlit(game_state->virtual_3,0,79,58,37,
				game_state->virtual_1,10,50);
          vmwSmallTextXY("NO?  AFFIRMATIVE. ",70,50,9,0,1,tb1_font,game_state->virtual_1);
          vmwSmallTextXY("ARMING REMOTE DESTRUCTION RAY.",70,60,9,0,1,tb1_font,game_state->virtual_1);
          vmwSmallTextXY("GOOD-BYE.",70,70,9,0,1,tb1_font,game_state->virtual_1);
          vmwBlitMemToDisplay(game_state->graph_state,virtual_1);
	  pauseawhile(4);
       }
      

       if ((ch=='Y') || (ch=='y')) {
	  vmwArbitraryCrossBlit(game_state->virtual_3,0,79,58,37,
				game_state->virtual_1,10,50);
          vmwSmallTextXY("'Y'=CORRECT PASSWORD. ",70,50,2,0,1,tb1_font,game_state->virtual_1);
          vmwSmallTextXY("WELCOME SUPREME TENTACLEE COMMANDER.",70,60,2,0,1,tb1_font,game_state->virtual_1);
          vmwSmallTextXY("INITIATING TRACTOR BEAM AND AUTOMATIC",70,70,2,0,1,tb1_font,game_state->virtual_1);
          vmwSmallTextXY("LANDING PROCEDURE.",70,80,2,0,1,tb1_font,game_state->virtual_1);
          vmwSmallTextXY("WE WILL BE DEPARTING FOR THE PLANET",70,90,2,0,1,tb1_font,game_state->virtual_1);
          vmwSmallTextXY("EERM IN THREE MICROCYCLE UNITS.",70,100,2,0,1,tb1_font,game_state->virtual_1);
          vmwBlitMemToDisplay(game_state->graph_state,virtual_1);
	  pauseawhile(5);
	  
          game_state->level=3;
          vmwClearKeyboardBuffer();
	  
          vmwArbitraryCrossBlit(game_state->virtual_3,0,42,58,37,
				game_state->virtual_1,10,110);
	  
          vmwSmallTextXY("Wha? Wait!",70,110,9,0,1,tb1_font,game_state->virtual_1);
          vmwSmallTextXY("What's happening?",70,120,9,0,1,tb1_font,game_state->virtual_1);
          vmwBlitMemToDisplay(game_state->graph_state,virtual_1);
	  pauseawhile(6);
       }
       
       vmwLoadPicPacked(0,0,game_state->virtual_3,0,1,
			tb1_data_file("level3/tbtract.tb1",game_state->path_to_data),
			game_state->graph_state);

       
       vmwArbitraryCrossBlit(game_state->virtual_3,0,0,240,50,
			     game_state->virtual_2,0,0);
       vmwClearScreen(game_state->virtual_1,0);
       
       setupsidebar(game_state,virtual_1);
       
       
       for(howmuchscroll=50;howmuchscroll>0;howmuchscroll--) {
	  vmwArbitraryCrossBlit(virtual_2,0,howmuchscroll,240,200,virtual_1,0,0);
	  usleep(30000);
	  vmwPutSprite(ship_shape[0],shipx,165,virtual_1);
          vmwBlitMemToDisplay(game_state->graph_state,virtual_1);    
       }

       if ((ch=='N') || (ch=='n')) {
          vmwClearKeyboardBuffer();
          vmwLine(7,6,shipx+10,180,4,virtual_1);
          vmwLine(shipx+37,180,231,6,4,virtual_1);
          vmwBlitMemToDisplay(game_state->graph_state,virtual_1);
	  pauseawhile(1);
          vmwClearKeyboardBuffer();
          for(i=shipx;i<shipx+48;i++) {
             vmwDrawVLine(i,165,30,4,virtual_1);
	  }
	  vmwBlitMemToDisplay(game_state->graph_state,virtual_1);
          pauseawhile(2);
	  vmwArbitraryCrossBlit(virtual_2,0,howmuchscroll,240,200,virtual_1,0,0);
          vmwBlitMemToDisplay(game_state->graph_state,virtual_1);
	  pauseawhile(2);      
       }

       else {
	  if (shipx-95==0) shipadd=0;
	  if (shipx-95>0) shipadd=1;
	  if (shipx-95<0) shipadd=-1;
	  shipy=165;
          while ((shipx!=95) || (shipy>10)) {
             if (shipx!=95) shipx-=shipadd;
             if (shipy>10) shipy--;
	     vmwArbitraryCrossBlit(virtual_2,0,howmuchscroll,240,200,virtual_1,0,0);
	     
             vmwLine(7,6,shipx+12,shipy+15,2,virtual_1);
             vmwLine(shipx+37,shipy+15,231,6,2,virtual_1);
             vmwPutSprite(ship_shape[0],shipx,shipy,virtual_1);
             vmwBlitMemToDisplay(game_state->graph_state,virtual_1);
	     usleep(30000);
	  }
          vmwClearKeyboardBuffer();
          pauseawhile(8);
          vmwClearScreen(virtual_1,0);
       }
/*
     while keypressed do ch:=readkey;
     if level=4 then begin
       vmwSmallTextXY('THE PLANET EERM?',20,20,10,0,1,tb1_font,game_state->virtual_1);
       vmwSmallTextXY('XENOCIDE FLEET?',20,30,10,0,1,tb1_font,game_state->virtual_1);
       vmwSmallTextXY('WHAT'S GOING ON?',20,40,10,0,1,tb1_font,game_state->virtual_1);
       vmwSmallTextXY('A MAJOR GOVERNMENT CONSPIRACY?  MASS HALUCINATIONS?',20,50,10,0,1,tb1_font,game_state->virtual_1);

       vmwSmallTextXY('WATCH FOR TOM BOMBEM LEVEL 3 (CURRENTLY IN THE DESIGN PHASE).',10,70,12,0,1,tb1_font,game_state->virtual_1);
       vmwSmallTextXY('ALL THESE QUESTIONS WILL BE ANSWERED!',10,80,12,0,1,tb1_font,game_state->virtual_1);
       vmwSmallTextXY('ALSO MORE FEATURES WILL BE ADDED:',10,90,12,0,1,tb1_font,game_state->virtual_1);
       vmwSmallTextXY('     BETTER GRAPHICS, SOUND AND SPEED.',10,100,12,0,1,tb1_font,game_state->virtual_1);

       vmwSmallTextXY('TO HASTEN COMPLETION, SEND QUESTIONS/COMMENTS/DONATIONS TO ',9,120,9,0,1,tb1_font,game_state->virtual_1);
       vmwSmallTextXY('THE AUTHOR (SEE THE REGISTER MESSAGE FOR RELEVANT ADDRESSES).',9,130,9,0,1,tb1_font,game_state->virtual_1);

       vmwSmallTextXY('THANK YOU FOR PLAYING TOM BOMBEM',80,150,14,0,1,tb1_font,game_state->virtual_1);
       unfade;
       pauseawhile(1800);
     end; */
          levelover=1;
    }
    }

}

  

void littleopener2(tb1_state *game_state) {
   
    vmwDrawBox(0,0,319,199,0,game_state->virtual_1);
    vmwLoadPicPacked(0,0,game_state->virtual_1,1,1,
		     tb1_data_file("level2/tbl2ship.tb1",game_state->path_to_data),
		     game_state->graph_state);
    vmwTextXY("Hmmmm... ",10,10,4,0,0,game_state->graph_state->default_font,
	      game_state->virtual_1);
    vmwTextXY("This Might Be Harder Than I Thought.",10,20,4,0,0,
	      game_state->graph_state->default_font,game_state->virtual_1);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(13); 
    vmwDrawBox(0,0,319,199,0,game_state->virtual_1);
 
}
