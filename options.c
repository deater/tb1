#include <stdio.h>
#include <unistd.h>

#include "svmwgraph/svmwgraph.h"
#include "tb1_state.h"
#include "menu_tools.h"
#include "hiscore.h"

void options(struct tb1_state *game_state)
{

    int opbarpos,argh=0,ch=0;
    vmwFont *tb1_font;
   
    tb1_font=game_state->graph_state->default_font;
   
    vmwClearScreen(game_state->virtual_1,8);
    coolbox(0,0,319,199,0,game_state->virtual_1);
    vmwTextXY("ESC QUITS",120,175,32,0,1,tb1_font,game_state->virtual_1);
    opbarpos=0;
    while(ch!=VMW_ESCAPE) {
       if (game_state->sound_enabled) {
          if (opbarpos==0) vmwTextXY("SOUND ON ",30,30,32,
				     7,1,tb1_font,game_state->virtual_1);
          else vmwTextXY("SOUND ON ",30,30,32,
			             0,1,tb1_font,game_state->virtual_1);
       }
       else {
          if (opbarpos==0) vmwTextXY("NO SOUND ",30,30,32,
				     7,1,tb1_font,game_state->virtual_1);
          else vmwTextXY("NO SOUND ",30,30,32,
			             0,1,tb1_font,game_state->virtual_1);
       }
       if (opbarpos==1) vmwTextXY("VIEW HIGH SCORES",30,40,32,
				     7,1,tb1_font,game_state->virtual_1);
       else vmwTextXY("VIEW HIGH SCORES",30,40,32,
		                     0,1,tb1_font,game_state->virtual_1);
       
       vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
       
       while(!(ch=vmwGetInput()) ) usleep(30);
       
       if ((ch==VMW_RIGHT) || (ch==VMW_DOWN)) opbarpos++;
       if ((ch==VMW_LEFT) || (ch==VMW_UP)) opbarpos--;
       if ((ch=='M')||(ch=='m')) opbarpos=0;
       if ((ch=='V')||(ch=='v')) opbarpos=1;
       if ((ch==VMW_ENTER) && (opbarpos==0)) game_state->sound_enabled=!game_state->sound_enabled;
       if ((ch==VMW_ENTER) && (opbarpos==1)){
          ch=VMW_ESCAPE;
          argh=4;
       }    
       if (opbarpos==2) opbarpos=0;
       if (opbarpos==-1) opbarpos=1;
    }
    if (argh==4) showhiscore(game_state,1);
}
