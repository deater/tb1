/*
 Level 1 Engine Code for Tom Bombem
 */

    /* The Includes */
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <ggi/libggi.h>
#include <stdlib.h>
#include "svmwgrap.h"
#include "levels.h"
#include "tblib.h"
#include "soundIt.h"

    /* Define this to get a frames per second readout */
/* #define DEBUG_ON */

    /* The sounds */
#define NUM_SAMPLES 4
#define SND_CC    0
#define SND_BONK  1
#define SND_KAPOW 2
#define SND_ZRRP  3

    /* The global variables ;)  I like global variables hehe */
extern vmw_font *tb1_font;
extern ggi_visual_t vis;
extern ggi_visual_t vaddr;
extern ggi_visual_t vaddr2;
extern ggi_color eight_bit_pal[256];
extern ggi_pixel tb1_pal[256];
extern ggi_directbuffer_t dbuf_vis;
extern ggi_directbuffer_t dbuf_vaddr;
extern ggi_directbuffer_t dbuf_vaddr2;
extern ggi_pixellinearbuffer *plb_vis;
extern ggi_pixellinearbuffer *plb_vaddr;
extern ggi_pixellinearbuffer *plb_vaddr2;
extern int sound_possible;
extern int sound_enabled;
extern int color_depth;
extern int stride_factor;

    /* I like structures also ;) */
struct enemyinfo {
  int x,y;
  int kind;
  int out,exploding,boundarycheck,dead;
  int explodeprogress;
  int minx,maxx,xspeed,yspeed;
  int hitsneeded;
};

struct bulletinfo {
  int out,x,y;
};

    /* Define how many sound effects there are */
Sample sound_effects[NUM_SAMPLES];

    /* o/~ more structures o/~ */
struct enemyinfo enemy[5];
struct bulletinfo bullet[3];
struct timeval timing_info;
struct timezone dontcare;

    /* This seemed like a good idea to modularize things */
int level_one_wave_behavior[]=
    {0,0,0,0,0, 1,1,1,1,1,
     1,1,2,2,2, 2,2,2,2,2,
     3,3,3,3,3, 3,3,3,3,3,
     2,2,2,2,2, 2,3,3,3,3,
     3,3,3,3,3, 3,1,1,1,1,
     1,3,3,3,3, 3,3,3,3,3,
     3,2,2,2,2, 2,2,2,2,2,
     2,2,2,2,2, 2,1,1,1,1,
     1,1,1,3,3, 3,2,2,2,2,
     2,2,2,2,2, 2,1,1,1,1,
     1,4,4,4,4};

    /* Yes I was too lazy to re-arrange the order of the functions */
void beforeboss();
    
    /* Defines the behavior of the objects in level 1 */
int level_one_behavior(int reset)
{
    int what,temp,whichone,need_to_pause=0;
    static int wave=0;
    static int saucersout=0;
   
    if (reset) { 
       wave=0;
       saucersout=0;
    }
   
    if (level_one_wave_behavior[wave]!=4) wave++;
    saucersout--;
    if (saucersout<0) saucersout=0;
    if (saucersout>5) saucersout=5;
 
    /* **START NEW WAVE ***/

    switch(level_one_wave_behavior[wave]) {
       
       /* STANDARD */
     case 0: if (saucersout==0) {
       saucersout=5;
       what=(3+rand()%8);
       for(temp=0; temp<5; temp++) { 
          enemy[temp].kind=what;
          enemy[temp].x=0;
          enemy[temp].y=0;
          enemy[temp].xspeed=5;
          enemy[temp].x=temp*20;
          enemy[temp].minx=(temp*20);
          enemy[temp].maxx=(temp*20)+120;
          enemy[temp].boundarycheck=1;
          enemy[temp].yspeed=10;
          enemy[temp].out=1;
          enemy[temp].exploding=0;
          enemy[temp].hitsneeded=1;
          enemy[temp].dead=0;
       }
     } break;
       /* *FALLING STRAIGHT* */
     case 3:
       for (temp=0; temp<5;temp++)
           if (!enemy[temp].out) {
	      enemy[temp].kind=rand()%8+3;
              enemy[temp].x=rand()%200+1;
              enemy[temp].y=0;
              enemy[temp].xspeed=0;
              enemy[temp].minx=enemy[temp].x;
              enemy[temp].maxx=enemy[temp].x;
              enemy[temp].boundarycheck=1;
              enemy[temp].yspeed=5+(wave/40);
              enemy[temp].out=1;
              enemy[temp].exploding=0;
              enemy[temp].hitsneeded=1;
              enemy[temp].dead=0;
              saucersout++;
	  } break;
    
       /* *FALLING GRADUALLY SIDEWAYS* */
     case 2:
       for(temp=0;temp<5;temp++)
          if (!enemy[temp].out) {
	     enemy[temp].kind=rand()%8+3;
             enemy[temp].y=0;
             enemy[temp].xspeed=5;
             enemy[temp].minx=rand()%100;
             enemy[temp].maxx=rand()%100+120;
             enemy[temp].x=enemy[temp].minx;
             enemy[temp].boundarycheck=0;
             enemy[temp].yspeed=1;
             enemy[temp].out=1;
             enemy[temp].exploding=0;
             enemy[temp].hitsneeded=1;
             enemy[temp].dead=0;
	     saucersout++;
	  } break;
    
       /**ZIG-ZAG**/
     case 1: if (!saucersout) {
       saucersout=5;
       whichone=rand()%8+3;
       for(temp=0;temp<5;temp++)  
	  if (!enemy[temp].out) {
             enemy[temp].kind=whichone;
             enemy[temp].y=temp*10;
             enemy[temp].xspeed=5;
             enemy[temp].minx=0;
             enemy[temp].maxx=220;
             enemy[temp].x=temp*20;
             enemy[temp].boundarycheck=0;
             enemy[temp].yspeed=1;
             enemy[temp].out=1;
             enemy[temp].exploding=0;
             enemy[temp].hitsneeded=1;
             enemy[temp].dead=0;
	  } 
	} break;
       
       /* Beginning of Boss */
      case 4:
       if (!saucersout) {
          beforeboss();
	  need_to_pause=1;
          enemy[0].kind=15;
          enemy[1].kind=15;
          enemy[2].kind=14;
          for(temp=0;temp<3;temp++) {
             enemy[temp].x=(temp*20)+10;
             enemy[temp].y=0;
             enemy[temp].xspeed=5;
             enemy[temp].minx=0;
             enemy[temp].maxx=220;
             enemy[temp].boundarycheck=1;
             enemy[temp].yspeed=0;
             enemy[temp].out=1;
             enemy[temp].exploding=0;
             enemy[temp].hitsneeded=3;
             enemy[temp].dead=0;
             saucersout++;
	  }
       } break;
     default: break;
    }
     
       /* Objects Cast off by the Boss */
    if (enemy[1].kind==15) {
          /* Detect if Level One is Over */
       if ((enemy[0].dead) && (enemy[1].dead) && (enemy[2].dead)) return 9;
          for(temp=3;temp<5;temp++) {
             saucersout++;
             if ((!enemy[temp].out) && (enemy[temp-3].out)) {
	        enemy[temp].kind=rand()%8+3;
                enemy[temp].x=enemy[temp-3].x;
                enemy[temp].y=20;
                enemy[temp].xspeed=0;
                enemy[temp].minx=enemy[temp].x;
                enemy[temp].maxx=enemy[temp].x;
                enemy[temp].boundarycheck=0;
                enemy[temp].yspeed=4;
                enemy[temp].out=1;
                enemy[temp].exploding=0;
                enemy[temp].hitsneeded=1;
                enemy[temp].dead=0;
	     }
	  }
       }
    return need_to_pause;
}
 
    /* The little Sequence Before you hit the Boss */
void beforeboss()
{
    char *tempst[300];

    clear_keyboard_buffer();
    GGILoadPicPacked(0,0,vaddr,0,1,
		     tb1_data_file("viewscr.tb1",(char *)tempst),
		     (ggi_color *)&eight_bit_pal,
		     (ggi_pixel *)&tb1_pal,color_depth);
    ggiSetGCForeground(vis,0);
    ggiDrawBox(vis,0,0,320,200);
    vmwArbitraryCrossBlit(plb_vaddr->read,0,5,58,37,
			  plb_vis->write,10,10,plb_vis->stride,stride_factor);
    VMWsmalltextxy("HUMAN!",70,10,tb1_pal[2],tb1_pal[0],1,tb1_font,vis);
    VMWsmalltextxy("WHAT ARE YOU DOING?!",70,20,tb1_pal[2],
		                                tb1_pal[0],1,tb1_font,vis);
    VMWsmalltextxy("YOUR SPECIES MUST BE TERMINATED!",70,30,tb1_pal[2],
		                                tb1_pal[0],1,tb1_font,vis);
    pauseawhile(5);
    vmwArbitraryCrossBlit(plb_vaddr->read,0,42,58,37,
			  plb_vis->write,10,50,plb_vis->stride,stride_factor);
    VMWsmalltextxy("I'M SORRY.",70,50,tb1_pal[9],tb1_pal[0],1,tb1_font,vis);
    VMWsmalltextxy("WE DIDN'T MEAN TO DESTROY YOUR ENVOY.",70,60,tb1_pal[9],
		                                tb1_pal[0],1,tb1_font,vis);
    VMWsmalltextxy("WILL YOU FORGIVE US AND TRY PEACE?",70,70,tb1_pal[9],
		                                tb1_pal[0],1,tb1_font,vis);
    pauseawhile(5);
    vmwArbitraryCrossBlit(plb_vaddr->read,0,5,58,37,
			  plb_vis->write,10,90,plb_vis->stride,stride_factor);
    VMWsmalltextxy("NO!  YOU MUST BE DESTROYED!",70,90,tb1_pal[2],
		                                tb1_pal[0],1,tb1_font,vis);
    VMWsmalltextxy("OUR FUNDING ... OUR ENVOY WAS DAMAGED BY",70,100,
		                     tb1_pal[2],tb1_pal[0],1,tb1_font,vis);
    VMWsmalltextxy("YOU!  VENGEANCE WILL BE OURS!  YOUR PUNY",70,110,
		                     tb1_pal[2],tb1_pal[0],1,tb1_font,vis);
    VMWsmalltextxy("PRIMITIVE SPACECRAFT WITH ITS INFERIOR",70,120,
		                     tb1_pal[2],tb1_pal[0],1,tb1_font,vis);
    VMWsmalltextxy("WEAPONS WOULD HAVE TO SCORE 9 DIRECT HITS",70,130,
		                     tb1_pal[2],tb1_pal[0],1,tb1_font,vis);
    VMWsmalltextxy("TO DESTROY MY SHIP!  DIE EARTH SCUM!!!!",70,140,
		                     tb1_pal[2],tb1_pal[0],1,tb1_font,vis);
    pauseawhile(5);
}

    /* The Sequence After You Defeat (hopefully) the Boss */
void afterboss()
{
    char *tempst[300];
   
    GGILoadPicPacked(0,0,vaddr,0,1,
		     tb1_data_file("viewscr.tb1",(char *)tempst),
		     (ggi_color *)&eight_bit_pal,
		     (ggi_pixel *)&tb1_pal,color_depth);
    ggiSetGCForeground(vis,tb1_pal[0]);
    ggiDrawBox(vis,0,0,320,200);
    vmwArbitraryCrossBlit(plb_vaddr->read,0,42,58,37,
			  plb_vis->write,10,10,plb_vis->stride,stride_factor);
    VMWsmalltextxy("HMM.. THEY DON'T BUILD SUPERIOR",70,10,
		          tb1_pal[9],tb1_pal[0],1,tb1_font,vis);
    VMWsmalltextxy("TECHNOLOGY LIKE THEY USED TO.",70,20,
		          tb1_pal[9],tb1_pal[0],1,tb1_font,vis);
    VMWsmalltextxy("I GUESS I CAN GO HOME NOW.",70,30,
		          tb1_pal[9],tb1_pal[0],1,tb1_font,vis);
    pauseawhile(5);
    vmwArbitraryCrossBlit(plb_vaddr->read,0,5,58,37,
			  plb_vis->write,10,50,plb_vis->stride,stride_factor);
    VMWsmalltextxy("NOT SO FAST!  YOU JUST DESTROYED AN ANTIQUATED",70,50,
		          tb1_pal[2],tb1_pal[0],1,tb1_font,vis);
    VMWsmalltextxy("DEFENSE SYSTEM THAT WAS PROGRAMMED BY A 16",70,60,
		          tb1_pal[2],tb1_pal[0],1,tb1_font,vis);
    VMWsmalltextxy("YEAR OLD!  OUR MAIN DEFENSE PROGRAMMER HAS ",70,70,
		          tb1_pal[2],tb1_pal[0],1,tb1_font,vis);
    VMWsmalltextxy("MUCH MORE SKILL NOW!  UNLESS YOU DESTROY OUR",70,80,
		          tb1_pal[2],tb1_pal[0],1,tb1_font,vis);
    VMWsmalltextxy("ENTIRE XENOCIDE... I MEAN PEACE... ENVOY",70,90,
		          tb1_pal[2],tb1_pal[0],1,tb1_font,vis);
    VMWsmalltextxy("WE WILL STILL DESTROY YOUR HOME PLANET.",70,100,
		          tb1_pal[2],tb1_pal[0],1,tb1_font,vis);
    VMWsmalltextxy("NICE TRY PUNY EARTHLING!",70,110,tb1_pal[2],
		          tb1_pal[0],1,tb1_font,vis);
    pauseawhile(7);
    vmwArbitraryCrossBlit(plb_vaddr->read,0,42,58,37,
			  plb_vis->write,10,130,plb_vis->stride,stride_factor);
    VMWsmalltextxy("HMM.. I GUESS I BETTER SAVE THE EARTH.",70,130,
		          tb1_pal[9],tb1_pal[0],1,tb1_font,vis);
    VMWsmalltextxy("I'D BETTER SAVE MY GAME TOO.",70,140,
		          tb1_pal[9],tb1_pal[0],1,tb1_font,vis);
    VMWsmalltextxy("D'OH!  I''M OUT OF BIG MISSILES! ",70,150,
		          tb1_pal[9],tb1_pal[0],1,tb1_font,vis);
    VMWsmalltextxy("WELL AT LEAST I HAVE SOME SMALLER SPARES.",70,160,
		          tb1_pal[9],tb1_pal[0],1,tb1_font,vis);
    pauseawhile(5);
}

    /* The Main Level One */
void levelone(int *level,int *shields,int *score)
{
    int ch=0;
    int i,j,grapherror;
    char tempst[300];
    int itemp,whatdelay=1,levelover=0;
    int shipx=36,shipadd=0,shipframe=1;
    int bigship1[1500],bigship2[1500],bigship3[1500];
    int shapetable[20][400];
    long oldsec,oldusec,time_spent;
    int howmuchscroll=0;
    int speed_factor=1,game_paused=0;
    int beginscore,beginshield;
   
       /* Set this up for Save Game */
    beginscore=*score;
    beginshield=*shields;
   
       /* Load All The Sounds */
    if (sound_possible) {
       Snd_loadRawSample(tb1_data_file("tbcc.raw",(char *)&tempst),
		      &sound_effects[0],0);
       Snd_loadRawSample(tb1_data_file("tbbonk.raw",(char *)&tempst),
		      &sound_effects[1],0);
       Snd_loadRawSample(tb1_data_file("tbkapow.raw",(char *)&tempst),
		      &sound_effects[2],0);
       Snd_loadRawSample(tb1_data_file("tbzrrp.raw",(char *)&tempst),
		      &sound_effects[3],0);
       
          /* Initialize Sound, if Possible */
       if (Snd_init( NUM_SAMPLES,sound_effects,22050,4,
		                             "/dev/dsp")==EXIT_FAILURE) {
          printf("ERROR! Cannot init sound.\n");
          sound_enabled=0;
       }
    }
       /* Load Sprites (or whatever you want to call them) */
    grapherror=GGILoadPicPacked(0,0,vaddr,1,1,
				tb1_data_file("ships.tb1",(char *)&tempst),
				(ggi_color *)&eight_bit_pal,
				(ggi_pixel *)&tb1_pal,color_depth);
    vmwGetSprite(vaddr,0,0,48,30,(int *)&bigship1);
    vmwGetSprite(vaddr,0,32,48,30,(int *)&bigship2);
    vmwGetSprite(vaddr,0,64,48,30,(int *)&bigship3);
   
    grapherror=GGILoadPicPacked(0,0,vaddr,1,1,
				tb1_data_file("tbshapes.tb1",(char *)&tempst),
				(ggi_color *)&eight_bit_pal,
				(ggi_pixel *)&tb1_pal,color_depth);
    for(j=0;j<2;j++) 
       for(i=0;i<10;i++) 
          vmwGetSprite(vaddr,1+(i*19),1+(j*19),18,20,
		       (int *)&shapetable[(j*10)+i]);

       /* Set up initial Enemy Structs */
    for(i=0;i<5;i++) {
       enemy[i].exploding=0;
       enemy[i].out=0;
       enemy[i].dead=0;
    }
    for(i=0;i<2;i++) {
       bullet[i].out=0;  
       bullet[i].x=0; 
       bullet[i].y=0;
    }
       /* Setup and draw the sidebar */
    setupsidebar(*score,0,*shields);
    ggiSetGCForeground(vis,tb1_pal[0]);
    vmwCrossBlit(plb_vaddr->write,plb_vaddr2->read,plb_vaddr->stride,200);
    sprintf(tempst,"%d",*level);
    ggiDrawBox(vaddr2,251,52,63,7);
    VMWtextxy(tempst,307,51,tb1_pal[12],tb1_pal[0],0,tb1_font,vaddr2);

       /* Clear the screen and draw the stars */
    ggiSetGCForeground(vaddr2,tb1_pal[0]);
    ggiDrawBox(vaddr2,0,0,320,400);
    for(i=0;i<100;i++) {
       vmwPutSprite(shapetable[11],18,18,stride_factor,
		    plb_vaddr2->write,rand()%238,
		    rand()%400,plb_vaddr2->stride);
       vmwPutSprite(shapetable[12],18,18,stride_factor,
		    plb_vaddr2->write,rand()%238,
		    rand()%400,plb_vaddr2->stride);
    }
    change_shields(shields);

       /* Draw the Little Box announcing the Start of the Level */
    ggiSetGCForeground(vis,tb1_pal[0]);
    ggiDrawBox(vis,0,0,320,200);
    coolbox(70,85,240,120,1,vis);
    VMWtextxy("   LEVEL ONE:",84,95,tb1_pal[4],tb1_pal[7],0,tb1_font,vis);
    VMWtextxy("INANIMATE OBJECTS",84,105,tb1_pal[4],tb1_pal[7],0,tb1_font,vis);

       /* Initiate some last variables */
    level_one_behavior(1);
    pauseawhile(5); 
    gettimeofday(&timing_info,&dontcare);
    oldsec=timing_info.tv_sec; oldusec=timing_info.tv_usec;
   
       /* MAIN GAME LOOP */
    while(!levelover) {
       ch=0;
          /* Scroll the Stars */
       if (speed_factor>1) howmuchscroll-=speed_factor; 
       else howmuchscroll--; 
       if (howmuchscroll<0) howmuchscroll=399;
       if (howmuchscroll>199) {
	  vmwArbitraryCrossBlit(plb_vaddr2->read,0,howmuchscroll,240,
				400-howmuchscroll,
				plb_vaddr->write,0,0,plb_vaddr->stride,
				stride_factor);
	  vmwArbitraryCrossBlit(plb_vaddr2->read,0,0,240,howmuchscroll-200,
			        plb_vaddr->write,0,400-howmuchscroll,
				plb_vaddr->stride,
			        stride_factor); 
       }
       else {
	  vmwArbitraryCrossBlit(plb_vaddr2->read,0,howmuchscroll,240,200,
				plb_vaddr->write,0,0,plb_vaddr->stride,
				stride_factor);
       }
       
	 
          /* Check for Collisions */
       for(i=0;i<5;i++) {
          if (!enemy[i].dead) {
             for(itemp=0;itemp<2;itemp++) {
	        if (bullet[itemp].out) 
                   if (collision(bullet[itemp].x,bullet[itemp].y,10,10,
                       enemy[i].x,enemy[i].y,9,9)) {
                      if (sound_enabled) Snd_effect(SND_KAPOW,2);
                      enemy[i].hitsneeded--;
                      if (enemy[i].hitsneeded<1) enemy[i].dead=1;
                      else enemy[i].dead=0;
                      enemy[i].exploding=1;
                      enemy[i].explodeprogress=0;
                      bullet[itemp].out=0;
                      (*score)+=10;
                      changescore(*score,shields);
		   }
             }
	  }
       }
       
          /* Explode the things that are exploding */
       for(i=0;i<5;i++) {
          if (enemy[i].exploding) {
             enemy[i].explodeprogress++;
             if (enemy[i].explodeprogress<=5)
	        vmwPutSprite(shapetable[enemy[i].explodeprogress+14],
			     18,18,stride_factor,
			     plb_vaddr->write,enemy[i].x,enemy[i].y,
			     plb_vaddr->stride);
	     else if (enemy[i].dead) {
                enemy[i].out=0;
                enemy[i].exploding=0;
                game_paused=level_one_behavior(0);
	     }
             else enemy[i].exploding=0;
	  }
       }
          /* Move the Missiles */
       for(i=0;i<2;i++) { 
          if (bullet[i].out) {
             if (speed_factor>1) bullet[i].y-=(5*speed_factor); 
	     else bullet[i].y-=5;
             if (bullet[i].y<5) bullet[i].out=0;
             else vmwPutSprite(shapetable[0],18,18,stride_factor,
			       plb_vaddr->write,bullet[i].x,bullet[i].y,
			       plb_vaddr->stride);
	  }
       }
       
          /* MOVE ENEMIES */
       for(i=0;i<5;i++) {
          if ((enemy[i].out) && (!enemy[i].dead)) {
	     vmwPutSprite(shapetable[enemy[i].kind-1],18,18,stride_factor,
		          plb_vaddr->write,enemy[i].x,enemy[i].y,
			  plb_vaddr->stride);
	     if (speed_factor==1) enemy[i].x+=enemy[i].xspeed;
	     else enemy[i].x+=(enemy[i].xspeed*speed_factor);
                /* Check Position */
	                     /* Check Position */
             if (!enemy[i].boundarycheck) 
	        if (speed_factor>1) enemy[i].y+=(enemy[i].yspeed*speed_factor);
                else enemy[i].y+=enemy[i].yspeed;
	     if ((enemy[i].x<=enemy[i].minx) || (enemy[i].x>=enemy[i].maxx)) {
                enemy[i].xspeed=-enemy[i].xspeed;
                if (speed_factor>1) enemy[i].x+=(enemy[i].xspeed*speed_factor);
	        else enemy[i].x+=enemy[i].xspeed;
                if (speed_factor>1) enemy[i].y+=(enemy[i].yspeed*speed_factor); 
	        else enemy[i].y+=enemy[i].yspeed;
	     }
                /* Too Low */

                /* Too Low */
             if (enemy[i].y>179) {
                enemy[i].out=0;
                game_paused=level_one_behavior(0);
	     }
             if (enemy[i].y>140) {
                if (collision(shipx,165,24,15,enemy[i].x,enemy[i].y,9,9)) {
                   if (sound_enabled) Snd_effect(SND_BONK,1);
                   enemy[i].hitsneeded--;
                   if (enemy[i].hitsneeded==0) enemy[i].dead=1;
                   else enemy[i].dead=0;
                   enemy[i].exploding=1;
                   enemy[i].explodeprogress=0;
                   (*shields)--;
                   if (*shields<0) levelover=1;
		   if (*shields>0) change_shields(shields);
		}
	     }
	  }
       }
          /* See if beat the level.  Yes, bad variable name.  Oh well */
       if (game_paused==9) {
          afterboss();
          *level=2;
          levelover=1;
       }
       
          /* **READ KEYBOARD** */
       if ( (ch=get_input())!=0) {
          switch(ch){  
	   case TB_ESC: levelover=1; break;
	   case TB_RIGHT: if (shipadd>=0) shipadd+=3; else shipadd=0; break;
           case TB_LEFT: if (shipadd<=0) shipadd-=3; else shipadd=0; break;
           case TB_F1: game_paused=1; help(); break;
           case '+': whatdelay++; if (whatdelay>25) whatdelay=25; break;
	   case 'P': case 'p': game_paused=1; 
	             coolbox(65,85,175,110,1,vis);
	             VMWtextxy("GAME PAUSED",79,95,tb1_pal[4],tb1_pal[7],
			                    0,tb1_font,vis);
	             while (get_input()==0) {
			usleep(30000);
		     }
	             break;
	   case '-': whatdelay--; if (whatdelay<1) whatdelay=1; break;
	   case 'S': 
	   case 's': if(sound_possible) sound_enabled=!(sound_enabled); break;
           case TB_F2: game_paused=1; savegame(*level,beginscore,beginshield);
	               break;
           case ' ':  for(j=0;j<2;j++)
	                if (!bullet[j].out) {
                           if (sound_enabled) Snd_effect(SND_CC,0);
                           bullet[j].out=1;
                           bullet[j].x=shipx+15;
                           bullet[j].y=165;
		           vmwPutSprite(shapetable[0],18,18,stride_factor,
				        plb_vaddr->write,bullet[j].x,
					bullet[j].y,plb_vaddr->stride);
		           j=3;
			}
	  }
       }

          /* **MOVE SHIP** */
       if (speed_factor>1) shipx+=(shipadd*speed_factor); 
       else shipx+=shipadd;
       if (shipx<1) shipx=1;
       if (shipx>190) shipx=190;
       switch(shipframe) {
        case 1: vmwPutSprite(bigship1,48,30,stride_factor,
			     plb_vaddr->write,shipx,165,
		             plb_vaddr->stride); break;
        case 3: vmwPutSprite(bigship2,48,30,stride_factor,
			     plb_vaddr->write,shipx,165,
			     plb_vaddr->stride); break;
	case 2:
	case 4: vmwPutSprite(bigship3,48,30,stride_factor,
			     plb_vaddr->write,shipx,165,
			     plb_vaddr->stride); break;
       }
       shipframe++;
       if (shipframe==5) shipframe=1;
       
          /* Flip Pages */
       vmwCrossBlit(plb_vis->write,plb_vaddr->read,plb_vis->stride,200);
       ggiFlush(vis);
       
          /* Calculate how much time has passed */
       gettimeofday(&timing_info,&dontcare);
       time_spent=timing_info.tv_usec-oldusec;
       if (timing_info.tv_sec-oldsec) time_spent+=1000000;
#ifdef DEBUG_ON
       printf("%f\n",1000000/(float)time_spent);
#endif
          /* If time passed was too little, wait a bit */
       while (time_spent<33000){
	  gettimeofday(&timing_info,&dontcare);
	  usleep(5);
	  time_spent=timing_info.tv_usec-oldusec;
	  if (timing_info.tv_sec-oldsec) time_spent+=1000000;
       }
          /* It game is paused, don't keep track of time */
       if (!game_paused) speed_factor=(time_spent/30000);
       oldusec=timing_info.tv_usec;
       oldsec=timing_info.tv_sec;
       if (game_paused) {
	  gettimeofday(&timing_info,&dontcare);
	  oldusec=timing_info.tv_usec;
	  oldsec=timing_info.tv_sec;
	  game_paused=0;
       }
    }
       /* All Done.  Close up sound */
    Snd_restore();
}

    /* The little opener before Level 1 */
void littleopener()
{
    int ship1[400],ship2[400];
    int i;
    char *tempst[300];
   
    ggiSetGCForeground(vaddr,tb1_pal[0]);
    ggiDrawBox(vaddr,0,0,320,200);
    GGILoadPicPacked(0,0,vaddr,1,1,
		     tb1_data_file("moon2.tb1",(char *)tempst),
		     (ggi_color *)&eight_bit_pal,
		     (ggi_pixel *)&tb1_pal,color_depth);
    GGILoadPicPacked(0,0,vis,1,0,
		     tb1_data_file("moon2.tb1",(char *)tempst),
		     (ggi_color *)&eight_bit_pal,
		     (ggi_pixel *)&tb1_pal,color_depth);
    vmwGetSprite(vaddr,9,178,15,18,ship1);
    vmwGetSprite(vaddr,30,178,15,18,ship2);
    ggiSetGCForeground(vis,tb1_pal[0]);
    ggiDrawBox(vaddr,0,178,319,21);
    vmwCrossBlit(plb_vis->write,plb_vaddr->read,plb_vis->stride,200);
    vmwCrossBlit(plb_vaddr2->write,plb_vaddr->read,plb_vaddr->stride,200);
    for(i=100;i>0;i--) {
       vmwArbitraryCrossBlit(plb_vaddr2->read,0,95,320,40,
			     plb_vaddr->write,0,95,
			     plb_vaddr->stride,stride_factor);
       vmwPutSprite(ship2,15,18,stride_factor,
		    plb_vaddr->write,i*2,100,plb_vaddr->stride);
       vmwArbitraryCrossBlit(plb_vaddr->read,0,95,320,40,
			     plb_vis->write,0,95,plb_vaddr->stride,
			     stride_factor);
       usleep(30000);
       if (get_input()!=0) break;       
    }
    vmwCrossBlit(plb_vis->write,plb_vaddr2->read,plb_vis->stride,200);
    VMWtextxy(">KCHK< TOM! WHERE ARE YOU GOING?",5,180,tb1_pal[15],tb1_pal[0],1,tb1_font,vis);
    ggiFlush(vis); 
    pauseawhile(3); 
    ggiDrawBox(vis,0,178,319,21); 
    VMWtextxy("Ooops. ",5,180,tb1_pal[24],tb1_pal[0],1,tb1_font,vis);
    pauseawhile(3);
    for(i=0;i<151;i++) {
       vmwArbitraryCrossBlit(plb_vaddr2->read,0,95,320,40,
			     plb_vaddr->write,0,95,plb_vaddr->stride,
			     stride_factor);
       vmwPutSprite(ship1,15,18,stride_factor,
		    plb_vaddr->write,i*2,100,plb_vaddr->stride);
       vmwArbitraryCrossBlit(plb_vaddr->read,0,95,320,40,
			     plb_vis->write,0,95,plb_vaddr->stride,
			     stride_factor);
       usleep(30000);
       if (get_input()!=0) break;
    }
    vmwCrossBlit(plb_vis->write,plb_vaddr2->read,plb_vis->stride,200);
    ggiSetGCForeground(vis,tb1_pal[0]);
    ggiDrawBox(vis,0,0,320,200);
    GGILoadPicPacked(0,0,vis,1,0,
		     tb1_data_file("tbgorg.tb1",(char *)tempst),
		     (ggi_color *)&eight_bit_pal,
		     (ggi_pixel *)&tb1_pal,color_depth);
   
}
