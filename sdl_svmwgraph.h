/* These are more or less the functions you need to implement one of my */
/* games.  This has been done before in SDL, GGI, and Turbo Pascal      */
/* so it should be easy to do for other targets... */

typedef struct {
      char *font_data;
      int width;
      int height;
      int numchars;
} vmw_font;

int vmwPack3Bytes(int r, int g, int b);
int vmwLoadCustomPalette(int pal[256]);

void vmwBlitMemToSDL(SDL_Surface *target, unsigned char *source);
void vmwClearScreen(unsigned char *target, int color);
   


void vmwPutPixel(int x,int y,int color, unsigned char *target);
unsigned char vmwGetPixel(int x,int y, unsigned char *source);

void vmwDrawHLine(int xstart,int ystart,int how_long,int color, 
		 unsigned char *target);

void vmwDrawVLine(int xstart,int ystart,int how_long,int color, unsigned char *target);
int vmwLoadPicPacked(int x1,int y1,unsigned char *target,
		     int LoadPal,int LoadPic,char *FileName);

void vmwFlipVirtual(unsigned char *destination, unsigned char *source);

int vmwCrossBlit(char *dest,char *src,int stride,int ysize);

int vmwCrossBlit240(char *dest,char *src,int stride,int stride_factor,
		    int ysize);

int vmwArbitraryCrossBlit(unsigned char *src,int x1,int y1,int w,int h,
			  unsigned char *dest,int x2,int y2);

int vmwPutPartialSprite(int *src,int w,int h,int stride_factor,
		        char *dest,int x,int y,int dest_stride,
			int x_start,int x_stop,int y_start,int y_stop);

vmw_font *vmwLoadFont(char *namest,int xsize,int ysize,int numchars);


void vmwTextLineOver(char *st,int x,int y,int col,int background,int line,
		     vmw_font *font,unsigned char *target);

void vmwTextLine(char *st,int x,int y,int col,int background,int line,
		 vmw_font *font,unsigned char *target);
   
void vmwTextXY(char *st,int x,int y,int col,int background,int overwrite,
	       vmw_font *font,unsigned char *target);

void vmwSmallTextXY(char *st,int x,int y,int col,int background,int overwrite,
		    vmw_font *font,unsigned char *target);


void line(int a,int b,int c,int d,int col,char *where);
void vmwFadeToBlack(void);
void vmwUnFade(void);
void vmwDrawBox(int x1,int y1,int x2,int y2,int col,unsigned char *where);
int vmwGetInput();
   
void clear_keyboard_buffer();

void pauseawhile(int howlong);
