typedef struct {
      char *font_data;
      int width;
      int height;
      int numchars;
} vmw_font;

int vmwCrossBlit(char *dest,char *src,int stride,int ysize);
int vmwCrossBlit240(char *dest,char *src,int stride,int stride_factor,
		                        int ysize);
int vmwArbitraryCrossBlit(char *src,int x1,int y1,int w,int h,
			  char *dest,int x2,int y2,int stride,
			  int stridesfactor);




/*int vmwPutSprite(ggi_visual_t visual,int x,int y,int w,int h,
                                        int transparent_color,int *sprite);
*/
int vmwPutSprite(int *src,int w,int h,int stride_factor,
		                  char *dest,int x,int y,int dest_stride);

int vmwPutPartialSprite(int *src,int w,int h,int stride_factor,
			char *dest,int x,int y,int dest_stride,
		       int x_start,int x_stop,int y_start,int y_stop);

int vmwGetSprite(ggi_visual_t visual,int x,int y,int w,int h,
		                                   int *sprite);


int GGILoadPicPacked(int x1,int y1,ggi_visual_t vis
		     ,int LoadPal,int LoadPic,char *FileName,
		     ggi_color *eight_bit_pal,ggi_pixel *tb1_pal,
		     int color_depth);
vmw_font *LoadVMWFont(char *namest,int xsize,int ysize,int numchars);
void VMWtextlineover(char *st,int x,int y,int col,int background,int line,
		                          vmw_font *font,ggi_visual_t vis);
void VMWtextline(char *st,int x,int y,int col,int background,int line,
		                  vmw_font *font,ggi_visual_t vis);
void VMWtextxy(char *st,int x,int y,int col,int background,int overwrite,
	                      vmw_font *font,ggi_visual_t vis);
void VMWsmalltextxy(char *st,int x,int y,int col, int background, int overwrite,
		              vmw_font *font,ggi_visual_t vis);


