/* WARNING!  These functions only work at 320x200x8bpp right now */

/* It is easy to obtain the docs to make this load/save arbitrary */
/* PCX files.  However, I don't have the time nor the inclination */
/* To do it right now ;) */

/* Routines to Load/Save PCX graphics files. */

#include "svmwgraph.h"

#include <stdio.h>  /* For FILE I/O */
#include <string.h> /* For strncmp */
#include <fcntl.h>  /* for open()  */
#include <unistd.h> /* for lseek() */
#include <sys/stat.h> /* for file modes */


int vmwLoadPCX(int x1,int y1,vmwVisual *target,
	       int LoadPal,int LoadPic,char *FileName,
	       vmwSVMWGraphState *graph_state)

{

    int pcx_fd,x,y,i,r,g,b,numacross;
    unsigned char pcx_header[128];
    unsigned char temp_byte;
   
       /* Open the file */                  
    pcx_fd=open(FileName,O_RDONLY);
    
    if (pcx_fd<0) {
       printf("ERROR!  File \"%s\" not found!\n",FileName);
       return VMW_ERROR_FILE;
    }
   
    read(pcx_fd,&pcx_header,128);
   
    /* Possibly add some sanity checking in the header at some point... */
    /* Or actually even get the proper VALUES from the header.  Some day... */
    
    if (LoadPic) {
    
       x=0; y=0;
   
       while (y<200) {
          read(pcx_fd,&temp_byte,1);
          if ((temp_byte>=192) && (temp_byte<=255)) {
	     numacross=temp_byte-192;
	     read(pcx_fd,&temp_byte,1);
	     vmwDrawHLine(x,y,numacross,temp_byte,target);
	     x+=numacross;
          }
          else {
	     vmwPutPixel(x,y,temp_byte,target);
	     x++;
          }
          if (x>319) {
	     x=0;
	     y++;
          }
       }
    }
   
       /*Load Palette*/
    if (LoadPal) {
    
       lseek(pcx_fd,-768,SEEK_END);
   
       for(i=0;i<255;i++) { 
          read(pcx_fd,&r,1);
	  read(pcx_fd,&g,1);
	  read(pcx_fd,&b,1);
	  vmwLoadPalette(graph_state,
			 r,
			 g,
			 b,i);
       }
       vmwFlushPalette(graph_state);
    }
   
    close(pcx_fd);
    return 0;
}

int vmwSavePCX(int x1,int y1,int xsize,int ysize,
	       vmwVisual *source,
	       int num_colors,
	       vmw24BitPal *palette,
	       char *FileName) {
   
   
    int pcx_fd,x,y,oldcolor,color,numacross,i;
    unsigned char *pcx_header;
    unsigned char temp_byte;
   
       
    pcx_fd=open(FileName,O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR);
    if (pcx_fd<0) {
       printf("Error opening \"%s\" for writing!\n",FileName);
       return VMW_ERROR_FILE;
    }
   
    pcx_header=calloc(1,128);
       /* Faked from a proper 320x200x256 pcx created with TheGIMP */
       /* and read with 'od -t x1 -N 128' */
       /* Values verified from PCGPE .pcx documentation */
    pcx_header[0]=0x0a;  /* Manufacturer ID-- A=ZSoft .pcx */
    pcx_header[1]=0x05;  /* Version # */
    pcx_header[2]=0x01;  /* Encoding.  1=RLE */
    pcx_header[3]=0x08;  /* Bits Per Pixel */
    pcx_header[8]=0x3f;  /* 4-11 Window.  XminXmin YminYmin XmaxXmax YmaxYmax*/
    pcx_header[9]=0x01;  /* Little Endian, so 0000 0000 013f 00c7= 319x199 */  
    pcx_header[10]=0xc7; /* " */
    pcx_header[12]=0x2c; /* Horizontal DPI */
    pcx_header[13]=0x01; /* " .. so 0x12c=300dpi */
    pcx_header[14]=0x2c; /* Vertical DPI */
    pcx_header[15]=0x01; /* " .. so 0x12c=300dpi */
    pcx_header[65]=0x01; /* Number of color planes */
    pcx_header[66]=0x40; /* bytes per line. */
    pcx_header[67]=0x01; /* "" .. so 0x140=320 */
    pcx_header[68]=0x01; /* Color Palette */
   
       /* 128 byte PCX Header */
    write(pcx_fd,pcx_header,128);

       /* All we support right now */
    xsize=320;
    ysize=200;
   
    y=y1;
    x=x1;
    numacross=1;
   
       /* Set up initial conditions */
    oldcolor=vmwGetPixel(x,y,source);
   
    while (y<y1+ysize) {
       color=vmwGetPixel(x,y,source);
       if ( (color==oldcolor)&&(numacross<63)&&(x<(x1+xsize-1)) ) numacross++;
       else { /* This pixel not the same color as previous */
//	  printf("G: %i,%i N=%i C=%i\n",x,y,numacross,color);
	  if ((numacross==1) && (oldcolor<192)) {
	     write(pcx_fd,&oldcolor,1);
	  }
	  else {
	     temp_byte=numacross+192;
	     write(pcx_fd,&temp_byte,1);
	     write(pcx_fd,&oldcolor,1);
             numacross=1;
	  }
       }
       oldcolor=color;
       x++;
       if (x>=x1+xsize) {
          x=0;
          y++;
          numacross=1;
//	  printf("%i %i %i\n",x,y,numacross);
//	  fflush(stdout);
       }
    }
   
    /* Urgh obscure */
    temp_byte=12;
    write(pcx_fd,&temp_byte,1);
   
           /* Write num_colors r,g,b */
    for(i=0;i<256;i++) {
       temp_byte=palette[i].r;
       write(pcx_fd,&temp_byte,1);
       temp_byte=palette[i].g;
       write(pcx_fd,&temp_byte,1);
       temp_byte=palette[i].b;
       write(pcx_fd,&temp_byte,1);
    }

    close(pcx_fd);
    return 0;
}
