#include <stdio.h>
#include <unistd.h> /* for usleep */

#include "svmwgraph/svmwgraph.h"
#include "tb1_state.h"

#include "tblib.h"
#include "sound.h"


    vmwSprite *bigFlame1,*bigFlame2,*smallFlame1,*smallFlame2;
    vmwSprite *rent,*truck1,*truck2,*barge;
    vmwSprite *explosion1,*explosion2;



void doflames(tb1_state *game_state) {
   
    static int flames=0;
    static char alienchar=34;
    char tempst[5];
   
    if (flames) {
       vmwPutSprite(bigFlame1,213,100,game_state->virtual_1);
       vmwPutSpriteNonTransparent(smallFlame2,105,90,game_state->virtual_1);
       vmwPutSpriteNonTransparent(smallFlame1,151,71,game_state->virtual_1);
       vmwPutSpriteNonTransparent(smallFlame2,218,72,game_state->virtual_1);
    }
    else {
       vmwPutSprite(bigFlame2,213,100,game_state->virtual_1);
       vmwPutSpriteNonTransparent(smallFlame1,105,90,game_state->virtual_1);
       vmwPutSpriteNonTransparent(smallFlame2,151,71,game_state->virtual_1);
       vmwPutSpriteNonTransparent(smallFlame1,218,72,game_state->virtual_1);
       sprintf(tempst,"%c",alienchar); 
       vmwTextXY(tempst,10,10,12,0,1,game_state->graph_state->default_font,game_state->virtual_1);
       alienchar++;
       if (alienchar>44) alienchar=34;
    }
    flames=(!flames);
}


void story(tb1_state *game_state)
{
    int i;
    int xtemp,ytemp;
    int thrustcol;
    float thrust;
    char tempch;
    int cycles,to_clear;
   
    vmwFont *tb1_font;
   
    tb1_font=game_state->graph_state->default_font;


    vmwLoadPicPacked(0,0,game_state->virtual_2,1,1,
		     tb1_data_file("story/tbsobj.tb1",game_state->path_to_data),
		     game_state->graph_state);
   
       /* Load up the sprites! */
    bigFlame1=vmwGetSprite(0,2,26,18,game_state->virtual_2);
    bigFlame2=vmwGetSprite(0,21,26,18,game_state->virtual_2);
   
    explosion1=vmwGetSprite(127,100,15,18,game_state->virtual_2);
    explosion2=vmwGetSprite(148,100,15,18,game_state->virtual_2);
   
    rent=vmwGetSprite(166,100,15,6,game_state->virtual_2);
    smallFlame1=vmwGetSprite(0,43,3,4,game_state->virtual_2);
    smallFlame2=vmwGetSprite(0,47,3,4,game_state->virtual_2);
    barge=vmwGetSprite(65,100,15,18,game_state->virtual_2);
    truck1=vmwGetSprite(85,100,6,8,game_state->virtual_2);
    truck2=vmwGetSprite(95,100,6,8,game_state->virtual_2);
      
       /******FIRST MESSAGE********/
    vmwClearScreen(game_state->virtual_1,0);
    vmwTextXY("THE STORY SO FAR...",20,20,4,0,0,tb1_font,game_state->virtual_1);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(7);

    vmwClearScreen(game_state->virtual_1,0);
   
    vmwLoadPicPacked(0,0,game_state->virtual_2,1,1,
		     tb1_data_file("story/tbcobj.tb1",game_state->path_to_data),
		     game_state->graph_state);
		     
    vmwArbitraryCrossBlit(game_state->virtual_2,129,56,49,132,
			  game_state->virtual_1,10,10);
    vmwTextXY("YOU ARE TOM BOMBEM,  A STRANGE",80,10,1,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("    BUT EFFICIENT MEMBER OF",80,20,1,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("    THE LUNAR SPACE FORCE.",80,30,1,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("YOU NEVER SAY MUCH AND YOU ARE",80,50,4,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("    RARELY SEEN OUTSIDE OF",80,60,4,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("    YOUR BLUE SPACESUIT.",80,70,4,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("YOU OFTEN GET YOURSELF IN ",80,90,2,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("    TROUBLE BY SCRATCHING",80,100,2,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("    YOUR HEAD AT INAPPROPRIATE",80,110,2,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("    TIMES.",80,120,2,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("PRESS ANY KEY....",96,185,15,0,0,tb1_font,game_state->virtual_1);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(15);

    vmwClearScreen(game_state->virtual_1,0);
      
    vmwArbitraryCrossBlit(game_state->virtual_2,129,56,49,132,
		        game_state->virtual_1,260,10);
    vmwArbitraryCrossBlit(game_state->virtual_2,99,104,29,81,
			game_state->virtual_1,287,13);

    vmwTextXY("IT IS THE YEAR 2028.",10,10,1,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("YOU HAVE BEEN SUMMONED BY",10,30,3,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("    LUNAR DICTATOR-IN-CHIEF",10,40,3,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("    VINCENT WEAVER ABOUT A",10,50,3,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("    TOP SECRET THREAT TO ",10,60,3,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("    INTERPLANETARY SECURITY.",10,70,3,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("YOU ATTEND THE BRIEFING WITH",10,90,5,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("    YOUR USUAL CONFUSED",10,100,5,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("    QUIETNESS.  YOU STILL DO",10,110,5,0,0,tb1_font,game_state->virtual_1);  
    vmwTextXY("    NOT UNDERSTAND YOUR OWN",10,120,5,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("    SUCCESSFULNESS.",10,130,5,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("PRESS ANY KEY....",96,185,15,0,0,tb1_font,game_state->virtual_1);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(12);

    vmwLoadPicPacked(0,0,game_state->virtual_1,1,1,
		    tb1_data_file("story/tbchief.tb1",game_state->path_to_data),
		    game_state->graph_state);

      /* Save the area where the error will go */
    vmwArbitraryCrossBlit(game_state->virtual_1,115,55,91,59,
		          game_state->virtual_2,115,255);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(6);
   
    vmwTextXY("Ahhh.... Mr. Bombem.... ",1,1,15,0,0,tb1_font,game_state->virtual_1);  
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
  if (game_state->sound_enabled) playGameFX(0);
    pauseawhile(6);
   
    if (game_state->sound_enabled) playGameFX(2);
      /* Show fake error message */
    vmwArbitraryCrossBlit(game_state->virtual_2,188,14,91,59,
			  game_state->virtual_1,115,55);
   
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(6);
   
      /* Restore background where error was */
    vmwArbitraryCrossBlit(game_state->virtual_2,115,255,91,59,
			  game_state->virtual_1,115,55);
 
    vmwTextXY("I'll be brief.                       ",1,1,15,0,1,tb1_font,game_state->virtual_1);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(5);
   
    vmwTextXY("Do you know how this base was founded?",1,1,15,0,1,tb1_font,game_state->virtual_1);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(5);
   
    vmwTextXY("No?  Well watch the screen.             ",1,1,15,0,1,tb1_font,game_state->virtual_1);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(5); 
   
       /* Put picture on screen */
    vmwArbitraryCrossBlit(game_state->virtual_2,210,75,85,60,
			  game_state->virtual_1,210,136);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(4);

       /******BARGE TAKING OFF***********/
   
    vmwClearScreen(game_state->virtual_2,0);
    vmwLoadPicPacked(0,0,game_state->virtual_2,1,1,
		     tb1_data_file("story/tbma1.tb1",game_state->path_to_data),
		     game_state->graph_state);
 
    vmwSmallTextXY("MY WIFE AND I FOUNDED",212,3,14,0,0,tb1_font,game_state->virtual_2);
    vmwSmallTextXY("THIS BASE IN 2008.",212,9,14,0,0,tb1_font,game_state->virtual_2);
    vmwSmallTextXY("THE ONLY WAY TO ",212,16,13,0,0,tb1_font,game_state->virtual_2);
    vmwSmallTextXY("FINANCE IT WAS TO",212,22,13,0,0,tb1_font,game_state->virtual_2);
    vmwSmallTextXY("ENGAGE IN A DUBIOUS",212,28,13,0,0,tb1_font,game_state->virtual_2);
    vmwSmallTextXY("BUSINESS.",212,34,13,0,0,tb1_font,game_state->virtual_2);
    vmwSmallTextXY("WE LAUNCHED EARTH'S",212,41,12,0,0,tb1_font,game_state->virtual_2);
    vmwSmallTextXY("TRASH INTO SPACE",212,47,12,0,0,tb1_font,game_state->virtual_2);
    vmwSmallTextXY("FOR A PROFIT.",212,53,12,0,0,tb1_font,game_state->virtual_2);
    vmwSmallTextXY("HERE IS FOOTAGE FROM",212,60,11,0,0,tb1_font,game_state->virtual_2);
    vmwSmallTextXY("THE LAST LAUNCH EIGHT",212,66,11,0,0,tb1_font,game_state->virtual_2);
    vmwSmallTextXY("YEARS AGO.",212,72,11,0,0,tb1_font,game_state->virtual_2);
 
  
    vmwFlipVirtual(game_state->virtual_1,game_state->virtual_2,320,200);

    vmwPutSprite(barge,141,157,game_state->virtual_1);

    
  /*  vmwCrossBlit(game_state->virtual_1,plb_vaddr->read,plb_vis->stride,200);*/
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(6);

 
       /* Roll the truck up to the barge */
    for(ytemp=190;ytemp>=164;ytemp--) { 
    
       if (ytemp>=172) 
	  vmwArbitraryCrossBlit(game_state->virtual_2,
				145,ytemp,7,10,
				game_state->virtual_1,145,ytemp);
       else
	  vmwArbitraryCrossBlit(game_state->virtual_2,
				145,172,7,10,
				game_state->virtual_1,145,172);
    
       if (ytemp%2) vmwPutPartialSprite(game_state->virtual_1,
					truck1,145,ytemp,
					0,319,172,199);
       else vmwPutPartialSprite(game_state->virtual_1,
			                truck2,145,ytemp,
			                0,319,172,199);
       
       vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
       
       usleep(80000);       
    }
    pauseawhile(2);

       /* Have the barge take off */
    vmwFlipVirtual(game_state->virtual_1,game_state->virtual_2,320,200);
   
    vmwPutSprite(barge,141,157,game_state->virtual_1);
    thrustcol=0;
    ytemp=157;
    to_clear=0;
    thrust=0;
    while (ytemp> -25) {
       thrust+=0.15;
       if (thrustcol<62) thrustcol++;
       to_clear=ytemp;
       if (ytemp<0) to_clear=0;
       vmwArbitraryCrossBlit(game_state->virtual_2,141,to_clear,30,30,
			     game_state->virtual_1,141,to_clear);
       for(i=1;i<17;i++) { 
          vmwPutPartialSprite(game_state->virtual_1,
			      barge,141,ytemp,
			      0,319,0,199);
       }
       usleep(50000);
       vmwLoadPalette(game_state->graph_state,thrustcol*4,0,0,250);
       ytemp=ytemp-(int)thrust;
       vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    }
    pauseawhile(5);


    vmwClearScreen(game_state->virtual_2,0);
   
       /****SECOND CHIEF*******/
    vmwLoadPicPacked(0,0,game_state->virtual_2,1,1,
		    tb1_data_file("story/tbcobj.tb1",game_state->path_to_data),
		     game_state->graph_state);
    vmwLoadPicPacked(0,0,game_state->virtual_1,1,1,
		    tb1_data_file("story/tbchief.tb1",game_state->path_to_data),
		     game_state->graph_state);
    vmwArbitraryCrossBlit(game_state->virtual_2,7,104,90,21,
		    game_state->virtual_1,6,174);
    vmwTextXY("You might wonder why this is important.",
	      1,1,15,0,1,tb1_font,game_state->virtual_1); 
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(6);
    vmwTextXY("Last week we received a message.       ",
	      1,1,15,0,1,tb1_font,game_state->virtual_1);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(6);
    vmwTextXY("It is of extra-terrestrial origin.     ",
	      1,1,15,0,1,tb1_font,game_state->virtual_1);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(6);
    vmwTextXY("Watch the screen.                      ",
	      1,1,15,0,1,tb1_font,game_state->virtual_1);
    
    vmwArbitraryCrossBlit(game_state->virtual_2,210,136,85,59,
			  game_state->virtual_1,210,136);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(4);
   
       /******ALIEN DELEGATION*****/
   
    /* Something is not quite right here */
//    vmwLoadPicPacked(0,0,game_state->virtual_2,1,1,
//		    tb1_data_file("story/tbcrash.tb1",
//				  game_state->path_to_data));
    vmwLoadPicPacked(0,0,game_state->virtual_1,1,1,
		     tb1_data_file("story/tbcrash.tb1", 
				   game_state->path_to_data),game_state->graph_state);
    vmwLoadPicPacked(0,0,game_state->virtual_1,1,0,
       	                       tb1_data_file("tbomb1.tb1",
                               game_state->path_to_data),game_state->graph_state);
   
   
    vmwFlipVirtual(game_state->virtual_2,game_state->virtual_1,320,200);
//  flipd320(vaddr,vga);
    vmwPutSprite(barge,97,180,game_state->virtual_1);
    xtemp=97;
    ytemp=181;

    while (ytemp>118) {
       vmwArbitraryCrossBlit(game_state->virtual_2,xtemp,ytemp,20,20,
			     game_state->virtual_1,xtemp,ytemp);
       vmwPutSprite(barge,xtemp,ytemp,game_state->virtual_1);
       doflames(game_state);
       ytemp--;
       xtemp++;
       vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);  
       usleep(80000);
    }
    vmwArbitraryCrossBlit(game_state->virtual_2,xtemp,ytemp,30,30,
			  game_state->virtual_1,xtemp,ytemp);
    vmwLoadPalette(game_state->graph_state,0xff,0xff,0xff,0);
    vmwPutSprite(explosion1,160,118,game_state->virtual_1);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    vmwLoadPalette(game_state->graph_state,0,0,0,0);
    usleep(80000);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
   
    if (game_state->sound_enabled) playGameFX(2);
    for(xtemp=0;xtemp<10;xtemp++) { 
       vmwPutSprite(explosion2,160,118,game_state->virtual_1);
       doflames(game_state);
       vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
       usleep(80000);
       vmwPutSprite(explosion1,160,118,game_state->virtual_1);
       doflames(game_state);
       vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
       usleep(80000);
    }
    vmwArbitraryCrossBlit(game_state->virtual_2,159,114,30,30,
			  game_state->virtual_1,159,114);
    vmwPutSpriteNonTransparent(rent,160,118,game_state->virtual_1);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
   
    cycles=0;
    while ( ((tempch=vmwGetInput())==0) && cycles<12) {
       doflames(game_state);
       vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
       cycles++;
       usleep(500000);
    }

       /****ALIEN MESSAGE*****/
    vmwLoadPicPacked(0,0,game_state->virtual_1,1,1,
		     tb1_data_file("story/tbgorg.tb1",game_state->path_to_data),
		     game_state->graph_state);
    vmwTextXY("GREETINGS EARTHLINGS.",0,162,12,0,0,tb1_font,game_state->virtual_1);  
    vmwTextXY("I AM GORGONZOLA THE REPULSIVE.",0,171,12,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("YOU HAVE MADE A BIG MISTAKE.",0,180,12,0,0,tb1_font,game_state->virtual_1);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(7);
    vmwTextXY("YOUR SHIP FULL OF REFUSE HAS",0,162,12,0,1,tb1_font,game_state->virtual_1);
    vmwTextXY("DAMAGED OUR OFFICIAL PEACE    ",0,171,12,0,1,tb1_font,game_state->virtual_1);
    vmwTextXY("ENVOY.  IT WAS ON ITS WAY TO ",0,180,12,0,1,tb1_font,game_state->virtual_1);
    vmwTextXY("YOUR PLANET.                  ",0,189,12,0,1,tb1_font,game_state->virtual_1);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(7);
    vmwTextXY("IN AN IRONIC FORM OF RETALLIATION",0,162,12,0,1,tb1_font,game_state->virtual_1);
    vmwTextXY("WE HAVE MADE YOUR TRASH EVIL AND",0,171,12,0,1,tb1_font,game_state->virtual_1);
    vmwTextXY("TURNED IT AGAINST YOU.          ",0,180,12,0,1,tb1_font,game_state->virtual_1);
    vmwTextXY("        DIE EARTH SCUM!         ",0,189,12,0,1,tb1_font,game_state->virtual_1);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(7);
 
       /****** THIRD CHIEF *******/
    vmwLoadPicPacked(0,0,game_state->virtual_2,1,1,
		    tb1_data_file("story/tbcobj.tb1",game_state->path_to_data),
		     game_state->graph_state);
    vmwLoadPicPacked(0,0,game_state->virtual_1,1,1,
		    tb1_data_file("story/tbchief.tb1",game_state->path_to_data),
		     game_state->graph_state);
    vmwArbitraryCrossBlit(game_state->virtual_2,7,127,90,21,
		          game_state->virtual_1,6,174);
   
    vmwTextXY("Tom, our radar detects approaching ",1,1,15,0,1,tb1_font,game_state->virtual_1);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(6);
    vmwTextXY("objects.  They are inside the      ",1,1,15,0,1,tb1_font,game_state->virtual_1);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(6);
    vmwTextXY("orbit of Jupiter.                  ",1,1,15,0,1,tb1_font,game_state->virtual_1);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(6);
    vmwTextXY("You are our only hope!             ",1,1,15,0,1,tb1_font,game_state->virtual_1);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(6);
    vmwTextXY("Will you fly our only spaceship    ",1,1,15,0,1,tb1_font,game_state->virtual_1);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(6);
    vmwTextXY("and save the human race?           ",1,1,15,0,1,tb1_font,game_state->virtual_1);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(6);
    vmwArbitraryCrossBlit(game_state->virtual_2,5,16,39,82,
			  game_state->virtual_1,146,59);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    usleep(500000);
    vmwArbitraryCrossBlit(game_state->virtual_2,46,16,39,82,
			  game_state->virtual_1,146,59);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    usleep(500000);
    vmwArbitraryCrossBlit(game_state->virtual_2,87,16,39,82,
			  game_state->virtual_1,146,59);
    vmwTextXY("Scratch.  Scratch. <Ow that itches>",1,1,9,0,1,tb1_font,game_state->virtual_1);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(5); 
    vmwTextXY("I knew you'd do it.  Good Luck!     ",1,1,15,0,1,tb1_font,game_state->virtual_1);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(6);
    vmwTextXY("<Huh?>                             ",1,1,9,0,1,tb1_font,game_state->virtual_1);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(10);

}
