void *openGL_setupGraphics(int *xsize,int *ysize,int *bpp,
			int fullscreen,int verbose);
void openGL_flushPalette(vmwSVMWGraphState *state);
void openGL_BlitMem(vmwSVMWGraphState *target_p, vmwVisual *source);
void openGL_clearKeyboardBuffer();
int openGL_getInput();
void openGL_closeGraphics();
