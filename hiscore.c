#include <stdio.h>

#include "svmwgraph/svmwgraph.h"
#include "tb1_state.h"
#include "menu_tools.h"


#include <unistd.h>  /* For usleep */

int showhiscore(struct tb1_state *game_state,int showchart)
{
   
    FILE *hilist;
    char tempstr[10];
    int i;
    char names[10][25];
    int scores[10];

    hilist=fopen("hiscore.tb1","r+");
    if (hilist==NULL) {
       printf("Error! can't open high score file!\n");
       return 0;
    }
    
    for (i=0;i<10;i++) fscanf(hilist,"%s",names[i]);
    for (i=0;i<10;i++) fscanf(hilist,"%i",&scores[i]);
    fclose(hilist);
    if (!showchart) return scores[0];
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

void write_hs_list(int score,char *hiname)
{
#if 0
   int i,place;
    FILE *hilist;
    char names[10][25];
    int scores[10];
   
    hilist=fopen("hiscore.tb1","r+");
    for (i=0;i<10;i++) fscanf(hilist,"%s",names[i]);
    for (i=0;i<10;i++) fscanf(hilist,"%i",&scores[i]);
    fclose(hilist);
   
    place=0;
    for(i=0;i<9;i++) 
       if (score>scores[i]) place++;
    place=10-place;
    for(i=9;i>place;i--) 
       scores[i]=scores[i-1];
    for(i=9;i>place;i--) 
       strcpy(names[i-1],names[i]);
    scores[place]=score;
    strcpy(hiname,names[place]);
    if (!read_only_mode) {
       hilist=fopen("hiscore.tb1","w");
       for(i=0;i<9;i++) fprintf(hilist,"%s\n",names[i]);
       for(i=0;i<9;i++) fprintf(hilist,"%i\n",scores[i]);
       fclose(hilist);
    }
#endif
}

