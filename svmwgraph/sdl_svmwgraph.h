/* The SDL hooks for the Super VMW graphics library */

#include <SDL.h>

void *SDL_setupGraphics(int xsize,int ysize,int bpp,int scale,int fullscreen,
			int verbose);
void SDL_NoScale16bpp_BlitMem(vmwSVMWGraphState *target, vmwVisual *source);
void SDL_Double16bpp_BlitMem(vmwSVMWGraphState *target_p, vmwVisual *source);
void SDL_clearKeyboardBuffer();
int SDL_getInput();
