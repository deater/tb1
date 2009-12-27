/****************************************************************\
\*    TOM BOMBEM AND THE INVASION OF THE INANIMATE_OBJECTS      */
/*                    version 2.9.16      26 December 2009    	*\
\*        by Vince Weaver       vince@deater.net                */
/*                                                              *\
\*  Originally written in Pascal and x86 assembly for DOS       */
/*          using the PCGPE code as an example in 1994          *\
\*  Ported to Linux, C, and ggi late 1997-early 1998            */
/*  Port continued to SDL in June of 2000                       *\
\*          This source is released under the GPL               */
/****************************************************************/

#define TB1_VERSION "2.9.16"

#include <stdio.h>
#include <stdlib.h>   /* for calloc */
#include <string.h>   /* for strncpy */
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#include "svmwgraph/svmwgraph.h"
#include "tb1_state.h"
#include "tblib.h"
#include "sound.h"

#include "graphic_tools.h"
#include "help.h"
#include "quit.h"
#include "story.h"
#include "credits.h"
#include "about.h"
#include "loadsave.h"
#include "options.h"
#include "playgame.h"

int command_line_help(int show_version,char *runas) {

   if (!show_version) {   
      printf("Usage:  %s [-curses] [-double] [-fullscreen] [-nosound]"
	               " [-version] [-?]\n\n",runas);
      printf("  -curses     : play in color text-mode\n");
      printf("  -double     : play in 640x480 mode\n");
      printf("  -fullscreen : play in fullscreen mode (if available)\n");
      printf("  -nosound    : disables sound within game\n");
      printf("  -opengl     : play in openGL mode\n");
      printf("  -version    : print version\n");
      printf("  -?          : print this help message\n");
      printf("\n");
   }
   return 0;
}


void vmw_opener(tb1_state *game_state, vmwVisual *virtual_1) {

   int x;

   for(x=0;x<256;x++) {
      vmwLoadPalette(game_state->graph_state,0,0,0,x); /* 0=black */
   }

      /* Do the VMW Software Production Logo */
   for(x=0;x<=40;x++) {
      vmwLoadPalette(game_state->graph_state, ((x+20)*4),0,0,100+x);
      vmwLoadPalette(game_state->graph_state,0,0, ( (x+20)*4 ),141+x);
      vmwLoadPalette(game_state->graph_state,0, ( (x+20)*4),0,182+x);
   }

      /* Set the white color */
   vmwLoadPalette(game_state->graph_state,0xff,0xff,0xff,15);
    
   vmwFlushPalette(game_state->graph_state);
   
      /* Actually draw the stylized VMW */
   for(x=0;x<=40;x++) {
      vmwDrawVLine(x+40,45,2*x,100+x,virtual_1);
      vmwDrawVLine(x+120,45,2*x,141+x,virtual_1);
      vmwDrawVLine(x+200,45,2*x,141+x,virtual_1);
      vmwDrawVLine(x+80,125-(2*x),2*x,182+x,virtual_1);
      vmwDrawVLine(x+160,125-(2*x),2*x,182+x,virtual_1);
   }
   for(x=40;x>0;x--){
      vmwDrawVLine(x+80,45,80-(2*x),140-x,virtual_1);
      vmwDrawVLine(x+160,45,80-(2*x),181-x,virtual_1);
      vmwDrawVLine(x+240,45,80-(2*x),181-x,virtual_1);
      vmwDrawVLine(x+120,45+(2*x),80-(2*x),222-x,virtual_1);
      vmwDrawVLine(x+200,45+(2*x),80-(2*x),222-x,virtual_1);
   }

   vmwTextXY("A VMW SOFTWARE PRODUCTION",60,140,
	     15,15,0,game_state->graph_state->default_font,virtual_1);   

   if ((game_state->sound_possible) && (game_state->music_enabled)) {
      playSound();
   }

   vmwBlitMemToDisplay(game_state->graph_state,virtual_1);
   pauseawhile(5);
   vmwFadeToBlack(game_state->graph_state,virtual_1,0);
}

int main(int argc,char **argv) {
   
   int i,grapherror,reloadpic=0;
   int ch,barpos,time_sec;
   int graphics_target=VMW_SDLTARGET;
   FILE *fff;
   char *dir_name,options_file[BUFSIZ];
   vmwVisual *virtual_1,*virtual_2,*virtual_3; 

   tb1_state *game_state;

   vmwFont *tb1_font;

   struct timeval time_info;
   struct timezone dontcare;

   printf("\nTom Bombem v%s by Vince Weaver vince@deater.net\n",TB1_VERSION);
   printf("        http://www.deater.net/weave/vmwprod/tb1\n\n");

      /* Setup the game state */
   if ( (game_state=calloc(1,sizeof(tb1_state)))==NULL) {
      printf("You are seriously low on RAM!\n");
      return 3;
   }

      /* Some sane defaults */
   game_state->level=0;
   game_state->shields=0;
   game_state->score=0;
   game_state->virtual_1=NULL;
   game_state->virtual_2=NULL;
   game_state->virtual_3=NULL;
   game_state->sound_possible=1;
   game_state->music_enabled=1;
   game_state->sound_enabled=1;
   game_state->sound_volume=5;
   game_state->music_volume=5;
   game_state->default_double_size=0;
   game_state->default_fullscreen=0;

      /* Load saved defaults, if any */
   dir_name=check_for_tb1_directory(game_state,0);
   if (dir_name==NULL) {
      printf("  + No ~/.tb1 directory.  Using default options.\n");
   }
   else {
      sprintf(options_file,"%s/options.tb1",dir_name);
      fff=fopen(options_file,"r");
      if (fff==NULL) {
	 printf("  + No ~/.tb1/options.tb1 file.  Using defaults.\n");
      }
      else {
         printf("  + Reading options from ~/.tb1/options.tb1\n");
	 fscanf(fff,"%i %i %i %i %i %i",
		&game_state->sound_enabled,
		&game_state->music_enabled,
		&game_state->sound_volume,
		&game_state->music_volume,
		&game_state->default_double_size,
		&game_state->default_fullscreen);
	 fclose(fff);
      }
   }
   
   
      /* Parse Command Line Arguments */
   i=1;
   while(i<argc) {
      if (argv[i][0]=='-') {         
	 switch (argv[i][1]) {
	   case 'h': 
	   case '?':
	     command_line_help(0,argv[0]); 
	     return 5; 
	     break;
	   case 'v':
	     command_line_help(1,argv[0]); 
	     return 5; 
	     break;
	   case 'f':
	     game_state->default_fullscreen=1; 
	     break;
	   case 'c':
	     graphics_target=VMW_CURSESTARGET; 
	     break;
	   case 'd':
	     game_state->default_double_size=1; 
	     break;
	   case 'n': 
	     game_state->sound_possible=0;
	     printf("  + Sound totally disabled\n");
	     break;
	   case 'o':
	     graphics_target=VMW_OPENGLTARGET; 
	     break;
	   default : 
	     command_line_help(0,argv[0]);
	     printf("Unknown Option: %s\n\n",argv[i]);
	     return 5;   
	 }
       }
       else {
	  command_line_help(0,argv[0]);
	  printf("Unknown Option: %s\n\n",argv[i]);
	  return 5;
       }
       i++;
   }
      
       /* Find the Data */
/* FIXME : User Defined Path Info*/
    if ( (fff=fopen("./data/tb1_data_files_here","r"))!=NULL) {
       strncpy(game_state->path_to_data,"./data/",20);
    }
    else if ( (fff=fopen("/usr/local/games/tb1/data/tb1_data_files_here","r"))
	      !=NULL) {
	strncpy(game_state->path_to_data,"/usr/local/games/tb1/data/",40);
    }
    else {
       char tempst[200];
	
       sprintf(tempst,"%s/.tb1/data/tb1_data_files_here",getenv("HOME"));
       if ( (fff=fopen(tempst,"r"))!=NULL) {
	  sprintf(game_state->path_to_data,"%s/.tb1/data/",getenv("HOME"));
       }
       else {
	  printf("ERROR!  Could not find tb1 data!\n");
	  printf("   Checked ./data, /usr/local/games/tb1/data\n");
	  printf("           and %s/.tb1/data\n\n",getenv("HOME"));
          return 9;	       
       }
    }
    printf("  + Found tb1 data in %s\n",game_state->path_to_data);
  
/* LEGACY OPERATION BOTTLECAP STUFF--->                 *\
\*      --> You are not meant to understand this <--    */
/* No 9-22-94    Back to yes 10-6-94     uh_oh 2-21-95  *\
\* gone for real long time 10-12-95                     */
/* 11-95 not AMG, but AAT                               *\
\* 3-96 oh well... gave up on AAT finally               */
/* 5-11-96 Now MLP...                                   *\
\* 9-22-2000 :(                                         */  
/* 3-25-2002 Now KRG ;)                                 */
   
      /* Randomize random number generator */
   srandom(time(NULL));
   printf("  + Seeding random number generator...\n");
   
      /* Load/Detect sound */
   if (game_state->sound_possible) {
      if (initSound(game_state->path_to_data)<0) {
	 game_state->sound_possible=0;
      }
         /* Load fanfare Music */
      if (game_state->sound_possible) {  
         loadSound(tb1_data_file("music/vmwfan.mod",game_state->path_to_data));
      }
      printf("  + Loaded sounds...\n");
   }
   
      /* Setup Graphics */
   if (!game_state->default_double_size) {
      if ( (game_state->graph_state=
	                 vmwSetupSVMWGraph(graphics_target,
					   320,200,
					   0,1,
					   game_state->default_fullscreen,
					   1))==NULL) {   
         fprintf(stderr,"ERROR: Couldn't get display set up properly.\n");
         return VMW_ERROR_DISPLAY;
      }
   }
   else { /* We are double-sized */
      if ( (game_state->graph_state=
	                 vmwSetupSVMWGraph(graphics_target,
				           640,480,
					   0,2,
					   game_state->default_fullscreen,
					   1))==NULL) {   
         fprintf(stderr,"ERROR: Couldn't get display set up properly.\n");
         return VMW_ERROR_DISPLAY;
      }
   }
     
      /* Load the tom bombem font */
   game_state->graph_state->default_font=
                vmwLoadFont(tb1_data_file("tbfont.tb1",
					  game_state->path_to_data),8,16,256);
   printf("  + Loaded tb1 font...\n");

      /* Allocate the 3 virtual screens */
   if ((game_state->virtual_1=vmwSetupVisual(320,200))==NULL) {
      fprintf(stderr,"ERROR: Couldn't get RAM for virtual screen 1!\n");
      return VMW_ERROR_MEM;
   }

   if ((game_state->virtual_2=vmwSetupVisual(320,400))==NULL) {
      fprintf(stderr,"ERROR: Couldn't get RAM for virtual screen 2!\n");
      return VMW_ERROR_MEM;
   }

   if ((game_state->virtual_3=vmwSetupVisual(320,200))==NULL) {
      fprintf(stderr,"ERROR: Couldn't get RAM for virtual screen 3!\n");
      return VMW_ERROR_MEM;
   }

   printf("  + Allocated virtual screens...\n");

      /* To ease typing burden */
   virtual_1=game_state->virtual_1;
   virtual_2=game_state->virtual_2;
   virtual_3=game_state->virtual_3;
   tb1_font=game_state->graph_state->default_font;

      /* The "fancy" opener */
   vmw_opener(game_state,virtual_1);

      /* Load menu music */
   if (game_state->sound_possible) {
      stopSound();
      loadSound(tb1_data_file("music/weave1.mod",game_state->path_to_data));
   }

      /* Load palette */
   grapherror=vmwLoadPicPacked(0,0,virtual_1,1,0,
			       tb1_data_file("tbomb1.tb1",
					      game_state->path_to_data),
				game_state->graph_state);

      /* Bit of a hack to load proper unfade colors */
   vmwFadeToBlack(game_state->graph_state,virtual_1,1);

      /* Load Title Picture */
   grapherror=vmwLoadPicPacked(0,0,virtual_1,1,1,
			       tb1_data_file("tbomb1.tb1",
					     game_state->path_to_data),
				game_state->graph_state);

      /* Copy the picture over */
   vmwFlipVirtual(virtual_3,virtual_1,320,200);

   vmwUnFade(game_state->graph_state,virtual_1);

      /* Main Menu Loop */
   while(1) {
         /* If virtual_3 was over-written, re-load it */
      if (reloadpic) {
         if ((game_state->sound_possible) && (game_state->music_enabled)) {
            loadSound(tb1_data_file("music/weave1.mod",
				    game_state->path_to_data));
	 }
         grapherror=vmwLoadPicPacked(0,0,virtual_3,1,1,
				     tb1_data_file("tbomb1.tb1",
						   game_state->path_to_data),
				     game_state->graph_state);
	 reloadpic=0;
      }

      vmwFlipVirtual(virtual_1,virtual_3,320,200);

         /* Play the menu-music */
      if ((game_state->sound_possible) && (game_state->music_enabled)) {
	 playSound();
      }
      vmwBlitMemToDisplay(game_state->graph_state,virtual_1);

         /* Don't show the menu until keypress/15s */
      pauseawhile(15);

      barpos=0;
      vmwTextXY("F1 HELP",0,190,9,7,0,tb1_font,virtual_1);   
      coolbox(117,61,199,140,1,virtual_1);

      vmwBlitMemToDisplay(game_state->graph_state,virtual_1);
      ch=0;

      while(ch!=VMW_ENTER){
         if (barpos==0) {
	    vmwTextXY("NEW GAME",123,67,32,0,1,tb1_font,virtual_1);
	 }
         else {
	    vmwTextXY("NEW GAME",123,67,32,7,1,tb1_font,virtual_1);
	 }
         if (barpos==1) {
	    vmwTextXY("OPTIONS",123,77,32,0,1,tb1_font,virtual_1);
	 }
         else {
	    vmwTextXY("OPTIONS",123,77,32,7,1,tb1_font,virtual_1);
	 }
         if (barpos==2) {
	    vmwTextXY("ABOUT",123,87,32,0,1,tb1_font,virtual_1);
	 }
         else {
	    vmwTextXY("ABOUT",123,87,32,7,1,tb1_font,virtual_1);
	 }
         if (barpos==3) {
	    vmwTextXY("LOAD GAME",123,97,32,0,1,tb1_font,virtual_1);
	 }
         else {
	    vmwTextXY("LOAD GAME",123,97,32,7,1,tb1_font,virtual_1);
	 }
         if (barpos==4) {
	    vmwTextXY("STORY",123,107,32,0,1,tb1_font,virtual_1);
	 }
         else {
	    vmwTextXY("STORY",123,107,32,7,1,tb1_font,virtual_1);
	 }
         if (barpos==5) {
	    vmwTextXY("CREDITS",123,117,32,0,1,tb1_font,virtual_1);
	 }
         else {
	    vmwTextXY("CREDITS",123,117,32,7,1,tb1_font,virtual_1);
	 }
         if (barpos==6) {
	    vmwTextXY("QUIT",123,127,32,0,1,tb1_font,virtual_1);
	 }
         else {
	    vmwTextXY("QUIT",123,127,32,7,1,tb1_font,virtual_1);
	 }

         vmwBlitMemToDisplay(game_state->graph_state,virtual_1);

	    /* If at title screen too long, run credits */
	 gettimeofday(&time_info,&dontcare);
         time_sec=time_info.tv_sec;  	  

	 while( ((ch=vmwGetInput())==0)) {
	    usleep(30);
            gettimeofday(&time_info,&dontcare);
	    if (time_info.tv_sec-time_sec>40) {
	       if (game_state->sound_possible) stopSound();
	       credits(game_state);
	       ch=VMW_ENTER;
	       barpos=9;
	       reloadpic=1;
	       break;
	    }
	 }

	    /* Change menu position based on key pressed */
         if ((ch==VMW_DOWN)||(ch==VMW_RIGHT)) barpos++;
         if ((ch==VMW_UP) || (ch==VMW_LEFT)) barpos--;
         if (ch==VMW_F1) {barpos=10; ch=VMW_ENTER;} /*F1*/
         if (ch=='n') barpos=0;    /*N*/
         if (ch=='o') barpos=1;    /*O*/
         if (ch=='a') barpos=2;    /*A*/
         if (ch=='l') barpos=3;    /*L*/
         if (ch=='s') barpos=4;    /*S*/
         if (ch=='c') barpos=5;    /*C*/
         if (ch=='q') barpos=6;    /*Q*/
         if (ch==VMW_ESCAPE){ /* escape */
            barpos=6;
            ch=VMW_ENTER;
         }
         if(barpos==7) barpos=0;
         if(barpos<0) barpos=6;
      }
      if (game_state->sound_possible) stopSound();
       
         /* Run whatever it was that the person pressed */
      switch (barpos) {
        case 0:  playthegame(game_state); reloadpic=1; break;
	case 1:  options(game_state); reloadpic=1; break;
        case 2:  about(game_state); reloadpic=1; break;
        case 3:  loadgame(game_state); reloadpic=1; break;
        case 4:  story(game_state); reloadpic=1; break;
        case 5:  credits(game_state);  break;
	case 6:  barpos=quit(game_state); break;
        case 10: help(game_state); reloadpic=1; break;
      }
   }
}
