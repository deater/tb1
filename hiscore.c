#include <stdio.h>

#include "svmwgraph/svmwgraph.h"
#include "tb1_state.h"
#include "graphic_tools.h"
#include "tblib.h"

#include <unistd.h>  /* For usleep */
#include <string.h>  /* For strncpy */

/* Yes these routines are simplistic, but they seemed amazingly */
/* clever when I came up with them 6 years ago. */

int showhiscore(tb1_state *game_state,int showchart,int return_highest)
{
   
    FILE *hilist;
    char tempstr[10];
    int i;
    char names[10][25];
    int scores[10];

    hilist=fopen(tb1_data_file("hiscore.tb1",game_state->path_to_data),"r");
    if (hilist==NULL) {
       printf("Error! can't open high score file!\n");
       return 0;
    }
    
    for (i=0;i<10;i++) fscanf(hilist,"%s",names[i]);
    for (i=0;i<10;i++) fscanf(hilist,"%i",&scores[i]);
    fclose(hilist);
    if (!showchart) {
       if (return_highest) return scores[0];
       else return scores[9];
    }
    if (showchart) {
       vmwDrawBox(45,40,240,120,7,game_state->virtual_1);
       coolbox(45,40,285,160,1,game_state->virtual_1);
       for(i=0;i<10;i++) 
          vmwTextXY(names[i],51,46+(i*10),9,7,1,game_state->graph_state->default_font,game_state->virtual_1);
       for(i=0;i<10;i++){
         sprintf(tempstr,"%i",scores[i]);
         vmwTextXY(tempstr,181,46+(i*10),9,7,1,game_state->graph_state->default_font,game_state->virtual_1);
       }
       vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
       
       while(vmwGetInput()==0) usleep(100);
    }
   return 0;
}

typedef struct {
   char name[12];
   int score;
} vmwHighScore;

void write_hs_list(tb1_state *game_state,int score,char *hiname,int wipe)
{

    int i,place,in_place;
    FILE *hilist;
    vmwHighScore old_list[10];
    vmwHighScore new_list[10];
   
    hilist=fopen(tb1_data_file("hiscore.tb1",game_state->path_to_data),"r");
    for (i=0;i<10;i++) fscanf(hilist,"%s",old_list[i].name);
    for (i=0;i<10;i++) fscanf(hilist,"%i",&old_list[i].score);
    fclose(hilist);
   
    place=0;
    i=0;
    in_place=0;
    while (i<10) {
       if ((score>old_list[place].score) && (in_place==0)) {
	  strncpy(new_list[i].name,hiname,10);
	  new_list[i].score=score;
	  i++;
	  in_place=1;
       }
       if (i<10) {
          strncpy(new_list[i].name,old_list[place].name,10);
          new_list[i].score=old_list[place].score;
       }
       place++;
       i++;
    }
	  
//    if (!read_only_mode) {
       if ((hilist=fopen(tb1_data_file("hiscore.tb1",
				       game_state->path_to_data),"w"))==NULL) {
	  printf("Error opening high score file!\n");
	  return;
       }
   
       if (wipe) {
	  fprintf(hilist,"Vince\nChipper\nMarie\nHairold\nKevin\n"
		         "Leonard\nLizann\nPete\nJim\nBrigid\n"
		         "5000\n4500\n4000\n3500\n3000\n"
		         "2500\n2000\n1500\n1000\n500\n");
       }
       else {
          for(i=0;i<10;i++) fprintf(hilist,"%s\n",new_list[i].name);
          for(i=0;i<10;i++) fprintf(hilist,"%i\n",new_list[i].score);
       }
       fclose(hilist);
//    }
}

