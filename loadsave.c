#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "svmwgraph/svmwgraph.h"
#include "tb1_state.h"
#include "graphic_tools.h"
#include "playgame.h"

int save_slots[10]={0,0,0,0,0, 0,0,0,0,0};

int list_saved_games(char *dir_name,vmwFont *tb1_font,vmwVisual *vis) {
   
    int i,num_found=0;
    char save_game_name[BUFSIZ],tempst2[BUFSIZ];
    int temp_level,temp_score,temp_shields;
    FILE *fff;
   
    vmwTextXY("SLOT#  LEVEL SHIELDS SCORE",10,80,12,0,0,tb1_font,vis);
    for(i=0;i<10;i++) {
       sprintf(save_game_name,"%s/sg%i.tb1",dir_name,i);
       if ((fff=fopen(save_game_name,"r"))!=NULL) {
	  num_found++;
	  fscanf(fff,"%i %i %i",
		 &temp_level,
		 &temp_score,
		 &temp_shields);
	  fclose(fff);	  
	  sprintf(tempst2,"%i:",i);
	  vmwTextXY(tempst2,20,90+(i*10),4,0,0,tb1_font,vis);
	  sprintf(tempst2,"%i",temp_level);
	  vmwTextXY(tempst2,85,90+(i*10),1,0,0,tb1_font,vis);
	  sprintf(tempst2,"%i",temp_shields);
	  vmwTextXY(tempst2,135,90+(i*10),1,0,0,tb1_font,vis);
	  sprintf(tempst2,"%i",temp_score);
	  vmwTextXY(tempst2,180,90+(i*10),1,0,0,tb1_font,vis);
	  save_slots[i]=1;
       } 
       else {
	  sprintf(tempst2,"%i:      ---- EMPTY ----",i);
	  vmwTextXY(tempst2,20,90+(i*10),4,0,0,tb1_font,vis);
	  save_slots[i]=0;
       }	  
    }
    return num_found;
}

void save_canceled(tb1_state *game_state) {
	  
    coolbox(0,0,319,199,1,game_state->virtual_3);
    vmwTextXY("SAVE CANCELED",90,95,12,4,0,
	      game_state->graph_state->default_font,game_state->virtual_3);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_3);
    vmwClearKeyboardBuffer();
    while(!vmwGetInput()) usleep(30);
}

int savegame(tb1_state *game_state)
{
 
    char ch,game_to_save;
    char tempst[BUFSIZ],save_game_name[BUFSIZ];
    FILE *fff;
    vmwFont *tb1_font;
    vmwVisual *vis;   
    char *dir_name;
   
   
    tb1_font=game_state->graph_state->default_font;
    vis=game_state->virtual_3;
   
       /* If ~/.tb1 does not exist, and wasn't made, then exit */
    if ((dir_name=check_for_tb1_directory(game_state,1))==NULL) return 0;
    
    coolbox(0,0,319,199,1,vis);
    vmwTextXY("SAVE GAME",110,10,1,0,0,tb1_font,vis);
    vmwTextXY("NOTE: THIS ONLY SAVES THE GAME",10,25,12,0,0,tb1_font,vis);
    vmwTextXY("AS IT WAS AT THE BEGINNING OF",10,35,12,0,0,tb1_font,vis);
    vmwTextXY("THE LEVEL!",10,45,12,0,0,tb1_font,vis);
    vmwTextXY("SAVE IN WHICH SLOT [0-9]?",10,60,4,0,0,tb1_font,vis);
    
    list_saved_games(dir_name,tb1_font,vis);   
   
    vmwBlitMemToDisplay(game_state->graph_state,vis);
    ch='M';
    vmwClearKeyboardBuffer();
    while( ((ch<'0') || (ch>'9')) && (ch!=VMW_ESCAPE)) {
       while((ch=vmwGetInput())==0) usleep(30);
    }

    if (ch==VMW_ESCAPE) {
       save_canceled(game_state);
       return 0;
    }
    game_to_save=ch;    
   
    if (save_slots[(ch-48)]) {
       coolbox(0,0,319,199,1,vis);
       sprintf(tempst,"OVERRITE EXISTING GAME %c?",ch);
       vmwTextXY(tempst,50,95,12,0,0,tb1_font,vis);  
       vmwTextXY("[Y/N]",50,105,12,0,0,tb1_font,vis);
       vmwBlitMemToDisplay(game_state->graph_state,vis);
       ch='M';
       while ( (ch!='Y') && (ch!='y') && (ch!='n') && (ch!='N') &&
	       (ch!=VMW_ESCAPE) ) {
	  while( (ch=vmwGetInput())==0) usleep(30);
       }
       if ( (ch==VMW_ESCAPE) || (ch=='n') || (ch=='N') ) {
	  save_canceled(game_state);
	  return 0;
       }
    }
       
    sprintf(save_game_name,"%s/sg%c.tb1",dir_name,game_to_save);
    if ((fff=fopen(save_game_name,"w"))!=NULL) {
       fprintf(fff,"%i\n%i\n%i\n%i\n",
	       game_state->level,
	       game_state->begin_score,
	       game_state->begin_shields,
	       game_state->checkpoints_passed);
       fclose(fff);
       coolbox(0,0,319,199,1,vis);
       sprintf(tempst,"GAME %c SAVED",game_to_save);
       vmwTextXY(tempst,90,95,12,0,0,tb1_font,vis);
    }
    else {
       coolbox(0,0,319,199,1,vis);
       vmwTextXY("ERROR SAVING FILE!",70,90,12,0,0,tb1_font,vis);
       vmwTextXY("GAME NOT SAVED!",80,100,12,0,0,tb1_font,vis);
    }
    
   
    vmwTextXY("PRESS ANY KEY...",80,180,4,0,0,tb1_font,vis);
    vmwBlitMemToDisplay(game_state->graph_state,vis);
    vmwClearKeyboardBuffer();
    while (!vmwGetInput() ) usleep(30);
    return 1;
}

void loadgame(tb1_state *game_state)
{

    char file_name[BUFSIZ];
    int num_of_save_games=0;
    FILE *fff;
    char ch, *dir_name;
    
    vmwVisual *vis;
    vmwFont   *tb1_font;
   
    vis=game_state->virtual_3;
    tb1_font=game_state->graph_state->default_font;
   
    coolbox(0,0,319,199,1,vis);
    vmwTextXY("LOAD GAME",110,10,9,0,0,tb1_font,vis);
   
    dir_name=check_for_tb1_directory(game_state,0);
    printf("%s\n",dir_name);
   
    if (dir_name!=NULL) {
       num_of_save_games=list_saved_games(dir_name,tb1_font,vis);
    }
    if ((num_of_save_games==0) || (dir_name==NULL)) {
       coolbox(0,0,319,199,1,vis);
       vmwTextXY("NO SAVED GAMES FOUND",60,50,12,0,0,tb1_font,vis);
       vmwTextXY("CREATE THEM WITH 'F2'",60,60,12,0,0,tb1_font,vis);
       vmwTextXY("DURING A GAME.",60,70,12,0,0,tb1_font,vis);
       vmwTextXY("PRESS ANY KEY...",80,180,4,0,0,tb1_font,vis);
       vmwBlitMemToDisplay(game_state->graph_state,vis);
       while( (ch=vmwGetInput())==0) usleep(30);
       return;
    }
    else {
       vmwTextXY("LOAD WHICH GAME [0-9]?",80,30,4,0,0,tb1_font,vis);
       ch='M';
       vmwBlitMemToDisplay(game_state->graph_state,vis);
       vmwClearKeyboardBuffer();
 IchLiebeMree:
       while( ((ch<'0') || (ch>'9')) && (ch!=VMW_ESCAPE)) {
	  while((ch=vmwGetInput())==0) usleep(30);
       }
       if (!(save_slots[ch-48])) goto IchLiebeMree;
       
       printf("%c\n",ch);
       
       if (ch==VMW_ESCAPE) {
	  coolbox(0,0,319,199,1,vis);
	  vmwTextXY("LOAD CANCELED",90,95,12,0,0,tb1_font,vis);
          vmwTextXY("PRESS ANY KEY...",80,180,4,0,0,tb1_font,vis);
	  vmwBlitMemToDisplay(game_state->graph_state,vis);
	  while( (ch=vmwGetInput())==0) usleep(30);
       }
       else {
          sprintf(file_name,"%s/sg%c.tb1",dir_name,ch);
	  if (( fff=fopen(file_name,"r"))!=NULL) {
	     fscanf(fff,"%d",&(game_state->level));
             fscanf(fff,"%d",&(game_state->score));
	     fscanf(fff,"%d",&(game_state->shields));
	     fscanf(fff,"%d",&(game_state->checkpoints_passed));
	     playthegame(game_state);
          }
       }
    }
}
