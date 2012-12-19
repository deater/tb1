void *curses_setupGraphics(int *xsize,int *ysize,int *bpp, int fullscreen,int verbose);
void curses_flushPalette(vmwSVMWGraphState *state);
void curses_BlitMem(vmwSVMWGraphState *target_p, vmwVisual *source);
void curses_clearKeyboardBuffer();
int curses_getInput();
void curses_closeGraphics(void);
