#include <stdio.h>
#include <unistd.h>

#include "svmwgraph/svmwgraph.h"
#include "tb1_state.h"
#include "graphic_tools.h"
#include "hiscore.h"
#include "tblib.h"

void put_on_off(int condition,int highlighted,int x,int y,
		vmwFont *tb1_font,vmwVisual *target) {
   
    if (condition) {
       vmwTextXY("ON ",x,y,12,8-highlighted,1,tb1_font,target);
    }
    else {
       vmwTextXY("OFF",x,y,12,8-highlighted,1,tb1_font,target);
    }
}

void put_yes_no(int condition,int highlighted,int x,int y,
		vmwFont *tb1_font,vmwVisual *target) {
   
    if (condition) {
       vmwTextXY("YES",x,y,12,8-highlighted,1,tb1_font,target);
    }
    else {
       vmwTextXY("NO ",x,y,12,8-highlighted,1,tb1_font,target);
    }
}

void put_number(int number,int highlighted,int x,int y,
		vmwFont *tb1_font,vmwVisual *target) {
   
   char tempst[2];
   
   number=number%10;  /* Only works for 0->9 right now */
   number+=48;  /* ASCII */
   tempst[0]=number;
   tempst[1]='\0';
   
   vmwTextXY(tempst,x,y,12,8-highlighted,1,tb1_font,target);
}

void put_string(char *tempst,int highlighted,int x,int y,
		vmwFont *tb1_font,vmwVisual *target) {
      
   vmwTextXY(tempst,x,y,12,8-highlighted,1,tb1_font,target);
}


void options(tb1_state *game_state)
{

    int opbarpos=0,ch=0,redraw=1,not_saved=0;
     
    vmwFont *tb1_font;
    char *dir_name;
    char file_name[BUFSIZ];
    FILE *fff;
  
    tb1_font=game_state->graph_state->default_font;
   
    while(ch!=VMW_ESCAPE) {
       if (redraw) {
          vmwClearScreen(game_state->virtual_1,8);
          coolbox(0,0,319,199,0,game_state->virtual_1);
          vmwTextXY("UP,DOWN=MOVE.  RIGHT,LEFT=CHANGE.",25,170,32,0,1,tb1_font,game_state->virtual_1);
          vmwTextXY("ENTER ACTIVATES.  ESC QUITS",80,180,32,0,1,tb1_font,game_state->virtual_1);
          redraw=0;
       }
       
       vmwTextXY("SOUND OPTIONS:",10,10,9,8,1,tb1_font,game_state->virtual_1);
       vmwTextXY("SOUND FX:",40,20,10,8,1,tb1_font,game_state->virtual_1);
       put_on_off(game_state->sound_enabled,(opbarpos==0),
		  120,20,tb1_font,game_state->virtual_1);
       vmwTextXY("MUSIC   :",40,30,10,8,1,tb1_font,game_state->virtual_1);
       put_on_off(game_state->music_enabled,(opbarpos==1),
		  120,30,tb1_font,game_state->virtual_1);
       vmwTextXY("SOUND VOLUME:",40,40,10,8,1,tb1_font,game_state->virtual_1);
       put_number(game_state->sound_volume,(opbarpos==2),150,40,
		  tb1_font,game_state->virtual_1);
       vmwTextXY("MUSIC VOLUME:",40,50,10,8,1,tb1_font,game_state->virtual_1);
       put_number(game_state->music_volume,(opbarpos==3),150,50,
		  tb1_font,game_state->virtual_1);
       
       vmwTextXY("HIGH SCORE LIST:",10,60,9,8,1,tb1_font,game_state->virtual_1);
       put_string("VIEW LIST",(opbarpos==4),40,70,tb1_font,game_state->virtual_1);
       put_string("WIPE LIST",(opbarpos==5),40,80,tb1_font,game_state->virtual_1);
       
       vmwTextXY("DEFAULT DISPLAY OPTIONS:",10,90,9,8,1,tb1_font,game_state->virtual_1);
       vmwTextXY("RUN DOUBLESIZE:",40,100,10,8,1,tb1_font,game_state->virtual_1);
       put_yes_no(game_state->default_double_size,(opbarpos==6),180,100,
		  tb1_font,game_state->virtual_1);      
       vmwTextXY("RUN FULLSCREEN:",40,110,10,8,1,tb1_font,game_state->virtual_1);
       put_yes_no(game_state->default_fullscreen,(opbarpos==7),180,110,
		  tb1_font,game_state->virtual_1);
       
       put_string("SAVE TO DISK AS DEFAULT VALUES",
		  (opbarpos==8),10,130,tb1_font,game_state->virtual_1);
       
       vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
       
       while(!(ch=vmwGetInput()) ) usleep(30);
       
       if (ch==VMW_DOWN) opbarpos++;
       if (ch==VMW_UP) opbarpos--;
       if (ch==VMW_RIGHT) {
	  switch(opbarpos) {
	   case 0:  game_state->sound_enabled=!game_state->sound_enabled;
	            break;
	   case 1:  game_state->music_enabled=!game_state->music_enabled;
	            break;
	   case 2:  game_state->sound_volume++;
	            if (game_state->sound_volume>9) game_state->sound_volume=9;
	            break;
	   case 3:  game_state->music_volume++;
	            if (game_state->music_volume>9) game_state->music_volume=9;
	            break;
	   case 6:  game_state->default_double_size=!game_state->default_double_size;
	            break;
	   case 7:  game_state->default_fullscreen=!game_state->default_fullscreen;
	            break;
	     
	  }
       }
       if (ch==VMW_LEFT){
	  switch(opbarpos) {
	   case 0:  game_state->sound_enabled=!game_state->sound_enabled;
	            break;
	   case 1:  game_state->music_enabled=!game_state->music_enabled;
	            break;
	   case 2:  game_state->sound_volume--;
	            if (game_state->sound_volume<0) game_state->sound_volume=0;
	            break;
	   case 3:  game_state->music_volume--;
	            if (game_state->music_volume<0) game_state->music_volume=0;
	            break;
	   case 6:  game_state->default_double_size=!game_state->default_double_size;
	            break;
	   case 7:  game_state->default_fullscreen=!game_state->default_fullscreen;
	            break;
	     
	  }
       }
       
       if (ch==VMW_ENTER) {
	  switch(opbarpos) {
	   case 4: /* View High Score List */
	     showhiscore(game_state,1,0);
	     redraw=1;
	     break;
	   case 5: /* Wipe High Score List */
	    if (are_you_sure(game_state,"WIPE HIGH SCORES?",
				           "ARE YOU SURE?",
				           "YES!",
				           "NO!") )
	     
	        write_hs_list(game_state,0,"Vince",1);
	    redraw=1;
	    break;
	     
	   case 8: /* Save Game State */
	       /* If ~/.tb1 does not exist, and wasn't made, then exit */
	    not_saved=0;
	    if ((dir_name=check_for_tb1_directory(game_state,1))==NULL) not_saved=1;
	    
	    if (!not_saved) {
	       sprintf(file_name,"%s/options.tb1",dir_name);
	       fff=fopen(file_name,"w");
	       if (fff==NULL) {
		  not_saved=1;
	       }
	       else {
	          fprintf(fff,"%i\n%i\n%i\n%i\n%i\n%i\n",
		  game_state->sound_enabled,
		  game_state->music_enabled,
		  game_state->sound_volume,
		  game_state->music_volume,
		  game_state->default_double_size,
		  game_state->default_fullscreen);
	          fclose(fff);
	       }
	    }
	    coolbox(90,75,250,105,1,game_state->virtual_1);
	    if (not_saved) vmwTextXY("ERROR WHILE SAVING",97,82,9,7,0,
				     tb1_font,game_state->virtual_1);
	    else vmwTextXY("OPTIONS SAVED",97,82,9,7,0,tb1_font,
			    game_state->virtual_1);
            vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
	     
	    pauseawhile(15); 
	     
	    redraw=1;
	    break;
	  }
        }    
       if (opbarpos>8) opbarpos=0;
       if (opbarpos<0) opbarpos=8;
    }
}
