#include <stdio.h>
#include <unistd.h>
#include "svmwgraph/svmwgraph.h"
#include "tb1_state.h"
#include "menu_tools.h"

void help(struct tb1_state *game_state)
{
    vmwVisual *vis;
    vmwFont *tb1_font;
   
    vis=game_state->virtual_1;
    tb1_font=game_state->graph_state->default_font;
   
    coolbox(0,0,319,199,1,vis);
    vmwTextXY("HELP",144,10,4,0,0,tb1_font,vis);
    vmwTextXY("--------------------------------------",10,20,
	                           12,0,0,tb1_font,vis);
    vmwTextXY("IN THE GAME:",10,30,4,0,0,tb1_font,vis);
    vmwTextXY("   ARROWS MANUEVER",10,40,
	                           4,0,0,tb1_font,vis);
    vmwTextXY("   SPACE BAR FIRES MISSILES",10,50,
	                           4,0,0,tb1_font,vis);
    vmwTextXY("   F2 SAVES GAME",10,60,
	                           4,0,0,tb1_font,vis);
    vmwTextXY("   P=PAUSE  S=TOGGLE SOUND  ESC=QUIT",10,70,
	                           4,0,0,tb1_font,vis);
    vmwTextXY("--------------------------------------",10,80,
	                           12,0,0,tb1_font,vis);
    vmwTextXY("MISSION: SHOOT THE INANIMATE OBJECTS",10,90,
	                           4,0,0,tb1_font,vis);
    vmwTextXY("         WHY?  WATCH THE STORY!",10,100,
	                           4,0,0,tb1_font,vis);
    vmwTextXY("--------------------------------------",10,110,
	                           12,0,0,tb1_font,vis);
    vmwTextXY("THE SPACE BAR SPEEDS UP MOVIE SCENES",10,120,
	                           4,0,0,tb1_font,vis);
    vmwTextXY("ESC QUITS THEM",10,130,
	                           4,0,0,tb1_font,vis);
    vmwTextXY("--------------------------------------",10,140,
	                           12,0,0,tb1_font,vis);
    vmwTextXY("   SEE \"README\" FOR MORE HELP/INFO",10,150,
	                           4,0,0,tb1_font,vis);
    vmwTextXY("--------------------------------------",10,160,
	                           12,0,0,tb1_font,vis);
    vmwTextXY("PRESS ANY KEY TO CONTINUE",64,185,
	                           4,0,0,tb1_font,vis);
    vmwBlitMemToDisplay(game_state->graph_state,vis);
    while(!vmwGetInput()) usleep(30);
}
