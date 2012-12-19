
#include <stdio.h>
#include <unistd.h> /* for usleep */

#include "svmwgraph/svmwgraph.h"
#include "tb1_state.h"
#include "tblib.h"
#include "graphic_tools.h"

void about(tb1_state *game_state)
{
    int pagenum=1,oldpagenum=0,numpages=4,ch=0;
    char tempst[40];
 
    vmwFont *tb1_font;
    vmwVisual *target1,*target2;
       
       /* For convenience */
    tb1_font=game_state->graph_state->default_font;
    target1=game_state->virtual_1;
    target2=game_state->virtual_2;
    
    vmwClearScreen(target1,0);
         
       /* Load the really old picture */
    vmwLoadPicPacked(0,0,target2,1,1,
		    tb1_data_file("./about/register.tb1",game_state->path_to_data),
		    game_state->graph_state); 

       /* And repeat until we want to quit */
    while (1) {
       
          /* If page one, copy my pic over.  Otherwise, clear to white */
       if (pagenum==1) {
	  vmwFlipVirtual(target1,target2,320,200);
	  coolbox(0,0,319,199,0,target1);
       }
       else {
	  coolbox(0,0,319,199,1,target1);
       }
       
       switch(pagenum) {
	case 1:
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
             break;
	case 2:
	     shadowrite("MY NAME IS VINCE WEAVER",10,10,10,2,tb1_font,target1);
	     shadowrite("   VISIT MY WEBSITE",10,20,10,2,tb1_font,target1);
	     shadowrite("   http://www.deater.net/weave/",10,30,10,2,tb1_font,target1);
/*	     shadowrite("   UP WHENEVER SCHOOL IS IN SESSION",10,40,10,2,tb1_font,target1);*/
	     shadowrite("UNTIL 2008 I WILL BE IN GRAD SCHOOL",10,60,13,5,tb1_font,target1);
	     shadowrite(" AT CORNELL UNIVERSITY.",10,70,13,5,tb1_font,target1);
	     shadowrite(" THEN, ONTO THE MOON!.",10,80,13,5,tb1_font,target1);
	     shadowrite("GET THE NEWEST VERSION OF TB1 AT",10,100,11,3,tb1_font,target1);
	     shadowrite(" THE OFFICIAL TB1 WEB SITE:",10,110,11,3,tb1_font,target1);
	     shadowrite("  www.deater.net/weave/vmwprod/tb1/",10,120,11,3,tb1_font,target1);
	     shadowrite("I CAN BE CONTACTED VIA E-MAIL AT:",10,140,12,4,tb1_font,target1);
	     shadowrite("    VINCE@DEATER.NET",10,150,9,1,tb1_font,target1);
	     shadowrite("FEEL FREE TO SEND COMMENTS.",10,160,12,4,tb1_font,target1);
             break;
        case 3:
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
             break;
	case 4:
             shadowrite("DISCLAIMER:",10,10,12,14,tb1_font,target1);
	     shadowrite("* MY PROGRAMS SHOULD NOT DAMAGE YOUR *",8,30,12,4,tb1_font,target1);
	     shadowrite("* COMPUTER IN ANY WAY.  PLEASE DON'T *",8,40,12,4,tb1_font,target1);
	     shadowrite("* USE MY SOFTWARE IN CRITICAL        *",8,50,12,4,tb1_font,target1);
	     shadowrite("* APPLICATIONS LIKE LIFE-SUPPORT     *",8,60,12,4,tb1_font,target1);
	     shadowrite("* EQUIPMENT, DEFLECTOR SHIELDS, OR   *",8,70,12,4,tb1_font,target1);
	     shadowrite("* AUTOMOBILE ENGINES.                *",8,80,12,4,tb1_font,target1);
	     shadowrite("* LINUX FOREVER! THE OS FOR EVERYONE *",8,90,12,4,tb1_font,target1);
             break;
	default: /* This better not happen */ break;
       }
       
       snprintf(tempst,35,"Page %d of %d: ESC QUITS",pagenum,numpages);
       shadowrite(tempst,50,180,15,7,tb1_font,target1);
       vmwBlitMemToDisplay(game_state->graph_state,target1);
       oldpagenum=pagenum;
    
       while (pagenum==oldpagenum) {
          while ( (ch=vmwGetInput() ) == 0)  usleep(100);
          if ((ch==VMW_ESCAPE) || (ch=='q')) return;
          if ((ch==' ') || (ch==VMW_ENTER)) pagenum++;
          if ((ch==VMW_RIGHT) || (ch==VMW_DOWN) || (ch==VMW_PGDN)) pagenum++;
          if ((ch==VMW_LEFT) || (ch==VMW_UP) || (ch==VMW_PGUP)) pagenum--;
          if (pagenum>4) pagenum=1;
          if (pagenum<1) pagenum=4;
       }
    }
}

