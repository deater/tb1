/* Implements the font handling of the Super VMW Graph Library */

#include "svmwgraph.h"

#include <stdio.h>  /* For File I/O */
#include <stdlib.h> /* For malloc/calloc */
#include <string.h> /* strlen() */

/* The font used is a standard VGA font.          */
/* To use the "smallText" functions, the VGA font */
/*    has to have a smaller (4x5) version of the  */
/*    font from chars 0-127 mirrored at 128-255   */


vmwFont *vmwLoadFont(char *namest,int xsize,int ysize,int numchars)
{
    unsigned char buff[16];
    FILE *f;
    int i,fonty,numloop;
    vmwFont *font;
    char *data;
   
    font=(vmwFont *)malloc(sizeof(vmwFont));
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

    /* Write a one-pixel cross section from location 'line'   */
    /*       of text at location x,y                          */
    /*       using color 'col' and background 'col'           */
    /*       using 'background' as a background color if      */
    /*       'transprent' not set.                            */ 
void vmwTextLine(char *st,int x,int y,int col,int background,int line,
		     int transparent,vmwFont *font,vmwVisual *target)
{
    int i,xx,len;
    len=strlen(st);
    for(i=0;i<len;i++)
       for(xx=0;xx<8;xx++){
          if ( (font->font_data[(st[i]*font->height)+line])&(128>>xx) ) 
	     vmwPutPixel((x+(i*8)+xx),y,col,target);
          else 
	     if (!transparent) vmwPutPixel((x+(i*8)+xx),y,background,target);
       }
}

    /* Output a string at location x,y */   
void vmwTextXY(char *st,int x,int y,int col,int background,int overwrite,
	       vmwFont *font,vmwVisual *target)
{
    int lineon,i,xx,len;

    len=strlen(st);
    for(lineon=0;lineon<8;lineon++){
       for(i=0;i<len;i++)
	  for(xx=0;xx<8;xx++){
	     if ( (font->font_data[(st[i]*font->height)+lineon])&(128>>xx) )
		vmwPutPixel((x+(i*8)+xx),y+lineon,col,target);
	     else
	        if (overwrite) 
		   vmwPutPixel((x+(i*8)+xx),y+lineon,background,target);
	  }
     } 
}
   
    /* Assume this is a special font with a "small" font at locations   */
    /*       128-255 and display the string                             */
void vmwSmallTextXY(char *st,int x,int y,int col,int background,int overwrite,
		    vmwFont *font,vmwVisual *target)
{
    int lineon,i,xx,len;

     /* This will print out a small 4x5 font located in chars */
     /* 128-255 in a normal VGA font.  These are custom made  */
     /* fonts; they are non-standard */
   
    len=strlen(st);
    for(lineon=0;lineon<5;lineon++) {
       for(i=0;i<len;i++)
	  for(xx=0;xx<5;xx++) {
	     if ( (font->font_data[((st[i]+128)*font->height)+lineon])
		  &(128>>xx) )
	        vmwPutPixel((x+(i*5)+xx),y+lineon,col,target);
             else
	       if (overwrite) 
		  vmwPutPixel((x+(i*5)+xx),y+lineon,background,target);
	  }
    }
}

