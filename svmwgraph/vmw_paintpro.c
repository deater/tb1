/* An implementation of my own legacy "PaintPro[gram] graphics format */

#include "svmwgraph.h"

#include <stdio.h>  /* For FILE I/O */
#include <string.h> /* For strncmp */

    /*---------------------------------------------------------------*\
    |    vmwLoadPicPacked                                             |
    |  There is so much history in this function, I won't get into    |
    |       it here.  See the paintpro documentation found elsewhere  |
    \*---------------------------------------------------------------*/

int vmwLoadPicPacked(int x1,int y1,vmwVisual *target,
		     int LoadPal,int LoadPic,char *FileName)

  /* Retro comments */
/*{ Loads a paintpro image, filename, at location x1,y1      *\
\*  to offset where (vga=$A000) and loadspal if LOADPAL=true */
/*  Loadsapicture if Loadpic=true and returns error}         */

{
    unsigned char temp1,temp2,temp3;
    int errorlev;
    int int1,int2,i,x,y;
    char buffer[300];
    int lastread=0;
    int buffpointer=0;
    int picwidth,picheight;
    FILE *fff;
    char header[10];
    int xsize;
    int ysize;
    int numcolors;
    int col,numacross;
    int temp_palette[256];
   
       /* Open the file */                  
    fff=fopen(FileName,"rb");	/* Windows chokes if no "b" */
    if (fff==NULL){
       printf("PPRO error... File \"%s\" not found.\n",FileName);
       return 1;
    }
   
    errorlev=fread(buffer,1,300,fff);
       /* Check to see if its really a Paintpro File */
    for (i=0;i<9;i++) {
       header[i]=buffer[i];
    }
    header[9]='\0';
    if (strncmp(header,"PAINTPROV",9)) {
       printf("PPRO error... %s is NOT a paintpro file!\n",FileName);
       return 2;
    }
   
       /* Check to see if it is the proper version (currently 6.0) */
    header[0]=buffer[9];
    header[1]=buffer[10];
    header[2]=buffer[11];
    header[3]='\0';       
    if (strncmp(header,"6.0",3)) {
       printf("PPRO error... Version %s unsupported, must be >6\n",header);
       return 3;
    }
   
    buffpointer=12;
       /* Read X and Y sizes */
    temp1=buffer[buffpointer];
    temp2=buffer[buffpointer+1];
    temp3=buffer[buffpointer+2];
    buffpointer+=3;
    if (buffpointer>=errorlev) {
       printf("PPRO error... Early end of file.\n");
       return 4;
    }
   
      /* Split 48 bits into two 32 bit values */
    xsize=(temp2>>4)+(temp1<<4);
    ysize=((temp2-((temp2>>4)<<4))<<16)+temp3;
   
    temp1=buffer[buffpointer];
    temp2=buffer[buffpointer+1];
    temp3=buffer[buffpointer+2];
    buffpointer+=3;
    if (buffpointer>=errorlev) {
       printf("PPRO error... Early end of file.\n");
       return 4;
    }
      
    numcolors=(temp2>>4)+(temp1<<4);
   
    picwidth=xsize+1;
    picheight=ysize+1;
       /*Load Palette*/
    if (numcolors!=256) printf("%d colors is not supported yet.\n",numcolors);
   
       /* Fun and games to convert the 24 bit color in paintpro to */
       /* 565 packed 16bit RGB */
    
    for(i=0;i<256;i++) { 
       temp_palette[i]= vmwPack3Bytes(buffer[buffpointer],
				     buffer[buffpointer+1],
				     buffer[buffpointer+2]);
       buffpointer+=3;
       if (buffpointer>=errorlev) {
	  if (errorlev==300) {
	     errorlev=fread(buffer,1,300,fff);
	     buffpointer=0;
	  }
	  else {
	     lastread=1;
	  }
       }    
    }
   
    if (LoadPal) {
       for (i=0;i<256;i++) target->palette[i]=temp_palette[i];
    }
   
   
    x=x1;
    y=y1;
   
    while ((!lastread)&&(LoadPic)&&(y<y1+ysize)) {
       temp1=buffer[buffpointer];
       temp2=buffer[buffpointer+1];
       temp3=buffer[buffpointer+2];
       buffpointer+=3;
       if (buffpointer>=errorlev) {
	  if (errorlev==300) {
	     errorlev=fread(buffer,1,300,fff);
	     buffpointer=0;
	  }
	  else lastread=1;
       }
       
       int1=(temp2>>4)+(temp1<<4);
       int2=((temp2-((temp2>>4)<<4))*256)+temp3;       
       
          /* If greater than 2047, we have two single pixels */
          /* of color int1-2048 and int2-2048                */
       if (int1>2047) {
          vmwPutPixel(x,y,int1-2048,target);
          x++;
          if (x>xsize+x1) {
	     x=x1; y++;
//	     if (y>ysize-1) {
//		printf("Blargh 1\n"); y--;
//	     }
	  }
          vmwPutPixel(x,y,int2-2048,target);
          x++;
          if (x>xsize+x1){
	     x=x1;y++;
//	     if (y>ysize) {
//		printf("Blargh 2\n");
//		y--;
//	     }
	  }
       }
       else {  /* Standard paintpro format */
          col=int1;
          numacross=int2;
          while ((x+numacross)>(xsize+x1)) {
	     vmwDrawHLine(x,y,((xsize+x1)-x),col,target);
             numacross=numacross-((xsize+1)-x);
	     x=x1;
	     y++;
	     if (y>y1+ysize-1) {
		printf("Blrgh 3\n");
	     }
          }
          if ((numacross!=0) && (y<y1+ysize)) {
	     if (numacross+x>xsize+x-1) numacross--; /* Stupid x overflow */ 
	     vmwDrawHLine(x,y,numacross,col,target);
          }
          x=x+numacross;
       }
    }

    if (fff!=NULL) fclose(fff);
    return 0;
}

int vmwSavePicPacked(int x1,int y1,int xsize,int ysize,
		     vmwVisual *source,char *FileName) {
   return 0;
   
}
  
  
