#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <ggi/libggi.h>
#include "soundIt.h"
#include "svmwgrap.h"
#include "levels.h"

extern vmw_font *tb1_font;
extern ggi_visual_t vis;
extern ggi_visual_t vaddr;
extern ggi_visual_t vaddr2;
extern ggi_directbuffer_t dbuf_vis;
extern ggi_directbuffer_t dbuf_vaddr;
extern ggi_directbuffer_t dbuf_vaddr2;
extern ggi_pixellinearbuffer *plb_vis;
extern ggi_pixellinearbuffer *plb_vaddr;
extern ggi_pixellinearbuffer *plb_vaddr2;
extern int stride_factor;
extern char path_to_data[256];
extern int sound_enabled;
extern int sound_possible;
extern int read_only_mode;
extern ggi_color eight_bit_pal[256];
extern ggi_pixel tb1_pal[256];
extern int color_depth;

#define TB_ESC   27
#define TB_ENTER 1024
#define TB_F1    1025
#define TB_F2    1026
#define TB_UP    1027
#define TB_DOWN  1028
#define TB_LEFT  1029
#define TB_RIGHT 1030
#define TB_PGUP  1031
#define TB_PGDOWN 1032


    /* Convenience Function */
int change_shields(int *shields)
{
       int itemp,jtemp;
   
       eight_bit_pal[254].r=(*shields)*0x1000;
       eight_bit_pal[254].g=0;
       eight_bit_pal[254].b=0;
   
       if (color_depth!=8)
            tb1_pal[254]=ggiMapColor(vis,&eight_bit_pal[254]);
       else {
	         tb1_pal[254]=254;
	         ggiSetPaletteVec(vis,254,1,&eight_bit_pal[254]);
       }
       ggiSetGCForeground(vaddr,tb1_pal[0]);
       ggiDrawBox(vaddr,250,71,64,8);
       for(itemp=0;itemp<(*shields)*4;itemp+=4)
            for(jtemp=71;jtemp<79;jtemp++) {
	                 ggiSetGCForeground(vaddr,tb1_pal[47-(itemp/4)]);
	                 ggiDrawHLine(vaddr,250+itemp,jtemp,4);
	    }
   
       return 0;
}
 
    /* TSIA [well actually FNSIA (function name says it all) */
int changescore(int score,int *shields)
{
       char scorest[20];
   
       sprintf(scorest,"%d",score);
       VMWtextxy(scorest,317-(strlen(scorest)*9),11,
                 tb1_pal[12],tb1_pal[0],1,tb1_font,vaddr);
       if ( ((score % 400)==0) && (*shields<16) ) {
	         if (sound_enabled) Snd_effect(/*SND_ZRRP*/3,3);
	         (*shields)++;
	         change_shields(shields);
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

char *tb1_data_file(char *name, char *store)
{
    sprintf(store,"%s/%s",path_to_data,name);
    return store;
}

int get_input() {

    int evmask;
    ggi_event ev;
    struct timeval t={0,0};
   
    evmask=emKeyPress|emKeyRelease|emPointer;

    while (ggiEventPoll(vis,evmask,&t)) {
       do {
	  ggiEventRead(vis,&ev,evmask);
       } while(! ( (1 << ev.any.type) & evmask ) );

       if (ev.any.type==evKeyPress) {
	  switch(KTYP(U(ev.key.sym))) { 
	     case KT_LATIN:
	     case KT_LETTER: 
	        return (KVAL(U(ev.key.sym))); break;
	     default: switch(U(ev.key.sym)) {
		case K_UP: /* CrSr up */
		case K_P8: /* Keypad 8 */
		     return TB_UP;break;
		case K_DOWN: /* CrSr down */
		case K_P2: /* Keypad 2 */
	  	     return TB_DOWN;break;
		case K_RIGHT: /* CrSr right */
		case K_P6: /* CrSr right */
		     return TB_RIGHT;break;
		case K_LEFT: /* CrSr left */
		case K_P4: /* CrSr left */
	  	     return TB_LEFT;break;
	      case K_F1:
		return TB_F1;break;
	      case K_F2:
		return TB_F2;break;
		case K_ENTER: /* enter */
		     return TB_ENTER;break;
		default: 
		/*printf("sym=%4x code=%4x\n",ev.key.sym,ev.key.code);*/break;
	     }
	  }
       }
    }
   return 0;
}


void clear_keyboard_buffer()
{
       while (get_input()!=0) ;
}

void drawsquare(int x1,int y1,int x2,int y2,int col,ggi_visual_t page)
{
   ggiSetGCForeground(page,tb1_pal[col]);
   /*ggiDrawHLine(page,x1,y1,(x2-x1));
   ggiDrawHLine(page,x1,y2,(x2-x1));
   ggiDrawVLine(page,x1,y1,(y2-y1));
   ggiDrawVLine(page,x2,y1,(y2-y1));*/
   ggiDrawLine(page,x1,y1,x1,y2);
   ggiDrawLine(page,x2,y1,x2,y2);
   ggiDrawLine(page,x1,y1,x2,y1);
   ggiDrawLine(page,x1,y2,x2,y2);
}

void coolbox(int x1,int y1,int x2,int y2,int fill,ggi_visual_t page)
{
    int i;
   
    for(i=0;i<5;i++) {
       /*ggiSetGCForeground(page,31-i);
       ggiDrawBox(page,x1+i,y1+i,(x2-x1-i-i),(y2-y1-i-i));*/
       drawsquare(x1+i,y1+i,x2-i,y2-i,31-i,page);
    }
    if (fill) {
       ggiSetGCForeground(page,tb1_pal[7]);
       for(i=y1+5;i<y2-4;i++) ggiDrawHLine(page,x1+5,i,(x2-x1-9));
     }  
}



int close_graphics()
{
    int err=0;
   
    err=ggiClose(vis);
    err+=ggiClose(vaddr);
    err+=ggiClose(vaddr2);
    if(err) fprintf(stderr,"Probelms shutting down GGI!\n");
    else fprintf(stderr,"Shutting down GGI...\n");
    ggiExit();
    return 0;
}


int quit()
{
   int barpos=0,ch=0,ch2;
   
    coolbox(90,75,230,125,1,vis);
    VMWtextxy("QUIT??? ARE YOU",97,82,tb1_pal[9],tb1_pal[7],0,tb1_font,vis);
    VMWtextxy("ABSOLUTELY SURE?",97,90,tb1_pal[9],tb1_pal[7],0,tb1_font,vis);
    while (ch!=TB_ENTER){
       if (barpos==0) VMWtextxy("YES-RIGHT NOW!",97,98,tb1_pal[150],tb1_pal[0],1,tb1_font,vis);
       else VMWtextxy("YES-RIGHT NOW!",97,98,tb1_pal[150],tb1_pal[7],1,tb1_font,vis);
       if (barpos==1) VMWtextxy("NO--NOT YET.",97,106,tb1_pal[150],tb1_pal[0],1,tb1_font,vis);
       else VMWtextxy("NO--NOT YET.",97,106,tb1_pal[150],tb1_pal[7],1,tb1_font,vis);
       while ( (ch=get_input())==0);
       ch2=toupper(ch);
       if ((ch==TB_UP)||(ch==TB_DOWN)||(ch==TB_LEFT)||(ch==TB_RIGHT)) barpos++;
       if (ch2=='Y') barpos=0;
       if (ch2=='N') barpos=1;
       if (barpos==2) barpos=0;
    }
    if (barpos==0){ 
       close_graphics();
       exit(1);
    }
    else return 6;
   /*move(imagedata,screen,4000); textcolor(7);*/ 
}


int showhiscore(int showchart)
{
   
    FILE *hilist;
    char tempstr[10];
    int i;
    char names[10][25];
    int scores[10];

    hilist=fopen("hiscore.tb1","r+");
    for (i=0;i<10;i++) fscanf(hilist,"%s",names[i]);
    for (i=0;i<10;i++) fscanf(hilist,"%i",&scores[i]);
    fclose(hilist);
    if (!showchart) return scores[0];
    if (showchart) {
       ggiSetGCForeground(vis,tb1_pal[7]);
       ggiDrawBox(vis,45,40,240,120);
       coolbox(45,40,285,160,1,vis);
       for(i=0;i<10;i++) 
          VMWtextxy(names[i],51,46+(i*10),tb1_pal[9],tb1_pal[7],1,tb1_font,vis);
       for(i=0;i<10;i++){
         sprintf(tempstr,"%i",scores[i]);
         VMWtextxy(tempstr,181,46+(i*10),tb1_pal[9],tb1_pal[7],1,tb1_font,vis);
       }
       ggiFlush(vis);
       while(get_input()==0);
    }
   return 0;
}

void write_hs_list(int score,char *hiname)
{
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
}



void help()
{
    coolbox(0,0,319,199,1,vis);
    VMWtextxy("HELP",144,10,4,0,0,tb1_font,vis);
    VMWtextxy("--------------------------------------",10,20,
	                           tb1_pal[12],tb1_pal[0],0,tb1_font,vis);
    VMWtextxy("IN THE GAME:",10,30,tb1_pal[4],tb1_pal[0],0,tb1_font,vis);
    VMWtextxy("   ARROWS MANUEVER",10,40,
	                           tb1_pal[4],tb1_pal[0],0,tb1_font,vis);
    VMWtextxy("   SPACE BAR FIRES MISSILES",10,50,
	                           tb1_pal[4],tb1_pal[0],0,tb1_font,vis);
    VMWtextxy("   F2 SAVES GAME",10,60,
	                           tb1_pal[4],tb1_pal[0],0,tb1_font,vis);
    VMWtextxy("   P=PAUSE  S=TOGGLE SOUND  ESC=QUIT",10,70,
	                           tb1_pal[4],tb1_pal[0],0,tb1_font,vis);
    VMWtextxy("--------------------------------------",10,80,
	                           tb1_pal[12],tb1_pal[0],0,tb1_font,vis);
    VMWtextxy("MISSION: SHOOT THE INANIMATE OBJECTS",10,90,
	                           tb1_pal[4],tb1_pal[0],0,tb1_font,vis);
    VMWtextxy("         WHY?  WATCH THE STORY!",10,100,
	                           tb1_pal[4],tb1_pal[0],0,tb1_font,vis);
    VMWtextxy("--------------------------------------",10,110,
	                           tb1_pal[12],tb1_pal[0],0,tb1_font,vis);
    VMWtextxy("THE SPACE BAR SPEEDS UP MOVIE SCENES",10,120,
	                           tb1_pal[4],tb1_pal[0],0,tb1_font,vis);
    VMWtextxy("ESC QUITS THEM",10,130,
	                           tb1_pal[4],tb1_pal[0],0,tb1_font,vis);
    VMWtextxy("--------------------------------------",10,140,
	                           tb1_pal[12],tb1_pal[0],0,tb1_font,vis);
    VMWtextxy("   SEE \"README\" FOR MORE HELP/INFO",10,150,
	                           tb1_pal[4],tb1_pal[0],0,tb1_font,vis);
    VMWtextxy("--------------------------------------",10,160,
	                           tb1_pal[12],tb1_pal[0],0,tb1_font,vis);
    VMWtextxy("PRESS ANY KEY TO CONTINUE",64,185,
	                           tb1_pal[4],tb1_pal[0],0,tb1_font,vis);
    while(get_input()==0) usleep(30);
}


void makehole(ggi_visual_t vis,int y)
{
  ggiSetGCForeground(vis,tb1_pal[0]);
  ggiDrawBox(vis,249,y,64,9);
  ggiSetGCForeground(vis,tb1_pal[24]);
  ggiDrawHLine(vis,249,y,65);
  ggiDrawVLine(vis,249,y,9);
  ggiSetGCForeground(vis,tb1_pal[18]);
  ggiDrawHLine(vis,249,y+10,64);
  ggiDrawVLine(vis,314,y+1,9);
}

void setupsidebar(int score,int hiscore,int shields)
{
    int i;
    char it[50]; 
   
    ggiSetGCForeground(vaddr2,tb1_pal[19]);
    ggiDrawBox(vaddr2,240,0,80,199);
   
    ggiSetGCForeground(vaddr2,tb1_pal[18]);
    ggiDrawVLine(vaddr2,240,0,199);
    ggiDrawHLine(vaddr2,240,0,79);
   
    ggiSetGCForeground(vaddr2,tb1_pal[24]);
    ggiDrawVLine(vaddr2,319,0,199);
    ggiDrawHLine(vaddr2,241,199,78);
      
    VMWtextxy("SCORE",241,1,tb1_pal[127],tb1_pal[0],0,tb1_font,vaddr2);
    VMWtextxy("SCORE",242,2,tb1_pal[143],tb1_pal[0],0,tb1_font,vaddr2);
    makehole(vaddr2,10);
    sprintf(it,"%d",score);
    VMWtextxy(it,250+(8*(8-strlen(it))),11,tb1_pal[12],tb1_pal[0],1,tb1_font,vaddr2);

    hiscore=showhiscore(0);
    VMWtextxy("HI-SCORE",241,21,tb1_pal[127],tb1_pal[0],0,tb1_font,vaddr2);
    VMWtextxy("HI-SCORE",242,22,tb1_pal[143],tb1_pal[0],0,tb1_font,vaddr2);
    sprintf(it,"%d",hiscore);
    makehole(vaddr2,30);
    VMWtextxy(it,250+(8*(8-strlen(it))),31,tb1_pal[12],tb1_pal[0],1,tb1_font,vaddr2);
  
    VMWtextxy("LEVEL",241,41,tb1_pal[127],tb1_pal[0],0,tb1_font,vaddr2);
    VMWtextxy("LEVEL",242,42,tb1_pal[143],tb1_pal[0],0,tb1_font,vaddr2);
    makehole(vaddr2,50);
    VMWtextxy("SHIELDS",241,61,tb1_pal[127],tb1_pal[0],0,tb1_font,vaddr2);
    VMWtextxy("SHIELDS",242,62,tb1_pal[143],tb1_pal[0],0,tb1_font,vaddr2);
    makehole(vaddr2,70);
    for(i=0;i<(4*shields);i++) {
       ggiSetGCForeground(vaddr2,tb1_pal[(47-(i/4))]);
       ggiDrawVLine(vaddr2,250+i,71,8);
    }
    VMWtextxy("WEAPONS",241,81,tb1_pal[127],tb1_pal[0],0,tb1_font,vaddr2);
    VMWtextxy("WEAPONS",242,82,tb1_pal[143],tb1_pal[0],0,tb1_font,vaddr2);
    makehole(vaddr2,90);

    ggiSetGCForeground(vaddr2,tb1_pal[0]);
    ggiDrawBox(vaddr2,249,111,65,78);
   
    ggiSetGCForeground(vaddr2,tb1_pal[24]);
    ggiDrawVLine(vaddr2,249,111,78);
    ggiDrawHLine(vaddr2,249,111,66);
    ggiSetGCForeground(vaddr2,tb1_pal[18]);
    ggiDrawVLine(vaddr2,315,111,78);
    ggiDrawHLine(vaddr2,249,189,66);
      
    VMWtextxy("  TB1   ",250,114,tb1_pal[2],tb1_pal[0],0,tb1_font,vaddr2);
    VMWtextxy("  TB1   ",251,115,tb1_pal[10],tb1_pal[0],0,tb1_font,vaddr2);
    VMWtextxy("F1-HELP  ",250,124,tb1_pal[2],tb1_pal[0],0,tb1_font,vaddr2);
    VMWtextxy("F1-HELP  ",251,125,tb1_pal[10],tb1_pal[0],0,tb1_font,vaddr2);
    VMWtextxy("F2-SAVES",250,134,tb1_pal[2],tb1_pal[0],0,tb1_font,vaddr2);
    VMWtextxy("F2-SAVES",251,135,tb1_pal[10],tb1_pal[0],0,tb1_font,vaddr2);
    VMWtextxy("ESC-QUIT",250,144,tb1_pal[2],tb1_pal[0],0,tb1_font,vaddr2);
    VMWtextxy("ESC-QUIT",251,145,tb1_pal[10],tb1_pal[0],0,tb1_font,vaddr2);
    VMWtextxy("P-PAUSES",250,154,tb1_pal[2],tb1_pal[0],0,tb1_font,vaddr2);
    VMWtextxy("P-PAUSES",251,155,tb1_pal[10],tb1_pal[0],0,tb1_font,vaddr2);
    VMWtextxy("S-SOUND ",250,164,tb1_pal[2],tb1_pal[0],0,tb1_font,vaddr2);
    VMWtextxy("S-SOUND ",251,165,tb1_pal[10],tb1_pal[0],0,tb1_font,vaddr2);
}





void pauseawhile(int howlong)
{
    struct timeval bob;
    struct timezone mree;
    long begin_s,begin_u;
   
    clear_keyboard_buffer();
    gettimeofday(&bob,&mree);
    begin_s=bob.tv_sec; begin_u=bob.tv_usec;
    while(( (bob.tv_sec-begin_s)<howlong) && (get_input()==0)) {
       usleep(30);
       gettimeofday(&bob,&mree);
    }
}

void savegame(int level,int begin_score,int begin_shields)
{
    char ch;
    struct stat buf;
    char tempst[300],save_game_name[320],tempst2[5];
    FILE *fff;
    int dot_tb1_exists=0;
    
    int i,j;
   
    coolbox(0,0,319,199,1,vis);
    VMWtextxy("SAVE GAME",110,10,tb1_pal[9],tb1_pal[0],0,tb1_font,vis);
    if (read_only_mode) {
       VMWtextxy("SORRY!  CANNOT SAVE GAME",40,40,tb1_pal[4],tb1_pal[0],0,tb1_font,vis);
       VMWtextxy("WHEN IN READ ONLY MODE",50,50,tb1_pal[4],tb1_pal[0],0,tb1_font,vis);
    }
    else {
       sprintf(tempst,"%s/.tb1",getenv("HOME"));
       stat(tempst,&buf);
       if (!S_ISDIR(buf.st_mode)) {
	  VMWtextxy("DIRECTORY:",20,20,tb1_pal[4],tb1_pal[0],0,tb1_font,vis);
	  if (strlen(tempst)>18)
	     VMWtextxy("$HOME/.tb1",120,20,tb1_pal[4],tb1_pal[0],0,tb1_font,vis);
	  else 
	     VMWtextxy(tempst,120,20,tb1_pal[4],tb1_pal[0],0,tb1_font,vis);
	  VMWtextxy("WHERE TB1 STORES SAVED GAMES",20,30,tb1_pal[4],tb1_pal[0],0,tb1_font,vis);
	  VMWtextxy("DOES NOT EXIST.  DO YOU WANT",20,40,tb1_pal[4],tb1_pal[0],0,tb1_font,vis);
	  VMWtextxy("TO CREATE THIS DIRECTORY?",20,50,tb1_pal[4],tb1_pal[0],0,tb1_font,vis); 
	  VMWtextxy("   PLEASE ANSWER [Y or N]",20,60,tb1_pal[4],tb1_pal[0],0,tb1_font,vis);
	  VMWtextxy("NOTE IF YOU ANSWER \"N\" YOU",20,80,tb1_pal[12],tb1_pal[0],0,tb1_font,vis);
	  VMWtextxy("WILL NOT BE ABLE TO SAVE",20,90,tb1_pal[12],tb1_pal[0],0,tb1_font,vis);
	  VMWtextxy("GAMES",20,100,tb1_pal[12],tb1_pal[0],0,tb1_font,vis);
	  ch='M';
	  clear_keyboard_buffer();
	  while( (ch!='Y') && (ch!='y') && (ch!='N') && (ch!='n')) {
	     while((ch=get_input())==0) usleep(30);
	  }
	  if (toupper(ch)=='Y')
	     if (!mkdir(tempst,744)) dot_tb1_exists=1;
	     else {
		coolbox(0,0,319,199,1,vis);
		VMWtextxy("WARNING! COULD NOT CREATE",30,30,tb1_pal[4],tb1_pal[0],0,tb1_font,vis);
		VMWtextxy("DIRECTORY!  ABANDONING SAVE!",20,40,tb1_pal[4],tb1_pal[0],0,tb1_font,vis);	
	     }
	     
       }
       else dot_tb1_exists=1;
    }
    if (dot_tb1_exists) {
       coolbox(0,0,319,199,1,vis);
       VMWtextxy("SAVE GAME",110,20,tb1_pal[9],tb1_pal[0],0,tb1_font,vis);
       VMWtextxy("NOTE: THIS ONLY SAVES THE GAME",10,40,tb1_pal[4],tb1_pal[0],0,tb1_font,vis);
       VMWtextxy("AT THE BEGINNING OF THE LEVEL!",10,50,tb1_pal[4],tb1_pal[0],0,tb1_font,vis);
       VMWtextxy("ALREADY EXISTING GAMES",10,70,tb1_pal[4],tb1_pal[0],0,tb1_font,vis);
       j=0;
       for(i=0;i<10;i++) {
	  sprintf(save_game_name,"%s/sg%i.tb1",tempst,i);
	  if ((fff=fopen(save_game_name,"r"))!=NULL) {
	     sprintf(tempst2,"%i",i);
	     VMWtextxy(tempst2,50+j*20,80,tb1_pal[12],tb1_pal[0],0,tb1_font,vis);
	     fclose(fff);  
	     j++;
	  }
       }	  
       if (j==0) VMWtextxy("NONE",140,90,tb1_pal[12],tb1_pal[0],0,tb1_font,vis);
       VMWtextxy("PRESS NUMBER OF GAME TO SAVE",20,110,tb1_pal[4],tb1_pal[0],0,tb1_font,vis);
       VMWtextxy("(0-9) [ESC CANCELS]",60,120,tb1_pal[4],tb1_pal[0],0,tb1_font,vis);
       ch='M';
       clear_keyboard_buffer();
       while( ((ch<'0') || (ch>='9')) && (ch!=TB_ESC)) {
	  while((ch=get_input())==0) usleep(30);
       }
       if (ch==TB_ESC) {
	  coolbox(0,0,320,200,1,vis);
	  VMWtextxy("SAVE CANCELED",90,95,tb1_pal[12],tb1_pal[4],0,tb1_font,vis);
       }
       else {
	  sprintf(save_game_name,"%s/sg%c.tb1",tempst,ch);
	  if ((fff=fopen(save_game_name,"w"))!=NULL) {
	     fprintf(fff,"%i\n%i\n%i\n",level,begin_score,begin_shields);
	     fclose(fff);
	     coolbox(0,0,320,200,1,vis);
	     sprintf(tempst,"GAME %c SAVED",ch);
	     VMWtextxy(tempst,90,95,tb1_pal[12],tb1_pal[0],0,tb1_font,vis);
	  }
	  else {
	     coolbox(0,0,320,200,1,vis);
	     VMWtextxy("ERROR SAVING FILE!",70,90,tb1_pal[12],tb1_pal[0],0,tb1_font,vis);
             VMWtextxy("GAME NOT SAVED!",80,100,tb1_pal[12],tb1_pal[0],0,tb1_font,vis);
	  }
       }
    }
    VMWtextxy("PRESS ANY KEY...",80,180,tb1_pal[4],tb1_pal[0],0,tb1_font,vis);
    while( (ch=get_input())==0) usleep(30);
}

void shadowrite(char *st,int x5,int y5,int forecol,int backcol,
		ggi_visual_t vis)
{
    VMWtextxy(st,x5+1,y5+1,tb1_pal[backcol],0,0,tb1_font,vis);
    VMWtextxy(st,x5,y5,tb1_pal[forecol],0,0,tb1_font,vis);
}

void options()
{
    int opbarpos,argh=0,ch=0;
   
    ggiSetGCForeground(vis,tb1_pal[8]);
    ggiFillscreen(vis);
    coolbox(0,0,319,199,0,vis);
    VMWtextxy("ESC QUITS",120,175,tb1_pal[32],tb1_pal[0],1,tb1_font,vis);
    opbarpos=0;
    while(ch!=TB_ESC) {
       if (sound_enabled) {
          if (opbarpos==0) VMWtextxy("SOUND ON ",30,30,tb1_pal[32],
				     tb1_pal[7],1,tb1_font,vis);
          else VMWtextxy("SOUND ON ",30,30,tb1_pal[32],
			             tb1_pal[0],1,tb1_font,vis);
       }
       else {
          if (opbarpos==0) VMWtextxy("NO SOUND ",30,30,tb1_pal[32],
				     tb1_pal[7],1,tb1_font,vis);
          else VMWtextxy("NO SOUND ",30,30,tb1_pal[32],
			             tb1_pal[0],1,tb1_font,vis);
       }
       if (opbarpos==1) VMWtextxy("VIEW HIGH SCORES",30,40,tb1_pal[32],
				     tb1_pal[7],1,tb1_font,vis);
       else VMWtextxy("VIEW HIGH SCORES",30,40,tb1_pal[32],
		                     tb1_pal[0],1,tb1_font,vis);
       
       while( (ch=get_input())==0) usleep(30);
       
       if ((ch==TB_RIGHT) || (ch==TB_DOWN)) opbarpos++;
       if ((ch==TB_LEFT) || (ch==TB_UP)) opbarpos--;
       if ((ch=='M')||(ch=='m')) opbarpos=0;
       if ((ch=='V')||(ch=='v')) opbarpos=1;
       if ((ch==TB_ENTER) && (opbarpos==0)) sound_enabled=!sound_enabled;
       if ((ch==TB_ENTER) && (opbarpos==1)){
          ch=TB_ESC;
          argh=4;
       }    
       if (opbarpos==2) opbarpos=0;
       if (opbarpos==-1) opbarpos=1;
    }
    if (argh==4) showhiscore(1);
}

void playthegame(int lev,int sc,int sh);


void loadgame()
{
    char tempst[300],tempst2[5],file_name[320];
    int i,j;
    FILE *fff;
    int game_exist[10]={0,0,0,0,0, 0,0,0,0,0};
    char ch;
    int level,score,shields;
     
    coolbox(0,0,320,200,1,vis);
    VMWtextxy("LOAD GAME",110,10,tb1_pal[9],tb1_pal[0],0,tb1_font,vis);
   
    sprintf(tempst,"%s/.tb1",getenv("HOME"));
     
    j=0;
    for(i=0;i<10;i++) {
       sprintf(file_name,"%s/sg%i.tb1",tempst,i);
       if ((fff=fopen(file_name,"r"))!=NULL) {
	  sprintf(tempst2,"%i",i);
	  VMWtextxy(tempst2,50+j*20,50,tb1_pal[12],tb1_pal[0],0,tb1_font,vis);
	  fclose(fff);
	  game_exist[i]=1;
	  j++;
       }
    }
    if (j==0) {
       VMWtextxy("NO SAVED GAMES FOUND",60,50,tb1_pal[12],tb1_pal[0],0,tb1_font,vis); 
       VMWtextxy("PRESS ANY KEY...",80,180,tb1_pal[4],tb1_pal[0],0,tb1_font,vis);
       while( (ch=get_input())==0) usleep(30);
    }
   else {
      VMWtextxy("LOAD WHICH GAME?",80,30,tb1_pal[4],tb1_pal[0],0,tb1_font,vis);
      VMWtextxy("THE FOLLOWING EXIST:",40,40,tb1_pal[4],tb1_pal[0],0,tb1_font,vis);
      VMWtextxy("PRESS A NUMBER. (ESC CANCELS)",20,60,tb1_pal[4],tb1_pal[0],0,tb1_font,vis); 
      ch='M';
      clear_keyboard_buffer();
 IchLiebeMree:
      while( ((ch<'0') || (ch>'9')) && (ch!=TB_ESC)) {
	  while((ch=get_input())==0) usleep(30);
      }
      if (!(game_exist[ch-48])) goto IchLiebeMree;
      if(ch==TB_ESC) {
	 coolbox(0,0,320,200,1,vis);
	 VMWtextxy("LOAD CANCELED",90,95,tb1_pal[12],tb1_pal[0],0,tb1_font,vis);
         VMWtextxy("PRESS ANY KEY...",80,180,tb1_pal[4],tb1_pal[0],0,tb1_font,vis);
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
}

void story()
{
    int error;
    int xtemp,ytemp;
    int thrustcol;
    float thrust;
    char tempch;
    int alienchar;
    int cycles;
    char *tempst[300]; 
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

    GGILoadPicPacked(0,0,vaddr,1,1,
		     tb1_data_file("tbsobj.tb1",(char *)tempst),
		     (ggi_color *)&eight_bit_pal,
		     (ggi_pixel *)&tb1_pal,color_depth);
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
    ggiSetGCForeground(vis,tb1_pal[0]);
    ggiDrawBox(vis,0,0,320,200);

   VMWtextxy("THE STORY SO FAR...",20,20,tb1_pal[4],tb1_pal[0],0,tb1_font,vis);
    pauseawhile(7);
   
    ggiSetGCForeground(vis,tb1_pal[0]);
    ggiDrawBox(vis,0,0,320,200);
   
    GGILoadPicPacked(0,0,vaddr2,1,1,
		     tb1_data_file("tbcobj.tb1",(char *)tempst),
		     (ggi_color *)&eight_bit_pal,
		     (ggi_pixel *)&tb1_pal,color_depth);
    vmwArbitraryCrossBlit(plb_vaddr2->read,129,56,49,132,
			  plb_vis->write,10,10,plb_vis->stride,
			  stride_factor);
  VMWtextxy("YOU ARE TOM BOMBEM,  A STRANGE",80,10,tb1_pal[1],tb1_pal[0],0,tb1_font,vis);
  VMWtextxy("    BUT EFFICIENT MEMBER OF",80,20,tb1_pal[1],tb1_pal[0],0,tb1_font,vis);
  VMWtextxy("    THE LUNAR SPACE FORCE.",80,30,tb1_pal[1],tb1_pal[0],0,tb1_font,vis);
  VMWtextxy("YOU NEVER SAY MUCH AND YOU ARE",80,50,tb1_pal[4],tb1_pal[0],0,tb1_font,vis);
  VMWtextxy("    RARELY SEEN OUTSIDE OF",80,60,tb1_pal[4],tb1_pal[0],0,tb1_font,vis);
  VMWtextxy("    YOUR BLUE SPACESUIT.",80,70,tb1_pal[4],tb1_pal[0],0,tb1_font,vis);
  VMWtextxy("YOU OFTEN GET YOURSELF IN ",80,90,tb1_pal[2],tb1_pal[0],0,tb1_font,vis);
  VMWtextxy("    TROUBLE BY SCRATCHING",80,100,tb1_pal[2],tb1_pal[0],0,tb1_font,vis);
  VMWtextxy("    YOUR HEAD AT INAPPROPRIATE",80,110,tb1_pal[2],tb1_pal[0],0,tb1_font,vis);
  VMWtextxy("    TIMES.",80,120,tb1_pal[2],tb1_pal[0],0,tb1_font,vis);
  VMWtextxy("PRESS ANY KEY....",96,185,tb1_pal[15],tb1_pal[0],0,tb1_font,vis);
  pauseawhile(12);
   
  ggiSetGCForeground(vis,tb1_pal[0]);
  ggiDrawBox(vis,0,0,320,200);
   
  vmwArbitraryCrossBlit(plb_vaddr2->read,129,56,49,132,
		        plb_vis->write,260,10,plb_vis->stride,
			stride_factor);
  vmwArbitraryCrossBlit(plb_vaddr2->read,99,104,29,81,
			plb_vis->write,287,13,plb_vis->stride,
			stride_factor);

    VMWtextxy("IT IS THE YEAR 2028.",10,10,tb1_pal[1],tb1_pal[0],0,tb1_font,vis);
    VMWtextxy("YOU HAVE BEEN SUMMONED BY",10,30,tb1_pal[3],tb1_pal[0],0,tb1_font,vis);
    VMWtextxy("    LUNAR DICTATOR-IN-CHIEF",10,40,tb1_pal[3],tb1_pal[0],0,tb1_font,vis);
    VMWtextxy("    VINCENT WEAVER ABOUT A",10,50,tb1_pal[3],tb1_pal[0],0,tb1_font,vis);
    VMWtextxy("    TOP SECRET THREAT TO ",10,60,tb1_pal[3],tb1_pal[0],0,tb1_font,vis);
    VMWtextxy("    INTERPLANETARY SECURITY.",10,70,tb1_pal[3],tb1_pal[0],0,tb1_font,vis);
    VMWtextxy("YOU ATTEND THE BRIEFING WITH",10,90,tb1_pal[5],tb1_pal[0],0,tb1_font,vis);
    VMWtextxy("    YOUR USUAL CONFUSED",10,100,tb1_pal[5],tb1_pal[0],0,tb1_font,vis);
    VMWtextxy("    QUIETNESS.  YOU STILL DO",10,110,tb1_pal[5],tb1_pal[0],0,tb1_font,vis);  
    VMWtextxy("    NOT UNDERSTAND YOUR OWN",10,120,tb1_pal[5],tb1_pal[0],0,tb1_font,vis);
    VMWtextxy("    SUCCESSFULNESS.",10,130,tb1_pal[5],tb1_pal[0],0,tb1_font,vis);
    VMWtextxy("PRESS ANY KEY....",96,185,tb1_pal[15],tb1_pal[0],0,tb1_font,vis);
    pauseawhile(10);
   
   GGILoadPicPacked(0,0,vis,1,1,
		    tb1_data_file("tbchief.tb1",(char *)tempst),
		    (ggi_color *)&eight_bit_pal,
		    (ggi_pixel *)&tb1_pal,color_depth);

    vmwArbitraryCrossBlit(plb_vis->read,115,55,91,59,
		          plb_vaddr->write,115,55,plb_vis->stride,
		          stride_factor);
     
    pauseawhile(6);
    VMWtextxy("Ahhh.... Mr. Bombem.... ",1,1,tb1_pal[15],tb1_pal[0],0,tb1_font,vis);  
  
  /* if sbeffects then startsound(sound[1],0,false);*/
  pauseawhile(2);
   
  /*if sbeffects then startsound(sound[3],0,false);*/
    vmwArbitraryCrossBlit(plb_vaddr2->read,188,14,91,59,
			  plb_vis->write,115,55,plb_vis->stride,
			  stride_factor);
   
    pauseawhile(6);
    vmwArbitraryCrossBlit(plb_vaddr->read,115,55,91,59,
			  plb_vis->write,115,55,plb_vis->stride,
			  stride_factor);
 
    VMWtextxy("I'll be brief.                       ",1,1,tb1_pal[15],tb1_pal[0],1,tb1_font,vis);
    pauseawhile(5);
    VMWtextxy("Do you know how this base was founded?",1,1,tb1_pal[15],tb1_pal[0],1,tb1_font,vis);
    pauseawhile(5);
    VMWtextxy("No?  Well watch the screen.             ",1,1,tb1_pal[15],tb1_pal[0],1,tb1_font,vis);
    pauseawhile(5); 
    vmwArbitraryCrossBlit(plb_vaddr2->read,210,75,85,60,
			  plb_vis->write,210,136,plb_vis->stride,
			  stride_factor);
    pauseawhile(4);
   
    /*pal(250,0,0,0);*/
   

       /******BARGE TAKING OFF***********/
   
    GGILoadPicPacked(0,0,vaddr2,1,1,
		     tb1_data_file("tbma1.tb1",(char *)tempst),
		     (ggi_color *)&eight_bit_pal,
		     (ggi_pixel *)&tb1_pal,color_depth);
 
    VMWsmalltextxy("MY WIFE AND I FOUNDED",212,3,tb1_pal[14],tb1_pal[0],0,tb1_font,vaddr2);
    VMWsmalltextxy("THIS BASE IN 2008.",212,9,tb1_pal[14],tb1_pal[0],0,tb1_font,vaddr2);
    VMWsmalltextxy("THE ONLY WAY TO ",212,16,tb1_pal[13],tb1_pal[0],0,tb1_font,vaddr2);
    VMWsmalltextxy("FINANCE IT WAS TO",212,22,tb1_pal[13],tb1_pal[0],0,tb1_font,vaddr2);
    VMWsmalltextxy("ENGAGE IN A DUBIOUS",212,28,tb1_pal[13],tb1_pal[0],0,tb1_font,vaddr2);
    VMWsmalltextxy("BUSINESS.",212,34,tb1_pal[13],tb1_pal[0],0,tb1_font,vaddr2);
    VMWsmalltextxy("WE LAUNCHED EARTH'S",212,41,tb1_pal[12],tb1_pal[0],0,tb1_font,vaddr2);
    VMWsmalltextxy("TRASH INTO SPACE",212,47,tb1_pal[12],tb1_pal[0],0,tb1_font,vaddr2);
    VMWsmalltextxy("FOR A PROFIT.",212,53,tb1_pal[12],tb1_pal[0],0,tb1_font,vaddr2);
    VMWsmalltextxy("HERE IS FOOTAGE FROM",212,60,tb1_pal[11],tb1_pal[0],0,tb1_font,vaddr2);
    VMWsmalltextxy("THE LAST LAUNCH EIGHT",212,66,tb1_pal[11],tb1_pal[0],0,tb1_font,vaddr2);
    VMWsmalltextxy("YEARS AGO.",212,72,tb1_pal[11],tb1_pal[0],0,tb1_font,vaddr2);
 
    vmwCrossBlit(plb_vaddr->write,plb_vaddr2->read,plb_vis->stride,200);

/*  putshape(bargeoff,vaddr,16,18,141,157);*/
  
    vmwCrossBlit(plb_vis->write,plb_vaddr->read,plb_vis->stride,200);
    pauseawhile(7);
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
   ggiSetGCForeground(vis,tb1_pal[0]);
   ggiDrawBox(vis,0,0,320,200);   
   GGILoadPicPacked(0,0,vaddr2,1,1,
		    tb1_data_file("tbcobj.tb1",(char *)tempst),
		    (ggi_color *)&eight_bit_pal,
		    (ggi_pixel *)&tb1_pal,color_depth);
   GGILoadPicPacked(0,0,vis,1,1,
		    tb1_data_file("tbchief.tb1",(char *)tempst),
		    (ggi_color *)&eight_bit_pal,
		    (ggi_pixel *)&tb1_pal,color_depth);
    vmwArbitraryCrossBlit(plb_vaddr2->read,7,104,90,21,
		    plb_vis->write,6,174,plb_vis->stride,
		    stride_factor);
   
    VMWtextxy("You might wonder why this is important.",1,1,tb1_pal[15],tb1_pal[0],1,tb1_font,vis); 
    pauseawhile(6);
    VMWtextxy("Last week we received a message.       ",1,1,tb1_pal[15],tb1_pal[0],1,tb1_font,vis);
    pauseawhile(6);
    VMWtextxy("It is of extra-terrestrial origin.     ",1,1,tb1_pal[15],tb1_pal[0],1,tb1_font,vis);
    pauseawhile(6);
    VMWtextxy("Watch the screen.                      ",1,1,tb1_pal[15],tb1_pal[0],1,tb1_font,vis);
    
    vmwArbitraryCrossBlit(plb_vaddr2->read,210,136,85,59,
			  plb_vis->write,210,136,plb_vis->stride,
			  stride_factor);
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
    VMWtextxy("GREETINGS EARTHLINGS.",0,162,tb1_pal[12],tb1_pal[0],0,tb1_font,vis);  
    VMWtextxy("I AM GORGONZOLA THE REPULSIVE.",0,171,tb1_pal[12],tb1_pal[0],0,tb1_font,vis);
    VMWtextxy("YOU HAVE MADE A BIG MISTAKE.",0,180,tb1_pal[12],tb1_pal[0],0,tb1_font,vis);
    pauseawhile(7);
    VMWtextxy("YOUR SHIP FULL OF REFUSE HAS",0,162,tb1_pal[12],tb1_pal[0],1,tb1_font,vis);
    VMWtextxy("DAMAGED OUR OFFICIAL PEACE    ",0,171,tb1_pal[12],tb1_pal[0],1,tb1_font,vis);
    VMWtextxy("ENVOY.  IT WAS ON ITS WAY TO ",0,180,tb1_pal[12],tb1_pal[0],1,tb1_font,vis);
    VMWtextxy("YOUR PLANET.                  ",0,189,tb1_pal[12],tb1_pal[0],1,tb1_font,vis);
    pauseawhile(7);
    VMWtextxy("IN AN IRONIC FORM OF RETALLIATION",0,162,tb1_pal[12],tb1_pal[0],1,tb1_font,vis);
    VMWtextxy("WE HAVE MADE YOUR TRASH EVIL AND",0,171,tb1_pal[12],tb1_pal[0],1,tb1_font,vis);
    VMWtextxy("TURNED IT AGAINST YOU.          ",0,180,tb1_pal[12],tb1_pal[0],1,tb1_font,vis);
    VMWtextxy("        DIE EARTH SCUM!         ",0,189,tb1_pal[12],tb1_pal[0],1,tb1_font,vis);
    pauseawhile(7);
 
       /****** THIRD CHIEF *******/
   GGILoadPicPacked(0,0,vaddr2,1,1,
		    tb1_data_file("tbcobj.tb1",(char *)tempst),
		    (ggi_color *)&eight_bit_pal,
		    (ggi_pixel *)&tb1_pal,color_depth);
   GGILoadPicPacked(0,0,vis,1,1,
		    tb1_data_file("tbchief.tb1",(char *)tempst),
		    (ggi_color *)&eight_bit_pal,
		    (ggi_pixel *)&tb1_pal,color_depth);
    vmwArbitraryCrossBlit(plb_vaddr2->read,7,127,90,21,
		          plb_vis->write,6,174,plb_vis->stride,
		          stride_factor);
   
    VMWtextxy("Tom, our radar detects approaching ",1,1,tb1_pal[15],tb1_pal[0],1,tb1_font,vis);
    pauseawhile(6);
    VMWtextxy("objects.  They are inside the      ",1,1,tb1_pal[15],tb1_pal[0],1,tb1_font,vis);
    pauseawhile(6);
    VMWtextxy("orbit of Jupiter.                  ",1,1,tb1_pal[15],tb1_pal[0],1,tb1_font,vis);
    pauseawhile(6);
    VMWtextxy("You are our only hope!             ",1,1,tb1_pal[15],tb1_pal[0],1,tb1_font,vis);
    pauseawhile(6);
    VMWtextxy("Will you fly our only spaceship    ",1,1,tb1_pal[15],tb1_pal[0],1,tb1_font,vis);
    pauseawhile(6);
    VMWtextxy("and save the human race?           ",1,1,tb1_pal[15],tb1_pal[0],1,tb1_font,vis);
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
    VMWtextxy("Scratch.  Scratch. <Ow that itches>",1,1,tb1_pal[9],tb1_pal[0],1,tb1_font,vis);
    pauseawhile(5); 
    VMWtextxy("I knew you'd do it.  Good Luck!     ",1,1,tb1_pal[15],tb1_pal[0],1,tb1_font,vis);
    pauseawhile(6);
    VMWtextxy("<Huh?>                             ",1,1,tb1_pal[9],tb1_pal[0],1,tb1_font,vis);
    pauseawhile(10);
}

void credits()
{
    int i,keypressed=0;
    char *tempst[300];
   
    ggiSetGCForeground(vaddr2,tb1_pal[0]);
    ggiDrawBox(vaddr2,0,0,320,400);
   
    VMWtextxy("               TOM BOMBEM",0,210,tb1_pal[4],tb1_pal[0],
	      1,tb1_font,vaddr2);
    VMWtextxy("    INVASION OF THE INANIMATE OBJECTS",0,220,tb1_pal[4],tb1_pal[0],1,tb1_font,vaddr2);
    VMWtextxy("              PROGRAMMING",0,240,tb1_pal[9],tb1_pal[0],1,tb1_font,vaddr2);
    VMWtextxy("            VINCENT M WEAVER",0,260,tb1_pal[9],tb1_pal[0],1,tb1_font,vaddr2);
    VMWtextxy("                GRAPHICS",0,290,tb1_pal[10],tb1_pal[0],1,tb1_font,vaddr2);
    VMWtextxy("            VINCENT M WEAVER",0,310,tb1_pal[10],tb1_pal[0],1,tb1_font,vaddr2);
    VMWtextxy("             SOUND EFFECTS",0,340,tb1_pal[11],tb1_pal[0],1,tb1_font,vaddr2);
    VMWtextxy("            VINCENT M WEAVER",0,360,tb1_pal[11],tb1_pal[0],1,tb1_font,vaddr2);
    VMWtextxy("          GRAPHICS INSPIRATION",0,390,tb1_pal[12],tb1_pal[0],1,tb1_font,vaddr2);
   
    for(i=0;i<200;i++){ 
       vmwArbitraryCrossBlit(plb_vaddr2->read,0,i,320,200,
		             plb_vis->write,0,0,
			     plb_vis->stride,stride_factor);
       usleep(30000);
       if (get_input()!=0) {
	  keypressed=1; break;
       }
    }
    vmwArbitraryCrossBlit(plb_vaddr2->read,0,200,320,200,
			  plb_vaddr2->write,0,0,
			  plb_vaddr2->stride,stride_factor);
    
    ggiSetGCForeground(vaddr2,tb1_pal[0]);
    ggiDrawBox(vaddr2,0,200,320,400);
   
    VMWtextxy("              JEFF WARWICK",0,210,tb1_pal[12],tb1_pal[0],1,tb1_font,vaddr2);
    VMWtextxy("              GENERAL HELP",0,240,tb1_pal[13],tb1_pal[0],1,tb1_font,vaddr2);
    VMWtextxy("              JOHN CLEMENS",0,260,tb1_pal[13],tb1_pal[0],1,tb1_font,vaddr2);
    VMWtextxy("              JASON GRIMM",0,280,tb1_pal[13],tb1_pal[0],1,tb1_font,vaddr2);
    VMWtextxy("           PCGPE AUTHORS, esp",0,310,tb1_pal[14],tb1_pal[0],1,tb1_font,vaddr2);
    VMWtextxy("              GRANT SMITH",0,330,tb1_pal[14],tb1_pal[0],1,tb1_font,vaddr2);
    VMWtextxy("         DOS SOUND BLASTER CODE",0,360,tb1_pal[15],tb1_pal[0],1,tb1_font,vaddr2);
    VMWtextxy("              ETHAN BRODSKY",0,380,tb1_pal[15],tb1_pal[0],1,tb1_font,vaddr2);
      
    if (!keypressed) for(i=0;i<200;i++){
       vmwArbitraryCrossBlit(plb_vaddr2->read,0,i,320,200,
			     plb_vis->write,0,0,
			     plb_vis->stride,stride_factor);
       usleep(30000);
       if (get_input()!=0) { 
	  keypressed=1; break; 
       }
    }
   
    vmwArbitraryCrossBlit(plb_vaddr2->read,0,200,320,200,
	                  plb_vaddr2->write,0,0,
	    		  plb_vaddr2->stride,stride_factor);
   
    ggiSetGCForeground(vaddr2,tb1_pal[0]);
    ggiDrawBox(vaddr2,0,200,320,400);
   
    VMWtextxy("         UNIX SOUNDBLASTER CODE",0,210,tb1_pal[12],tb1_pal[0],1,tb1_font,vaddr2);
    VMWtextxy("              BRAD PITZEL",0,230,tb1_pal[12],tb1_pal[0],1,tb1_font,vaddr2);
    VMWtextxy("              PETER EKBERG",0,250,tb1_pal[12],tb1_pal[0],1,tb1_font,vaddr2);
    VMWtextxy("      THANKS TO ALL THE DEVELOPERS",0,280,tb1_pal[13],tb1_pal[0],1,tb1_font,vaddr2);
    VMWtextxy("          OF LINUX, ESPECIALLY",0,300,tb1_pal[13],tb1_pal[0],1,tb1_font,vaddr2);
    VMWtextxy("             LINUS TORVALDS",0,320,tb1_pal[13],tb1_pal[0],1,tb1_font,vaddr2);
    VMWtextxy("         ALSO SPECIAL THANKS TO",0,350,tb1_pal[14],tb1_pal[0],1,tb1_font,vaddr2);
    VMWtextxy("        THE GGI DEVELOPMENT TEAM",0,370,tb1_pal[14],tb1_pal[0],1,tb1_font,vaddr2);
    
    if (!keypressed) for(i=0;i<200;i++){
       vmwArbitraryCrossBlit(plb_vaddr2->read,0,i,320,200,
                             plb_vis->write,0,0,
			     plb_vis->stride,stride_factor);
       usleep(30000);
       if (get_input()!=0) {
	  keypressed=1; break;
       }
    }
   
    vmwArbitraryCrossBlit(plb_vaddr2->read,0,200,320,200,
	                 plb_vaddr2->write,0,0,
			 plb_vaddr2->stride,stride_factor);
   
    ggiSetGCForeground(vaddr2,tb1_pal[0]);
    ggiDrawBox(vaddr2,0,200,320,400); 
   
    VMWtextxy("               INSPIRATION",0,210,tb1_pal[9],tb1_pal[0],1,tb1_font,vaddr2);
    VMWtextxy("              DOUGLAS ADAMS",0,230,tb1_pal[9],tb1_pal[0],1,tb1_font,vaddr2);
    VMWtextxy("               CLIFF STOLL",0,250,tb1_pal[9],tb1_pal[0],1,tb1_font,vaddr2);
    VMWtextxy("             ARTHUR C CLARKE",0,270,tb1_pal[9],tb1_pal[0],1,tb1_font,vaddr2);
    VMWtextxy("               ISAAC ASIMOV",0,290,tb1_pal[9],tb1_pal[0],1,tb1_font,vaddr2);
    VMWtextxy("              GORDON KORMAN",0,310,tb1_pal[9],tb1_pal[0],1,tb1_font,vaddr2);
    VMWtextxy("         THANKS TO ALL THE AGENTS",0,340,tb1_pal[10],tb1_pal[0],1,tb1_font,vaddr2);
    VMWtextxy("        B,D,JL,L,N,P,S,W,PM,E,G,TK",0,360,tb1_pal[10],tb1_pal[0],1,tb1_font,vaddr2);
    VMWtextxy("           AND ESPECIALLY MP",0,380,tb1_pal[10],tb1_pal[0],1,tb1_font,vaddr2);
    
    if (!keypressed) for(i=0;i<200;i++){
       vmwArbitraryCrossBlit(plb_vaddr2->read,0,i,320,200,
		             plb_vis->write,0,0,
			     plb_vis->stride,stride_factor);
       usleep(30000);
       if (get_input()!=0) {
	  keypressed=1; break;
       }
   }
   vmwArbitraryCrossBlit(plb_vaddr2->read,0,200,320,200,
		         plb_vaddr2->write,0,0,
			 plb_vaddr2->stride,stride_factor);
   
   GGILoadPicPacked(0,200,vaddr2,1,1,
		    tb1_data_file("tbomb1.tb1",(char *)tempst),
		    (ggi_color *)&eight_bit_pal,
		    (ggi_pixel *)&tb1_pal,color_depth);
  
   if (keypressed) {
      ggiSetGCForeground(vaddr2,tb1_pal[0]);
      ggiDrawBox(vaddr2,0,0,320,200);
   }
   
   for(i=0;i<200;i++){
      vmwArbitraryCrossBlit(plb_vaddr2->read,0,i,320,200,
			    plb_vis->write,0,0,
			    plb_vis->stride,stride_factor);
      usleep(30000);
      if (get_input()!=0) {
	 break;
      }
  }
  vmwArbitraryCrossBlit(plb_vaddr2->read,0,200,320,200,
                        plb_vaddr2->write,0,0,
			plb_vaddr2->stride,stride_factor);
}

void about()
{
    int pagenum=1,firstime=0,oldpagenum=0,numpages=4,ch=0;
    char tempst[300];
    

   ggiSetGCForeground(vaddr,tb1_pal[0]);
   ggiFillscreen(vaddr);
      
    GGILoadPicPacked(0,0,vaddr,1,1,
		     tb1_data_file("register.tb1",(char *)&tempst),
		     (ggi_color *)&eight_bit_pal,
		     (ggi_pixel *)&tb1_pal,color_depth); 
    GGILoadPicPacked(0,0,vis,1,0,
		     tb1_data_file("register.tb1",(char *)&tempst),
		     (ggi_color *)&eight_bit_pal,
		     (ggi_pixel *)&tb1_pal,color_depth); 
   ggiSetGCForeground(vaddr2,tb1_pal[0]);
   ggiFillscreen(vaddr2);
   
   coolbox(0,0,319,199,0,vaddr);
   coolbox(0,0,319,199,1,vaddr2);
    while((ch!=TB_ESC)&&(ch!='q')){
       while(( (ch=get_input())==0) && (firstime));
       if(!firstime) firstime=1;
       if ((ch==' ') || (ch==TB_ENTER)) pagenum++;
       if ((ch==TB_RIGHT) || (ch==TB_DOWN) || (ch==TB_PGDOWN)) pagenum++;
       if ((ch==TB_LEFT) || (ch==TB_UP) || (ch==TB_PGUP)) pagenum--;
       if (pagenum>4) pagenum=1;
       if (pagenum<1) pagenum=4;
       if (oldpagenum!=pagenum){
          if (pagenum==1) {
	     /*ggiCrossBlit(vaddr,0,0,320,200,vis,0,0,320,200);*/
	     vmwCrossBlit(plb_vis->write,plb_vaddr->read,plb_vis->stride,200);
	     shadowrite("              INFORMATION",10,10,9,1,vis);
	     shadowrite("I STARTED THIS GAME IN LATE",70,30,9,1,vis);
	     shadowrite("  1994, WHEN I WAS 16.",70,40,9,1,vis);
	     shadowrite("I WROTE THIS GAME ENTIRELY IN",75,50,9,1,vis);
	     shadowrite("  MY FREE TIME.",74,60,9,1,vis);
	     shadowrite("   ^(AUTHOR 1N 1995)",10,70,10,2,vis);
	     shadowrite("ORIGINALLY THIS GAME WAS CODED IN",10,90,12,4,vis);
	     shadowrite("  TURBO PASCAL AND IN-LINE ASSEMBLY",10,100,12,4,vis);
	     shadowrite("  OPTIMIZED TO RUN ON A 386.  NOW I",10,110,12,4,vis);
	     shadowrite("  HAVE PORTED IT TO LINUX AND GGI.",10,120,12,4,vis);
	     shadowrite("  IN THAT SPIRIT I HAVE NOW GPL'ED",10,130,12,4,vis);
	     shadowrite("  THE CODE.",10,140,12,4,vis);
          } else vmwCrossBlit(plb_vis->write,plb_vaddr2->read,plb_vis->stride,200); 
	    /*ggiCrossBlit(vaddr2,0,0,320,200,vis,0,0,320,200);*/
          if (pagenum==2){
	     shadowrite("MY NAME IS VINCE WEAVER",10,10,10,2,vis);
	     shadowrite("   VISIT MY TALKER",10,20,10,2,vis);
	     shadowrite("   DERANGED.STUDENT.UMD.EDU 7000",10,30,10,2,vis);
	     shadowrite("   UP WHENEVER SCHOOL IS IN SESSION",10,40,10,2,vis);
	     shadowrite("UNTIL MAY OF 2000 I WILL BE",10,60,13,5,vis);
	     shadowrite(" ATTENDING COLLEGE, AT THE UNIVERSITY",10,70,13,5,vis);
	     shadowrite(" OF MARYLAND, COLLEGE PARK.",10,80,13,5,vis);
	     shadowrite("GET THE NEWEST VERSION OF TB1 AT",10,100,11,3,vis);
	     shadowrite(" THE OFFICIAL TB1 WEB SITE:",10,110,11,3,vis);
	     shadowrite(" http://www.glue.umd.edu/~weave/tb1/",10,120,11,3,vis);
	     shadowrite("I CAN BE CONTACTED VIA E-MAIL AT:",10,140,12,4,vis);
	     shadowrite("    WEAVE@ENG.UMD.EDU",10,150,9,1,vis);
	     shadowrite("FEEL FREE TO SEND COMMENTS.",10,160,12,4,vis);
          }
          if (pagenum==3){
	     shadowrite("OTHER VMW SOFTWARE PRODUCTIONS:",10,10,15,7,vis);
	     shadowrite(" PAINTPRO:",10,30,13,5,vis);
 	     shadowrite("   LOAD AND SAVE GRAPHICS PICTURES",10,40,13,5,vis);
 	     shadowrite(" LINUX_LOGO",10,50,11,3,vis);
 	     shadowrite("   A USERLAND ANSI LOGIN DISPLAY",10,60,11,3,vis);
 	     shadowrite(" SPACEWAR III:",10,70,9,1,vis);
 	     shadowrite("   NEVER COMPLETED GAME",10,80,9,1,vis);
	     shadowrite(" AITAS: (ADVENTURES IN TIME AND SPACE)",10,90,12,4,vis);
	     shadowrite("   A GAME I'VE BEEN WANTING TO WRITE",10,100,12,4,vis);
	     shadowrite("   FOR 5 YEARS.  [INCOMPLETE]",10,110,12,4,vis);
	     shadowrite(" FONT_PRINT",10,120,9,1,vis);
	     shadowrite("   PRINT VGA FONTS IN DOS AND LINUX",10,130,9,1,vis);
	     shadowrite(" SEABATTLE:",10,140,13,5,vis);
	     shadowrite("   A BATTLESHIP CLONE CODED IN C",10,150,13,5,vis);
          }
          if (pagenum==4){
             shadowrite("DISCLAIMER:",10,10,12,14,vis);
	     shadowrite("* MY PROGRAMS SHOULD NOT DAMAGE YOUR *",8,30,12,4,vis);
	     shadowrite("* COMPUTER IN ANY WAY.  PLEASE DON'T *",8,40,12,4,vis);
	     shadowrite("* USE MY SOFTWARE IN CRITICAL        *",8,50,12,4,vis);
	     shadowrite("* APPLICATIONS LIKE LIFE-SUPPORT     *",8,60,12,4,vis);
	     shadowrite("* EQUIPMENT, DEFLECTOR SHIELDS, OR   *",8,70,12,4,vis);
	     shadowrite("* AUTOMOBILE ENGINES.                *",8,80,12,4,vis);
	     shadowrite("* LINUX FOREVER! THE OS FOR EVERYONE *",8,90,12,4,vis);
	     shadowrite("% WARRANTY ESPECIALLY VOID IF USED   %",8,110,11,3,vis);
	     shadowrite("% ON ANY MICROSOFT(tm) OS (YUCK)     %",8,120,11,3,vis);
          }
          sprintf(tempst,"Page %d of %d: ESC QUITS",pagenum,numpages);
          shadowrite(tempst,50,180,15,7,vis);
	  
	  ggiFlush(vis);
          oldpagenum=pagenum;
       }
    }
    GGILoadPicPacked(0,0,vis,1,0,
		     tb1_data_file("tbgorg.tb1",(char *)&tempst),
		     (ggi_color *)&eight_bit_pal,
		     (ggi_pixel *)&tb1_pal,color_depth);
   
}


void playthegame(int lev,int sc,int sh)
{
    int shields,score,level;
   
    level=lev;
    score=sc;
    shields=sh;  
   
    ggiSetGCForeground(vaddr2,tb1_pal[0]);
    ggiDrawBox(vis,0,0,320,200);
   
    if (level==0) {
       littleopener();
       shields=12;
       score=0;
       level++;
    }
   
    if (level==1) {
       levelone(&level,&shields,&score);
       if(level==2) littleopener2();
    }
    if (level==2) {
       leveltwoengine(&level,&shields,&score);
    }
    if (level==3) {
       /*littleopener3();
       levelthree();*/
    }
    if (level==4) {
       leveltwoengine(&level,&shields,&score);
    }
    coolbox(70,85,170,110,1,vis);
    VMWtextxy("GAME OVER",84,95,tb1_pal[4],tb1_pal[7],0,tb1_font,vis);
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

