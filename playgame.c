#include <stdio.h>

#include "svmwgraph/svmwgraph.h"
#include "tb1_state.h"

#include "tblib.h"
#include "menu_tools.h"

#include "levels.h"

void playthegame(struct tb1_state *game_state)
{
   
    if (game_state->level==0) {
       littleopener(game_state);
       game_state->shields=12;
       game_state->score=0;
       game_state->level++;
    }
   
    if (game_state->level==1) {
       game_state->level=2;
       leveltwoengine(game_state);
//       levelone(game_state);
//       if(level==2) littleopener2();
    }
    if (game_state->level==2) {
//       leveltwoengine(&level,&shields,&score);
    }
    if (game_state->level==3) {
       /*littleopener3();
       levelthree();*/
    }
    if (game_state->level==4) {
//       leveltwoengine(&level,&shields,&score);
    }
    coolbox(70,85,170,110,1,game_state->virtual_1);
    vmwTextXY("GAME OVER",84,95,4,7,0,
	      game_state->graph_state->default_font,game_state->virtual_1);
    game_state->level=0;
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    vmwClearKeyboardBuffer(); 
    pauseawhile(20);
   
 

 /* hiscore:=showhiscore(false,false);
  if score>lowscore then begin
     coolbox(10,75,310,125,true,vga);
     outtextxy('NEW HIGH SCORE!',90,80,12,7,vga,false);
     outtextxy('ENTER YOUR NAME (10 Chars)',30,90,0,7,vga,false);
     hiname:=grinput(110,110,10,11,7);
     hiscore:=showhiscore(false,true);
  end;
  hiscore:=showhiscore(true,false);
  fade;
  cls(0,vga);
*/

}

