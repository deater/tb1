/* Defines for the Super VMW Graphics Library */


/* Structures */
   
typedef struct {
   int xsize;
   int ysize;
   unsigned char *memory;
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


typedef struct {
   int xsize;
   int ysize;
   int bpp;
   int scale;
   int *palette;
   int palette_size;
   void *output_screen;
   vmwFont *default_font;
} vmwSVMWGraphState;

/* Constants */

     /* Display Target Constants */
#define VMW_SDLTARGET     1

     /* Error Constants */

#define VMW_ERROR_DISPLAY 1
#define VMW_ERROR_FILE    2
#define VMW_ERROR_SIZE    3
#define VMW_ERROR_MEM     4

    /* Keypress Constants */
#define VMW_BACKSPACE 1024
#define VMW_ESCAPE      27
#define VMW_ENTER     1025
#define VMW_UP        1026
#define VMW_DOWN      1027
#define VMW_RIGHT     1028
#define VMW_LEFT      1029
#define VMW_F1        1030
#define VMW_F2        1031
#define VMW_PGUP      1032
#define VMW_PGDN      1033


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
		    vmwVisual *source,int xsize, int ysize);
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
		     int LoadPal,int LoadPic,char *FileName,
		     vmwSVMWGraphState *graph_state);

int vmwSavePicPacked(int x1,int y1,int xsize,int ysize,
		     vmwVisual *source,char *FileName);


    /* From vmw_palette.c */
void vmwLoadPalette(vmwSVMWGraphState *state,unsigned char r,
		    unsigned char g,unsigned char b,int color); 
void vmwFadeToBlack(vmwVisual *source);
void vmwUnFade(vmwVisual *source);

    /* From vmw_setup.c */


extern void *(*vmwSetupGraphics)(int *xsize,int *ysize, int *bpp, 
				 int fullscreen,int verbose);
extern void (*vmwBlitMemToDisplay)(vmwSVMWGraphState *display, vmwVisual *source);
extern void (*vmwFlushPalette)(vmwSVMWGraphState *state);
extern void (*vmwClearKeyboardBuffer)(void);
extern int (*vmwGetInput)(void);
vmwSVMWGraphState *vmwSetupSVMWGraph(int display_type,int xsize,int ysize,
				     int bpp,int scale,int fullscreen,
				     int verbose);
vmwVisual *vmwSetupVisual(int xsize,int ysize);

    /* From vmw_sprite.c */

vmwSprite *vmwGetSprite(int x, int y,
			int xsize, int ysize,vmwVisual *screen);

void vmwPutSprite(vmwSprite *sprite,int x,int y,
		  vmwVisual *screen);

void vmwPutSpriteNonTransparent(vmwSprite *sprite,int x,int y,
		  vmwVisual *screen);

void vmwPutPartialSprite(vmwVisual *destination,
			 vmwSprite *sprite,int x,int y,
			 int x_start,int x_stop,
			 int y_start,int y_stop);

