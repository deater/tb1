/* Generic tblib stuff usable by other apps */

#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <ggi/libggi.h>
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
       *    ggiDrawHLine(page,x1,y2,(x2-x1));
       *    ggiDrawVLine(page,x1,y1,(y2-y1));
       *    ggiDrawVLine(page,x2,y1,(y2-y1));*/
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
    if (err) fprintf(stderr,"Probelms shutting down GGI!\n");
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



int pauseawhile(int howlong)
{
    struct timeval bob;
    struct timezone mree;
    long begin_s,begin_u;
    int ch=0;
   
    clear_keyboard_buffer();
    gettimeofday(&bob,&mree);
    begin_s=bob.tv_sec; begin_u=bob.tv_usec;
    while(( (bob.tv_sec-begin_s)<howlong) && ( (ch=get_input())==0)) {
       usleep(30);
       gettimeofday(&bob,&mree);
    }
    return ch;
}

void shadowrite(char *st,int x5,int y5,int forecol,int backcol,
		ggi_visual_t vis)
{
    VMWtextxy(st,x5+1,y5+1,tb1_pal[backcol],0,0,tb1_font,vis);
    VMWtextxy(st,x5,y5,tb1_pal[forecol],0,0,tb1_font,vis);
}
