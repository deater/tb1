#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <SDL/SDL.h>

#include "sdl_svmwgraph.h"
#include "tb1_state.h"
#include "levels.h"
#include "tb_keypress.h"
#include "string.h"
#include "sound.h"

    /* Convenience Function */
int change_shields(struct tb1_state *game_state)
{

       int itemp,jtemp;
  
       game_state->shield_color=(game_state->shields)*0x1000;
   
       vmwDrawBox(250,71,64,8,0,game_state->virtual_1);
       for(itemp=0;itemp<(game_state->shields)*4;itemp+=4)
            for(jtemp=71;jtemp<79;jtemp++) {
	       vmwDrawHLine(250+itemp,jtemp,4,(47-(itemp/4)),game_state->virtual_1);
	    }
       return 0;
}
 
    /* TSIA [well actually FNSIA (function name says it all) */
int changescore(struct tb1_state *game_state)
{

       char scorest[20];
       unsigned char *vaddr;
       vmw_font *tb1_font;
   
       tb1_font=game_state->tb1_font;
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

    /* The collision detection routine.  Optimize? */
    /* Detects if x1,y1 is within x2,y2            */
int collision(x1,y1,xsize,ysize,x2,y2,x2size,y2size)
{
       if (abs((y1+ysize)-(y2+y2size))<(ysize+y2size)){
	         if (abs((x1+xsize)-(x2+x2size))<(xsize+x2size)) return 1;
       }
       return 0;
}

char *tb1_data_file(char *name,char *path)
{
   
    char tempst[BUFSIZ];
   
    sprintf(tempst,"%s/%s",path,name);
    return strdup(tempst);
}

void coolbox(int x1,int y1,int x2,int y2,int fill,unsigned char *target)
{
    int i;
   
    for(i=0;i<5;i++) {
       vmwDrawHLine(x1+i,y1+i,(x2-x1-i-i),31-i,target);
       vmwDrawHLine(x1+i,y2-i,(x2-x1-i-i),31-i,target);
       vmwDrawVLine(x1+i,y1+i,(y2-y1-i-i),31-i,target);
       vmwDrawVLine(x2-i,y1+i,(y2-y1-i-i),31-i,target);
    }
    if (fill) {
       for(i=y1+5;i<y2-4;i++) vmwDrawHLine(x1+5,i,(x2-x1-9),7,target);
     }  
}



int close_graphics()
{
#if 0
    int err=0;
   
    err=ggiClose(vis);
    err+=ggiClose(vaddr);
    err+=ggiClose(vaddr2);
    if(err) fprintf(stderr,"Probelms shutting down GGI!\n");
    else fprintf(stderr,"Shutting down GGI...\n");
    ggiExit();
#endif
    return 0;

}


int quit(struct tb1_state *game_state)
{

    int barpos=0,ch=0;
    vmw_font *tb1_font;
    unsigned char *target;
    SDL_Surface *sdl_screen;
   
    tb1_font=game_state->tb1_font;
    target=game_state->virtual_1;
    sdl_screen=game_state->sdl_screen;
   
    coolbox(90,75,230,125,1,target);
    vmwTextXY("QUIT??? ARE YOU",97,82,9,7,0,tb1_font,target);
    vmwTextXY("ABSOLUTELY SURE?",97,90,9,7,0,tb1_font,target);
   
    while (ch!=TB_ENTER){
       if (barpos==0) vmwTextXY("YES-RIGHT NOW!",97,98,150,0,1,tb1_font,target);
       else vmwTextXY("YES-RIGHT NOW!",97,98,150,7,1,tb1_font,target);
       if (barpos==1) vmwTextXY("NO--NOT YET.",97,106,150,0,1,tb1_font,target);
       else vmwTextXY("NO--NOT YET.",97,106,150,7,1,tb1_font,target);
       vmwBlitMemToSDL(sdl_screen,target);
       
       while ( !(ch=vmwGetInput()) ) {
	  usleep(30);
       }
       if ((ch==TB_UP)||(ch==TB_DOWN)||(ch==TB_LEFT)||(ch==TB_RIGHT)) barpos++;
       if (ch=='y') barpos=0;
       if (ch=='n') barpos=1;
       if (barpos==2) barpos=0;
    }
    if (barpos==0){ 
       shutdownSound();
       close_graphics();
       exit(1);
    }
    else return 6;

}


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
          vmwTextXY(names[i],51,46+(i*10),9,7,1,game_state->tb1_font,game_state->virtual_1);
       for(i=0;i<10;i++){
         sprintf(tempstr,"%i",scores[i]);
         vmwTextXY(tempstr,181,46+(i*10),9,7,1,game_state->tb1_font,game_state->virtual_1);
       }
       vmwBlitMemToSDL(game_state->sdl_screen,game_state->virtual_1);        
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



void help(struct tb1_state *game_state)
{
    unsigned char *vis;
    vmw_font *tb1_font;
   
    vis=game_state->virtual_1;
    tb1_font=game_state->tb1_font;
   
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
    vmwBlitMemToSDL(game_state->sdl_screen,vis);
    while(!vmwGetInput()) usleep(30);
}


void makehole(int y,unsigned char *target)
{

  vmwDrawBox(249,y,64,9,0,target);
  vmwDrawHLine(249,y,65,24,target);
  vmwDrawVLine(249,y,9,24,target);
  vmwDrawHLine(249,y+10,64,18,target);
  vmwDrawVLine(314,y+1,9,18,target);

}

void setupsidebar(struct tb1_state *game_state)
{

    int i,hiscore;
    char it[50]; 
   
    unsigned char *vaddr2;
    vmw_font *tb1_font;
   
    tb1_font=game_state->tb1_font;
    vaddr2=game_state->virtual_2;
   
    vmwDrawBox(240,0,80,199,19,game_state->virtual_2);
   
    vmwDrawVLine(240,0,199,18,game_state->virtual_2);
    vmwDrawHLine(240,0,79,18,game_state->virtual_2);

    vmwDrawVLine(319,0,199,24,game_state->virtual_2);
    vmwDrawHLine(241,199,78,24,game_state->virtual_2);
     
    printf("1\n"); fflush(stdout);
   
    vmwTextXY("SCORE",241,1,127,0,0,tb1_font,vaddr2);
    vmwTextXY("SCORE",242,2,143,0,0,tb1_font,vaddr2);
    makehole(10,vaddr2);
    sprintf(it,"%d",game_state->score);
    vmwTextXY(it,250+(8*(8-strlen(it))),11,12,0,1,tb1_font,vaddr2);

    printf("2\n"); fflush(stdout);
   
   
    hiscore=showhiscore(game_state,0);
    printf("show high done\n"); fflush(stdout);
   
    vmwTextXY("HI-SCORE",241,21,127,0,0,tb1_font,vaddr2);
    vmwTextXY("HI-SCORE",242,22,143,0,0,tb1_font,vaddr2);
    sprintf(it,"%d",hiscore);
    makehole(30,vaddr2);
    vmwTextXY(it,250+(8*(8-strlen(it))),31,12,0,1,tb1_font,vaddr2);
  
    printf("3\n"); fflush(stdout);
   
   
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

    printf("4\n"); fflush(stdout);
   
   
    vmwDrawBox(249,111,65,78,0,vaddr2);
   
    vmwDrawVLine(249,111,78,24,vaddr2);
    vmwDrawHLine(249,111,66,24,vaddr2);
    vmwDrawVLine(315,111,78,18,vaddr2);
    vmwDrawHLine(249,189,66,18,vaddr2);
   
    printf("5\n"); fflush(stdout);
   
   
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




void savegame(int level,int begin_score,int begin_shields)
{
   
#if 0
    char ch;
    struct stat buf;
    char tempst[300],save_game_name[320],tempst2[5];
    FILE *fff;
    int dot_tb1_exists=0;
    
    int i,j;
   
    coolbox(0,0,319,199,1,vis);
    vmwTextXY("SAVE GAME",110,10,9],0],0,tb1_font,vis);
    if (read_only_mode) {
       vmwTextXY("SORRY!  CANNOT SAVE GAME",40,40,4],0],0,tb1_font,vis);
       vmwTextXY("WHEN IN READ ONLY MODE",50,50,4],0],0,tb1_font,vis);
    }
    else {
       sprintf(tempst,"%s/.tb1",getenv("HOME"));
       stat(tempst,&buf);
       if (!S_ISDIR(buf.st_mode)) {
	  vmwTextXY("DIRECTORY:",20,20,4],0],0,tb1_font,vis);
	  if (strlen(tempst)>18)
	     vmwTextXY("$HOME/.tb1",120,20,4],0],0,tb1_font,vis);
	  else 
	     vmwTextXY(tempst,120,20,4],0],0,tb1_font,vis);
	  vmwTextXY("WHERE TB1 STORES SAVED GAMES",20,30,4],0],0,tb1_font,vis);
	  vmwTextXY("DOES NOT EXIST.  DO YOU WANT",20,40,4],0],0,tb1_font,vis);
	  vmwTextXY("TO CREATE THIS DIRECTORY?",20,50,4],0],0,tb1_font,vis); 
	  vmwTextXY("   PLEASE ANSWER [Y or N]",20,60,4],0],0,tb1_font,vis);
	  vmwTextXY("NOTE IF YOU ANSWER \"N\" YOU",20,80,12],0],0,tb1_font,vis);
	  vmwTextXY("WILL NOT BE ABLE TO SAVE",20,90,12],0],0,tb1_font,vis);
	  vmwTextXY("GAMES",20,100,12],0],0,tb1_font,vis);
	  ch='M';
	  clear_keyboard_buffer();
	  while( (ch!='Y') && (ch!='y') && (ch!='N') && (ch!='n')) {
	     while((ch=get_input())==0) usleep(30);
	  }
	  if (toupper(ch)=='Y')
	     if (!mkdir(tempst,744)) dot_tb1_exists=1;
	     else {
		coolbox(0,0,319,199,1,vis);
		vmwTextXY("WARNING! COULD NOT CREATE",30,30,4],0],0,tb1_font,vis);
		vmwTextXY("DIRECTORY!  ABANDONING SAVE!",20,40,4],0],0,tb1_font,vis);	
	     }
	     
       }
       else dot_tb1_exists=1;
    }
    if (dot_tb1_exists) {
       coolbox(0,0,319,199,1,vis);
       vmwTextXY("SAVE GAME",110,20,9],0],0,tb1_font,vis);
       vmwTextXY("NOTE: THIS ONLY SAVES THE GAME",10,40,4],0],0,tb1_font,vis);
       vmwTextXY("AT THE BEGINNING OF THE LEVEL!",10,50,4],0],0,tb1_font,vis);
       vmwTextXY("ALREADY EXISTING GAMES",10,70,4],0],0,tb1_font,vis);
       j=0;
       for(i=0;i<10;i++) {
	  sprintf(save_game_name,"%s/sg%i.tb1",tempst,i);
	  if ((fff=fopen(save_game_name,"r"))!=NULL) {
	     sprintf(tempst2,"%i",i);
	     vmwTextXY(tempst2,50+j*20,80,12],0],0,tb1_font,vis);
	     fclose(fff);  
	     j++;
	  }
       }	  
       if (j==0) vmwTextXY("NONE",140,90,12],0],0,tb1_font,vis);
       vmwTextXY("PRESS NUMBER OF GAME TO SAVE",20,110,4],0],0,tb1_font,vis);
       vmwTextXY("(0-9) [ESC CANCELS]",60,120,4],0],0,tb1_font,vis);
       ch='M';
       clear_keyboard_buffer();
       while( ((ch<'0') || (ch>='9')) && (ch!=TB_ESC)) {
	  while((ch=get_input())==0) usleep(30);
       }
       if (ch==TB_ESC) {
	  coolbox(0,0,320,200,1,vis);
	  vmwTextXY("SAVE CANCELED",90,95,12],4],0,tb1_font,vis);
       }
       else {
	  sprintf(save_game_name,"%s/sg%c.tb1",tempst,ch);
	  if ((fff=fopen(save_game_name,"w"))!=NULL) {
	     fprintf(fff,"%i\n%i\n%i\n",level,begin_score,begin_shields);
	     fclose(fff);
	     coolbox(0,0,320,200,1,vis);
	     sprintf(tempst,"GAME %c SAVED",ch);
	     vmwTextXY(tempst,90,95,12],0],0,tb1_font,vis);
	  }
	  else {
	     coolbox(0,0,320,200,1,vis);
	     vmwTextXY("ERROR SAVING FILE!",70,90,12],0],0,tb1_font,vis);
             vmwTextXY("GAME NOT SAVED!",80,100,12],0],0,tb1_font,vis);
	  }
       }
    }
    vmwTextXY("PRESS ANY KEY...",80,180,4],0],0,tb1_font,vis);
    while( (ch=get_input())==0) usleep(30);
#endif
}

void shadowrite(char *st,int x5,int y5,int forecol,int backcol,
		vmw_font *tb1_font,unsigned char *target)
{
    vmwTextXY(st,x5+1,y5+1,backcol,0,0,tb1_font,target);
    vmwTextXY(st,x5,y5,forecol,0,0,tb1_font,target);
}

void options(struct tb1_state *game_state)
{

    int opbarpos,argh=0,ch=0;
    vmw_font *tb1_font;
   
    tb1_font=game_state->tb1_font;
   
    vmwClearScreen(game_state->virtual_1,8);
    coolbox(0,0,319,199,0,game_state->virtual_1);
    vmwTextXY("ESC QUITS",120,175,32,0,1,tb1_font,game_state->virtual_1);
    opbarpos=0;
    while(ch!=TB_ESCAPE) {
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
       
       vmwBlitMemToSDL(game_state->sdl_screen,game_state->virtual_1);
       
       while(!(ch=vmwGetInput()) ) usleep(30);
       
       if ((ch==TB_RIGHT) || (ch==TB_DOWN)) opbarpos++;
       if ((ch==TB_LEFT) || (ch==TB_UP)) opbarpos--;
       if ((ch=='M')||(ch=='m')) opbarpos=0;
       if ((ch=='V')||(ch=='v')) opbarpos=1;
       if ((ch==TB_ENTER) && (opbarpos==0)) game_state->sound_enabled=!game_state->sound_enabled;
       if ((ch==TB_ENTER) && (opbarpos==1)){
          ch=TB_ESCAPE;
          argh=4;
       }    
       if (opbarpos==2) opbarpos=0;
       if (opbarpos==-1) opbarpos=1;
    }
    if (argh==4) showhiscore(game_state,1);
}

void loadgame()
{
#if 0
    char tempst[300],tempst2[5],file_name[320];
    int i,j;
    FILE *fff;
    int game_exist[10]={0,0,0,0,0, 0,0,0,0,0};
    char ch;
    int level,score,shields;
     
    coolbox(0,0,320,200,1,vis);
    vmwTextXY("LOAD GAME",110,10,9],0],0,tb1_font,vis);
   
    sprintf(tempst,"%s/.tb1",getenv("HOME"));
     
    j=0;
    for(i=0;i<10;i++) {
       sprintf(file_name,"%s/sg%i.tb1",tempst,i);
       if ((fff=fopen(file_name,"r"))!=NULL) {
	  sprintf(tempst2,"%i",i);
	  vmwTextXY(tempst2,50+j*20,50,12],0],0,tb1_font,vis);
	  fclose(fff);
	  game_exist[i]=1;
	  j++;
       }
    }
    if (j==0) {
       vmwTextXY("NO SAVED GAMES FOUND",60,50,12],0],0,tb1_font,vis); 
       vmwTextXY("PRESS ANY KEY...",80,180,4],0],0,tb1_font,vis);
       while( (ch=get_input())==0) usleep(30);
    }
   else {
      vmwTextXY("LOAD WHICH GAME?",80,30,4],0],0,tb1_font,vis);
      vmwTextXY("THE FOLLOWING EXIST:",40,40,4],0],0,tb1_font,vis);
      vmwTextXY("PRESS A NUMBER. (ESC CANCELS)",20,60,4],0],0,tb1_font,vis); 
      ch='M';
      clear_keyboard_buffer();
 IchLiebeMree:
      while( ((ch<'0') || (ch>'9')) && (ch!=TB_ESC)) {
	  while((ch=get_input())==0) usleep(30);
      }
      if (!(game_exist[ch-48])) goto IchLiebeMree;
      if(ch==TB_ESC) {
	 coolbox(0,0,320,200,1,vis);
	 vmwTextXY("LOAD CANCELED",90,95,12],0],0,tb1_font,vis);
         vmwTextXY("PRESS ANY KEY...",80,180,4],0],0,tb1_font,vis);
	 while( (ch=get_input())==0) usleep(30);
      }
      else {
       sprintf(file_name,"%s/sg%c.tb1",tempst,ch);
       if (( fff=fopen(file_name,"r"))!=NULL) {
	  fscanf(fff,"%d",&level);
          fscanf(fff,"%d",&score);
	  fscanf(fff,"%d",&shields);
	  playthegame(level,score,shields);
       }
      }
    }
#endif
}

void story(struct tb1_state *game_state)
{
    int error;
    int xtemp,ytemp;
    int thrustcol;
    float thrust;
    char tempch;
    int alienchar;
    int cycles;
   
    vmw_font *tb1_font;
   
    tb1_font=game_state->tb1_font;

/*
procedure doflames;
begin
   
   if flames=true then begin
      putshape(bigflame1off,vaddr,27,17,213,100);
      putshapeover(sflame2off,vaddr,4,4,105,90);
      putshapeover(sflame1off,vaddr,4,4,151,71);
      putshapeover(sflame2off,vaddr,4,4,218,72);
    end
    else
       begin
       putshape(bigflame2off,vaddr,27,17,213,100);
       putshapeover(sflame1off,vaddr,4,4,105,90);
       putshapeover(sflame2off,vaddr,4,4,151,71);
       putshapeover(sflame1off,vaddr,4,4,218,72);
       outtextxy(chr(alienchar),10,10,12,0,vaddr,true);
       inc(alienchar);
       if alienchar>44 then alienchar:=34;
    end;
    flipd320(vaddr,vga);
    vdelay(5);
    flames:=not(flames);
end;

procedure puttruck(xp,yp,frame:integer;where:word);
var xtemp,into,ytemp:integer;
    col:byte;
begin
  for into:=0 to 7 do begin
     if(into+yp>=172) then
      if frame=0 then putshapeline(truck1off,where,6,8,xp,yp,into)
                 else putshapeline(truck2off,where,6,8,xp,yp,into);
  end;
end;
*/

    vmwLoadPicPacked(0,0,game_state->virtual_2,1,1,
		     tb1_data_file("tbsobj.tb1",game_state->path_to_data));
  /*
   for ytemp:=0 to 18 do
      for xtemp:=0 to 26 do begin
          ShapeTable1^[((ytemp*27)+xtemp)+bigflame1off]:=getpixel(xtemp,ytemp+2,vaddr);
          ShapeTable1^[((ytemp*27)+xtemp)+bigflame2off]:=getpixel(xtemp,ytemp+21,vaddr);
      end;
  for ytemp:=0 to 18 do
      for xtemp:=0 to 15 do begin
          ShapeTable1^[((ytemp*16)+xtemp)+explo1off]:=getpixel(127+xtemp,ytemp+100,vaddr);
          ShapeTable1^[((ytemp*16)+xtemp)+explo2off]:=getpixel(148+xtemp,ytemp+100,vaddr);
  end;
  for ytemp:=0 to 6 do
      for xtemp:=0 to 15 do
          ShapeTable1^[((ytemp*16)+xtemp)+rent1off]:=getpixel(168+xtemp,ytemp+100,vaddr);

  for ytemp:=0 to 4 do
      for xtemp:=0 to 3 do begin
          ShapeTable1^[((ytemp*4)+xtemp)+sflame1off]:=getpixel(xtemp,ytemp+43,vaddr);
          ShapeTable1^[((ytemp*4)+xtemp)+sflame2off]:=getpixel(xtemp,ytemp+47,vaddr);
      end;

  for ytemp:=0 to 18 do
      for xtemp:=0 to 15 do
          ShapeTable1^[((ytemp*16)+xtemp)+bargeoff]:=getpixel(xtemp+65,ytemp+100,vaddr);

  for ytemp:=0 to 8 do
      for xtemp:=0 to 5 do begin
          ShapeTable1^[((ytemp*6)+xtemp)+truck1off]:=getpixel(xtemp+85,ytemp+100,vaddr);
          ShapeTable1^[((ytemp*6)+xtemp)+truck2off]:=getpixel(xtemp+95,ytemp+100,vaddr);
      end;
*/
   
       /******FIRST MESSAGE********/
    vmwClearScreen(game_state->virtual_1,0);
    vmwTextXY("THE STORY SO FAR...",20,20,4,0,0,tb1_font,game_state->virtual_1);
    vmwBlitMemToSDL(game_state->sdl_screen,game_state->virtual_1);
    pauseawhile(7);

    vmwClearScreen(game_state->virtual_1,0);
   
    vmwLoadPicPacked(0,0,game_state->virtual_2,1,1,
		     tb1_data_file("tbcobj.tb1",game_state->path_to_data));
		     
    vmwArbitraryCrossBlit(game_state->virtual_2,129,56,49,132,
			  game_state->virtual_1,10,10);
    vmwTextXY("YOU ARE TOM BOMBEM,  A STRANGE",80,10,1,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("    BUT EFFICIENT MEMBER OF",80,20,1,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("    THE LUNAR SPACE FORCE.",80,30,1,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("YOU NEVER SAY MUCH AND YOU ARE",80,50,4,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("    RARELY SEEN OUTSIDE OF",80,60,4,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("    YOUR BLUE SPACESUIT.",80,70,4,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("YOU OFTEN GET YOURSELF IN ",80,90,2,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("    TROUBLE BY SCRATCHING",80,100,2,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("    YOUR HEAD AT INAPPROPRIATE",80,110,2,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("    TIMES.",80,120,2,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("PRESS ANY KEY....",96,185,15,0,0,tb1_font,game_state->virtual_1);
    vmwBlitMemToSDL(game_state->sdl_screen,game_state->virtual_1);
    pauseawhile(15);

    vmwClearScreen(game_state->virtual_1,0);
      
    vmwArbitraryCrossBlit(game_state->virtual_2,129,56,49,132,
		        game_state->virtual_1,260,10);
    vmwArbitraryCrossBlit(game_state->virtual_2,99,104,29,81,
			game_state->virtual_1,287,13);

    vmwTextXY("IT IS THE YEAR 2028.",10,10,1,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("YOU HAVE BEEN SUMMONED BY",10,30,3,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("    LUNAR DICTATOR-IN-CHIEF",10,40,3,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("    VINCENT WEAVER ABOUT A",10,50,3,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("    TOP SECRET THREAT TO ",10,60,3,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("    INTERPLANETARY SECURITY.",10,70,3,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("YOU ATTEND THE BRIEFING WITH",10,90,5,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("    YOUR USUAL CONFUSED",10,100,5,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("    QUIETNESS.  YOU STILL DO",10,110,5,0,0,tb1_font,game_state->virtual_1);  
    vmwTextXY("    NOT UNDERSTAND YOUR OWN",10,120,5,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("    SUCCESSFULNESS.",10,130,5,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("PRESS ANY KEY....",96,185,15,0,0,tb1_font,game_state->virtual_1);
    vmwBlitMemToSDL(game_state->sdl_screen,game_state->virtual_1);
    pauseawhile(12);

    vmwLoadPicPacked(0,0,game_state->virtual_1,1,1,
		    tb1_data_file("tbchief.tb1",game_state->path_to_data));

      /* Save the area where the error will go */
    vmwArbitraryCrossBlit(game_state->virtual_1,115,55,91,59,
		          game_state->virtual_2,115,255);
    vmwBlitMemToSDL(game_state->sdl_screen,game_state->virtual_1);
    pauseawhile(6);
   
    vmwTextXY("Ahhh.... Mr. Bombem.... ",1,1,15,0,0,tb1_font,game_state->virtual_1);  
    vmwBlitMemToSDL(game_state->sdl_screen,game_state->virtual_1);
  if (game_state->sound_enabled) playGameFX(0);
    pauseawhile(2);
   
    if (game_state->sound_enabled) playGameFX(2);
      /* Show fake error message */
    vmwArbitraryCrossBlit(game_state->virtual_2,188,14,91,59,
			  game_state->virtual_1,115,55);
   
    vmwBlitMemToSDL(game_state->sdl_screen,game_state->virtual_1);
    pauseawhile(6);
   
      /* Restore background where error was */
    vmwArbitraryCrossBlit(game_state->virtual_2,115,255,91,59,
			  game_state->virtual_1,115,55);
 
    vmwTextXY("I'll be brief.                       ",1,1,15,0,1,tb1_font,game_state->virtual_1);
    vmwBlitMemToSDL(game_state->sdl_screen,game_state->virtual_1);
    pauseawhile(5);
   
    vmwTextXY("Do you know how this base was founded?",1,1,15,0,1,tb1_font,game_state->virtual_1);
    vmwBlitMemToSDL(game_state->sdl_screen,game_state->virtual_1);
    pauseawhile(5);
   
    vmwTextXY("No?  Well watch the screen.             ",1,1,15,0,1,tb1_font,game_state->virtual_1);
    vmwBlitMemToSDL(game_state->sdl_screen,game_state->virtual_1);
    pauseawhile(5); 
   
       /* Put picture on screen */
    vmwArbitraryCrossBlit(game_state->virtual_2,210,75,85,60,
			  game_state->virtual_1,210,136);
    vmwBlitMemToSDL(game_state->sdl_screen,game_state->virtual_1);
    pauseawhile(4);

       /******BARGE TAKING OFF***********/
   
    vmwLoadPicPacked(0,0,game_state->virtual_2,1,1,
		     tb1_data_file("tbma1.tb1",game_state->path_to_data));
 
    vmwSmallTextXY("MY WIFE AND I FOUNDED",212,3,14,0,0,tb1_font,game_state->virtual_2);
    vmwSmallTextXY("THIS BASE IN 2008.",212,9,14,0,0,tb1_font,game_state->virtual_2);
    vmwSmallTextXY("THE ONLY WAY TO ",212,16,13,0,0,tb1_font,game_state->virtual_2);
    vmwSmallTextXY("FINANCE IT WAS TO",212,22,13,0,0,tb1_font,game_state->virtual_2);
    vmwSmallTextXY("ENGAGE IN A DUBIOUS",212,28,13,0,0,tb1_font,game_state->virtual_2);
    vmwSmallTextXY("BUSINESS.",212,34,13,0,0,tb1_font,game_state->virtual_2);
    vmwSmallTextXY("WE LAUNCHED EARTH'S",212,41,12,0,0,tb1_font,game_state->virtual_2);
    vmwSmallTextXY("TRASH INTO SPACE",212,47,12,0,0,tb1_font,game_state->virtual_2);
    vmwSmallTextXY("FOR A PROFIT.",212,53,12,0,0,tb1_font,game_state->virtual_2);
    vmwSmallTextXY("HERE IS FOOTAGE FROM",212,60,11,0,0,tb1_font,game_state->virtual_2);
    vmwSmallTextXY("THE LAST LAUNCH EIGHT",212,66,11,0,0,tb1_font,game_state->virtual_2);
    vmwSmallTextXY("YEARS AGO.",212,72,11,0,0,tb1_font,game_state->virtual_2);
 
    vmwFlipVirtual(game_state->virtual_1,game_state->virtual_2);
   
/*  putshape(bargeoff,vaddr,16,18,141,157);*/
  
  /*  vmwCrossBlit(plb_vis->write,plb_vaddr->read,plb_vis->stride,200);*/
    vmwBlitMemToSDL(game_state->sdl_screen,game_state->virtual_1);
    pauseawhile(7);

#if 0
/*   

 
  for ytemp:=191 downto 164 do begin
      if ytemp>=172 then blockmove(145,ytemp,152,ytemp+10,vaddr2,145,ytemp,vaddr)
                    else blockmove(145,172,152,182,vaddr2,145,172,vaddr);
      puttruck(145,ytemp,ytemp mod 2,vaddr);
      pauseawhile(7);
      if keypressed then if readkey=#27 then exit;
      flipd320(vaddr,vga);
  end;

  pauseawhile(20);
  flipd320(vaddr2,vaddr);
  putshape(bargeoff,vaddr,16,18,141,157);
  thrustcol:=0;
  ytemp:=157;
  thrust:=0;
  while ytemp>-25 do begin
      thrust:=thrust+0.05;
      if thrustcol<63 then inc(thrustcol);
      blockmove(141,ytemp,171,ytemp+30,vaddr2,141,ytemp,vaddr);
      for i:=1 to 17 do
          if ytemp+i>=0 then
          putshapeline(bargeoff,vaddr,16,18,141,ytemp,i);
      pauseawhile(5);
      if keypressed then if readkey=#27 then exit;
      pal(250,thrustcol,0,0);
      flipd320(vaddr,vga);
      ytemp:=ytemp-round(thrust);
  end;
*/
  pauseawhile(5);
  

       /****SECOND CHIEF*******/
   ggiSetGCForeground(vis,0]);
   ggiDrawBox(vis,0,0,320,200);   
   GGILoadPicPacked(0,0,vaddr2,1,1,
		    tb1_data_file("tbcobj.tb1",(char *)tempst),
		    (ggi_color *)&eight_bit_pal,
		    (ggi_pixel *)&tb1_pal,color_depth);
   GGILoadPicPacked(0,0,vis,1,1,
		    tb1_data_file("tbchief.tb1",(char *)tempst),
		    (ggi_color *)&eight_bit_pal,
		    (ggi_pixel *)&tb1_pal,color_depth);
/*    vmwArbitraryCrossBlit(plb_vaddr2->read,7,104,90,21,
		    plb_vis->write,6,174,plb_vis->stride,
		    stride_factor);
  */ 
    vmwTextXY("You might wonder why this is important.",1,1,15],0],1,tb1_font,virtual_1); 
    pauseawhile(6);
    vmwTextXY("Last week we received a message.       ",1,1,15],0],1,tb1_font,virtual_1);
    pauseawhile(6);
    vmwTextXY("It is of extra-terrestrial origin.     ",1,1,15],0],1,tb1_font,virtual_1);
    pauseawhile(6);
    vmwTextXY("Watch the screen.                      ",1,1,15],0],1,tb1_font,virtual_1);
    
    /*vmwArbitraryCrossBlit(plb_vaddr2->read,210,136,85,59,
			  plb_vis->write,210,136,plb_vis->stride,
			  stride_factor);*/
    pauseawhile(4);
   
       /******ALIEN DELEGATION*****/
    GGILoadPicPacked(0,0,vaddr2,1,1,
		    tb1_data_file("tbcrash.tb1",(char *)tempst),
		    (ggi_color *)&eight_bit_pal,
		    (ggi_pixel *)&tb1_pal,color_depth);
     
   /*alienchar:=34;
  flipd320(vaddr2,vaddr);
  flipd320(vaddr,vga);
  putshape(bargeoff,vaddr,16,18,97,180);
  unfade;
  xtemp:=97;
  ytemp:=181;
  flames:=true;
     while ytemp>118 do begin
      blockmove(xtemp,ytemp,xtemp+20,ytemp+20,vaddr2,xtemp,ytemp,vaddr);
      putshape(bargeoff,vaddr,16,18,xtemp,ytemp);
      doflames;
      dec(ytemp);
      inc(xtemp);
      pauseawhile(1);
      if keypressed then if readkey=#27 then exit;
     end;
      blockmove(xtemp,ytemp,xtemp+30,ytemp+30,vaddr2,xtemp,ytemp,vaddr);
     pal(0,63,63,63);
     putshape(explo1off,vga,16,18,160,118);
     pauseawhile(5);
     if keypressed then if readkey=#27 then exit;
     pal(0,0,0,0);
     if sbeffects then startsound(sound[3],0,false);
     for xtemp:=0 to 10 do begin
        putshapeover(explo2off,vaddr,16,18,160,118);
        doflames;
        putshapeover(explo1off,vaddr,16,18,160,118);
        doflames;
     end;
     blockmove(159,114,189,144,vaddr2,159,114,vaddr);
     putshapeover(rent1off,vaddr,16,6,160,115);
     flipd320(vaddr,vga);
     cycles:=0;
  repeat
    doflames;
    inc(cycles);
  until ((keypressed) or (cycles>60));
  if keypressed then if readkey=#27 then exit;
    */ 

       /****ALIEN MESSAGE*****/
    GGILoadPicPacked(0,0,vis,1,1,
		     tb1_data_file("tbgorg.tb1",(char *)tempst),
		     (ggi_color *)&eight_bit_pal,
		     (ggi_pixel *)&tb1_pal,color_depth);
    vmwTextXY("GREETINGS EARTHLINGS.",0,162,12],0],0,tb1_font,virtual_1);  
    vmwTextXY("I AM GORGONZOLA THE REPULSIVE.",0,171,12],0],0,tb1_font,virtual_1);
    vmwTextXY("YOU HAVE MADE A BIG MISTAKE.",0,180,12],0],0,tb1_font,virtual_1);
    pauseawhile(7);
    vmwTextXY("YOUR SHIP FULL OF REFUSE HAS",0,162,12],0],1,tb1_font,virtual_1);
    vmwTextXY("DAMAGED OUR OFFICIAL PEACE    ",0,171,12],0],1,tb1_font,virtual_1);
    vmwTextXY("ENVOY.  IT WAS ON ITS WAY TO ",0,180,12],0],1,tb1_font,virtual_1);
    vmwTextXY("YOUR PLANET.                  ",0,189,12],0],1,tb1_font,virtual_1);
    pauseawhile(7);
    vmwTextXY("IN AN IRONIC FORM OF RETALLIATION",0,162,12],0],1,tb1_font,virtual_1);
    vmwTextXY("WE HAVE MADE YOUR TRASH EVIL AND",0,171,12],0],1,tb1_font,virtual_1);
    vmwTextXY("TURNED IT AGAINST YOU.          ",0,180,12],0],1,tb1_font,virtual_1);
    vmwTextXY("        DIE EARTH SCUM!         ",0,189,12],0],1,tb1_font,virtual_1);
    pauseawhile(7);
 
       /****** THIRD CHIEF *******/
   GGILoadPicPacked(0,0,vaddr2,1,1,
		    tb1_data_file("tbcobj.tb1",(char *)tempst),
		    (ggi_color *)&eight_bit_pal,
		    (ggi_pixel *)&tb1_pal,color_depth);
   GGILoadPicPacked(0,0,virtual_1,1,1,
		    tb1_data_file("tbchief.tb1",(char *)tempst),
		    (ggi_color *)&eight_bit_pal,
		    (ggi_pixel *)&tb1_pal,color_depth);
    vmwArbitraryCrossBlit(plb_vaddr2->read,7,127,90,21,
		          plb_vis->write,6,174,plb_vis->stride,
		          stride_factor);
   
    vmwTextXY("Tom, our radar detects approaching ",1,1,15],0],1,tb1_font,virtual_1);
    pauseawhile(6);
    vmwTextXY("objects.  They are inside the      ",1,1,15],0],1,tb1_font,virtual_1);
    pauseawhile(6);
    vmwTextXY("orbit of Jupiter.                  ",1,1,15],0],1,tb1_font,virtual_1);
    pauseawhile(6);
    vmwTextXY("You are our only hope!             ",1,1,15],0],1,tb1_font,virtual_1);
    pauseawhile(6);
    vmwTextXY("Will you fly our only spaceship    ",1,1,15],0],1,tb1_font,virtual_1);
    pauseawhile(6);
    vmwTextXY("and save the human race?           ",1,1,15],0],1,tb1_font,virtual_1);
    pauseawhile(6);
    vmwArbitraryCrossBlit(plb_vaddr2->read,5,16,39,82,
			  plb_vis->write,146,59,plb_vis->stride,
			  stride_factor);
    pauseawhile(1);
    vmwArbitraryCrossBlit(plb_vaddr2->read,46,16,39,82,
			  plb_vis->write,146,59,plb_vis->stride,
			  stride_factor);
    pauseawhile(1);
    vmwArbitraryCrossBlit(plb_vaddr2->read,87,16,39,82,
			  plb_vis->write,146,59,plb_vis->stride,
			  stride_factor);
    vmwTextXY("Scratch.  Scratch. <Ow that itches>",1,1,9],0],1,tb1_font,virtual_1);
    pauseawhile(5); 
    vmwTextXY("I knew you'd do it.  Good Luck!     ",1,1,15],0],1,tb1_font,virtual_1);
    pauseawhile(6);
    vmwTextXY("<Huh?>                             ",1,1,9],0],1,tb1_font,virtual_1);
    pauseawhile(10);
#endif
}

void credits(struct tb1_state *game_state)
{

    int i,keypressed=0;
   
    vmw_font *tb1_font;
   
    tb1_font=game_state->tb1_font;

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
       vmwBlitMemToSDL(game_state->sdl_screen,game_state->virtual_1);
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
       vmwBlitMemToSDL(game_state->sdl_screen,game_state->virtual_1);
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
       vmwBlitMemToSDL(game_state->sdl_screen,game_state->virtual_1);
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
       vmwBlitMemToSDL(game_state->sdl_screen,game_state->virtual_1);
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
      vmwBlitMemToSDL(game_state->sdl_screen,game_state->virtual_1);
      usleep(30000);
      if (vmwGetInput()) {
	 break;
      }
  }
  vmwArbitraryCrossBlit(game_state->virtual_2,0,200,320,200,
                        game_state->virtual_1,0,0);

  vmwBlitMemToSDL(game_state->sdl_screen,game_state->virtual_1);
   
}

void about(struct tb1_state *game_state)
{
    int pagenum=1,firstime=0,oldpagenum=0,numpages=4,ch=0;
    char tempst[300];
 
    vmw_font *tb1_font;
    unsigned char *target1,*target2;
    SDL_Surface *sdl_screen;
   
    tb1_font=game_state->tb1_font;
    target1=game_state->virtual_1;
    target2=game_state->virtual_2;
    sdl_screen=game_state->sdl_screen;

    vmwClearScreen(target1,0);
         
    vmwLoadPicPacked(0,0,target2,1,1,
		    tb1_data_file("register.tb1",game_state->path_to_data)); 
      
    while ((ch!=TB_ESCAPE)&&(ch!='q')){
       while(( (ch=vmwGetInput())==0) && (firstime));
       if(!firstime) firstime=1;
       if ((ch==' ') || (ch==TB_ENTER)) pagenum++;
       if ((ch==TB_RIGHT) || (ch==TB_DOWN) || (ch==TB_PGDN)) pagenum++;
       if ((ch==TB_LEFT) || (ch==TB_UP) || (ch==TB_PGUP)) pagenum--;
       if (pagenum>4) pagenum=1;
       if (pagenum<1) pagenum=4;
       if (oldpagenum!=pagenum){
          if (pagenum==1) {
	     vmwFlipVirtual(target1,target2);
	     coolbox(0,0,319,199,0,target1);
	     shadowrite("              INFORMATION",10,10,9,1,tb1_font,target1);
	     shadowrite("I STARTED THIS GAME IN LATE",70,30,9,1,tb1_font,target1);
	     shadowrite("  1994, WHEN I WAS 16.",70,40,9,1,tb1_font,target1);
	     shadowrite("I WROTE THIS GAME ENTIRELY IN",75,50,9,1,tb1_font,target1);
	     shadowrite("  MY FREE TIME.",74,60,9,1,tb1_font,target1);
	     shadowrite("   ^(AUTHOR 1N 1995)",10,70,10,2,tb1_font,target1);
	     shadowrite("ORIGINALLY THIS GAME WAS CODED IN",10,90,12,4,tb1_font,target1);
	     shadowrite("  TURBO PASCAL AND IN-LINE ASSEMBLY",10,100,12,4,tb1_font,target1);
	     shadowrite("  OPTIMIZED TO RUN ON A 386.  NOW I",10,110,12,4,tb1_font,target1);
	     shadowrite("  HAVE PORTED IT TO LINUX AND SDL.",10,120,12,4,tb1_font,target1);
	     shadowrite("  IN THAT SPIRIT I HAVE NOW GPL'ED",10,130,12,4,tb1_font,target1);
	     shadowrite("  THE CODE.",10,140,12,4,tb1_font,target1);
          } else coolbox(0,0,319,199,1,target1);	  

          if (pagenum==2){
	     shadowrite("MY NAME IS VINCE WEAVER",10,10,10,2,tb1_font,target1);
	     shadowrite("   VISIT MY TALKER",10,20,10,2,tb1_font,target1);
	     shadowrite("   DERANGED.STUDENT.UMD.EDU 7000",10,30,10,2,tb1_font,target1);
	     shadowrite("   UP WHENEVER SCHOOL IS IN SESSION",10,40,10,2,tb1_font,target1);
	     shadowrite("UNTIL DECEMBER OF 2000 I WILL BE",10,60,13,5,tb1_font,target1);
	     shadowrite(" ATTENDING COLLEGE, AT THE UNIVERSITY",10,70,13,5,tb1_font,target1);
	     shadowrite(" OF MARYLAND, COLLEGE PARK.",10,80,13,5,tb1_font,target1);
	     shadowrite("GET THE NEWEST VERSION OF TB1 AT",10,100,11,3,tb1_font,target1);
	     shadowrite(" THE OFFICIAL TB1 WEB SITE:",10,110,11,3,tb1_font,target1);
	     shadowrite(" http://www.glue.umd.edu/~weave/tb1/",10,120,11,3,tb1_font,target1);
	     shadowrite("I CAN BE CONTACTED VIA E-MAIL AT:",10,140,12,4,tb1_font,target1);
	     shadowrite("    WEAVE@ENG.UMD.EDU",10,150,9,1,tb1_font,target1);
	     shadowrite("FEEL FREE TO SEND COMMENTS.",10,160,12,4,tb1_font,target1);
          }
	  
          if (pagenum==3){
	     shadowrite("OTHER VMW SOFTWARE PRODUCTIONS:",10,10,15,7,tb1_font,target1);
	     shadowrite(" PAINTPRO:",10,30,13,5,tb1_font,target1);
 	     shadowrite("   LOAD AND SAVE GRAPHICS PICTURES",10,40,13,5,tb1_font,target1);
 	     shadowrite(" LINUX_LOGO",10,50,11,3,tb1_font,target1);
 	     shadowrite("   A USERLAND ANSI LOGIN DISPLAY",10,60,11,3,tb1_font,target1);
 	     shadowrite(" SPACEWAR III:",10,70,9,1,tb1_font,target1);
 	     shadowrite("   NEVER COMPLETED GAME",10,80,9,1,tb1_font,target1);
	     shadowrite(" AITAS: (ADVENTURES IN TIME AND SPACE)",10,90,12,4,tb1_font,target1);
	     shadowrite("   A GAME I'VE BEEN WANTING TO WRITE",10,100,12,4,tb1_font,target1);
	     shadowrite("   FOR 5 YEARS.  [INCOMPLETE]",10,110,12,4,tb1_font,target1);
	     shadowrite(" FONT_PRINT",10,120,9,1,tb1_font,target1);
	     shadowrite("   PRINT VGA FONTS IN DOS AND LINUX",10,130,9,1,tb1_font,target1);
	     shadowrite(" SEABATTLE:",10,140,13,5,tb1_font,target1);
	     shadowrite("   A BATTLESHIP CLONE CODED IN C",10,150,13,5,tb1_font,target1);
          }
	  
          if (pagenum==4){
             shadowrite("DISCLAIMER:",10,10,12,14,tb1_font,target1);
	     shadowrite("* MY PROGRAMS SHOULD NOT DAMAGE YOUR *",8,30,12,4,tb1_font,target1);
	     shadowrite("* COMPUTER IN ANY WAY.  PLEASE DON'T *",8,40,12,4,tb1_font,target1);
	     shadowrite("* USE MY SOFTWARE IN CRITICAL        *",8,50,12,4,tb1_font,target1);
	     shadowrite("* APPLICATIONS LIKE LIFE-SUPPORT     *",8,60,12,4,tb1_font,target1);
	     shadowrite("* EQUIPMENT, DEFLECTOR SHIELDS, OR   *",8,70,12,4,tb1_font,target1);
	     shadowrite("* AUTOMOBILE ENGINES.                *",8,80,12,4,tb1_font,target1);
	     shadowrite("* LINUX FOREVER! THE OS FOR EVERYONE *",8,90,12,4,tb1_font,target1);
	     shadowrite("% WARRANTY ESPECIALLY VOID IF USED   %",8,110,11,3,tb1_font,target1);
	     shadowrite("% ON ANY MICROSOFT(tm) OS (YUCK)     %",8,120,11,3,tb1_font,target1);
          }
          sprintf(tempst,"Page %d of %d: ESC QUITS",pagenum,numpages);
          shadowrite(tempst,50,180,15,7,tb1_font,target1);
	  vmwBlitMemToSDL(sdl_screen,target1);
          oldpagenum=pagenum;
       }
    }
}


void playthegame(struct tb1_state *game_state)
{
   
    if (game_state->level==0) {
       littleopener(game_state);
       game_state->shields=12;
       game_state->score=0;
       game_state->level++;
    }
   
    if (game_state->level==1) {
       levelone(game_state);
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
    vmwTextXY("GAME OVER",84,95,4,7,0,game_state->tb1_font,game_state->virtual_1);
    game_state->level=0;
    vmwBlitMemToSDL(game_state->sdl_screen,game_state->virtual_1);
    clear_keyboard_buffer(); 
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

