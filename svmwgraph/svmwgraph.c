/* I will finish this game!!! */
/* This is an implementation of the Super VMW graphics library */
/* Which was originally a lot of Turbo-Pascal w inline assembly     */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include "tb_keypress.h"

/* For now, sort of hard-coded in */
#include "SDL_svmwgraph.h"

//#define DEBUG 1 /* Until we get to Beta */

int vmwPack3Bytes(int r, int g, int b) {

   return  ( ( ( (r>>3)&0x1f) <<11)+
             ( ( (g>>2)&0x3f) <<5)+ 
             ( ( (b>>3)&0x1f) ) );
}

void vmwLoadCustomPalette(vmwGraphState *state, int pal[256]) {
   int i;
   for (i=0;i<256;i++) {
       state->palette[i]=pal[i];
   }
}


    /*---------------------------------------------------------------*\
    |          vmwPutPixel                                            |
    \*---------------------------------------------------------------*/
     
void vmwPutPixel(int x,int y,int color,vmwVisual *target) {
   
#ifdef DEBUG   
    if ( (y> (target->ysize-1) ) || (x> (target->xsize-1) ) ) {
       printf("Out of bounds with PutPixel %i,%i\n",x,y);
       return;
    }
#endif

    target->memory[(y*target->xsize)+x]=color;
      
}

unsigned char vmwGetPixel(int x,int y,vmwVisual *source) {
    
   return source->memory[(y*source->xsize)+x];
   
}


void vmwClearScreen(vmwVisual *target, int color) {
   
  memset(target->memory,color,target->xsize*target->ysize);
    
}

    /*---------------------------------------------------------------*\
    |     vmwDrawHLine                                                |
    |  This is really begging to be optimized.  I am sure some        |
    |       targets can do this in hardware                           |
    \*---------------------------------------------------------------*/
   
void vmwDrawHLine(int xstart,int ystart,int how_long,int color, 
		  vmwVisual *target) {
   
   unsigned char *temp_pointer;
   int i;
   
   temp_pointer=target->memory+(ysart*target->xsize)+xstart;
   
   for(i=0;i<how_long;i++) {
      *temp_pointer=color;
      temp_pointer++;
   }
}


void vmwDrawVLine(int xstart,int ystart,int how_long,int color, 
		  vmwVisual *target) {
   
   unsigned char *temp_pointer;
   int i;
   
   temp_pointer=target_memory+(ystart*target->xsize)+xstart;
   
   for(i=0;i<how_long;i++) {
      *temp_pointer=color;
      temp_pointer+=target->xsize;
   }
}


    /*---------------------------------------------------------------*\
    |    vmwLoadPicPacked                                             |
    |  There is so much history in this function, I won't get into    |
    |       it here.                                                  |
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
       temp_palette[i]=vmwPack3Bytes(buffer[buffpointer],
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
       for (i=0;i<256;i++) global_palette[i]=temp_palette[i];
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

void vmwFlipVirtual(vmwVisual *destination, 
		    vmwVisual *source) {
 
   memcpy(destination,source,source->xsize*source->ysize);
   
}

int vmwCrossBlit(char *dest,char *src,int stride,int ysize)
{
  /*
   * 
  int y;

  for (y = 0; y < ysize; y++) {
      memcpy (dest, src, stride);
      src += stride;
      dest += stride;
  }
   */
  return 0;   
}

int vmwCrossBlit240(char *dest,char *src,int stride,int stride_factor,
		    int ysize)
{
   /*
     int y;
   
     for (y = 0; y < ysize; y++) {
	      memcpy (dest, src, (240*stride_factor));
	      src += stride;
	      dest += stride;
     }
    */
     return 0;
}

int vmwArbitraryCrossBlit(vmwVisual *src,int x1,int y1,int w,int h,
			  vmwVisual *dest,int x2,int y2)
{
   
   int y;
   unsigned char *source,*destination;
   
   
   source=src+(320*2*y1);
   destination=dest+(320*2*y2);
   
   for(y=0;y<h;y++) {
      memcpy ((destination+x2*2),(source+x1*2),(w*2));
      source+=320*2;
      destination+=320*2;
   }
    
   return 0;   
    
}


vmw_font *vmwLoadFont(char *namest,int xsize,int ysize,int numchars)
{
    unsigned char buff[16];
    FILE *f;
    int i,fonty,numloop;
    vmw_font *font;
    char *data;
   
    font=(vmw_font *)malloc(sizeof(vmw_font));
    data=(char *)calloc(numchars*ysize,(sizeof(char)));
   
    f=fopen(namest,"r");
    if (f==NULL) {
       printf("ERROR loading font file %s.\n",namest);
       return NULL;
    }
    numloop=(numchars*ysize);
       
    font->width=xsize;
    font->height=ysize;
    font->numchars=numchars;
    font->font_data=data;
    fonty=0;
    while ( (!feof(f))&&(fonty<numloop)) {
       fread(buff,1,16,f);
       for(i=0;i<16;i++) font->font_data[fonty+i]=buff[i];
       fonty+=16;
    }
    fclose(f);
    return font;
}

/*int bit(unsigned char mree,int whichbit)
{
   return mree&(128>>whichbit);
}
 */

void vmwTextLineOver(char *st,int x,int y,int col,int background,int line,
		     vmw_font *font,vmwVisual *target)
{
    int i,xx,len;
    len=strlen(st);
    for(i=0;i<len;i++)
       for(xx=0;xx<8;xx++){
          if ( (font->font_data[(st[i]*font->height)+line])&(128>>xx) ) 
	     vmwPutPixel((x+(i*8)+xx),y,col,target);
          else 
	     vmwPutPixel((x+(i*8)+xx),y,background,target);
       }
}


void vmwTextLine(char *st,int x,int y,int col,int background,int line,
		 vmw_font *font,vmwVisual *target)
{
    int i,len,xx;
    len=strlen(st);
    for(i=0;i<len;i++)
       for(xx=0;xx<8;xx++)
	  if( ((unsigned char) (font->font_data[(st[i]*16)+line]))
	      &(128>>xx) ) {
	         vmwPutPixel(x+(i*8)+xx,y,col,target);
              }
}
   
void vmwTextXY(char *st,int x,int y,int col,int background,int overwrite,
	       vmw_font *font,vmwVisual *target)
{
    int lineon,i,xx,len;

    len=strlen(st);
    for(lineon=0;lineon<8;lineon++){
       for(i=0;i<len;i++)
	  for(xx=0;xx<8;xx++){
	     if ( (font->font_data[(st[i]*font->height)+lineon])&(128>>xx) )
		vmwPutPixel((x+(i*8)+xx),y+lineon,col,target);
	     else
	        if(overwrite) vmwPutPixel((x+(i*8)+xx),y+lineon,background,target);
	  }
     } 
       /*VMWtextlineover(st,x,y+lineon,col,background,lineon,font,vis);*/
       /* VMWtextline(st,x,y+lineon,col,background,lineon,font,vis);*/
}
   

void vmwSmallTextXY(char *st,int x,int y,int col,int background,int overwrite,
		    vmw_font *font,vmwVisual *target)
{
    int lineon,i,xx,len;

     /* This will print out a small 4x5 font located in chars */
     /* 128-255 in a normal VGA font.  These are custom made  */
     /* fonts they are non-standard */
   
    len=strlen(st);
    for(lineon=0;lineon<5;lineon++) {
       for(i=0;i<len;i++)
	  for(xx=0;xx<5;xx++) {
	     if ( (font->font_data[((st[i]+128)*font->height)+lineon])&(128>>xx) )
	        vmwPutPixel((x+(i*5)+xx),y+lineon,col,target);
             else
	       if(overwrite) vmwPutPixel((x+(i*5)+xx),y+lineon,background,target);
	  }
    }
}


void vmwLine(int x1, int y1, int x2, int y2, int color, vmwVisual *destination) {

/*function sgn(a:real):integer;
begin
     if a>0 then sgn:=+1;
     if a<0 then sgn:=-1;
     if a=0 then sgn:=0;
end;

procedure line(a,b,c,d,col:integer;where:word);
var u,s,v,d1x,d1y,d2x,d2y,m,n:real;
    i:integer;
begin
     u:= c - a;
     v:= d - b;
     d1x:= SGN(u);
     d1y:= SGN(v);
     d2x:= SGN(u);
     d2y:= 0;
     m:= ABS(u);
     n := ABS(v);
     IF NOT (M>N) then
     BEGIN
          d2x := 0 ;
          d2y := SGN(v);
          m := ABS(v);
          n := ABS(u);
     END;
     s := INT(m / 2);
     FOR i := 0 TO round(m) DO
     BEGIN
          putpixel(a,b,col,where);
          s := s + n;
          IF not (s<m) THEN
          BEGIN
               s := s - m;
               a:= a +round(d1x);
               b := b + round(d1y);
          END
          ELSE
          BEGIN
               a := a + round(d2x);
               b := b + round(d2y);
          END;
     end;
END;
*/

}
   
void vmwFadeToBlack(vmwVisual *source) {
    int temp_pal[256];
    // temp_pal=real_pal
    // for (i=0;i<256;i++) {
    //    temp_pal[i]--;
    //    vmwBlitMemToDisplay()
    //    pauseawhile()
    // 
}

void vmwUnFade(vmwVisual *source) {
}

void vmwDrawBox(int x1,int y1,int xsize,int ysize,int col, vmwVisual *where) {
   int i;
   
   for(i=0;i<ysize;i++) {
      vmwDrawHLine(x1,y1+i,xsize,col,where);
   }
}
