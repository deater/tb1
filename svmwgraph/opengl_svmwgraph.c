/* The OpenGL hooks for the Super VMW graphics library */

/* based on the "glxheads.c" file from the Mesa 3.4 demos distribution */


#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>

#include "svmwgraph.h"
#include <stdlib.h>  /* For atexit() */
#include <stdio.h>

static float rotation=0.0;

unsigned char palette[256][4];

#define TEXTURE_SIZE 32

typedef struct {
   GLubyte *texture_data;
   GLuint  texture_name;
   GLfloat x1,y1,x2,y2;
} texture_block;

texture_block *texture_grid;
texture_block *current_texture;

GLuint temp_texture_name;
int texnumx,texnumy;

static Display *display;
static Window win;
static GLXContext ctx;

    /* Setup the Graphics */
    /* Pass '0' to auto-detect bpp */
void *openGL_setupGraphics(int *xsize,int *ysize,int *bpp,
			int fullscreen,int verbose) {
	    
    int scrnum,x,y;
    XSetWindowAttributes attr;
    Window root;
    XVisualInfo *visinfo;
    unsigned long mask;
    XSizeHints sizehints;
   
    int attrib[] = { GLX_RGBA,
	             GLX_RED_SIZE, 1,
	             GLX_GREEN_SIZE, 1,
	             GLX_BLUE_SIZE, 1,
	             GLX_DOUBLEBUFFER,
	             None };
   
   
   
    display=XOpenDisplay(NULL);
    if (display==NULL) {
       printf("Unable to open X display!\n");
       return NULL;
    }
   
    scrnum=DefaultScreen(display);
    root=RootWindow(display,scrnum);
    visinfo=glXChooseVisual(display,scrnum,attrib);
    if (visinfo==NULL) {
       printf("Unable to make RGB double-buffered visual\n");
       return NULL;
    }
   
       /* Window Attributes */
    attr.background_pixel=0;
    attr.border_pixel=0;
    attr.colormap=XCreateColormap(display,root,visinfo->visual,AllocNone);
    attr.event_mask=StructureNotifyMask | ExposureMask | KeyPressMask;
    mask =CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;
   
    win = XCreateWindow(display,root,0,0,640,400,
		       0,visinfo->depth,InputOutput,visinfo->visual,mask,
		       &attr);
    if (!win) {
       printf("Could not create X-window!\n");
       return NULL;
    }
   
    sizehints.x=10;
    sizehints.y=10;
    sizehints.width=640;
    sizehints.height=400;
    sizehints.flags=USSize | USPosition;
    XSetNormalHints(display,win,&sizehints);
    XSetStandardProperties(display,win,NULL,NULL,
			  None,(char **)NULL,0,&sizehints);
    ctx=glXCreateContext(display,visinfo,NULL,True);
    if (ctx==NULL) {
       printf("Can't create GLX context!\n");
       return NULL;
    }
    XMapWindow(display,win);
    if (!glXMakeCurrent(display,win,ctx)) {
       printf("glXMakeCurrent failed!\n");
       return NULL;
     }
   
     printf("  GL_VERSION:  %s\n", glGetString(GL_VERSION));
     printf("  GL_VENDOR:   %s\n", glGetString(GL_VENDOR));
     printf("  GL_RENDERER: %s\n", glGetString(GL_RENDERER));
      
     glClearColor(0.0,0.0,0.0,0.0);
     glShadeModel(GL_FLAT);
//     glShadeModel(GL_SMOOTH);
//     glEnable(GL_POLYGON_SMOOTH);
//     glEnable(GL_BLEND);
//     glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
   
     glPixelStorei(GL_UNPACK_ALIGNMENT,1);
   
//     for (i=0;i<NUM_TEXTURES;i++) {
  //   for (x=0;x<TEXTURE_SIZE;x++) {
//	for (y=0;y<TEXTURE_SIZE;y++) {
//	   textureBlock[i][x][y][3]=255;
//	}
  //   }
    // }

     printf("Init!\n"); fflush(stdout);

// Initializing Textures!!
//
   
     texnumx=320/TEXTURE_SIZE;
     if (320%TEXTURE_SIZE) texnumx++;
     texnumy=200/TEXTURE_SIZE;
     if (200%TEXTURE_SIZE) texnumy++;
   
     texture_grid=calloc(texnumx*texnumy,sizeof(texture_block));
   
     for(x=0;x<texnumx;x++) {
	for(y=0;y<texnumy;y++) {
	   current_texture=texture_grid+y*texnumx+x;
	   printf("%p\n",current_texture);
           current_texture->texture_data=malloc(TEXTURE_SIZE*TEXTURE_SIZE);
           
	   current_texture->x1=x*TEXTURE_SIZE;
	   current_texture->y1=y*TEXTURE_SIZE;
	   current_texture->x2=(x+1)*TEXTURE_SIZE;
	   if (current_texture->x2>320) current_texture->x2=320;
	   current_texture->y2=(y+1)*TEXTURE_SIZE;
	   if (current_texture->y2>200) current_texture->y2=200;
	   
	   glGenTextures(1,&temp_texture_name);	   
    
	   current_texture->texture_name=temp_texture_name;
	   
	   glBindTexture(GL_TEXTURE_2D, temp_texture_name);
	   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP);
	   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP);
	   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	   glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,TEXTURE_SIZE,
		       TEXTURE_SIZE,0,GL_RGBA,GL_UNSIGNED_BYTE,
		       current_texture->texture_data);
//	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	
	}
     }
   
   
      glOrtho(-20,340,-20,220,-100,100);
  
//     gluPerspective(60,1.6,-10,10);
   
     return (char *)(1);
}


void openGL_flushPalette(vmwSVMWGraphState *state) {

    int i;
       
    for(i=0;i<256;i++) {
       palette[i][0]=((state->palette[i]>>11)<<3);
       palette[i][1]=( ((state->palette[i]>>5)&0x3f)<<2);
       palette[i][2]=((state->palette[i]&0x1f)<<3);
       palette[i][3]=255;
    }

}


void openGL_BlitMem(vmwSVMWGraphState *target_p, vmwVisual *source) {

   
   int x=0,y=0,temp_col,i,j,ending_j,ending_i;
   unsigned char *s_pointer;
   
   GLubyte *t_pointer;
   
   if (!glXMakeCurrent(display,win,ctx)) {
      printf("Error getting context!\n");
      return;
   }
   
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glPushMatrix();
   
   glRotatef(rotation,160,100,0);
//   glEnable(GL_DEPTH_TEST);
   glEnable(GL_TEXTURE_2D);
   
   for(y=0;y<texnumy;y++) {
      for(x=0;x<texnumx;x++) {
	 current_texture=texture_grid+y*texnumx+x;
	 
         glBindTexture(GL_TEXTURE_2D,current_texture->texture_name);
      
	 s_pointer=source->memory+((y*TEXTURE_SIZE)*320)+(x*TEXTURE_SIZE);
	 
	 t_pointer=(current_texture->texture_data);
	 
	 ending_j=TEXTURE_SIZE;
	 if (((y*TEXTURE_SIZE)+TEXTURE_SIZE)>200)
	    ending_j=200%TEXTURE_SIZE;
	 	 
	 ending_i=TEXTURE_SIZE;
	 if (((y*TEXTURE_SIZE)+TEXTURE_SIZE)>320)
	    ending_i=320%TEXTURE_SIZE;
//	 printf("==>%i %i %p\n",x,y,current_texture); fflush(stdout);
	 
	 
	 for(j=0;j<ending_j;j++) {
            for(i=0;i<ending_i;i++) {
	       temp_col=*(s_pointer);
//	       printf(">>>%p %p=%i\n",t_pointer,s_pointer,temp_col); fflush(stdout);
	       memcpy(t_pointer,palette[temp_col],3);
	       s_pointer++;
	       t_pointer+=4;
	     }
	     s_pointer+=(320-TEXTURE_SIZE);
	  }

          glTexSubImage2D(GL_TEXTURE_2D,
		          0,0,0,TEXTURE_SIZE,TEXTURE_SIZE,
		          GL_RGBA,
		          GL_UNSIGNED_BYTE,
		          current_texture->texture_data);
      
          glBegin(GL_QUADS);
          glTexCoord2f(0.0,0.0); glVertex3f(current_texture->x1,current_texture->y1,0.0);
          glTexCoord2f(1.0,0.0); glVertex3f(current_texture->x2,current_texture->y1,0.0);
          glTexCoord2f(1.0,1.0); glVertex3f(current_texture->x2,current_texture->y2,0.0);
          glTexCoord2f(0.0,1.0); glVertex3f(current_texture->x1,current_texture->y2,0.0);
          glEnd();
      }
       
    }
    glFlush();
   
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glPopMatrix();
    
    glXSwapBuffers(display,win);
   
   /*
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
    */
}

void openGL_clearKeyboardBuffer() {
/*   SDL_Event event;
   while (SDL_PollEvent(&event)) {
*/      
   
}

void reshape(int w,int h) {
      glViewport(0,0,(GLsizei)w,(GLsizei)h);
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      if (w<=h)
          glOrtho(-30,30,-30*(GLfloat)h/(GLfloat)w,
		               30*(GLfloat)h/(GLfloat)w,-30,30);
      else
          glOrtho(-20,340,-20,
		               220,-100,100);
            
            
            
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
}
      



int openGL_getInput() {
    
    static int keydown=0;
    KeySym keysym;
    XEvent E;
    char keyname[18];
   
    while (XPending(display) > 0) {
       XNextEvent(display, &E);
       if (E.xany.window == win) {
	  switch (E.type) {
	      case Expose:
		   //Redraw();
		   break;
	      case ConfigureNotify:
		   reshape(E.xconfigure.width, E.xconfigure.height);
		   break;
	      case KeyPress:
	           keydown=1;
	      case KeyRelease:
	             /* get bare keysym, for the scancode */
	             keysym = XLookupKeysym ((XKeyEvent *) &E, 0);
	             /* get key name, using modifiers for the unicode */
	             XLookupString ((XKeyEvent *) &E, keyname, 16, NULL, NULL);
	     
//	             fprintf(stderr, "Keyevent keycode: %04X, keysym: %04X, unicode: %02X\n",
//	                  E.xkey.keycode, (unsigned int)keysym, (unsigned int)keyname[0]); 
	     
	             /* look which table should be used */
//	             if ( (keysym & ~0x1ff) == 0xfe00 )
//	                  event.scancode = extended_code_table[keysym & 0x01ff];
//	             else if (keysym < 0xff)
//	                  event.scancode = code_table[keysym & 0x00ff];
//	             else
//	                  event.scancode = 0;
//	     
//	             event.unicode = keyname[0];
	     
//	             keyboard_register_event(&event);
                     switch(E.xkey.keycode) {
		      case 0x09: return VMW_ESCAPE;
		      case 0x24: return VMW_ENTER;
		      case 0x62: return VMW_UP;
		      case 0x68: return VMW_DOWN;
		      case 0x66: return VMW_RIGHT;
		      case 0x64: return VMW_LEFT;
		      case 0x41: return ' ';
		      case 0x43: return VMW_F1;
		      case 0x44: return VMW_F2;
		      case 0x63: return VMW_PGUP;
		      case 0x69: return VMW_PGDN;
		      case 0x4A: /* F8 */
			rotation+=10.0;
			printf("Rotation now=%f\n",rotation);
			return 0;
		     }
	             break;
	     
	      default:
		   /*no-op*/ ;
	  }
       }
    }
        
   
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
