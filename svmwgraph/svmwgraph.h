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
   char ppro_string[8];
   char version[4];
   int xsize;
   int ysize;
   int num_colors;
} vmwPaintProHeader;

typedef struct {
   char r,g,b;
} vmw24BitPal;

typedef struct {
   int xsize;
   int ysize;
   int bpp;
   int scale;
   int *palette;
   int palette_size;
   vmw24BitPal *actual_pal;
   void *output_screen;
   vmwFont *default_font;
} vmwSVMWGraphState;

/* Constants */

     /* Display Target Constants */
#define VMW_NULLTARGET    0
#define VMW_SDLTARGET     1
#define VMW_CURSESTARGET  2  /* Yes I am insane */
#define VMW_OPENGLTARGET  3  /* Even Moreso */

     /* Error Constants */

#define VMW_ERROR_DISPLAY 1
#define VMW_ERROR_FILE    2
#define VMW_ERROR_SIZE    3
#define VMW_ERROR_MEM     4
#define VMW_ERROR_INVALID 5

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
#define VMW_F3        1032
#define VMW_PGUP      1050
#define VMW_PGDN      1051
#define VMW_HOME      1052
#define VMW_END       1053
#define VMW_INS       1054
#define VMW_DEL       1055

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

vmwPaintProHeader *vmwGetPaintProHeader(char *filename);
   
int vmwLoadPicPacked(int x1,int y1,vmwVisual *target,
		     int LoadPal,int LoadPic,char *FileName,
		     vmwSVMWGraphState *graph_state);

int vmwSavePicPacked(int x1,int y1,int xsize,int ysize,
		                          vmwVisual *source,
		                          int num_colors,
		                          vmw24BitPal *palette,
		                          char *FileName);

    /* From vmw_palette.c */
void vmwLoadPalette(vmwSVMWGraphState *state,unsigned char r,
		    unsigned char g,unsigned char b,int color); 
void vmwFadeToBlack(vmwSVMWGraphState *state,vmwVisual *source,int justLoadPal);
void vmwUnFade(vmwSVMWGraphState *state,vmwVisual *source);

 
    /* From vmw_pcx.c */

#define PCX_UNKNOWN 0
#define PCX_8BITPAL 1
#define PCX_24BIT   2

int vmwGetPCXInfo(char *FileName, int *xsize, int *ysize, int *type);
int vmwLoadPCX(int x1,int y1,vmwVisual *target,
		     int LoadPal,int LoadPic,char *FileName,
		     vmwSVMWGraphState *graph_state);

int vmwSavePCX(int x1,int y1,int xsize,int ysize,
		                          vmwVisual *source,
		                          int num_colors,
		                          vmw24BitPal *palette,
		                          char *FileName);


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

extern void (*vmwCloseGraphics)(void);

    /* From vmw_sprite.c */

vmwSprite *vmwGetSprite(int x, int y,
			int xsize, int ysize,vmwVisual *screen);

void vmwFreeSprite(vmwSprite *sprite);
void vmwPutSprite(vmwSprite *sprite,int x,int y,
		  vmwVisual *screen);

void vmwPutSpriteNonTransparent(vmwSprite *sprite,int x,int y,
		  vmwVisual *screen);

void vmwPutPartialSprite(vmwVisual *destination,
			 vmwSprite *sprite,int x,int y,
			 int x_start,int x_stop,
			 int y_start,int y_stop);

/* vmw_dialogs.c */
void vmwCoolBox(int x1,int y1,int x2,int y2,int fill,vmwVisual *target);
int vmwAreYouSure(vmwSVMWGraphState *graph_state,
		  vmwFont *tb1_font,
		  vmwVisual *target,
		  char *warning_1,
		  char *warning_2,
		  char *yes_option,
		  char *no_option);
