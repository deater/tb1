/* Routines for drawing the sidebar */

#include <stdio.h>

#include "svmwgraph/svmwgraph.h"
#include "tb1_state.h"
#include "sound.h"

#include "hiscore.h"

    /* Convenience Function */
int change_shields(tb1_state *game_state)
{

       int itemp,jtemp;
  
//       game_state->shield_color=(game_state->shields)*0x1000;
       game_state->virtual_1->palette[254]=(game_state->shields)*0x1000;
   
       vmwDrawBox(250,71,64,8,0,game_state->virtual_1);
       for(itemp=0;itemp<(game_state->shields)*4;itemp+=4)
            for(jtemp=71;jtemp<79;jtemp++) {
	       vmwDrawHLine(250+itemp,jtemp,4,(47-(itemp/4)),game_state->virtual_1);
	    }
       return 0;
}
 
    /* TSIA [well actually FNSIA (function name says it all) */
int changescore(tb1_state *game_state)
{

       char scorest[20];
       vmwVisual *vaddr;
       vmwFont *tb1_font;
   
       tb1_font=game_state->graph_state->default_font;
       vaddr=game_state->virtual_1;
   
       sprintf(scorest,"%d",game_state->score);
       vmwTextXY(scorest,317-(strlen(scorest)*9),11,
                 12,0,1,tb1_font,vaddr);
       if ( ((game_state->score % 400)==0) && (game_state->shields<16) ) {
	         if (game_state->sound_enabled) 
	            playGameFX(/*SND_ZRRP*/7);
	         game_state->shields++;
	         change_shields(game_state);
       }
       return 0;
}




void makehole(int y,vmwVisual *target)
{

  vmwDrawBox(249,y,64,9,0,target);
  vmwDrawHLine(249,y,65,24,target);
  vmwDrawVLine(249,y,9,24,target);
  vmwDrawHLine(249,y+10,64,18,target);
  vmwDrawVLine(314,y+1,9,18,target);

}

void setupsidebar(tb1_state *game_state,vmwVisual *vaddr2)
{

    int i,hiscore;
    char it[50]; 
   
    vmwFont *tb1_font;
   
    tb1_font=game_state->graph_state->default_font;
      
    vmwDrawBox(240,0,80,199,19,vaddr2);
   
    vmwDrawVLine(240,0,199,18,vaddr2);
    vmwDrawHLine(240,0,79,18,vaddr2);

    vmwDrawVLine(319,0,199,24,vaddr2);
    vmwDrawHLine(241,199,78,24,vaddr2);
     
    vmwTextXY("SCORE",241,1,127,0,0,tb1_font,vaddr2);
    vmwTextXY("SCORE",242,2,143,0,0,tb1_font,vaddr2);
    makehole(10,vaddr2);
    sprintf(it,"%d",game_state->score);
    vmwTextXY(it,250+(8*(8-strlen(it))),11,12,0,1,tb1_font,vaddr2);

    hiscore=showhiscore(game_state,0,1);
    printf("show high done\n"); fflush(stdout);
   
    vmwTextXY("HI-SCORE",241,21,127,0,0,tb1_font,vaddr2);
    vmwTextXY("HI-SCORE",242,22,143,0,0,tb1_font,vaddr2);
    sprintf(it,"%d",hiscore);
    makehole(30,vaddr2);
    vmwTextXY(it,250+(8*(8-strlen(it))),31,12,0,1,tb1_font,vaddr2);
  
    vmwTextXY("LEVEL",241,41,127,0,0,tb1_font,vaddr2);
    vmwTextXY("LEVEL",242,42,143,0,0,tb1_font,vaddr2);
    makehole(50,vaddr2);
    vmwTextXY("SHIELDS",241,61,127,0,0,tb1_font,vaddr2);
    vmwTextXY("SHIELDS",242,62,143,0,0,tb1_font,vaddr2);
    makehole(70,vaddr2);
    for(i=0;i<(4*game_state->shields);i++) {
       vmwDrawVLine(250+i,71,8,(47-(i/4)),vaddr2);
    }
    vmwTextXY("WEAPONS",241,81,127,0,0,tb1_font,vaddr2);
    vmwTextXY("WEAPONS",242,82,143,0,0,tb1_font,vaddr2);
    makehole(90,vaddr2);

    vmwDrawBox(249,111,65,78,0,vaddr2);
   
    vmwDrawVLine(249,111,78,24,vaddr2);
    vmwDrawHLine(249,111,66,24,vaddr2);
    vmwDrawVLine(315,111,78,18,vaddr2);
    vmwDrawHLine(249,189,66,18,vaddr2);
   
    vmwTextXY("  TB1   ",250,114,2,0,0,tb1_font,vaddr2);
    vmwTextXY("  TB1   ",251,115,10,0,0,tb1_font,vaddr2);
    vmwTextXY("F1-HELP  ",250,124,2,0,0,tb1_font,vaddr2);
    vmwTextXY("F1-HELP  ",251,125,10,0,0,tb1_font,vaddr2);
    vmwTextXY("F2-SAVES",250,134,2,0,0,tb1_font,vaddr2);
    vmwTextXY("F2-SAVES",251,135,10,0,0,tb1_font,vaddr2);
    vmwTextXY("ESC-QUIT",250,144,2,0,0,tb1_font,vaddr2);
    vmwTextXY("ESC-QUIT",251,145,10,0,0,tb1_font,vaddr2);
    vmwTextXY("P-PAUSES",250,154,2,0,0,tb1_font,vaddr2);
    vmwTextXY("P-PAUSES",251,155,10,0,0,tb1_font,vaddr2);
    vmwTextXY("S-SOUND ",250,164,2,0,0,tb1_font,vaddr2);
    vmwTextXY("S-SOUND ",251,165,10,0,0,tb1_font,vaddr2);

}

