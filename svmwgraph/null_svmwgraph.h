/* The NULL target, for when we don't have an actual output */

void *null_setupGraphics(int *xsize,int *ysize,int *bpp,
			int fullscreen,int verbose);

void null_flushPalette(vmwSVMWGraphState *state);
void null_BlitMem(vmwSVMWGraphState *target_p, vmwVisual *source);
void null_clearKeyboardBuffer();
int null_getInput();
void null_closeGraphics();
