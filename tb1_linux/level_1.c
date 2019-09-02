/*
 Level 1 Engine Code for Tom Bombem
 * 
 * November 2000 -- Changed the inner workings of engine.
 *                  Now somewhat different than original feel.
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
/*#define DEBUG_ON */

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


#define NUM_ENEMIES  5
#define NUM_MISSILES 2

struct enemyinfo {
  int x,y;
  int kind;
  int out,exploding,boundarycheck,dead;
  int explodeprogress;
  int minx,maxx,xspeed,yspeed;
  int hitsneeded;
};

struct missileinfo {
  int out,x,y;
};

    /* Define how many sound effects there are */


    /* Global Variables*/
struct enemyinfo enemy[NUM_ENEMIES];
struct missileinfo missile[NUM_MISSILES];
struct timeval timing_info;
struct timezone dontcare;

int enemies_out;

#define LEVEL_OVER 9

#define STANDARD 0
#define DIAGONAL 1
#define DIAGONAL_NO_WAIT 2
#define WIGGLING 3
#define RAINING  4

#define SCROLL_A_BIT 7
#define BEFORE_BOSS   10
#define BOSS_BEHAVIOR 11
#define AFTER_BOSS    12
#define BOSS_DESTROYED 13
#define INVISIBLE_WAVE 15
#define THE_END 100

typedef struct {
   int enemy_type;
   int how_many;
}level_one_behavior_t;

    /* This seemed like a good idea to modularize things */
    /* Should make it a file read from disk */
level_one_behavior_t level_one_wave_behavior[]=
    {{STANDARD,5},
     {STANDARD,5},
     {DIAGONAL,5},
     {DIAGONAL_NO_WAIT,5},
     {WIGGLING,8},
     {RAINING,10},
     {WIGGLING,6},
     {RAINING,12},
     {DIAGONAL,5},
     {RAINING,10},
     {WIGGLING,16},
     {DIAGONAL,5},
     {DIAGONAL_NO_WAIT,5},
     {RAINING,3},
     {WIGGLING,10},
     {DIAGONAL,5}, 
     {SCROLL_A_BIT,100},
     {BEFORE_BOSS,1},
     {SCROLL_A_BIT,100},
     {BOSS_BEHAVIOR,1},
     {BOSS_DESTROYED,100},
     {AFTER_BOSS,1},
     {THE_END,1}};



    /* The little Sequence Before you hit the Boss */
void beforeboss(tb1_state *game_state)
{

    vmwClearKeyboardBuffer();
    vmwLoadPicPacked(0,0,game_state->virtual_3,1,1,
		     tb1_data_file("level1/viewscr.tb1",game_state->path_to_data),
		     game_state->graph_state);
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
   
    vmwLoadPicPacked(0,0,game_state->virtual_3,1,1,
		     tb1_data_file("level1/viewscr.tb1",game_state->path_to_data),
		     game_state->graph_state);
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
    /* Should change so it works even if NUM_ENEMIES>5 */
int add_another_enemy(int start_of_level, 
		      tb1_state *game_state) {

    int what_type,i,need_to_pause=0;
    static int wave_pointer=0;
    static int wait_full_wave=0;
    static int enemies_left_in_wave=0;
    static int fighting_boss=0;
   
       /* If re-initing, then clear some variables */
       /* If we don't do this, the static variables won't get reset */
       /* After Level1 starts over [i.e. you die]                   */
    if (start_of_level) { 
       wave_pointer=0;
       enemies_out=0;
       enemies_left_in_wave=0;
       fighting_boss=0;
    }
   
       /* If waiting for empty, then return w/o adding any */
    if ((wait_full_wave) && (enemies_out)) return 0;
    else {
       wait_full_wave=0;
    }
   
//    if (level_one_wave_behavior[behavior_pointer]!=BOSS_BEHAVIOR) behavior_pointer++;
   
       /* In order to be called, an enemy was destroyed */

       /* Are these sanity checks needed? */
    if (enemies_out<0) {
       enemies_out=0;
       printf("Blargh 0\n");
    }
    if (enemies_out>NUM_ENEMIES) {
       enemies_out=NUM_ENEMIES;
       printf("Blargh 7\n");
    }
   
    if (enemies_left_in_wave<=0) {
       wave_pointer++;
       enemies_left_in_wave=level_one_wave_behavior[wave_pointer].how_many;
    }
   
       /* Move on to next behvior */

    switch(level_one_wave_behavior[wave_pointer].enemy_type) {
       
       /* STANDARD */
       /* That is, 5 enemies bouncing back/forth, gradually lowering*/
    case STANDARD: 
          /* We add all 5 at once, so there better not be anything else out */
       if (enemies_out==0) {
          enemies_out=5;
	  enemies_left_in_wave-=5;
	  wait_full_wave=1;
             /* Randomly pick what type they are */
	  what_type=(2+rand()%8);
       
	  for(i=0; i<5; i++) { 
             enemy[i].kind=what_type;
             enemy[i].x=i*20;                /* Space them in one line */
             enemy[i].y=0;
             enemy[i].xspeed=5;
             enemy[i].minx=(i*20);           /* Make sure they "bounce" */
             enemy[i].maxx=(i*20)+120;       /* Properly even when some */
             enemy[i].boundarycheck=1;       /* Destroyed */
             enemy[i].yspeed=10;
             enemy[i].out=1;
             enemy[i].exploding=0;
             enemy[i].hitsneeded=1;
             enemy[i].dead=0;
	  }
       }
       break;
       
       /* *FALLING STRAIGHT* */
       /* That is, raining down from above */
    case RAINING:
          /* Fill up all empty slots?  Interesting behavior */
       for(i=0; i<5;i++) {
          if ((!enemy[i].out) && ( (rand()%5)==3) ){
	     enemies_left_in_wave--;
	     enemy[i].kind=rand()%8+2;
             enemy[i].x=rand()%200+1;
             enemy[i].y=0;
             enemy[i].xspeed=0;
             enemy[i].minx=enemy[i].x;   /* Hacky way of making sure they */
             enemy[i].maxx=enemy[i].x;   /* Fall vertically               */
             enemy[i].boundarycheck=1;
             enemy[i].yspeed=5+(wave_pointer/8); /* Fall faster as game */
	                                              /*      goes on */
             enemy[i].out=1;
             enemy[i].exploding=0;
             enemy[i].hitsneeded=1;
             enemy[i].dead=0;
             enemies_out++;
	  }
       }
       break;
       
       /* *FALLING GRADUALLY SIDEWAYS* */
       /* AKA Wiggling back and forth independently */
    case WIGGLING:
       for(i=0;i<5;i++) {
          if (!enemy[i].out) {
	     enemies_left_in_wave--;
	     enemy[i].kind=rand()%8+2;
             enemy[i].y=0;
             enemy[i].xspeed=5;
             enemy[i].minx=rand()%100;     /* Set a random range to "wiggle */
             enemy[i].maxx=rand()%100+120; 
             enemy[i].x=enemy[i].minx;
             enemy[i].boundarycheck=0;
             enemy[i].yspeed=1;            /* Constantly Fall */
             enemy[i].out=1;
             enemy[i].exploding=0;
             enemy[i].hitsneeded=1;
             enemy[i].dead=0;
	     enemies_out++;
	  }
       }
       break;
    
       /**ZIG-ZAG**/
       /* That is, fall in a diagonal formation */
    case DIAGONAL:
         if (!enemies_out) wait_full_wave=1;
    case DIAGONAL_NO_WAIT:
          /* Another one of these that we need all to be empty */
       if (!enemies_out) {
          enemies_out=5;
	  enemies_left_in_wave-=5;
          what_type=rand()%8+2;
          for(i=0;i<5;i++) { 
	     if (!enemy[i].out) {
                enemy[i].kind=what_type;
                enemy[i].x=i*20;             /* Nice diagonal pattern */
		enemy[i].y=i*10;
                enemy[i].xspeed=5;
                enemy[i].minx=0;
                enemy[i].maxx=220;
                enemy[i].boundarycheck=0;
                enemy[i].yspeed=1;           /* Gradually fall */
                enemy[i].out=1;
                enemy[i].exploding=0;
                enemy[i].hitsneeded=1;
                enemy[i].dead=0;
	     }
	  }
       }
       break;
       
    case SCROLL_A_BIT:
        enemies_left_in_wave--;
        break;
       
     case BEFORE_BOSS:
        beforeboss(game_state);
        enemies_left_in_wave--;
          if ((game_state->sound_possible) &&(game_state->music_enabled)) {
	     loadSound(tb1_data_file("music/boss1.mod",game_state->path_to_data));
	     playSound();
	  }
       
        break;
       
     case BOSS_DESTROYED:
        enemies_left_in_wave--;
        if ((enemies_left_in_wave>25) &&  ( !(enemies_left_in_wave%3)) &&
       	    (game_state->sound_possible)&&(game_state->sound_enabled)) {
	     playGameFX(SND_KAPOW);
	    }
            break;

       
     case AFTER_BOSS:
        stopSound();
        afterboss(game_state);
        enemies_left_in_wave--;
        break;
         
     case THE_END:
        return LEVEL_OVER;
       
       /* Beginning of Boss */
    case BOSS_BEHAVIOR:
       if ((!enemies_out) && (!fighting_boss)) {
	  fighting_boss=1;
          enemy[0].kind=14;
          enemy[1].kind=14;
          enemy[2].kind=13;
          for(i=0;i<3;i++) {
             enemy[i].x=(i*20)+10;
             enemy[i].y=0;
             enemy[i].xspeed=5;
             enemy[i].minx=0;
             enemy[i].maxx=220;
             enemy[i].boundarycheck=1;
             enemy[i].yspeed=0;
             enemy[i].out=1;
             enemy[i].exploding=0;
             enemy[i].hitsneeded=5;
             enemy[i].dead=0;
             enemies_out++;
	  }
       } break;
     default: break;
    }
     
    if (fighting_boss) {
       
       /* Objects Cast off by the Boss */
       /* Detect if Level One is Over */
       if ((enemy[0].dead) && (enemy[1].dead) && (enemy[2].dead)) {       
	  enemies_left_in_wave--;
	  fighting_boss=0;
       }
       else  
       for(i=3;i<5;i++) {
          if ((!enemy[i].out) && (enemy[i-3].out)) {
	     enemies_out++;
	     enemy[i].kind=rand()%8+2;
             enemy[i].x=enemy[i-3].x;
             enemy[i].y=20;
             enemy[i].xspeed=0;
             enemy[i].minx=enemy[i].x;
             enemy[i].maxx=enemy[i].x;
             enemy[i].boundarycheck=0;
             enemy[i].yspeed=4;
             enemy[i].out=1;
             enemy[i].exploding=0;
             enemy[i].hitsneeded=1;
             enemy[i].dead=0;
	  }
       }
    }
    return need_to_pause;
}
 



    /* The Main Level One */
void LevelOneEngine(tb1_state *game_state) {

    int ch=0;
    int i,j,grapherror;
    char tempst[300];
    int itemp,levelover=0;
    int shipx=36,shipadd=0,shipframe=1;
    vmwSprite *bigship1,*bigship2,*bigship3;
    vmwSprite *shapetable[20];
    long oldusec,time_spent=1; // oldsec;
    int howmuchscroll=0;
    int game_paused=0;
    int done_waiting=0;

    vmwVisual *virtual_1,*virtual_2;
    vmwFont *tb1_font;

       /* For convenience */
    tb1_font=game_state->graph_state->default_font;
    virtual_1=game_state->virtual_1;
    virtual_2=game_state->virtual_2;

       /* Set this up for Save Game */
    game_state->begin_score=game_state->score;
    game_state->begin_shields=game_state->shields;

       /* Load Ship Sprites */
    grapherror=vmwLoadPicPacked(0,0,virtual_1,1,1,
	       tb1_data_file("level1/ships.tb1",game_state->path_to_data),
	       game_state->graph_state);
	if (grapherror) {
		return;
	}

    bigship1=vmwGetSprite(0,0,48,30,virtual_1);
    bigship2=vmwGetSprite(0,32,48,30,virtual_1);
    bigship3=vmwGetSprite(0,64,48,30,virtual_1);

       /* Load Inanimate Object Shapes */
    grapherror=vmwLoadPicPacked(0,0,virtual_1,0,1,
	       tb1_data_file("level1/tbshapes.tb1",game_state->path_to_data),
	       game_state->graph_state);
   
    for(j=0;j<2;j++) 
       for(i=0;i<10;i++) 
          shapetable[(j*10)+i]=vmwGetSprite(1+(i*19),1+(j*19),18,18,virtual_1);

       /* Set up initial system conditions [ie, no enemies] */
    for(i=0;i<NUM_ENEMIES;i++) {
       enemy[i].exploding=0;
       enemy[i].out=0;
       enemy[i].dead=0;
    }
    for(i=0;i<NUM_MISSILES;i++) {
       missile[i].out=0;  
       missile[i].x=0; 
       missile[i].y=0;
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
//    vmwDrawBox(251,52,62,7,0,virtual_1);
    vmwTextXY(tempst,307,51,12,0,0,tb1_font,virtual_1);

       /* Clear the screen and draw the stars */
    vmwDrawBox(0,0,320,400,0,virtual_2);
    for(i=0;i<100;i++) {
       vmwPutSprite(shapetable[11],rand()%238,rand()%380,virtual_2);
       vmwPutSprite(shapetable[12],rand()%238,rand()%380,virtual_2);
    }
       /* Initialize shield state */
    change_shields(game_state);

       /* Initiate some last variables */
    add_another_enemy(1,game_state);
    pauseawhile(5);

       /* Get time values for frame-limiting */
    gettimeofday(&timing_info,&dontcare);
    //oldsec=timing_info.tv_sec;
    oldusec=timing_info.tv_usec;

       /* MAIN GAME LOOP */
    while(!levelover) {
       ch=0;
          /* Scroll the Stars */
          /* We have a 240x400 field of stars */
       howmuchscroll--;
       if (howmuchscroll<0) howmuchscroll=399;

          /* If scroll>199, then we have to split into two copies */
          /* One from scroll to 400 */
          /* Another from 0-(scroll-200) */
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

          /* Add new enemies and move to next wave if needed */
       if (enemies_out<NUM_ENEMIES) {
	  if (add_another_enemy(0,game_state)==LEVEL_OVER) {
	     game_state->level=2;
	     levelover=1;
	     return;
	  }
       }

          /* See if the enemies have hit anything/scrolled off screen */
       for(i=0;i<NUM_ENEMIES;i++) {
          if (!enemy[i].dead) {

                /* Check to see if our missiles have hit any enemies */
             for(itemp=0;itemp<NUM_MISSILES;itemp++) {
	        if (missile[itemp].out) {
                   if (collision(missile[itemp].x,missile[itemp].y,10,10,
                       enemy[i].x,enemy[i].y,9,9)) {
                      if ((game_state->sound_possible)&&(game_state->sound_enabled)) 
			 playGameFX(SND_KAPOW);
                      enemy[i].hitsneeded--;
                      if (enemy[i].hitsneeded<1) enemy[i].dead=1;
                      else enemy[i].dead=0;
                      enemy[i].exploding=1;
                      enemy[i].explodeprogress=0;
                      missile[itemp].out=0;
                      game_state->score+=10;
                      changescore(game_state);
		   }
		}
	     }

	        /* While we are at it, see if scrolled off screen */
	     if (enemy[i].y>179) {
		enemy[i].out=0;
		enemy[i].dead=1;
	        enemies_out--;
	     }
	  }
       }

          /* Explode the things that are exploding */
       for(i=0;i<NUM_ENEMIES;i++) {
          if (enemy[i].exploding) {
             enemy[i].explodeprogress++;
             if (enemy[i].explodeprogress<10)
	        vmwPutSprite(shapetable[(enemy[i].explodeprogress/2)+15],
                             enemy[i].x,enemy[i].y,virtual_1);
	     else if (enemy[i].dead) { /* Handle for objects w hitpoints >1 */
                enemy[i].out=0;
                enemy[i].exploding=0;
		enemies_out--;
	     }
             else enemy[i].exploding=0;
	  }
       }

          /* Move the Missiles */
       for(i=0;i<NUM_MISSILES;i++) {
          if (missile[i].out) {
	     missile[i].y-=5;
             if (missile[i].y<5) missile[i].out=0;
             else vmwPutSprite(shapetable[0],missile[i].x,missile[i].y,
			       virtual_1);
	  }
       }

          /* MOVE ENEMIES */
       for(i=0;i<NUM_ENEMIES;i++) {
          if ((enemy[i].out) && (!enemy[i].dead)) {
	     vmwPutSprite(shapetable[enemy[i].kind],
			  enemy[i].x,enemy[i].y,virtual_1);
	        /* Move in X direction */
	     enemy[i].x+=enemy[i].xspeed;
                
	        /* Move in Y direction */
             if (!enemy[i].boundarycheck) { 
                enemy[i].y+=enemy[i].yspeed;
	     }
	        /* Move down if an oscilating type */
	     if ((enemy[i].x<=enemy[i].minx) || (enemy[i].x>=enemy[i].maxx)) {
                enemy[i].xspeed=-enemy[i].xspeed;
	        enemy[i].x+=enemy[i].xspeed;
	        enemy[i].y+=enemy[i].yspeed;
	     }

	        /* See if colliding with spaceship */
             if (enemy[i].y>140) {
                if (collision(shipx,165,24,15,enemy[i].x,enemy[i].y,9,9)) {
                   if ((game_state->sound_possible)&&(game_state->sound_enabled)) 
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
       
          /* **READ KEYBOARD** */
       if ( (ch=vmwGetInput())!=0) {
          switch(ch){  
	   case VMW_ESCAPE: levelover=1; break;
	   case VMW_RIGHT: if (shipadd>=0) shipadd+=3; else shipadd=0; break;
           case VMW_LEFT: if (shipadd<=0) shipadd-=3; else shipadd=0; break;
           case VMW_F1: game_paused=1; help(game_state); break;
	   case 'P': case 'p': game_paused=1; 
	             coolbox(65,85,175,110,1,virtual_1);
	             vmwTextXY("GAME PAUSED",79,95,4,7,
			                    0,tb1_font,virtual_1);
	             vmwBlitMemToDisplay(game_state->graph_state,virtual_1);
	             while (vmwGetInput()==0) {
			usleep(30000);
		     }
	             break;
	   case 'S':
	   case 's': if (game_state->sound_possible) {
	                	game_state->sound_enabled=!(game_state->sound_enabled);
			}
			break;
           case VMW_F2: game_paused=1;
	                savegame(game_state);
	               break;
           case ' ':  for(j=0;j<2;j++)
	                if (!missile[j].out) {
                           if ((game_state->sound_possible)&&(game_state->sound_enabled)) 
			      playGameFX(SND_CC);
                           missile[j].out=1;
                           missile[j].x=shipx+15;
                           missile[j].y=165;
		           vmwPutSprite(shapetable[0],
				        missile[j].x,
					missile[j].y,virtual_1);
		           j=3;
			}
	  }
       }

          /* **MOVE SHIP** */
       shipx+=shipadd;
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
#ifdef DEBUG_ON       
       sprintf(tempst,"%li",1000000/time_spent);
       vmwTextXY(tempst,10,10,4,7,0,tb1_font,virtual_1);
#endif
       vmwBlitMemToDisplay(game_state->graph_state,virtual_1);
       
          /* If time passed was too little, wait a bit */
          /* 33,333 would frame rate to 30Hz */
          /* Linux with 100Hz scheduling only gives +- 10000 accuracy */
       done_waiting=0;
       while (!done_waiting){
	  
	  gettimeofday(&timing_info,&dontcare);
	  time_spent=timing_info.tv_usec-oldusec;
	     /* Assume we don't lag more than a second */
	     /* Seriously, if we lag more than 10ms we are screwed anyway */
	  if (time_spent<0) time_spent+=1000000;
	  if (time_spent<30000) usleep(100);
	  else (done_waiting=1);
       }
       oldusec=timing_info.tv_usec;
       //oldsec=timing_info.tv_sec;

          /* If game is paused, don't keep track of time */
       if (game_paused) {
	  gettimeofday(&timing_info,&dontcare);
	  oldusec=timing_info.tv_usec;
	  //oldsec=timing_info.tv_sec;
	  game_paused=0;
       }
    }
}

    /* The little opener before Level 1 */
void LevelOneLittleOpener(tb1_state *game_state) {

    vmwSprite *ship1,*ship2;
    int i;

    vmwClearScreen(game_state->virtual_2,0);
   
    vmwLoadPicPacked(0,0,game_state->virtual_2,1,1,
		     tb1_data_file("level1/moon2.tb1",game_state->path_to_data),
		     game_state->graph_state);
//    vmwLoadPicPacked(0,0,game_state->virtual_1,1,0,  /* Load palette */
//		     tb1_data_file("level1/moon2.tb1",game_state->path_to_data));
   
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
		     tb1_data_file("tbomb1.tb1",game_state->path_to_data),
		     game_state->graph_state);
   
}
