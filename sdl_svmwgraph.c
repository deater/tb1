/* I will finish this game!!! */
/* This is the SDL implementation of the Super VMW graphics library */
/* Which was originally a lot of Turbo-Pascal w inline assembly     */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include "SDL.h"
#include "tb_keypress.h"


/* hard coded for now */
#define X_WIDTH 320
#define Y_HEIGHT 200
#define BPP 2 /* Bytes Per Pixel, ie 16bpp */

//#define DEBUG 1 /* Until we get to Beta */

unsigned int global_palette[256];


int vmwPack3Bytes(int r, int g, int b) {

   return  ( ( ( (r>>3)&0x1f) <<11)+
             ( ( (g>>2)&0x3f) <<5)+ 
             ( ( (b>>3)&0x1f) ) );
}

void vmwLoadCustomPalette(int pal[256]) {
   int i;
   for (i=0;i<256;i++) {
      global_palette[i]=pal[i];

   }
}


void vmwBlitMemToSDL(SDL_Surface *target, unsigned char *source) {
   
   int x,y,Bpp;
   
   unsigned char *s_pointer,*t_pointer;
   

   if ( SDL_MUSTLOCK(target) ) {
      if ( SDL_LockSurface(target) < 0 )
      return;
   }
   
   Bpp= target->format->BytesPerPixel;
   
   s_pointer=source;
   t_pointer=((Uint8 *)target->pixels);
   
   for (x=0;x<320;x++)
       for (y=0;y<200;y++) {
           *((Uint16 *)(t_pointer))=*((Uint16 *)(s_pointer));
           s_pointer+=2; t_pointer+=2;
       }
   
   
     /* Update the display */
      if ( SDL_MUSTLOCK(target) ) {
	 SDL_UnlockSurface(target);
      }
   
      /* Write this out to the screen */
   SDL_UpdateRect(target, 0, 0, 320, 200);
   
}



    /*---------------------------------------------------------------*\
    |          vmwPutPixel                                            |
    \*---------------------------------------------------------------*/
     
void vmwPutPixel(int x,int y,int color, unsigned char *target) {
    Uint8   *bits;
   
#ifdef DEBUG   
    if ( (y> (Y_HEIGHT-1) ) || (x> (X_WIDTH-1) ) ) {
       printf("Out of bounds with PutPixel %i,%i\n",x,y);
       return;
    }
#endif
      
       /* Really Ugly SDL Framebuffer stuff */
    bits = ((Uint8 *)target)+(y*320+x)*2;
    *((Uint16 *)(bits)) = global_palette[color];
   
}

unsigned char vmwGetPixel(int x,int y, unsigned char *source) {
   /* This is really hacked together, based on the old */
   /* 256 color palette model used by paintPro graphics */
   /* and the old pascal version of this game.  I may try */
   /* to improve this in the future */
 
   unsigned char i;
   
   for (i=0;i<255;i++) {
       if (*(Uint16* )(((Uint8 *)source)+(y*320+x)*2)==global_palette[i])
	  return i;
   }
   printf("Unknown color!  This shouldn't happen!\n");
   return 0;
   
}


void vmwClearScreen(unsigned char *target, int color) {
 
  int x,y; 
   
  if (color==0) memset(target,0,320*200*2);
   
  else { /* Super in-efficient */
     for (x=0;x<320;x++)
         for (y=0;y<200;y++) 
	     vmwPutPixel(x,y,color,target);
  }
   
}

    /*---------------------------------------------------------------*\
    |     vmwDrawHLine                                                |
    |  This is really begging to be optimized.  I am sure some        |
    |       targets can do this in hardware                           |
    \*---------------------------------------------------------------*/
   
void vmwDrawHLine(int xstart,int ystart,int how_long,int color, 
		 unsigned char *target) {
   
   int i;
   
   for(i=0;i<how_long;i++) {
      vmwPutPixel(xstart+i,ystart,color,target);    
   }
}


void vmwDrawVLine(int xstart,int ystart,int how_long,int color, unsigned char *target) {
   int i;
   
   for(i=0;i<how_long;i++) {
      vmwPutPixel(xstart,ystart+i,color,target);
   }
}


    /*---------------------------------------------------------------*\
    |    vmwLoadPicPacked                                             |
    |  There is so much history in this function, I won't get into    |
    |       it here.                                                  |
    \*---------------------------------------------------------------*/

int vmwLoadPicPacked(int x1,int y1,unsigned char *target,
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


void clear_keyboard_buffer() {
   SDL_Event event;
   while (SDL_PollEvent(&event)) {
      
   }
}

void pauseawhile(int howlong) {
       
    struct timeval bob;
    struct timezone mree;
    long begin_s,begin_u;
   
    SDL_Event event;
       
    clear_keyboard_buffer();
    gettimeofday(&bob,&mree);
    begin_s=bob.tv_sec; begin_u=bob.tv_usec;
    while ((bob.tv_sec-begin_s)<howlong) {
       if (SDL_PollEvent(&event)) {
	  if (event.type==SDL_KEYDOWN) return;
       }
       usleep(30);
       gettimeofday(&bob,&mree);
    }
}   

typedef struct {
   char *font_data;
   int width;
   int height;
   int numchars;
} vmw_font;



void vmwFlipVirtual(unsigned char *destination, 
		    unsigned char *source) {
 
   memcpy(destination,source,320*200*2);
   

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

int vmwArbitraryCrossBlit(unsigned char *src,int x1,int y1,int w,int h,
			  unsigned char *dest,int x2,int y2)
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


int vmwPutPartialSprite(int *src,int w,int h,int stride_factor,
		        char *dest,int x,int y,int dest_stride,
			int x_start,int x_stop,int y_start,int y_stop)
{                          /* x_start/stop not implemented yet */
    int xx,yy;
   /*
    dest+=(dest_stride*y);
    for(yy=0;yy<h;yy++){ 
       for(xx=0;xx<w;xx++) 
          if ((*(src+xx)) && ((yy>=y_start) && (yy<=y_stop)) )
	     memcpy(dest+(stride_factor*(xx+x)),(src+xx),stride_factor);
	 // **(dest+xx+x)=15;
   src+=w;
   dest+=dest_stride;
    }
    */
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
		     vmw_font *font,unsigned char *target)
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
		 vmw_font *font,unsigned char *target)
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
	       vmw_font *font,unsigned char *target)
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
		    vmw_font *font,unsigned char *target)
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

/*

procedure outsmalltextxy(st:string;x,y,col,background:integer;where:word;overwrite:boolean);
label l0105;
var l,len,i,xadd,yadd,lineon:integer;
    n,k,o,min,max,qwerty:byte;
begin
  for lineon:=0 to 4 do
      if overwrite then
         outsmalltextlineover(st,x,y+lineon,col,background,lineon,where)
      else
         outsmalltextline(st,x,y+lineon,col,background,lineon,where);

end;



function sgn(a:real):integer;
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

void vmwFadeToBlack(void) {
}

void vmwUnFade(void) {
}

void vmwDrawBox(int x1,int y1,int xsize,int ysize,int col, unsigned char *where) {
   int i;
   
   for(i=0;i<ysize;i++) {
      vmwDrawHLine(x1,y1+i,xsize,col,where);
   }
}

int vmwGetInput() {
   
   SDL_Event event;
   int keypressed;
   
   while ( SDL_PollEvent(&event)) {
      
       switch(event.type) {
	case SDL_KEYDOWN:
	     keypressed=event.key.keysym.sym;
	     switch (keypressed) {
	      case SDLK_BACKSPACE: return TB_BACKSPACE;
	      case SDLK_ESCAPE   : return TB_ESCAPE;
	      case SDLK_RETURN   : return TB_ENTER;
	      case SDLK_UP       : return TB_UP;
	      case SDLK_DOWN     : return TB_DOWN;
	      case SDLK_RIGHT    : return TB_RIGHT;
	      case SDLK_LEFT     : return TB_LEFT;
	      case SDLK_F1       : return TB_F1;
	      case SDLK_F2       : return TB_F2;
	      case SDLK_PAGEUP   : return TB_PGUP;
	      case SDLK_PAGEDOWN : return TB_PGDN;
	      default:             return keypressed;
	     }
	default: break;
       }
   }
   return 0;
}
