
#include <stdio.h>
#include <unistd.h>

#include "svmwgraph/svmwgraph.h"
#include "tb1_state.h"
#include "tblib.h"

void credits(tb1_state *game_state)
{

    int i,keypressed=0;
   
    vmwFont *tb1_font;
   
    tb1_font=game_state->graph_state->default_font;

    vmwDrawBox(0,0,320,400,0,game_state->virtual_2);
   
    vmwTextXY("               TOM BOMBEM",0,210,4,0,1,tb1_font,game_state->virtual_2);
    vmwTextXY("    INVASION OF THE INANIMATE OBJECTS",0,220,4,0,1,tb1_font,game_state->virtual_2);
    vmwTextXY("              PROGRAMMING",0,240,9,0,1,tb1_font,game_state->virtual_2);
    vmwTextXY("            VINCENT M WEAVER",0,260,9,0,1,tb1_font,game_state->virtual_2);
    vmwTextXY("                GRAPHICS",0,290,10,0,1,tb1_font,game_state->virtual_2);
    vmwTextXY("            VINCENT M WEAVER",0,310,10,0,1,tb1_font,game_state->virtual_2);
    vmwTextXY("             SOUND EFFECTS",0,340,11,0,1,tb1_font,game_state->virtual_2);
    vmwTextXY("            VINCENT M WEAVER",0,360,11,0,1,tb1_font,game_state->virtual_2);
    vmwTextXY("          GRAPHICS INSPIRATION",0,390,12,0,1,tb1_font,game_state->virtual_2);
   
    for(i=0;i<200;i++){ 
       vmwArbitraryCrossBlit(game_state->virtual_2,0,i,320,200,
		             game_state->virtual_1,0,0);
       vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
       usleep(30000);
       if (vmwGetInput()) {
	  keypressed=1; break;
       }
    }
   
    vmwArbitraryCrossBlit(game_state->virtual_2,0,200,320,200,
			  game_state->virtual_1,0,0);
    vmwArbitraryCrossBlit(game_state->virtual_1,0,0,320,200,
			  game_state->virtual_2,0,0);
    vmwDrawBox(0,200,320,200,0,game_state->virtual_2);
   
    vmwTextXY("              JEFF WARWICK",0,210,12,0,1,tb1_font,game_state->virtual_2);
    vmwTextXY("              GENERAL HELP",0,240,13,0,1,tb1_font,game_state->virtual_2);
    vmwTextXY("              JOHN CLEMENS",0,260,13,0,1,tb1_font,game_state->virtual_2);
    vmwTextXY("              JASON GRIMM",0,280,13,0,1,tb1_font,game_state->virtual_2);
    vmwTextXY("           PCGPE AUTHORS, esp",0,310,14,0,1,tb1_font,game_state->virtual_2);
    vmwTextXY("              GRANT SMITH",0,330,14,0,1,tb1_font,game_state->virtual_2);
    vmwTextXY("         DOS SOUND BLASTER CODE",0,360,15,0,1,tb1_font,game_state->virtual_2);
    vmwTextXY("              ETHAN BRODSKY",0,380,15,0,1,tb1_font,game_state->virtual_2);
      
    if (!keypressed) for(i=0;i<200;i++){
       vmwArbitraryCrossBlit(game_state->virtual_2,0,i,320,200,
			     game_state->virtual_1,0,0);
       vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
       usleep(30000);
       if (vmwGetInput()) { 
	  keypressed=1; break; 
       }
    }
   
    vmwArbitraryCrossBlit(game_state->virtual_2,0,200,320,200,
	                  game_state->virtual_2,0,0);

    vmwDrawBox(0,200,320,200,0,game_state->virtual_2);
   
    vmwTextXY("           GLTRON SOUND CODE",0,210,12,0,1,tb1_font,game_state->virtual_2);
    vmwTextXY("             ANDREAS UMBACH",0,230,12,0,1,tb1_font,game_state->virtual_2);
    vmwTextXY("      THANKS TO ALL THE DEVELOPERS",0,260,13,0,1,tb1_font,game_state->virtual_2);
    vmwTextXY("          OF LINUX, ESPECIALLY",0,280,13,0,1,tb1_font,game_state->virtual_2);
    vmwTextXY("             LINUS TORVALDS",0,300,13,0,1,tb1_font,game_state->virtual_2);
    vmwTextXY("         ALSO SPECIAL THANKS TO",0,330,14,0,1,tb1_font,game_state->virtual_2);
    vmwTextXY("        THE SDL DEVELOPMENT TEAM",0,350,14,0,1,tb1_font,game_state->virtual_2);
    
    if (!keypressed) for(i=0;i<200;i++){
       vmwArbitraryCrossBlit(game_state->virtual_2,0,i,320,200,
                             game_state->virtual_1,0,0);
       vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
       usleep(30000);
       if (vmwGetInput()) {
	  keypressed=1; break;
       }
    }
   
    vmwArbitraryCrossBlit(game_state->virtual_2,0,200,320,200,
	                  game_state->virtual_2,0,0);
   
    vmwDrawBox(0,200,320,200,0,game_state->virtual_2); 
   
    vmwTextXY("               INSPIRATION",0,210,9,0,1,tb1_font,game_state->virtual_2);
    vmwTextXY("              DOUGLAS ADAMS",0,230,9,0,1,tb1_font,game_state->virtual_2);
    vmwTextXY("               CLIFF STOLL",0,250,9,0,1,tb1_font,game_state->virtual_2);
    vmwTextXY("             ARTHUR C CLARKE",0,270,9,0,1,tb1_font,game_state->virtual_2);
    vmwTextXY("               ISAAC ASIMOV",0,290,9,0,1,tb1_font,game_state->virtual_2);
    vmwTextXY("              GORDON KORMAN",0,310,9,0,1,tb1_font,game_state->virtual_2);
    vmwTextXY("         THANKS TO ALL THE AGENTS",0,340,10,0,1,tb1_font,game_state->virtual_2);
    vmwTextXY("        B,D,JL,L,N,P,S,W,PM,E,G,TK",0,360,10,0,1,tb1_font,game_state->virtual_2);
    vmwTextXY("           AND ESPECIALLY MP",0,380,10,0,1,tb1_font,game_state->virtual_2);
    
    if (!keypressed) for(i=0;i<200;i++){
       vmwArbitraryCrossBlit(game_state->virtual_2,0,i,320,200,
		             game_state->virtual_1,0,0);
       vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
       usleep(30000);
       if (vmwGetInput()) {
	  keypressed=1; break;
       }
   }
   vmwArbitraryCrossBlit(game_state->virtual_2,0,200,320,200,
		         game_state->virtual_2,0,0);
   
   vmwLoadPicPacked(0,200,game_state->virtual_2,1,1,
		    tb1_data_file("tbomb1.tb1",game_state->path_to_data));
  
   if (keypressed) {
      vmwDrawBox(0,0,320,200,0,game_state->virtual_2);
   }
   
   for(i=0;i<200;i++){
      vmwArbitraryCrossBlit(game_state->virtual_2,0,i,320,200,
			    game_state->virtual_1,0,0);
      vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
      usleep(30000);
      if (vmwGetInput()) {
	 break;
      }
  }
  vmwArbitraryCrossBlit(game_state->virtual_2,0,200,320,200,
                        game_state->virtual_1,0,0);

  vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
   
}
