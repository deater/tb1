/* The OpenGL hooks for the Super VMW graphics library */


#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include "svmwgraph.h"
#include <stdlib.h>  /* For atexit() */
#include <stdio.h>

float palette[256][3];


    /* Setup the Graphics */
    /* Pass '0' to auto-detect bpp */
void *openGL_setupGraphics(int *xsize,int *ysize,int *bpp,
			int fullscreen,int verbose)
{

   
     /* ^^*&^$%*&$^*&%^$  I am too stubborn to pass the real argc and */
     /* argv through.. and I can't get this to work otherwise.  */
     /* I am tired of screwing with it though, so this is known broken */
    int argc2=3,i;
    char argv2[3][10];
   
    strcpy(argv2[0],"vmw");
    strcpy(argv2[1],"mlp");
    strcpy(argv2[2],"amg");
      
    for(i=0;i<argc2;i++) {
       printf("%i:  %s\n",i,argv2[i]);
    }
   
    printf("KNOWN BUG! OPENGL TARGET DOESN'T WORK\n"); fflush(stdout);
    glutInit(&argc2,(char **)&argv2);
    printf("B\n"); fflush(stdout);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize( *xsize,*ysize);
    glutInitWindowPosition(40,40);
    glutCreateWindow("SVMWGraph Target");
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_MODELVIEW);
//    glutDisplayFunc(display);
//    glutKeyboardFunc(keyboard);
    
   
       /* Clean up on exit */
   
    return (char *)(1);
}


void openGL_flushPalette(vmwSVMWGraphState *state) {

    int i;
       
    for(i=0;i<256;i++) {
       palette[i][0]=((float) ((state->palette[i]>>11)<<3))/256.0;
       palette[i][1]=((float) ( ((state->palette[i]>>5)&0x3f)<<2))/256.0;
       palette[i][2]=((float) ((state->palette[i]&0x1f)<<3))/256.0;
    }

}


void openGL_BlitMem(vmwSVMWGraphState *target_p, vmwVisual *source) {
   
   int x,y;
   
   unsigned char *s_pointer;
   
   s_pointer=source->memory;
   
   for (x=0;x<source->xsize;x++)
       for (y=0;y<source->ysize;y++) {
	   glColor3f(palette[*s_pointer][0],palette[*s_pointer][1],palette[*s_pointer][2]);
	   glRectf(x,y,x+1,y+1);
	   
//           *((Uint16 *)(t_pointer))=target_p->palette[*(s_pointer)];
           s_pointer++; 
       }
   glFlush();
}

void openGL_clearKeyboardBuffer() {
/*   SDL_Event event;
   while (SDL_PollEvent(&event)) {
*/      
   
}

int openGL_getInput() {
   
/*   SDL_Event event;
   int keypressed;
   
   while ( SDL_PollEvent(&event)) {
      
       switch(event.type) {
	case SDL_KEYDOWN:
	     keypressed=event.key.keysym.sym;
	     switch (keypressed) {
	      case SDLK_BACKSPACE: return VMW_BACKSPACE;
	      case SDLK_ESCAPE   : return VMW_ESCAPE;
	      case SDLK_RETURN   : return VMW_ENTER;
	      case SDLK_UP       : return VMW_UP;
	      case SDLK_DOWN     : return VMW_DOWN;
	      case SDLK_RIGHT    : return VMW_RIGHT;
	      case SDLK_LEFT     : return VMW_LEFT;
	      case SDLK_F1       : return VMW_F1;
	      case SDLK_F2       : return VMW_F2;
	      case SDLK_PAGEUP   : return VMW_PGUP;
	      case SDLK_PAGEDOWN : return VMW_PGDN;
	      default: if ((keypressed>='a') && (keypressed<='z')  
		          && (event.key.keysym.mod & KMOD_SHIFT) )
		          return keypressed-32;
		       else return keypressed;
	     }
	default: break;
       }
   }
 */
   return 0;
}

void openGL_closeGraphics() {
}
