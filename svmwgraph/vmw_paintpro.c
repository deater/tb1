/* An implementation of my own legacy "PaintPro[gram] graphics format */

/* The format is seemingly very random, but it was sort of OK for */
/*     my skill level at the time */

/* Version 1 was just, in text ASCII, a list of COLOR followed on a 
 *    new line by num-across, a very primitive RLE [Run-Length-Encoding]
 *    Compression scheme [which I thought up on my own] */

/* Later versions supported 256 color mode, and made sure the BASIC
 *   loader worked */

/* There was an actual "PaintPro program, written in TurboPascal
 *   that was fairly decent considering.  It would edit in
 *   the EGA and VGA 16 color modes, and later mode13 320x200x256 */

/* The files were still pretty large, so Version 5 implemented
 *    "Packing", basically shoving the color and numacross into
 *    a 24 bit value.  This, along with block-reading rapidly
 *    reduced load times */

/* Version 6 added a fancy header, along with palette saving, and
 * an optimization of storing multiple 1-pixel runs in half the size.
 * Before this palette files were stored separately. */

/* The resulting file format is very similar [quite accidentally] to
 *    the PCX file format.  Why don't I make paintpro files obsolete?
 *    nostalgia mostly.  */

/* Paintpro files usually have the extension .AMG [AMazing Graphics,
 *    an inside joke], .PPP [Paint Pro Packed], .PRO, or .TB1 [from
 *    Tom Bombem */




#include "svmwgraph.h"

#include <stdio.h>  /* For FILE I/O */
#include <string.h> /* For strncmp */
#include <fcntl.h>  /* for open()  */
#include <unistd.h> /* for lseek() */
#include <sys/stat.h> /* for file modes */


   /* Take a 24bit value [3 bytes] and split the top and bottom */
   /* 12 bits into 2 integers */
#define THREE_BYTES_INT1(a,b)   ( ( ( (int)(a))<<4)+ ((b)>>4) )
#define THREE_BYTES_INT2(b,c)   ( ( ( ( (int)(b))&0x0f)<<8)+ (c)) 

   /* Do the opposite.  Take 2 ints and make 3 bytes */

#define TWO_INTS_THREE_BYTES(a,b)  three_bytes[0]=((unsigned char)(((a)>>4)&0xff)); three_bytes[1]=((unsigned char) ( (((a)&0x0f)<<4)+(((b)>>8)&0x0f))); three_bytes[2]=((unsigned char)((b)&0xff));


vmwPaintProHeader *vmwGetPaintProHeader(char *filename) {
 
    FILE *fff;
    static vmwPaintProHeader *header=NULL;
    unsigned char buffer[18];
    int i;
   
    if ( (fff=fopen(filename,"rb"))==NULL) {  /* b for w32 stupidity */
       printf("Error!  %s does not exist!\n",filename);
       return NULL;
    }
        
    if (fread(buffer,1,18,fff)!=18) {
       printf("Error!  Corrupted paintpro header!\n");
       return NULL;
    }
   
    fclose(fff);
   
    if (header==NULL) {
       if (( header=calloc(1,sizeof(vmwPaintProHeader)))==NULL) {
          printf("Error!  Seriously low on memory!\n");
          return NULL;
       }
    }
    
       /* Load PAINTPRO string */
   for(i=0;i<8;i++) header->ppro_string[i]=buffer[i];
   for(i=0;i<4;i++) header->version[i]=buffer[i+8];   
   header->xsize=THREE_BYTES_INT1(buffer[12],buffer[13]);
   header->ysize=THREE_BYTES_INT2(buffer[13],buffer[14]);
   header->num_colors=THREE_BYTES_INT1(buffer[15],buffer[16]);
   
   return header;
}

    /*---------------------------------------------------------------*\
    |    vmwLoadPicPacked                                             |
    |  There is so much history in this function, I won't get into    |
    |       it here.  See the paintpro documentation found elsewhere  |
    \*---------------------------------------------------------------*/

int vmwLoadPicPacked(int x1,int y1,vmwVisual *target,
		     int LoadPal,int LoadPic,char *FileName,
		     vmwSVMWGraphState *graph_state)

  /* Retro comments */
/*{ Loads a paintpro image, filename, at location x1,y1      *\
\*  to offset where (vga=$A000) and loadspal if LOADPAL=true */
/*  Loadsapicture if Loadpic=true and returns error}         */

{
    int x,y,i;
    int ppro_fd;
    int color,numacross;
    int read_status;
    vmwPaintProHeader *ppro_header;
    unsigned char three_bytes[3];
    int broken=0;
   
    if ( (ppro_header=vmwGetPaintProHeader(FileName))==NULL) {
       printf("ERROR!  Invalid Header in %s!\n",FileName);
       return VMW_ERROR_FILE;
    }   
   
       /* Open the file */                  
    ppro_fd=open(FileName,O_RDONLY);
    
    if (ppro_fd<0) {
       printf("ERROR!  File \"%s\" not found!\n",FileName);
       return VMW_ERROR_FILE;
    }
   
       /* Check to see if its really a Paintpro File */
    if (strncmp(ppro_header->ppro_string,"PAINTPRO",8)) {
       printf("ERROR!  %s is NOT a paintpro file!\n",FileName);
       return VMW_ERROR_INVALID;
    }
   
       /* Check to see if it is the proper version (currently 6.0) */
    if (strncmp(ppro_header->version,"V6.1",4)) {
       if (!strncmp(ppro_header->version,"V6.0",4)) {
	  printf("Warning!  Broken 6.0 file! Please convert.\n");
	  broken=1;
       }
       else {
          printf("ERROR! %s unsupported, must be >=6.0\n",
	         ppro_header->version);
          return VMW_ERROR_INVALID;
       }
    }
      
       /*Load Palette*/
    if (ppro_header->num_colors>256) 
       printf("More than 256 colors not supported yet (%d)\n",
	      ppro_header->num_colors);
   
       /* Seek past the header */
    lseek(ppro_fd,18,SEEK_SET);
   
       /* Fun and games to convert the 24 bit color in paintpro to */
       /* 565 packed 16bit RGB */

    if (!LoadPal) {
          /* Skip past palette if not loading it */
       lseek(ppro_fd,3*ppro_header->num_colors,SEEK_CUR);    
    }
    else {
       for(i=0;i<ppro_header->num_colors;i++) { 
          read_status=read(ppro_fd,&three_bytes,3);
	  vmwLoadPalette(graph_state,
			 three_bytes[0],
			 three_bytes[1],
			 three_bytes[2],i);
       }
       vmwFlushPalette(graph_state);
    }
   
    if (!LoadPic) {
       close(ppro_fd);
       return 0;  /* We already loaded the palette */
    }
   
    x=x1;
    y=y1;

    while (y<y1+ppro_header->ysize) {
          /* Read in next 3 bytes */
       read_status=read(ppro_fd,&three_bytes,3);
       
          /* If we read less than 3, something is wrong */
       if (read_status<3) {
	  printf("Ran out of data too soon!\n");
	  break;
       }
       
       color=THREE_BYTES_INT1(three_bytes[0],three_bytes[1]);
       numacross=THREE_BYTES_INT2(three_bytes[1],three_bytes[2]);
       
          /* If int1 greater than 2047, we have two single pixels */
          /* of color int1-2048 and int2-2048                    */
       if (color>2047) {
          vmwPutPixel(x,y,color-2048,target);
          x++;
          if (x>=ppro_header->xsize+x1) {
	     x=x1; y++;
	  }
          vmwPutPixel(x,y,numacross-2048,target);
          x++;
          if (x>=ppro_header->xsize+x1){
	     x=x1;y++;
	  }
       }
       else {  /* Standard paintpro format */
	     /* Wrap if numacross is bigger than width */
          while ((x+numacross)>=(ppro_header->xsize+x1)) {
	     vmwDrawHLine(x,y,((ppro_header->xsize+x1)-x),color,target);
                /* Old versions of paintro created broken   */
	        /* Files which need an extra +1 after xsize */
	        /* In the following line */
	     numacross=numacross-((ppro_header->xsize+broken)-x);
	     x=x1;
	     y++;
          }
	  
          if (numacross!=0) {
	     if (y>=y1+ppro_header->ysize) {
		printf("Trying to draw past end of screen %d\n",y);
	     }
	     if (numacross > ppro_header->xsize-1) {
		printf("X too big?%d\n",numacross);
	     }
	     vmwDrawHLine(x,y,numacross,color,target);
	  }
          x=x+numacross;
       }
    }
    close(ppro_fd);
    return 0;
}

int vmwSavePicPacked(int x1,int y1,int xsize,int ysize,
		     vmwVisual *source,
		     int num_colors,
		     vmw24BitPal *palette,
		     char *FileName) {
   
   int ppro_fd,i,x,y,color=0,oldcolor=-1,numacross=0;
   int already_have_single_pixel=0;
   int save_pixel=0;
   char ppro_string[]="PAINTPRO";
   char ppro_version[]="V6.1";
   unsigned char three_bytes[3];
   
    ppro_fd=open(FileName,O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR);
    if (ppro_fd<0) {
       printf("Error opening \"%s\" for writing!\n",FileName);
       return VMW_ERROR_FILE;
    }
   
       /* 8 bytes, "PAINTPRO" */
    write(ppro_fd,ppro_string,8);
       /* 4 bytes, "V6.0" */
    write(ppro_fd,ppro_version,4);
   
       /* xsize, ysize  */
    TWO_INTS_THREE_BYTES(xsize,ysize);
    write(ppro_fd,three_bytes,3);
        /* num_colors, 0 */
    TWO_INTS_THREE_BYTES(num_colors,0);
    write(ppro_fd,three_bytes,3);
   
        /* Write num_colors r,g,b */
    for(i=0;i<num_colors;i++) {
       three_bytes[0]=palette[i].r;
       three_bytes[1]=palette[i].g;
       three_bytes[2]=palette[i].b;
       write(ppro_fd,three_bytes,3);
    }
   
    y=y1;
    x=x1;
   
       /* Set up initial conditions */
    oldcolor=vmwGetPixel(x,y,source);
   
    while (y<y1+ysize) {
       color=vmwGetPixel(x,y,source);
       if ((color==oldcolor)&&(numacross<2046)) numacross++;
       else { /* This pixel not the same color as previous */
	  if (numacross==1) { 
	        /* Special case for 1 pixel value */
	     if (!already_have_single_pixel) {
		   /* We are the first, so save it in hopes of a 2nd */
		save_pixel=oldcolor;
		already_have_single_pixel=1;
	     }
	     else {
		   /* We got a second.  Pack the two together */
		TWO_INTS_THREE_BYTES(save_pixel+2048,oldcolor+2048);
		write(ppro_fd,three_bytes,3);
		already_have_single_pixel=0;
	     }
	  }
	  else { /* We are more than 1 pixel in a row */
	     if (already_have_single_pixel) {
		   /* Oops.. wanted to pack, but we can't */
		TWO_INTS_THREE_BYTES(save_pixel,1);
		write(ppro_fd,three_bytes,3);
		already_have_single_pixel=0;
	     }
	        /* Write out color, and numacross */
	     TWO_INTS_THREE_BYTES(oldcolor,numacross);
	     write(ppro_fd,three_bytes,3);
	  }
	  oldcolor=color;
	  numacross=1;
       }
       x++;
       if (x>=x1+xsize) {
	  x=x1;
          y++;
       }
    }
       /* Write out data left */
    if (already_have_single_pixel) {
          /* Oops.. wanted to pack, but we can't */
       TWO_INTS_THREE_BYTES(save_pixel,1);
       write(ppro_fd,three_bytes,3);
    }
   
       /* Write out the last run */
    TWO_INTS_THREE_BYTES(color,numacross);
    write(ppro_fd,three_bytes,3);   
   
    close(ppro_fd);
    return 0;
   
}
