/*
 Level 1 Engine Code for Tom Bombem
 */

    /* The Includes */
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>

#include "svmwgraph/svmwgraph.h"
#include "tb1_state.h"
#include "levels.h"

#include "sound.h"

#include "tblib.h"
#include "sidebar.h"
#include "help.h"
#include "loadsave.h"
#include "graphic_tools.h"

    /* Define this to get a frames per second readout */
/* #define DEBUG_ON */

    /* The sounds */
  
#define NUM_SAMPLES 8
#define SND_AHH    0
#define SND_CC     1
#define SND_KAPOW  2
#define SND_SCREAM 3
#define SND_BONK   4
#define SND_CLICK  5
#define SND_OW     6
#define SND_ZRRP   7

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



    /* The little Sequence Before you hit the Boss */
void beforeboss(tb1_state *game_state)
{

    vmwClearKeyboardBuffer();
    vmwLoadPicPacked(0,0,game_state->virtual_3,0,1,
		     tb1_data_file("level1/viewscr.tb1",game_state->path_to_data));
    vmwClearScreen(game_state->virtual_1,0);
    vmwArbitraryCrossBlit(game_state->virtual_3,0,5,58,37,
			  game_state->virtual_1,10,10);
    vmwSmallTextXY("HUMAN!",70,10,2,0,1,game_state->graph_state->default_font,game_state->virtual_1);
    vmwSmallTextXY("WHAT ARE YOU DOING?!",70,20,2,
		                                0,1,game_state->graph_state->default_font,game_state->virtual_1);
    vmwSmallTextXY("YOUR SPECIES MUST BE TERMINATED!",70,30,2,
		                                0,1,game_state->graph_state->default_font,game_state->virtual_1);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(5);
   
    vmwArbitraryCrossBlit(game_state->virtual_3,0,42,58,37,
			  game_state->virtual_1,10,50);
    vmwSmallTextXY("I'M SORRY.",70,50,9,0,1,game_state->graph_state->default_font,game_state->virtual_1);
    vmwSmallTextXY("WE DIDN'T MEAN TO DESTROY YOUR ENVOY.",70,60,9,
		                                0,1,game_state->graph_state->default_font,game_state->virtual_1);
    vmwSmallTextXY("WILL YOU FORGIVE US AND TRY PEACE?",70,70,9,
		                                0,1,game_state->graph_state->default_font,game_state->virtual_1);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(5);
   
    vmwArbitraryCrossBlit(game_state->virtual_3,0,5,58,37,
			  game_state->virtual_1,10,90);
    vmwSmallTextXY("NO!  YOU MUST BE DESTROYED!",70,90,2,
		                                0,1,game_state->graph_state->default_font,game_state->virtual_1);
    vmwSmallTextXY("OUR FUNDING ... OUR ENVOY WAS DAMAGED BY",70,100,
		                     2,0,1,game_state->graph_state->default_font,game_state->virtual_1);
    vmwSmallTextXY("YOU!  VENGEANCE WILL BE OURS!  YOUR PUNY",70,110,
		                     2,0,1,game_state->graph_state->default_font,game_state->virtual_1);
    vmwSmallTextXY("PRIMITIVE SPACECRAFT WITH ITS INFERIOR",70,120,
		                     2,0,1,game_state->graph_state->default_font,game_state->virtual_1);
    vmwSmallTextXY("WEAPONS WOULD HAVE TO SCORE 9 DIRECT HITS",70,130,
		                     2,0,1,game_state->graph_state->default_font,game_state->virtual_1);
    vmwSmallTextXY("TO DESTROY MY SHIP!  DIE EARTH SCUM!!!!",70,140,
		                     2,0,1,game_state->graph_state->default_font,game_state->virtual_1);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(5);
    setupsidebar(game_state,game_state->virtual_2);
    vmwFlipVirtual(game_state->virtual_1,game_state->virtual_2,320,200);
}

    /* The Sequence After You Defeat (hopefully) the Boss */
void afterboss(tb1_state *game_state)
{
   
    vmwFont *tb1_font;
   
    tb1_font=game_state->graph_state->default_font;
   
    vmwLoadPicPacked(0,0,game_state->virtual_3,0,1,
		     tb1_data_file("level1/viewscr.tb1",game_state->path_to_data));
    vmwDrawBox(0,0,320,200,0,game_state->virtual_1);
    vmwArbitraryCrossBlit(game_state->virtual_3,0,42,58,37,
			  game_state->virtual_1,10,10);
    vmwSmallTextXY("HMM.. THEY DON'T BUILD SUPERIOR",70,10,
		          9,0,1,tb1_font,game_state->virtual_1);
    vmwSmallTextXY("TECHNOLOGY LIKE THEY USED TO.",70,20,
		          9,0,1,tb1_font,game_state->virtual_1);
    vmwSmallTextXY("I GUESS I CAN GO HOME NOW.",70,30,
		          9,0,1,tb1_font,game_state->virtual_1);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(5);
   
    vmwArbitraryCrossBlit(game_state->virtual_3,0,5,58,37,
			  game_state->virtual_1,10,50);
    vmwSmallTextXY("NOT SO FAST!  YOU JUST DESTROYED AN ANTIQUATED",70,50,
		          2,0,1,tb1_font,game_state->virtual_1);
    vmwSmallTextXY("DEFENSE SYSTEM THAT WAS PROGRAMMED BY A 16",70,60,
		          2,0,1,tb1_font,game_state->virtual_1);
    vmwSmallTextXY("YEAR OLD!  OUR MAIN DEFENSE PROGRAMMER HAS ",70,70,
		          2,0,1,tb1_font,game_state->virtual_1);
    vmwSmallTextXY("MUCH MORE SKILL NOW!  UNLESS YOU DESTROY OUR",70,80,
		          2,0,1,tb1_font,game_state->virtual_1);
    vmwSmallTextXY("ENTIRE XENOCIDE... I MEAN PEACE... ENVOY",70,90,
		          2,0,1,tb1_font,game_state->virtual_1);
    vmwSmallTextXY("WE WILL STILL DESTROY YOUR HOME PLANET.",70,100,
		          2,0,1,tb1_font,game_state->virtual_1);
    vmwSmallTextXY("NICE TRY PUNY EARTHLING!",70,110,2,
		          0,1,tb1_font,game_state->virtual_1);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(7);
   
    vmwArbitraryCrossBlit(game_state->virtual_3,0,42,58,37,
			  game_state->virtual_1,10,130);
    vmwSmallTextXY("HMM.. I GUESS I BETTER SAVE THE EARTH.",70,130,
		          9,0,1,tb1_font,game_state->virtual_1);
    vmwSmallTextXY("I'D BETTER SAVE MY GAME TOO.",70,140,
		          9,0,1,tb1_font,game_state->virtual_1);
    vmwSmallTextXY("D'OH!  I''M OUT OF BIG MISSILES! ",70,150,
		          9,0,1,tb1_font,game_state->virtual_1);
    vmwSmallTextXY("WELL AT LEAST I HAVE SOME SMALLER SPARES.",70,160,
		          9,0,1,tb1_font,game_state->virtual_1);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(5);
    game_state->level++;

}



    /* Defines the behavior of the objects in level 1 */
int level_one_behavior(int reset, tb1_state *game_state)
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
          beforeboss(game_state);
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
 



    /* The Main Level One */
void levelone(tb1_state *game_state) {
   
    int ch=0;
    int i,j,grapherror;
    char tempst[300];
    int itemp,whatdelay=1,levelover=0;
    int shipx=36,shipadd=0,shipframe=1;
    vmwSprite *bigship1,*bigship2,*bigship3;
    vmwSprite *shapetable[20];
    long oldsec,oldusec,time_spent;
    int howmuchscroll=0;
    int speed_factor=1,game_paused=0;
    
    vmwVisual *virtual_1,*virtual_2;
    vmwFont *tb1_font;
   
       /* For convenience */
    tb1_font=game_state->graph_state->default_font;
    virtual_1=game_state->virtual_1;
    virtual_2=game_state->virtual_2;
   
       /* Set this up for Save Game */
    game_state->begin_score=game_state->score;
    game_state->begin_shields=game_state->shields;
   
       /* Load Sprites */
    grapherror=vmwLoadPicPacked(0,0,virtual_1,1,1,
	       tb1_data_file("level1/ships.tb1",game_state->path_to_data));

    bigship1=vmwGetSprite(0,0,48,30,virtual_1);
    bigship2=vmwGetSprite(0,32,48,30,virtual_1);
    bigship3=vmwGetSprite(0,64,48,30,virtual_1);
   
    grapherror=vmwLoadPicPacked(0,0,virtual_1,1,1,
	       tb1_data_file("level1/tbshapes.tb1",game_state->path_to_data));
   
    for(j=0;j<2;j++) 
       for(i=0;i<10;i++) 
          shapetable[(j*10)+i]=vmwGetSprite(1+(i*19),1+(j*19),18,18,
		       virtual_1);

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
   
           /* Draw the Little Box announcing the Start of the Level */

    vmwDrawBox(0,0,320,200,0,virtual_1);
    coolbox(70,85,240,120,1,virtual_1);
    vmwTextXY("   LEVEL ONE:",84,95,4,7,0,tb1_font,virtual_1);
    vmwTextXY("INANIMATE OBJECTS",84,105,4,7,0,tb1_font,virtual_1);
    vmwBlitMemToDisplay(game_state->graph_state, virtual_1);

      
       /* Setup and draw the sidebar */
    setupsidebar(game_state,virtual_2);

    vmwFlipVirtual(virtual_1,virtual_2,320,200);
    sprintf(tempst,"%d",game_state->level);
    vmwDrawBox(251,52,63,7,0,virtual_2);
    vmwTextXY(tempst,307,51,12,0,0,tb1_font,virtual_2);

       /* Clear the screen and draw the stars */
    vmwDrawBox(0,0,320,400,0,virtual_2);
    for(i=0;i<100;i++) {
       vmwPutSprite(shapetable[11],rand()%238,rand()%380,virtual_2);
       vmwPutSprite(shapetable[12],rand()%238,rand()%380,virtual_2);
    }
    change_shields(game_state);
   
       /* Initiate some last variables */
    level_one_behavior(1,game_state);
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
	  vmwArbitraryCrossBlit(virtual_2,0,howmuchscroll,240,
				400-howmuchscroll,
				virtual_1,0,0);
	  vmwArbitraryCrossBlit(virtual_2,0,0,240,howmuchscroll-200,
			        virtual_1,0,400-howmuchscroll); 
       }
       else {
	  vmwArbitraryCrossBlit(virtual_2,0,howmuchscroll,240,200,
				virtual_1,0,0);
       }

	 
          /* Check for Collisions */
       for(i=0;i<5;i++) {
          if (!enemy[i].dead) {
             for(itemp=0;itemp<2;itemp++) {
	        if (bullet[itemp].out) 
                   if (collision(bullet[itemp].x,bullet[itemp].y,10,10,
                       enemy[i].x,enemy[i].y,9,9)) {
                      if (game_state->sound_enabled) 
			 playGameFX(SND_KAPOW);
                      enemy[i].hitsneeded--;
                      if (enemy[i].hitsneeded<1) enemy[i].dead=1;
                      else enemy[i].dead=0;
                      enemy[i].exploding=1;
                      enemy[i].explodeprogress=0;
                      bullet[itemp].out=0;
                      game_state->score+=10;
                      changescore(game_state);
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
                             enemy[i].x,enemy[i].y,
			     virtual_1);
	     else if (enemy[i].dead) {
                enemy[i].out=0;
                enemy[i].exploding=0;
                game_paused=level_one_behavior(0,game_state);
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
             else vmwPutSprite(shapetable[0],
			       bullet[i].x,bullet[i].y,
			       virtual_1);
	  }
       }
       
          /* MOVE ENEMIES */
       for(i=0;i<5;i++) {
          if ((enemy[i].out) && (!enemy[i].dead)) {
	     vmwPutSprite(shapetable[enemy[i].kind-1],
			  enemy[i].x,enemy[i].y,
			  virtual_1);
	     if (speed_factor==1) enemy[i].x+=enemy[i].xspeed;
	     else enemy[i].x+=(enemy[i].xspeed*speed_factor);
                /* Check Position */
	                     /* Check Position */
             if (!enemy[i].boundarycheck) { 
	        if (speed_factor>1) enemy[i].y+=(enemy[i].yspeed*speed_factor);
                else enemy[i].y+=enemy[i].yspeed;
	     }
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
                game_paused=level_one_behavior(0,game_state);
	     }
             if (enemy[i].y>140) {
                if (collision(shipx,165,24,15,enemy[i].x,enemy[i].y,9,9)) {
                   if (game_state->sound_enabled) 
		      playGameFX(SND_BONK);
                   enemy[i].hitsneeded--;
                   if (enemy[i].hitsneeded==0) enemy[i].dead=1;
                   else enemy[i].dead=0;
                   enemy[i].exploding=1;
                   enemy[i].explodeprogress=0;
                   game_state->shields--;
                   if (game_state->shields<0) levelover=1;
		   if (game_state->shields>0) change_shields(game_state);
		}
	     }
	  }
       }
          /* See if beat the level.  Yes, bad variable name.  Oh well */
       if (game_paused==9) {
          afterboss(game_state);
          game_state->level=2;
          levelover=1;
       }
       
          /* **READ KEYBOARD** */
       if ( (ch=vmwGetInput())!=0) {
          switch(ch){  
	   case VMW_ESCAPE: levelover=1; break;
	   case VMW_RIGHT: if (shipadd>=0) shipadd+=3; else shipadd=0; break;
           case VMW_LEFT: if (shipadd<=0) shipadd-=3; else shipadd=0; break;
           case VMW_F1: game_paused=1; help(game_state); break;
           case '+': whatdelay++; if (whatdelay>25) whatdelay=25; break;
	   case 'P': case 'p': game_paused=1; 
	             coolbox(65,85,175,110,1,virtual_1);
	             vmwTextXY("GAME PAUSED",79,95,4,7,
			                    0,tb1_font,virtual_1);
	             vmwBlitMemToDisplay(game_state->graph_state,virtual_1);
	             while (vmwGetInput()==0) {
			usleep(30000);
		     }
	             break;
	   case '-': whatdelay--; if (whatdelay<1) whatdelay=1; break;
	   case 'S': 
	   case 's': if (game_state->sound_enabled) 
	                game_state->sound_enabled=!(game_state->sound_enabled); break;
           case VMW_F2: game_paused=1; 
	                savegame(game_state);
	               break;
           case ' ':  for(j=0;j<2;j++)
	                if (!bullet[j].out) {
                           if (game_state->sound_enabled) 
			      playGameFX(SND_CC);
                           bullet[j].out=1;
                           bullet[j].x=shipx+15;
                           bullet[j].y=165;
		           vmwPutSprite(shapetable[0],
				        bullet[j].x,
					bullet[j].y,virtual_1);
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
	  
        case 1: vmwPutSprite(bigship1,shipx,165,virtual_1); 
	        break;
        case 3: vmwPutSprite(bigship2,shipx,165,virtual_1); 
	        break;
	case 2:
	case 4: vmwPutSprite(bigship3,shipx,165,virtual_1);
	        break;
       }
       shipframe++;
       if (shipframe==5) shipframe=1;
       
          /* Flip Pages */
       
       vmwBlitMemToDisplay(game_state->graph_state,virtual_1);
       
       
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
   
}

    /* The little opener before Level 1 */
void littleopener(tb1_state *game_state)
{

    vmwSprite *ship1,*ship2;
    int i;

    vmwClearScreen(game_state->virtual_2,0);
   
    vmwLoadPicPacked(0,0,game_state->virtual_2,1,1,
		     tb1_data_file("level1/moon2.tb1",game_state->path_to_data));
    vmwLoadPicPacked(0,0,game_state->virtual_1,1,0,  /* Load palette */
		     tb1_data_file("level1/moon2.tb1",game_state->path_to_data));
   
    ship1=vmwGetSprite(9,178,15,18,game_state->virtual_2);
    ship2=vmwGetSprite(30,178,15,18,game_state->virtual_2);
    vmwDrawBox(0,178,319,21,0,game_state->virtual_2);
   
    for(i=100;i>0;i--) {
       vmwFlipVirtual(game_state->virtual_1,game_state->virtual_2,320,200);
       vmwPutSprite(ship2,i*2,100,game_state->virtual_1);
       vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
       usleep(30000);
       if (vmwGetInput()) break;       
    }
    vmwFlipVirtual(game_state->virtual_1,game_state->virtual_2,320,200);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
   
    vmwTextXY(">KCHK< TOM! WHERE ARE YOU GOING?",5,180,15,0,1,
	      game_state->graph_state->default_font,game_state->virtual_1);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(3); 
    vmwDrawBox(0,178,319,21,0,game_state->virtual_1); 
    vmwTextXY("Ooops. ",5,180,24,0,1,game_state->graph_state->default_font,game_state->virtual_1);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(3);
    for(i=0;i<151;i++) {
       vmwFlipVirtual(game_state->virtual_1,game_state->virtual_2,320,200);
       vmwPutSprite(ship1,i*2,100,game_state->virtual_1);
       vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
       usleep(15000);
       if (vmwGetInput()) break;
    }
    vmwFlipVirtual(game_state->virtual_1,game_state->virtual_2,320,200);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
      
    vmwDrawBox(0,0,320,200,0,game_state->virtual_1);
    vmwLoadPicPacked(0,0,game_state->virtual_1,1,0, /* Restore Palette */
		     tb1_data_file("tbomb1.tb1",game_state->path_to_data));
   
}
