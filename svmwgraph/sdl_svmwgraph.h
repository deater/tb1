/* The SDL hooks for the Super VMW graphics library */

#include <SDL.h>

void *SDL_setupGraphics(int *xsize,int *ysize,int *bpp,int fullscreen,
			int verbose);
void SDL_WritePaletteColor(vmwSVMWGraphState *state,unsigned char r,
			   unsigned char g,unsigned char b,int color);
void SDL_NoScale16bpp_BlitMem(vmwSVMWGraphState *target, vmwVisual *source);
void SDL_Double16bpp_BlitMem(vmwSVMWGraphState *target_p, vmwVisual *source);
void SDL_NoScale8bpp_BlitMem(vmwSVMWGraphState *target, vmwVisual *source);
void SDL_clearKeyboardBuffer();
int SDL_getInput();
