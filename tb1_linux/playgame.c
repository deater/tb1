#include <stdio.h>

#include "svmwgraph/svmwgraph.h"
#include "tb1_state.h"

#include "tblib.h"
#include "graphic_tools.h"

#include "levels.h"
#include "level2_engine.h"
#include "hiscore.h"

void playthegame(tb1_state *game_state)
{
    int lowscore;
    char *hiname;
   
    if (game_state->level==0) {
       LevelOneLittleOpener(game_state);
       game_state->shields=12;
       game_state->score=0;
       game_state->level++;
    }
   
    if (game_state->level==1) {
       LevelOneEngine(game_state);
       if (game_state->level==2) littleopener2(game_state);
    }
    if (game_state->level==2) {
       leveltwoengine(game_state,"level1/ships.tb1","level2/level2.dat",
		      "level2/tbaship.tb1","   LEVEL TWO:",
		      "THE \"PEACE ENVOY\"",NULL);
       if (game_state->level==3) LevelThreeLittleOpener(game_state);
    }
    if (game_state->level==3) {
       LevelThreeEngine(game_state);
	  
    }
    if (game_state->level==4) {
       leveltwoengine(game_state,"level1/ships.tb1","level4/level4.dat",
		      "level4/tbeerm.tb1","   LEVEL FOUR:"," THE PLANET EERM",
		      NULL);
    }
    coolbox(70,85,170,110,1,game_state->virtual_1);
    vmwTextXY("GAME OVER",84,95,4,7,0,
	      game_state->graph_state->default_font,game_state->virtual_1);
    game_state->level=0;
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    vmwClearKeyboardBuffer(); 
    pauseawhile(10);

    lowscore=showhiscore(game_state,0,0);
    if (game_state->score >lowscore) {
       coolbox(10,75,310,125,1,game_state->virtual_1);
       vmwTextXY("NEW HIGH SCORE!",90,80,12,7,0,game_state->graph_state->default_font,
		 game_state->virtual_1);
       vmwTextXY("ENTER YOUR NAME (10 Chars)",30,90,0,7,0,
		 game_state->graph_state->default_font,game_state->virtual_1);
       vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
       hiname=vmwGrInput(game_state,110,110,10,11,0,game_state->graph_state->default_font,
			 game_state->virtual_1);
       write_hs_list(game_state,game_state->score,hiname,0);
    }
    showhiscore(game_state,1,0);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
       
    vmwClearKeyboardBuffer();
    pauseawhile(20);
   
}
