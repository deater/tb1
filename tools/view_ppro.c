#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ggi/libggi.h>
#include "svmwgrap.h"

ggi_visual_t vis,vaddr;
uint white;
ggi_color eight_bit_pal[256];     
ggi_pixel tb1_pal[256];
ggi_directbuffer_t      dbuf;
ggi_directbuffer_t      dbuf2;
ggi_pixellinearbuffer   *plb = NULL;
ggi_pixellinearbuffer   *plb2= NULL;
char *dest,*src;

int ch;

int get_input() {

   int evmask;
   ggi_event ev;
   struct timeval t={0,0};
   
   evmask=emKeyPress|emKeyRelease|emPointer;

   while (ggiEventPoll(vis,evmask,&t))
   {
	/*ggiEventPoll(vis,evmask,NULL);*/

	do {
		ggiEventRead(vis,&ev,evmask);
	} while(! ( (1 << ev.any.type) & evmask ) );

       if (ev.any.type==evKeyPress) {
	  switch(KTYP(U(ev.key.sym))) { 
	     case KT_LATIN:
	     case KT_LETTER:
		  switch(KVAL(U(ev.key.sym))) {
		     case ' ': /* space */
		  		printf("Space\n");exit (1); break; 
		     case '\x1b': /* esc */
		 	 	printf("escape\n");break;
		     case ',':
			  	printf("Comma\n");break;
		     case '.':
		  		printf("Period\n");break;
		     case '0':break;
		     case '1':break;
		     case '2':break;
		     case '3':break;
		     case '4':break;
		     case '5':break;
		     case '6':break;
		     case '7':break;
		     case '8':break;
		     case '9':printf("9\n");break;
		     default: printf("latin sym=%4x code=%4x (%c)\n",ev.key.sym,ev.key.code,KVAL(U(ev.key.sym)));break;
		  }
	        break;
	     default:
		switch(U(ev.key.sym)) {
		     case K_UP: /* CrSr up */
		     case K_P8: /* Keypad 8 */
			printf("Up\n");break;
		     case K_DOWN: /* CrSr down */
		     case K_P2: /* Keypad 2 */
	  		printf("Down\n");break;
		     case K_RIGHT: /* CrSr right */
		     case K_P6: /* CrSr right */
			printf("Right\n");break;
		     case K_LEFT: /* CrSr left */
		     case K_P4: /* CrSr left */
	  		printf("Left\n");break;
		     case K_ENTER: /* enter */
		  	printf("Enter\n");break;
		     case K_P9:
		     case K_P7:
	  		break;
		     case K_P3:
		     case K_P1:
	  		break;	     
		     default: printf("sym=%4x code=%4x\n",ev.key.sym,ev.key.code);break;
		 }
	     }
       }
	     else if (ev.any.type==evKeyRelease){
		switch(KTYP(U(ev.key.sym))) { 
		    case KT_LATIN:
		    case KT_LETTER:
		       switch(KVAL(U(ev.key.sym))) {
			  case ' ': /* space */
		  		printf("Sp2\n");break;
			  case '\x1b': /* esc */
		 	 	printf("Escape2\n");break;
			  case ',':
			  case '.':
		  	        break;
			  case '0':break;
			  case '1':break;
			  case '2':break;
			  case '3':break;
			  case '4':break;
			  case '5':break;
			  case '6':break;
			  case '7':break;
			  case '8':break;
			  case '9':break;
		       }
		       break;
		  default:
		    switch(U(ev.key.sym)) {
		       case K_UP: /* CrSr up */
		       case K_P8: /* Keypad 8 */
		       case K_DOWN: /* CrSr down */
		       case K_P2: /* Keypad 2 */
	  		printf("What is this?\n");break;
		       case K_RIGHT: /* CrSr right */
		       case K_P6: /* CrSr right */
		       case K_LEFT: /* CrSr left */
		       case K_P4: /* CrSr left */
	  		printf("Hmmmmmmm\n");;break;
		       case K_ENTER: /* enter */
		  	printf("Boom\n");break;
		       case K_P9:
		       case K_P7:
		       case K_P3:
		       case K_P1:
	  		break;
		  }
		}
	
	}
	
	}
   return 4;
   }


                       



int setup_graphics()
{
    int err;
   
    ggiInit();
   
    vis=ggiOpen(NULL);

    err=ggiSetGraphMode(vis,320,200,320,200,GT_8BIT);
    
   if (err) {
	  printf("Problem opening 320x200x8 vis\n");
          return 2;
       }
    
    printf("GGI Graphics Initialization OK...\n");
    return 0;
}


int main(int argc,char **argv)
{
      
   if(argc!=2) 
     printf("\n\nUsage: %s filename\n\n",argv[0]);
   else {
     setup_graphics();
     GGILoadPicPacked(0,0,vis,1,1,argv[1],
			       (ggi_color *)&eight_bit_pal,
			       (ggi_pixel *)&tb1_pal,16);
  
   ggiFlush(vis);
   sleep(5);
   while( ((ch=get_input())!=0));
   while( ((ch=get_input())==0)) usleep(30);
   }
}
