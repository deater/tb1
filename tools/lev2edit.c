
#define TB1_VERSION "2.9.0"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#include <ggi/libggi.h>
#include "../svmwgrap.h"
#include "../gtblib.h"

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

/*----------------*/

int game_map[12][200];
int max_x,max_y,u_x,u_y,x_pos=0,y_pos=0,x,y;
char tempst[40];


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
   
    printf("\nTom Bombem Level Editor by Vince Weaver weave@eng.umd.edu\n");
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
      
       /* Randomize random number generator */
    srandom(time(NULL));
   
       /* Load the tom bombem font */
    tb1_font=LoadVMWFont("../data/tbfont.tb1",8,16,256);
   
       /* Setup Graphics */
    if (setup_graphics(force_8bpp)==2) {   
       fprintf(stderr,"ERROR: Couldn't get display set up properly.\n");
       return 2;
    }
     
/*       ggiDrawVLine(vis,x+200,45+(2*x),80-(2*x));
    }
   
    ggiSetGCForeground(vis,tb1_pal[15]);
    VMWtextxy("A VMW SOFTWARE PRODUCTION",60,140,
	      tb1_pal[15],tb1_pal[15],0,tb1_font,vis);   
    ggiFlush(vis);
    pauseawhile(5);
  
    ggiSetGCForeground(vis,tb1_pal[0]); 
    ggiFillscreen(vis);
    
        Load the title screen */
    grapherror=GGILoadPicPacked(0,0,vis,1,1,
				"../data/tbomb1.tb1",
			        (ggi_color *)&eight_bit_pal,
				(ggi_pixel *)&tb1_pal,color_depth); 
    grapherror=GGILoadPicPacked(0,0,vaddr2,1,1,
				"../data/tbomb1.tb1",
			        (ggi_color *)&eight_bit_pal,
			        (ggi_pixel *)&tb1_pal,color_depth);
    ggiFlush(vis);
    ggiFlush(vaddr2);
    pauseawhile(5); 

   max_x=12;max_y=200;
    x_pos=0; y_pos=0;   
    
   for(x=0;x<max_x;x++) 
       for(y=0;y<max_y;y++) 
          game_map[x][y]=(rand()%26)+65;
   
       /* Main Menu Loop */
    while (1) {
       
       vmwCrossBlit(plb_vis->write,plb_vaddr->read,plb_vis->stride,200);
       ggiFlush(vis);
              
       barpos=0;
  
       ggiFlush(vis);
       ch=0;
  
       
          
	  while( ((ch=get_input())==0)) {
	     usleep(10);
	  }
	  
	      /* Change menu position based on key pressed */
	  ch2=toupper(ch);
          if (ch==TB_DOWN) y_pos++;
	  if (ch==TB_RIGHT) x_pos++;
          if (ch==TB_UP) y_pos--;
	  if (ch==TB_LEFT) x_pos--;
          if (y_pos<0) y_pos=0; if(x_pos<0) x_pos=0;	  
	  if (y_pos>(max_y-20)) y_pos=max_y-20;
	  if (x_pos>(max_x)) x_pos=max_x;
	
           ggiSetGCForeground(vaddr,tb1_pal[0]);
           ggiFillscreen(vaddr);
       
       
          for(x=0;x<max_x;x++)
	     for(y=y_pos;y<(y_pos+20);y++) {
		sprintf(tempst,"%c",game_map[x][y]);
	        VMWtextxy(tempst,x*20,(y-y_pos)*10,
			  tb1_pal[32],tb1_pal[0],1,tb1_font,vaddr);
		
	     }
	      
          if (ch2=='Q') barpos=6;    /*Q*/
          if (ch==27){ /* escape */
             barpos=6;
             ch=TB_ENTER;
          }
          if(barpos==7) barpos=0;
          if(barpos<0) barpos=6;
    
          /* Run whatever it was that the person pressed */
      if(barpos==6) quit();
    }
}
