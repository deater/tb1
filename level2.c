/*
 *  Level 2 Engine Code for Tom Bombem
 *  */

    /* The Includes */
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <ggi/libggi.h>
#include <stdlib.h>
#include "svmwgrap.h"
#include "levels.h"
#include "gtblib.h"
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

struct obstruction {
    int x,y;
    int shooting,dead,exploding;
    int explodeprogress;
    int howmanyhits;
    int kind,lastshot;
};

    /* Define how many sound effects there are */
Sample sound_effects[NUM_SAMPLES];


void leveltwoengine(int *level, int *shields, int *score)
{
    int ch,i;
    char tempst[300];
    int k,game_paused=0,speed_factor=1;
    int shipx=36;
    int whatdelay=1,beginscore,beginshield;
    FILE *f=NULL;
    int levelover=0,j,backrow=0;
    int background[201][13];
    struct enemyinfo enemy[10];
    struct bulletinfo bullet[4];
    struct timeval timing_info;
    struct timezone dontcare;
   
    long oldsec,oldusec,time_spent;
    int howmuchscroll=0;
    struct obstruction passive[50];
    int shipadd=0,shipframe=1;
    int our_row,our_shape,rows_goneby=0;

    int ship_shape[3][1600];
    int shape_table[40][200];
    int enemies_drawn[200];
   
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
    GGILoadPicPacked(0,0,vaddr,1,1,
		     tb1_data_file("ships.tb1",(char *)tempst),
		     (ggi_color *)&eight_bit_pal,
		     (ggi_pixel *)&tb1_pal,color_depth);
    
    vmwGetSprite(vaddr,0,0,48,30,(int *)&ship_shape[0]);
    vmwGetSprite(vaddr,0,32,48,30,(int *)&ship_shape[1]);
    vmwGetSprite(vaddr,0,64,48,30,(int *)&ship_shape[2]);
   
    if (*level==2) GGILoadPicPacked(0,0,vaddr,1,1,
				 tb1_data_file("tbaship.tb1",(char *)tempst),
				 (ggi_color *)&eight_bit_pal,
				 (ggi_pixel *)&tb1_pal,color_depth);
    if (*level==4) GGILoadPicPacked(0,0,vaddr,1,1,
				 tb1_data_file("tbeerm.tb1",(char *)tempst),
				 (ggi_color *)&eight_bit_pal,
				 (ggi_pixel *)&tb1_pal,color_depth);
   
    for(j=0;j<4;j++)
       for(i=0;i<10;i++) 
          vmwGetSprite(vaddr,1+i*21,1+j*11,20,10,(int *)&shape_table[j*10+i]);

       /* Load Background Data */
    if (*level==2) f=fopen(tb1_data_file("level2.dat",(char *)tempst),"r");
    if (*level==4) f=fopen(tb1_data_file("level4.dat",(char *)tempst),"r");
    if (f==NULL) printf("ERROR! Could't open level %d data!\n",*level);
  
    for(j=0;j<200;j++) 
       for(i=0;i<12;i++) fscanf(f,"%d",&background[j][i]);
    fclose(f);

       /* Initialize Structures for enemy, bullets, and background */
    for(i=0;i<50;i++) {  
       passive[i].dead=1;
       passive[i].exploding=0;
    }
    for(i=0;i<10;i++) enemy[i].out=0;
    for(i=0;i<3;i++) {
       bullet[i].out=0;
       bullet[i].x=0;
       bullet[i].y=0;
    }
   for(i=0;i<200;i++) enemies_drawn[i]=0;
   
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
       vmwPutSprite(shape_table[32],20,10,stride_factor,
                    plb_vaddr2->write,rand()%238,
                    rand()%400,plb_vaddr2->stride);
       vmwPutSprite(shape_table[33],20,10,stride_factor,
                    plb_vaddr2->write,rand()%238,
                    rand()%400,plb_vaddr2->stride);
    }
    change_shields(shields);
   
       /* Announce the Start of the Level */
    ggiSetGCForeground(vis,tb1_pal[0]);
    ggiDrawBox(vis,0,0,320,200);
    coolbox(70,85,240,120,1,vis);
    if (*level==2) {
       VMWtextxy("   LEVEL TWO:",84,95,tb1_pal[4],tb1_pal[7],0,tb1_font,vis);
       VMWtextxy("THE \"PEACE ENVOY\"",84,105,tb1_pal[4],tb1_pal[7],0,tb1_font,vis);
    }
    if (*level==4) {
       VMWtextxy("   LEVEL FOUR:",84,95,tb1_pal[4],tb1_pal[7],0,tb1_font,vis);
       VMWtextxy(" THE PLANET EERM",84,105,tb1_pal[4],tb1_pal[7],0,tb1_font,vis);
    }
    clear_keyboard_buffer();
    pauseawhile(5);
   
    gettimeofday(&timing_info,&dontcare);
    oldsec=timing_info.tv_sec; oldusec=timing_info.tv_usec;
   
       /**** GAME LOOP ****/
    while (!levelover) { 
       ch=0;
          /* Scroll the Background */
       if (speed_factor>1) howmuchscroll-=speed_factor;
       else howmuchscroll--;
       if (howmuchscroll<0) {
	  howmuchscroll=200+howmuchscroll;
	  ggiCopyBox(vaddr2,0,0,240,200,0,200);
	  for(i=0;i<12;i++) 
	     for(j=19;j>=0;j--) {
		our_shape=background[backrow+(19-j)][i];
	        vmwPutSprite(shape_table[our_shape],
			     20,10,stride_factor,
			     plb_vaddr2->write,i*20,j*10,
			     plb_vaddr2->stride);
	     }
	     backrow+=20;
       }
          /* Setup Obstructions */
       our_row=rows_goneby/10;
       if (!enemies_drawn[our_row]) {
	  enemies_drawn[our_row]=1;
	  for(i=0;i<12;i++) {
	     our_shape=background[our_row][i];
	     if ((our_shape>9)&&(our_shape<20)) {
		   k=0;
		   while ((!passive[k].dead) && (k<40)) k++;
		   passive[k].dead=0;
		   passive[k].kind=our_shape;
		   if ((our_shape>10) && (our_shape<15)) passive[k].shooting=1;
		   else passive[k].shooting=0;
		   passive[k].exploding=0;
		   passive[k].x=i*20;
		   passive[k].y=(rows_goneby%10)-9;
		   passive[k].howmanyhits=1;
		   passive[k].lastshot=0;
	     }
	  }
       }
          /* Flip the far background to vaddr */
       vmwArbitraryCrossBlit(plb_vaddr2->read,0,0+howmuchscroll,240,200,
                                plb_vaddr->write,0,0,plb_vaddr->stride,
                                stride_factor);
             
          /***Collision Check***/
       for(i=0;i<40;i++) 
	  if ((!passive[i].dead) && (!passive[i].exploding)) {
	     for(j=0;j<3;j++) {
                if ((bullet[j].out) && 
		    (collision(bullet[j].x,bullet[j].y,3,4,passive[i].x,
						  passive[i].y,10,5))){
                    if (passive[i].kind!=10) {
		       if (sound_enabled) Snd_effect(SND_KAPOW,2);
                       passive[i].exploding=1;
                       passive[i].explodeprogress=0;
                       bullet[j].out=0;
                       *score+=10;
                       changescore(*score,shields);
		    }
                    else {
                       bullet[j].out=0;
                       k=0;
		       while ((!enemy[k].out) && (k<10)) k++;
		       if (k<9) {
                          enemy[k].out=1;
                          enemy[k].y=bullet[j].y;
                          enemy[k].x=bullet[j].x;
                          enemy[k].yspeed=7;
                          enemy[k].kind=21;
		       }
		    }
		 }
	     }
	  
             /* See if ship is hitting any Obstructions*/
          if ((passive[i].y>155) && (passive[i].kind!=10)) {
             if ((collision(passive[i].x,passive[i].y,10,5,shipx+16,165,5,5))||
		(collision(passive[i].x,passive[i].y,10,5,shipx+6,175,18,8))) {
                if (sound_enabled) Snd_effect(SND_BONK,1);
                passive[i].dead=1;
                (*shields)--;
                if(*shields<0) levelover=1;
		vmwPutSprite(shape_table[34],
			     20,10,stride_factor,plb_vaddr2->write,
			     passive[i].x,passive[i].y+howmuchscroll,
			     plb_vaddr2->stride);
		change_shields(shields);
		}
	  }
       }
          /* See if hit by lasers */
       for (i=0;i<10;i++) 
	 if (enemy[i].out) {
            if ((collision(enemy[i].x,enemy[i].y,2,5,shipx+16,165,5,5)) ||
                (collision(enemy[i].x,enemy[i].y,2,5,shipx+6,175,18,8))) {
                if (sound_enabled) Snd_effect(SND_BONK,1);
                enemy[i].out=0;
                (*shields)--;
                if (*shields<0) levelover=1;
	        change_shields(shields);
	    }
	 }
       
          /***DO EXPLOSIONS***/
       for(i=0;i<40;i++) 
          if (passive[i].exploding) { 
             passive[i].explodeprogress++;
	     vmwPutSprite(shape_table[35+passive[i].explodeprogress],
                          20,10,stride_factor,plb_vaddr2->write,
			  passive[i].x,passive[i].y+howmuchscroll,
			  plb_vaddr2->stride);
             if (passive[i].explodeprogress>4) {
                passive[i].dead=1;
                passive[i].exploding=0;
	        vmwPutSprite(shape_table[34],20,10,stride_factor,
			     plb_vaddr2->write,passive[i].x,
			     passive[i].y+howmuchscroll,
			     plb_vaddr2->stride);
	      
           }
        }
       
       /***MOVE BULLET***/
    for(i=0;i<3;i++) {
       if (bullet[i].out) {
	  if (speed_factor>1) bullet[i].y-=(5*speed_factor);
	  else bullet[i].y-=5;
	  if (bullet[i].y<5) bullet[i].out=0;
	  else vmwPutSprite(shape_table[20],20,10,stride_factor,
                            plb_vaddr->write,bullet[i].x,bullet[i].y,
                            plb_vaddr->stride);
       }
    }
       
       /***MOVE ENEMIES***/
    for(j=0;j<40;j++) {
       if (!passive[j].dead) {
          if (speed_factor==1) passive[j].y++;
	  else passive[j].y+=speed_factor;
          if (passive[j].y>190) passive[j].dead=1;
       }
       if (passive[j].lastshot>0) passive[j].lastshot--;
       if ((!passive[j].dead) && (passive[j].shooting)
          && (!passive[j].lastshot) && (passive[j].y>0)) {
          k=0;
	  while ((enemy[k].out) && (k<10)) k++;
	  if (k<9) {
             passive[j].lastshot=30;
             enemy[k].out=1;
             enemy[k].y=passive[j].y;
             enemy[k].x=passive[j].x+5;
             enemy[k].yspeed=5;
             enemy[k].kind=25;
             if (passive[j].kind==11) enemy[k].kind=26;
	  }
       }
    }
    
    for(j=0;j<10;j++) {
       if (enemy[j].out) {
	  vmwPutSprite(shape_table[enemy[j].kind],20,10,stride_factor,
		             plb_vaddr->write,enemy[j].x,enemy[j].y,
		             plb_vaddr->stride);
          if (speed_factor==1) enemy[j].y+=enemy[j].yspeed;
	  else enemy[j].y+=(enemy[j].yspeed*speed_factor);
          if (enemy[j].y>189) enemy[j].out=0;
       }
    }
 
       /***READ KEYBOARD***/
    if ((ch=get_input())!=0) { 
       switch(ch) {
	case TB_ESC: levelover=1; break;
	case TB_RIGHT: if (shipadd>=0) shipadd+=3; else shipadd=0; break;
	case TB_LEFT:  if (shipadd<=0) shipadd-=3; else shipadd=0; break;
	case TB_F1: game_paused=1; help(); break;      
        case '+': whatdelay++; break;
	case 'P': case 'p': game_paused=1;
	                    coolbox(65,85,175,110,1,vis);
	                    VMWtextxy("GAME PAUSED",79,95,tb1_pal[4],tb1_pal[7],
					            0,tb1_font,vis);
	                    while (get_input()==0) usleep(30000);
			    break;
	case '-': whatdelay--; break;
	case 'S':
	case 's': if(sound_possible) sound_enabled=!(sound_enabled); break;
        case TB_F2: game_paused=1; savegame(*level,beginscore,beginshield);
	            break;
	case ' ':  for(j=0;j<3;j++)
	              if (!bullet[j].out) {
			 if (sound_enabled) Snd_effect(SND_CC,0);
			 bullet[j].out=1;
			 bullet[j].x=shipx+21;
			 bullet[j].y=165;
			 vmwPutSprite(shape_table[20],20,10,stride_factor,
			                plb_vaddr->write,bullet[j].x,
		                        bullet[j].y,plb_vaddr->stride);
			j=4;
		      }
	  
       }
    }
	  
	  

       /***MOVE SHIP***/
    if (speed_factor>1) {
       shipx+=(shipadd*speed_factor);
       rows_goneby+=(speed_factor);
    }
    else {
       shipx+=shipadd;
       rows_goneby++;
    }
    if (shipx<1) shipx=1;
    if (shipx>190) shipx=190;
    switch(shipframe) {
       case 1: vmwPutSprite(ship_shape[0],48,30,stride_factor,
			    plb_vaddr->write,shipx,165,
			    plb_vaddr->stride); break;
       case 3: vmwPutSprite(ship_shape[1],48,30,stride_factor,
			    plb_vaddr->write,shipx,165,
			    plb_vaddr->stride); break;
       case 2:
       case 4: vmwPutSprite(ship_shape[2],48,30,stride_factor,
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
 
    if (game_paused) {
       gettimeofday(&timing_info,&dontcare);
       oldusec=timing_info.tv_usec;
       oldsec=timing_info.tv_sec;
       game_paused=0;
       speed_factor=1;
    }
    else {
       speed_factor=(time_spent/30000);
       oldusec=timing_info.tv_usec;
       oldsec=timing_info.tv_sec;
    }
    

    /*printf("%i\n",rows_goneby);*/
    if (rows_goneby>1950) {
       printf("%i\n",rows_goneby);
       /*
     clearkeyboardbuffer;
     pauseawhile(200);
     fade;
      grapherror:=Mode13LoadPicPacked(0,0,vaddr,false,true,'viewscr.tb1');
      cls(0,vga);
      blockmove(0,79,58,116,vaddr,10,10,vga);
      clearkeyboardbuffer;
      outsmalltextxy('UNIDENTIFIED SPACECRAFT!',70,10,2,0,vga,true);
      outsmalltextxy('DO YOU WISH TO DEACTIVATE ',70,20,2,0,vga,true);
      outsmalltextxy('THIS SHIP''S SECURITY SYSTEMS? (Y/N)',70,30,2,0,vga,true);
     unfade;
      clearkeyboardbuffer;
      ch:='!';
      repeat
         if keypressed then ch:=readkey;
      until (upcase(ch)='Y') or (upcase(ch)='N');

      if upcase(ch)='N' then begin
         blockmove(0,79,58,116,vaddr,10,50,vga);
         outsmalltextxy('NO?  AFFIRMATIVE. ',70,50,9,0,vga,true);
         outsmalltextxy('ARMING REMOTE DESTRUCTION RAY.',70,60,9,0,vga,true);
         outsmalltextxy('GOOD-BYE.',70,70,9,0,vga,true);
         pauseawhile(400);
         fade;
      end;

      if upcase(ch)='Y' then begin
         blockmove(0,79,58,116,vaddr,10,50,vga);
         outsmalltextxy('"Y"=CORRECT PASSWORD. ',70,50,2,0,vga,true);
         outsmalltextxy('WELCOME SUPREME TENTACLEE COMMANDER.',70,60,2,0,vga,true);
         outsmalltextxy('INITIATING TRACTOR BEAM AND AUTOMATIC',70,70,2,0,vga,true);
         outsmalltextxy('LANDING PROCEDURE.',70,80,2,0,vga,true);
         outsmalltextxy('WE WILL BE DEPARTING FOR THE PLANET',70,90,2,0,vga,true);
         outsmalltextxy('EERM IN THREE MICROCYCLE UNITS.',70,100,2,0,vga,true);
         pauseawhile(550);
         level:=3;
         clearkeyboardbuffer;
         blockmove(0,42,58,79,vaddr,10,110,vga);
         outsmalltextxy('Wha? Wait!',70,110,9,0,vga,true);
         outsmalltextxy('What''s happening?',70,120,9,0,vga,true);
         pauseawhile(550);
         fade;
     end;

         grapherror:=Mode13LoadPicPacked(0,0,vaddr,false,true,'tbtract.tb1');
         for i:=0 to 239 do
             for j:=0 to 49 do
                 putpixel240(i,j,getpixel(i,j,vaddr),vaddr2);
         cls(0,vga);
         unfade;
         for howmuchscroll:=50 downto 1 do begin
            flipd240(howmuchscroll,vaddr,vaddr2);
            putshape (bigship3off,vaddr,43,30,shipx,165);
            waitretrace;
            flipd320(vaddr,vga);
        end;

        if upcase(ch)='N' then begin
           clearkeyboardbuffer;
           line(7,6,shipx+10,180,4,vga);
           line(shipx+37,180,231,6,4,vga);
           pauseawhile(50);
           clearkeyboardbuffer;
           for i:=shipx to shipx+48 do
               verticalline(165,195,i,4,vga);
           pauseawhile(200);
           flipd240(howmuchscroll,vaddr,vaddr2);
           flipd320(vaddr,vga);
           pauseawhile(150);
        end;


     if upcase(ch)='Y' then begin;
        shipadd:=sgn(shipx-95);
        shipy:=165;
        repeat
          if shipx<>95 then shipx:=shipx-shipadd;
          if shipy>9 then dec(shipy);
          flipd240(howmuchscroll,vaddr,vaddr2);
          line(7,6,shipx+10,shipy+15,2,vaddr);
          line(shipx+37,shipy+15,231,6,2,vaddr);
          putshape (bigship3off,vaddr,43,30,shipx,shipy);
          waitretrace;
          flipd320(vaddr,vga);
        until (shipx=95) and (shipy=9);
        clearkeyboardbuffer;
     pauseawhile(850);
     fade;
        cls(0,vga);


     while keypressed do ch:=readkey;
     if level=4 then begin
       outsmalltextxy('THE PLANET EERM?',20,20,10,0,vga,true);
       outsmalltextxy('XENOCIDE FLEET?',20,30,10,0,vga,true);
       outsmalltextxy('WHAT''S GOING ON?',20,40,10,0,vga,true);
       outsmalltextxy('A MAJOR GOVERNMENT CONSPIRACY?  MASS HALUCINATIONS?',20,50,10,0,vga,true);

       outsmalltextxy('WATCH FOR TOM BOMBEM LEVEL 3 (CURRENTLY IN THE DESIGN PHASE).',10,70,12,0,vga,true);
       outsmalltextxy('ALL THESE QUESTIONS WILL BE ANSWERED!',10,80,12,0,vga,true);
       outsmalltextxy('ALSO MORE FEATURES WILL BE ADDED:',10,90,12,0,vga,true);
       outsmalltextxy('     BETTER GRAPHICS, SOUND AND SPEED.',10,100,12,0,vga,true);

       outsmalltextxy('TO HASTEN COMPLETION, SEND QUESTIONS/COMMENTS/DONATIONS TO ',9,120,9,0,vga,true);
       outsmalltextxy('THE AUTHOR (SEE THE REGISTER MESSAGE FOR RELEVANT ADDRESSES).',9,130,9,0,vga,true);

       outsmalltextxy('THANK YOU FOR PLAYING TOM BOMBEM',80,150,14,0,vga,true);
       unfade;
       pauseawhile(1800);
     end; */
          levelover=1;
       }
    }
    Snd_restore();
}

  

void littleopener2()
{
    char tempst[300];
   
    ggiSetGCForeground(vis,tb1_pal[0]);
    ggiDrawBox(vis,0,0,320,200);
    ggiSetGCForeground(vaddr,tb1_pal[0]);
    ggiDrawBox(vaddr,0,0,320,200);
    GGILoadPicPacked(0,0,vis,1,1,
		     tb1_data_file("tbl2ship.tb1",(char *)tempst),
		     (ggi_color *)&eight_bit_pal,
		     (ggi_pixel *)&tb1_pal,color_depth);
    VMWtextxy("Hmmmm... ",10,10,tb1_pal[4],tb1_pal[0],0,tb1_font,vis);
    VMWtextxy("This Might Be Harder Than I Thought.",10,20,tb1_pal[4],tb1_pal[0],0,tb1_font,vis);
    pauseawhile(13); 
    ggiSetGCForeground(vis,tb1_pal[0]);
    ggiDrawBox(vis,0,0,320,200);
   
}
