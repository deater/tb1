/****************************************************************\
\*    TOM BOMBEM AND THE INVASION OF THE INANIMATE_OBJECTS      */
/*                    version 2.9.0      February 28, 1998      *\
\*        by Vince Weaver       weave@eng.umd.edu               */
/*                                                              *\
\*  Originally written in Pascal and x86 assembly for DOS       */
/*  Ported to Linux, C, and ggi late 1997-early 1998            *\
\*          This source is released under the GPL               */
/****************************************************************/

#define TB1_VERSION "2.9.0"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#include <ggi/libggi.h> 

#include "svmwgrap.h"
#include "tblib.h"

    /* Exported Global Variables */
ggi_visual_t vis,vaddr,vaddr2;
vmw_font *tb1_font;
uint white;
ggi_color eight_bit_pal[256];     
ggi_pixel tb1_pal[256];
int color_depth;
ggi_directbuffer_t      dbuf_vis,dbuf_vaddr,dbuf_vaddr2;
ggi_pixellinearbuffer   *plb_vis = NULL,*plb_vaddr= NULL,*plb_vaddr2=NULL;
int stride_factor=1;
int sound_enabled=1,sound_possible=1,read_only_mode=0;
char path_to_data[256];

struct timeval time_info;
struct timezone dontcare;

    /* Setup the Graphics */
int setup_graphics(int force_8bpp)
{
    int err;
    ggi_mode mode;
    int vx,vy,sx,sy;
   
    ggiInit();
    vis=ggiOpen(NULL);
       
    if (force_8bpp)
       err=ggiSetGraphMode(vis,320,200,320,200,GT_8BIT);
    else
       err=ggiSetGraphMode(vis,320,200,320,200,GGI_AUTO);
    if (err) {
       fprintf(stderr,"ERROR!  Problem opening 320x200 vis\n\n");
       return 2;
    } 
    
    ggiGetMode(vis,&mode);
    vx=mode.virt.x;   vy=mode.virt.y;
    sx=mode.visible.x;sy=mode.visible.y;
    switch (mode.graphtype) {
      case GT_1BIT: color_depth=1;break;
      case GT_4BIT: color_depth=4;break;
      case GT_8BIT: color_depth=8;break;
      case GT_15BIT: color_depth=15;break;
      case GT_16BIT: color_depth=16;break;
      case GT_24BIT: color_depth=24;break;
      case GT_32BIT: color_depth=32;break;
      default: break;
    }
    printf("  + Opened a %d x %d (%d x %d) mode with %d bpp\n",
	      sx,sy,vx,vy,color_depth);
    
    err = ggiDBGetBuffer (vis, &dbuf_vis);
    if (err) { 
       printf("Error! Could not get directbuffer\n"); 
       return 2;
    }
    if (!(ggiDBGetLayout (dbuf_vis) == blPixelLinearBuffer)) {
       printf("Error! Nonlinear Display Buffer.\n");
       return 2;
    }
    if (!(plb_vis = ggiDBGetPLB (dbuf_vis)) ) {
       printf("Error! Problem getting pixel linear buffer.\n");
       return 2;
    }
   
    vaddr=ggiOpen("display-memory",NULL);
    err=ggiSetGraphMode(vaddr,320,200,320,200,mode.graphtype);
    if (err) {
       printf("ERROR! Problem opening 320x200x%d vaddr\n",color_depth);
       return 2;
    }
    err = ggiDBGetBuffer (vaddr, &dbuf_vaddr);
    if (err) {
       printf("Error! Could not get directbuffer\n");
       return 2;
    }
    if (!(ggiDBGetLayout (dbuf_vaddr) == blPixelLinearBuffer)) {
       printf("Error! Nonlinear Display Buffer.\n");
       return 2;
    }
    if (!(plb_vaddr = ggiDBGetPLB (dbuf_vaddr)) ) {
       printf("Error! Problem getting pixel linear buffer.\n");
       return 2;
    }
   
    vaddr2=ggiOpen("display-memory",NULL);
    err=ggiSetGraphMode(vaddr2,320,400,320,400,mode.graphtype);
    if (err){
       printf("ERROR! Problem opening 320x400x%d vaddr2\n",color_depth);
       return 2;
    }
    err = ggiDBGetBuffer (vaddr2, &dbuf_vaddr2);
    if (err) {
       printf("Error! Could not get directbuffer\n");
       return 2;
    }
    if (!(ggiDBGetLayout (dbuf_vaddr2) == blPixelLinearBuffer)) {
       printf("Error! Nonlinear Display Buffer.\n");
       return 2;
    }
    if (!(plb_vaddr2 = ggiDBGetPLB (dbuf_vaddr2)) ) {
       printf("Error! Problem getting pixel linear buffer.\n");
       return 2;
    }
    stride_factor=(plb_vis->stride)/320;
    printf("  + Using a stride factor of %d\n",stride_factor);
    printf("  + GGI Graphics Initialization successful...\n");
    printf("  + Running TB1 in %dbpp Mode...\n",color_depth);
    return 0;
}

int command_line_help(int show_version,char *runas)
{
    if (!show_version) {   
       printf("Usage:  %s [-force8bpp] [-nosound] [-readonly]"
	                  " [-version] [-?]\n\n",runas);
       printf("  -force8bpp : force to run in 8bpp mode\n");
       printf("  -nosound   : disables sound within game\n");
       printf("  -readonly  : don't try to write files to disk\n");
       printf("  -version   : print version\n");
       printf("  -?         : print this help message\n");
       printf("\n");
    }
   return 0;
}


int main(int argc,char **argv)
{
    int i,grapherror,reloadpic=0,force_8bpp=0;
    int ch,ch2,x,barpos,time_sec;
    char *tempst[300];
    FILE *fff;
   
    printf("\nTom Bombem v%s by Vince Weaver weave@eng.umd.edu\n",TB1_VERSION);
    printf("         http://www.glue.umd.edu/~weave/tb1\n\n");
   
       /* Parse Command Line Arguments */
    i=1;
    while(i<argc) {
       if (argv[i][0]=='-') {         
	  switch (argv[i][1]) {
	   case 'h': case '?':
	     command_line_help(0,argv[0]); return 5; break;
	   case 'v':
	     command_line_help(1,argv[0]); return 5; break;
	   case 'f':
	     force_8bpp=1; break;
	   case 'n': 
	     sound_enabled=0;
	     sound_possible=0;
	     printf("  + Sound totally disabled\n");
	     break;
	   case 'r': 
	     read_only_mode=1;
	     printf("  + Read Only mode enabled\n");
	     break;
	   default : command_line_help(0,argv[0]);
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
    if ( (fff=fopen("./data/data_files_here","r"))!=NULL) {
       strncpy(path_to_data,"./data/",20);
    }
    else if ( (fff=fopen("/usr/local/games/tb1/data/data_files_here","r"))
	      !=NULL) {
	strncpy(path_to_data,"/usr/local/games/tb1/data/",40);
    }
    else {
       char tempst[200];
	
       sprintf(tempst,"%s/.tb1/data/data_files_here",getenv("HOME"));
       if ( (fff=fopen(tempst,"r"))!=NULL) {
	  sprintf(path_to_data,"%s/.tb1/data/",getenv("HOME"));
       }
       else {
	  printf("ERROR!  Could not find tb1 data!\n");
	  printf("   Checked ./data, /usr/local/games/tb1/data\n");
	  printf("           and %s/.tb1/data\n\n",getenv("HOME"));
          return 9;	       
       }
    }
    printf("  + Found tb1 data in %s\n",path_to_data);
   
/* FIXME : find where writing info out to */
   
  
       /* REMNANT OPERATION BOTTLECAP STUFF--->                *\
       \* No 9-22-94    Back to yes 10-6-94     uh_oh 2-21-95  */
       /* gone for real long time 10-12-95                     *\
       \* 11-95 not Amy anymore, but Gus                       */
       /* 3-96 oh well... gave up on Gus finally               *\
       \* 5-11-96 Now Marie... what fun life is                */
      
       /* Randomize random number generator */
    srandom(time(NULL));
   
       /* Load the tom bombem font */
    tb1_font=LoadVMWFont(tb1_data_file("tbfont.tb1",(char *)tempst),8,16,256);
   
       /* Setup Graphics */
    if (setup_graphics(force_8bpp)==2) {   
       fprintf(stderr,"ERROR: Couldn't get display set up properly.\n");
       return 2;
    }
     
       /* Do the VMW Software Production Logo */
    for(x=0;x<=40;x++){
       eight_bit_pal[100+x].r=((x+20)*4)*0x100;
       eight_bit_pal[100+x].g=0;
       eight_bit_pal[100+x].b=0;
       
       eight_bit_pal[141+x].r=0;
       eight_bit_pal[141+x].g=0;
       eight_bit_pal[141+x].b=((x+20)*4)*0x100;
              
       eight_bit_pal[182+x].r=0;
       eight_bit_pal[182+x].g=((x+20)*4)*0x100;;
       eight_bit_pal[182+x].b=0;
       
       if (color_depth!=8) {
	  tb1_pal[100+x]=ggiMapColor(vis,&eight_bit_pal[100+x]);
	  tb1_pal[141+x]=ggiMapColor(vis,&eight_bit_pal[141+x]);
	  tb1_pal[182+x]=ggiMapColor(vis,&eight_bit_pal[182+x]);
       }
       else {
	  for(i=0;i<256;i++) tb1_pal[i]=(ggi_pixel)i;  
       }
    }
       /* Set the white color */
    eight_bit_pal[15].r=255*0x100;
    eight_bit_pal[15].g=255*0x100;
    eight_bit_pal[15].b=255*0x100;
    
       /* Finalize Pallette Stuff */
    if (color_depth!=8) {
       tb1_pal[15]=ggiMapColor(vis,&eight_bit_pal[15]);
    }
    else ggiSetPaletteVec(vis,0,256,eight_bit_pal);
         
       /* Actually draw the stylized VMW */
    for(x=0;x<=40;x++){ 
       ggiSetGCForeground(vis,tb1_pal[100+x]);   
       ggiDrawVLine(vis,x+40,45,2*x);
       ggiSetGCForeground(vis,tb1_pal[141+x]);
       ggiDrawVLine(vis,x+120,45,2*x);
       ggiDrawVLine(vis,x+200,45,2*x);
       ggiSetGCForeground(vis,tb1_pal[182+x]);
       ggiDrawVLine(vis,x+80,125-(2*x),2*x);
       ggiDrawVLine(vis,x+160,125-(2*x),2*x);
    }
    for(x=40;x>0;x--){
       ggiSetGCForeground(vis,tb1_pal[140-x]);
       ggiDrawVLine(vis,x+80,45,80-(2*x));
       ggiSetGCForeground(vis,tb1_pal[181-x]);
       ggiDrawVLine(vis,x+160,45,80-(2*x));
       ggiDrawVLine(vis,x+240,45,80-(2*x));
       ggiSetGCForeground(vis,tb1_pal[222-x]);
       ggiDrawVLine(vis,x+120,45+(2*x),80-(2*x));
       ggiDrawVLine(vis,x+200,45+(2*x),80-(2*x));
    }
   
    ggiSetGCForeground(vis,tb1_pal[15]);
    VMWtextxy("A VMW SOFTWARE PRODUCTION",60,140,
	      tb1_pal[15],tb1_pal[15],0,tb1_font,vis);   
    ggiFlush(vis);
    pauseawhile(5);
   
       /* Clear the Screen and get ready for the Menu */
    ggiSetGCForeground(vis,tb1_pal[0]); 
    ggiFillscreen(vis);
    
       /* Load the title screen */
    grapherror=GGILoadPicPacked(0,0,vis,1,1,
				tb1_data_file("tbomb1.tb1",(char *)tempst),
			        (ggi_color *)&eight_bit_pal,
				(ggi_pixel *)&tb1_pal,color_depth); 
    grapherror=GGILoadPicPacked(0,0,vaddr2,1,1,
				tb1_data_file("tbomb1.tb1",(char *)tempst),
			        (ggi_color *)&eight_bit_pal,
			        (ggi_pixel *)&tb1_pal,color_depth);
    ggiFlush(vis);
    ggiFlush(vaddr2);
    pauseawhile(5); 
    
       /* Main Menu Loop */
    while (1) {
       if (reloadpic) {
          grapherror=GGILoadPicPacked(0,0,vaddr2,1,1,
				    tb1_data_file("tbomb1.tb1",(char *)tempst),
				    (ggi_color *)&eight_bit_pal,
				    (ggi_pixel *)&tb1_pal,color_depth);
          ggiFlush(vaddr2);
	  reloadpic=0;
       }
       vmwCrossBlit(plb_vis->write,plb_vaddr2->read,plb_vis->stride,200);
       ggiFlush(vis);
              
       barpos=0;
       VMWtextxy("F1 HELP",0,190,tb1_pal[9],tb1_pal[7],0,tb1_font,vis);   
       coolbox(117,61,199,140,1,vis);
       ggiFlush(vis);
       ch=0;
       while(ch!=TB_ENTER){
          if (barpos==0) VMWtextxy("NEW GAME",123,67,
				   tb1_pal[32],tb1_pal[0],1,tb1_font,vis);
                    else VMWtextxy("NEW GAME",123,67,
			           tb1_pal[32],tb1_pal[7],1,tb1_font,vis);
          if (barpos==1) VMWtextxy("OPTIONS",123,77,
				   tb1_pal[32],tb1_pal[0],1,tb1_font,vis);
                    else VMWtextxy("OPTIONS",123,77,
				   tb1_pal[32],tb1_pal[7],1,tb1_font,vis);
          if (barpos==2) VMWtextxy("ABOUT",123,87,
				   tb1_pal[32],tb1_pal[0],1,tb1_font,vis);
                    else VMWtextxy("ABOUT",123,87,
				   tb1_pal[32],tb1_pal[7],1,tb1_font,vis);
          if (barpos==3) VMWtextxy("LOAD GAME",123,97,
				   tb1_pal[32],tb1_pal[0],1,tb1_font,vis);
                    else VMWtextxy("LOAD GAME",123,97,
				   tb1_pal[32],tb1_pal[7],1,tb1_font,vis);
          if (barpos==4) VMWtextxy("STORY",123,107,
				   tb1_pal[32],tb1_pal[0],1,tb1_font,vis);
                    else VMWtextxy("STORY",123,107,
				   tb1_pal[32],tb1_pal[7],1,tb1_font,vis);
          if (barpos==5) VMWtextxy("CREDITS",123,117,
				   tb1_pal[32],tb1_pal[0],1,tb1_font,vis);
                    else VMWtextxy("CREDITS",123,117,
				   tb1_pal[32],tb1_pal[7],1,tb1_font,vis);
          if (barpos==6) VMWtextxy("QUIT",123,127,
				   tb1_pal[32],tb1_pal[0],1,tb1_font,vis);
                    else VMWtextxy("QUIT",123,127,
				   tb1_pal[32],tb1_pal[7],1,tb1_font,vis);
          ggiFlush(vis); 
          
	      /* If at title screen too long, run credits */
	  gettimeofday(&time_info,&dontcare);
          time_sec=time_info.tv_sec;  	  
	  while( ((ch=get_input())==0)) {
	     usleep(10);
             gettimeofday(&time_info,&dontcare);
	     if (time_info.tv_sec-time_sec>40) {
		credits();
		ch=TB_ENTER;
		barpos=9;
		reloadpic=1;
		gettimeofday(&time_info,&dontcare);
		time_sec=time_info.tv_sec;
	     }
	  }
	  
	      /* Change menu position based on key pressed */
	  ch2=toupper(ch);
          if ((ch==TB_DOWN)||(ch==TB_RIGHT)) barpos++;
          if ((ch==TB_UP) || (ch==TB_LEFT)) barpos--;
          if (ch==TB_F1) {barpos=10; ch=TB_ENTER;} /*F1*/
          if (ch2=='N') barpos=0;    /*N*/
          if (ch2=='O') barpos=1;    /*O*/
          if (ch2=='A') barpos=2;    /*A*/
          if (ch2=='L') barpos=3;    /*L*/
          if (ch2=='S') barpos=4;    /*S*/
          if (ch2=='C') barpos=5;    /*C*/
          if (ch2=='Q') barpos=6;    /*Q*/
          if (ch==27){ /* escape */
             barpos=6;
             ch=TB_ENTER;
          }
          if(barpos==7) barpos=0;
          if(barpos<0) barpos=6;
       }
          /* Run whatever it was that the person pressed */
       switch (barpos) {
        case 0: playthegame(0,0,12); reloadpic=1; break;
	case 1: options(vis); reloadpic=1; break;
        case 2: about(vis); reloadpic=1; break;
        case 3: loadgame(); reloadpic=1; break;
        case 4: story(); reloadpic=1; break;
        case 5: credits(); reloadpic=1; break;
	case 6: barpos=quit(vis); break;
        case 10: help(vis); break;
       }
    }
}
