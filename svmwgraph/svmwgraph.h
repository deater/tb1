/* Defines for the Super VMW Graphics Library */

typedef struct {
   int xsize;
   int ysize;
   unsigned char *memory;
   int palette[256];
} vmwVisual;

typedef struct {
   char *font_data;
   int width;
   int height;
   int numchars;
} vmwFont;

typedef struct {
   int xsize;
   int ysize;
   unsigned char *spritedata;
} vmwSprite;

/* Function declarations */

    /* From vmw_core.c */
void vmwPutPixel(int x,int y,int color,vmwVisual *target);
unsigned char vmwGetPixel(int x,int y,vmwVisual *source);
void vmwClearScreen(vmwVisual *target, int color);
void vmwDrawHLine(int xstart,int ystart,int how_long,int color, 
		  vmwVisual *target);
void vmwDrawVLine(int xstart,int ystart,int how_long,int color, 
		  vmwVisual *target);
void vmwLine(int x1, int y1, int x2, int y2, int color,
	                  vmwVisual *destination);
void vmwDrawBox(int x1,int y1,int xsize,int ysize,int col, vmwVisual *where);

    /* From vmw_flip.c */
void vmwFlipVirtual(vmwVisual *destination, 
		    vmwVisual *source);
int vmwArbitraryCrossBlit(vmwVisual *src,int x1,int y1,int w,int h,
			  vmwVisual *dest,int x2,int y2);

    /* From vmw_font.c */

vmwFont *vmwLoadFont(char *namest,int xsize,int ysize,int numchars);
void vmwTextLine(char *st,int x,int y,int col,int background,int line,
		     int transparent,vmwFont *font,vmwVisual *target);
void vmwTextXY(char *st,int x,int y,int col,int background,int overwrite,
	       vmwFont *font,vmwVisual *target);
void vmwSmallTextXY(char *st,int x,int y,int col,int background,int overwrite,
		    vmwFont *font,vmwVisual *target);

    /* From vmw_paintpro.c */

int vmwLoadPicPacked(int x1,int y1,vmwVisual *target,
		     int LoadPal,int LoadPic,char *FileName);

int vmwSavePicPacked(int x1,int y1,int xsize,int ysize,
		     vmwVisual *source,char *FileName);


    /* From vmw_palette.c */
void vmwLoadCustomPalette(vmwVisual *source, int pal[256]);
void vmwFadeToBlack(vmwVisual *source);
void vmwUnFade(vmwVisual *source);


    /* From vmw_sprite.c */

vmwSprite *vmwGetSprite(int x, int y,
			int xsize, int ysize,vmwVisual *screen);

void vmwPutSprite(vmwSprite *sprite,int x,int y,int shield_color,
		  vmwVisual *screen);

void vmwPutPartialSprite(vmwVisual *destination,
			 vmwSprite *sprite,
			 int x_start,int y_start,
			 int x_stop, int y_stop);




