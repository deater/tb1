/* Converts a 24-bit PCX file to 256-color SNES background tiles */

#include <stdio.h>  /* For FILE I/O */
#include <string.h> /* For strncmp */
#include <fcntl.h>  /* for open()  */
#include <unistd.h> /* for lseek() */
#include <sys/stat.h> /* for file modes */
#include <stdlib.h>  /* exit() */

static unsigned int hflip_byte(unsigned int byte) {

   int new_byte=0,i;

   for(i=0;i<8;i++) {
      new_byte<<=1;
      if (byte&(1<<i)) new_byte|=1;
   }

   return new_byte;
}

/* Convert to 15-bpp bgr */
static int rgb2bgr(int r,int g, int b) {
  int r2,g2,b2,bgr;

  r2=(r>>3)&0x1f;
  g2=(g>>3)&0x1f;
  b2=(b>>3)&0x1f;

  bgr=(b2<<10)|(g2<<5)|r2;

  return bgr;
}

static char symbol_name[BUFSIZ]="temp";

#define MAX_TILE_X 32
#define MAX_TILE_Y 32

static unsigned short tilemap[MAX_TILE_X*MAX_TILE_Y]; /* 2k */
static unsigned short tiledata[MAX_TILE_X*MAX_TILE_Y][8][4];
static unsigned short tiledata_hflip[MAX_TILE_X*MAX_TILE_Y][8][4];
static unsigned short temp_tile[8][4];
static int total_tiles=0;
static int compressed_tiles=0;



/* File already open */
static int vmwLoadPCX(int pcx_fd) {

   int debug=1,bpp;
   int x,y;
   int i,numacross,planes=0;
   int xsize,ysize,plane;
   int xmin,ymin,xmax,ymax,version;
   unsigned char pcx_header[128];
   unsigned char temp_byte;

    /*************** DECODE THE HEADER *************************/

    read(pcx_fd,&pcx_header,128);

    xmin=(pcx_header[5]<<8)+pcx_header[4];
    ymin=(pcx_header[7]<<8)+pcx_header[6];

    xmax=(pcx_header[9]<<8)+pcx_header[8];
    ymax=(pcx_header[11]<<8)+pcx_header[10];

    version=pcx_header[1];
    bpp=pcx_header[3];

    if (debug) {

       fprintf(stderr,"Manufacturer: ");
       if (pcx_header[0]==10) fprintf(stderr,"Zsoft\n");
       else fprintf(stderr,"Unknown %i\n",pcx_header[0]);

       fprintf(stderr,"Version: ");

       switch(version) {
        case 0: fprintf(stderr,"2.5\n"); break;
        case 2: fprintf(stderr,"2.8 w palette\n"); break;
        case 3: fprintf(stderr,"2.8 w/o palette\n"); break;
        case 4: fprintf(stderr,"Paintbrush for Windows\n"); break;
        case 5: fprintf(stderr,"3.0+\n"); break;
        default: fprintf(stderr,"Unknown %i\n",version);
       }
       fprintf(stderr,"Encoding: ");
       if (pcx_header[2]==1) fprintf(stderr,"RLE\n");
       else fprintf(stderr,"Unknown %i\n",pcx_header[2]);

       fprintf(stderr,"BitsPerPixelPerPlane: %i\n",bpp);
       fprintf(stderr,"File goes from %i,%i to %i,%i\n",xmin,ymin,xmax,ymax);

       fprintf(stderr,"Horizontal DPI: %i\n",(pcx_header[13]<<8)+pcx_header[12]);
       fprintf(stderr,"Vertical   DPI: %i\n",(pcx_header[15]<<8)+pcx_header[14]);

       fprintf(stderr,"Number of colored planes: %i\n",pcx_header[65]);
       fprintf(stderr,"Bytes per line: %i\n",(pcx_header[67]<<8)+pcx_header[66]);
       fprintf(stderr,"Palette Type: %i\n",(pcx_header[69]<<8)+pcx_header[68]);
       fprintf(stderr,"Hscreen Size: %i\n",(pcx_header[71]<<8)+pcx_header[70]);
       fprintf(stderr,"Vscreen Size: %i\n",(pcx_header[73]<<8)+pcx_header[72]);

    }
    planes=pcx_header[65];

    xsize=((xmax-xmin)+1);
    ysize=((ymax-ymin)+1);

    char *output;

    output=calloc((xsize*ysize),sizeof(unsigned int));
    if (output==NULL) return -1;

   x=0; y=0;

   while(y<ysize) {
      for(plane=0;plane<planes;plane++) {
      x=0;
       while (x<xsize) {
          read(pcx_fd,&temp_byte,1);
          if (0xc0 == (temp_byte&0xc0)) {
	     numacross=temp_byte&0x3f;
	     read(pcx_fd,&temp_byte,1);
//	     fprintf(stderr,"%i pixels of %i\n",numacross,temp_byte);
	     for(i=0;i<numacross;i++) {
	       output[(y*xsize)+x]=temp_byte;
	       //	        printf("%x ",temp_byte);
		x++;
	     }
          }
          else {
//	     fprintf(stderr,"%i, %i Color=%i\n",x,y,temp_byte);
//	    printf("%x ",temp_byte);
	    output[(y*xsize)+x]=temp_byte;
	     x++;
          }
       }

      }
      y++;
   }

#define X_CHUNKSIZE 8
#define Y_CHUNKSIZE 8


   unsigned int plane0,plane1,plane2,plane3;
   unsigned int plane4,plane5,plane6,plane7,offset;

   printf("%s_tile_data:\n",symbol_name);
   int ychunk,xchunk;
   for(ychunk=0;ychunk<ysize/Y_CHUNKSIZE;ychunk++) {
      for(xchunk=0;xchunk<xsize/X_CHUNKSIZE;xchunk++) {

//	printf("\t; Tile %d %d, Plane 0 Plane 1\n",xchunk,ychunk);

        for(y=0;y<Y_CHUNKSIZE;y++){
           plane0=0;plane1=0;
           for(x=0;x<X_CHUNKSIZE;x++) {
              plane0<<=1;
              plane1<<=1;
	      offset=((ychunk*Y_CHUNKSIZE+y)*xsize)+(xchunk*X_CHUNKSIZE)+x;
              plane0|=(output[offset])&1;
              plane1|=(((output[offset])&2)>>1);
	   }
	   temp_tile[y][0]=(plane1<<8)|plane0;
//           printf("\t.word $%02x%02x\n",plane1,plane0);
        }

//        printf("\t; Plane 2 Plane 3\n");
        for(y=0;y<Y_CHUNKSIZE;y++){
           plane2=0;plane3=0;
           for(x=0;x<X_CHUNKSIZE;x++) {
              plane2<<=1;
              plane3<<=1;

	      offset=((ychunk*Y_CHUNKSIZE+y)*xsize)+(xchunk*X_CHUNKSIZE)+x;
              plane2|=(((output[offset])&4)>>2);
              plane3|=(((output[offset])&8)>>3);
	   }
	   temp_tile[y][1]=(plane3<<8)|plane2;
//	   printf("\t.word $%02x%02x\n",plane3,plane2);
	}

//        printf("\t; Plane 4 Plane 5\n");
        for(y=0;y<Y_CHUNKSIZE;y++){
           plane4=0;plane5=0;
           for(x=0;x<X_CHUNKSIZE;x++) {
              plane4<<=1;
              plane5<<=1;

	      offset=((ychunk*Y_CHUNKSIZE+y)*xsize)+(xchunk*X_CHUNKSIZE)+x;
              plane4|=(((output[offset])&16)>>4);
              plane5|=(((output[offset])&32)>>5);
	   }
	   temp_tile[y][2]=(plane5<<8)|plane4;
//           printf("\t.word $%02x%02x\n",plane5,plane4);
        }

//        printf("\t; Plane 6 Plane 7\n");
        for(y=0;y<Y_CHUNKSIZE;y++){
           plane6=0;plane7=0;
           for(x=0;x<X_CHUNKSIZE;x++) {
              plane6<<=1;
              plane7<<=1;

	      offset=((ychunk*Y_CHUNKSIZE+y)*xsize)+(xchunk*X_CHUNKSIZE)+x;
              plane6|=(((output[offset])&64)>>6);
              plane7|=(((output[offset])&128)>>7);
	   }
	   temp_tile[y][3]=(plane7<<8)|plane6;
//	   printf("\t.word $%02x%02x\n",plane7,plane6);
	}


	int i,plane,v=0,h=0,o=0,pal=0,found_tile=0,found=0,match;

        /* see if the new tile matches an existing one */

	for(i=0;i<compressed_tiles;i++) {
           /* see if matches direct */
           match=1;
           for(plane=0;plane<4;plane++) {
              for(y=0;y<8;y++) {
                 if (temp_tile[y][plane]!=tiledata[i][y][plane]) match=0;
              }
           }
           if (match) {
              v=0; h=0;
              found_tile=i;
              found=1;
              break;
           }

           /* see if matches with vflip */
           match=1;
           for(plane=0;plane<4;plane++) {
              for(y=0;y<8;y++) {
                 if (temp_tile[y][plane]!=tiledata[i][7-y][plane]) match=0;
              }
           }
           if (match) {
              v=1; h=0;
              found_tile=i;
              found=1;
              break;
           }

           /* see if matches with hflip */
           match=1;
           for(plane=0;plane<4;plane++) {
              for(y=0;y<8;y++) {
                 if (temp_tile[y][plane]!=tiledata_hflip[i][y][plane]) match=0;
              }
           }
           if (match) {
              v=0; h=1;
              found_tile=i;
              found=1;
              break;
           }
           /* see if matches with hflip and vflip */
           match=1;
           for(plane=0;plane<4;plane++) {
              for(y=0;y<8;y++) {
                 if (temp_tile[y][plane]!=tiledata_hflip[i][7-y][plane]) match=0;
              }
           }
           if (match) {
              v=1; h=1;
              found_tile=i;
              found=1;
              break;
           }

        }

        if (!found) {

           /* print tile data */
	   for(x=0;x<4;x++) {
	      printf("\t; Tile %d %d, Plane %d Plane %d\n",xchunk,ychunk,
	             x*2,(x*2)+1);
	      for(y=0;y<Y_CHUNKSIZE;y++) {
	         printf("\t.word $%04x\n",temp_tile[y][x]);
              }
           }
           found_tile=compressed_tiles;

           /* put data in lookup table */
           for(plane=0;plane<4;plane++) {
              for(y=0;y<8;y++) {
                 tiledata[compressed_tiles][y][plane]=temp_tile[y][plane];
              }
           }
           /* put hflip in lookup table */
           for(plane=0;plane<4;plane++) {
              for(y=0;y<8;y++) {
                 tiledata_hflip[compressed_tiles][y][plane]=
			(hflip_byte( (temp_tile[y][plane]>>8)&0xff)<<8) |
                         hflip_byte( temp_tile[y][plane]&0xff);
              }
           }


           compressed_tiles++;

        }
	else {
        }

	tilemap[total_tiles]=
	   ( ((v&1)<<15) |
             ((h&1)<<14) |
             ((o&1)<<13) |
             ((pal&7)<<10) |
             (found_tile&0x3ff));


	total_tiles++;
      }
   }

   fprintf(stderr,"Total tiles: %d    Compressed tiles %d\n",
	total_tiles,compressed_tiles);
   fprintf(stderr,"Original size\t= %d bytes\n",total_tiles*2*8*4);
   fprintf(stderr,"Compressed size\t= %d bytes\n",
	(compressed_tiles*2*8*4)+(total_tiles*2)); 

   printf("%s_tilemap:\n",symbol_name);

   for(x=0;x<total_tiles;x++) {
      printf("\t.word $%04x\t; %dv %dh %do %dp %d\n",
             tilemap[x],
             (tilemap[x]>>15)&1,
             (tilemap[x]>>14)&1,
             (tilemap[x]>>13)&1,
             (tilemap[x]>>10)&7,
             (tilemap[x])&0x3ff);

   }

   printf("%s_palette:\n",symbol_name);

   /* read in palette */
   read(pcx_fd,&temp_byte,1);
   if (temp_byte!=12) {
     fprintf(stderr,"Error!  No palette found!\n");
   }
   else {
     int r,g,b;
     for(i=0;i<256;i++) {
       read(pcx_fd,&temp_byte,1);
       r=temp_byte;
       read(pcx_fd,&temp_byte,1);
       g=temp_byte;
       read(pcx_fd,&temp_byte,1);
       b=temp_byte;
       printf("\t.word $%x\t; r=%x g=%x b=%x\n",rgb2bgr(r,g,b),r,g,b);
     }
   }

    return 0;
}


int main(int argc, char **argv) {

    int result;

    if (argc>1) {
       strncpy(symbol_name,argv[1],BUFSIZ);
    }

    /* read from stdin */

    result=vmwLoadPCX(fileno(stdin));

    if (result<0) {
       fprintf(stderr,"Error reading PCX from stdin\n");
       exit(1);
    }

    return 0;
}
