/* The OpenGL hooks for the Super VMW graphics library */

#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "svmwgraph.h"
#include <stdlib.h>  /* For atexit() */
#include <stdio.h>

static float rotation=0.0;

unsigned char palette[256][4];

#define TEXTURE_SIZE 64

int texture_grid[5][4];
GLubyte *texture_data[5][4];


typedef struct {
   GLubyte *texture_data;
   GLuint  texture_name;
   GLfloat x1,y1,x2,y2;
} texture_block;


texture_block *current_texture;

GLuint temp_texture_name;
int texnumx,texnumy;



void reshape(int w,int h) {
      glViewport(0,0,(GLsizei)w,(GLsizei)h);
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      gluPerspective(60.0,(GLfloat)w/(GLfloat)h,1.0,100.0);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      //glTranslatef(0.0,0.0,-3.6);
}



    /* Setup the Graphics */
    /* Pass '0' to auto-detect bpp */
void *openGL_setupGraphics(int *xsize,int *ysize,int *bpp,
			int fullscreen,int verbose) {

    SDL_Surface *sdl_screen=NULL;
    SDL_Joystick *joy;   
    int mode,x,y;

       /* Initialize SDL */
    if ( SDL_Init(SDL_INIT_VIDEO|SDL_INIT_JOYSTICK)<0) {
       printf("Error trying to init SDL: %s\n",SDL_GetError());
       return NULL;
    }
   
    if (fullscreen) mode=SDL_OPENGL | SDL_FULLSCREEN;
    else mode=SDL_OPENGL;
   
       /* Create the OpenGL screen */
    if ( (sdl_screen=SDL_SetVideoMode(*xsize, *ysize, 0, mode)) == NULL ) {
       printf("Unable to create OpenGL screen: %s\n", SDL_GetError());
       SDL_Quit();
       return NULL;
    }
   
    atexit(SDL_Quit);
   
    SDL_WM_SetCaption("Tom Bombem...",NULL);

       /* Try to init Joysticks */
    if (SDL_NumJoysticks()>0){
       /* Open joystick */
       joy=SDL_JoystickOpen(0);
       if (joy) {
	  printf("Opened Joystick 0\n");
	  printf("Name: %s\n", SDL_JoystickName(0));
	  SDL_JoystickEventState(SDL_ENABLE);
       }
    }
   
    printf("  GL_VERSION:  %s\n", glGetString(GL_VERSION));
    printf("  GL_VENDOR:   %s\n", glGetString(GL_VENDOR));
    printf("  GL_RENDERER: %s\n", glGetString(GL_RENDERER));
        
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    glClearColor(0.0,0.0,0.0,0.0);
    glClearDepth(1.0);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glLoadIdentity();
   
    reshape(*xsize,*ysize);
      
    printf("Init!\n"); fflush(stdout);

// Initializing Textures!!
//


     for(x=0;x<5;x++) {
	for(y=0;y<4;y++) {
	   texture_data[x][y]=calloc(64*64,sizeof(char)*4);
	   glBindTexture(GL_TEXTURE_2D,texture_grid[x][y]);
	   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64,
			64 ,0,GL_RGBA,GL_UNSIGNED_BYTE,texture_data[x][y]);
	   
	}
     }



#if 0   
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
#endif
     return sdl_screen;

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


   int x=0,y=0,i,j;//temp_col,ending_j,ending_i;
   unsigned char *s_pointer;

    GLubyte temp_data[64*64*4];

//    GLubyte *t_pointer;

    GLfloat light_position[]={0.0,0.0,25.0,0.0};
//    GLfloat light_ambient[]={0.65,0.65,0.65,1.0};
    GLfloat lmodel_ambient[]={0.4,0.4,0.4,1.0};
    GLfloat white_light[]={1.0,1.0,1.0,1.0};

    glLoadIdentity();

    gluLookAt(0.0,0.0,20.0,
	      0.0,0.0,0.0,
	      0.0,1.0,1.0);

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
    glLightfv(GL_LIGHT0, GL_SPECULAR,white_light);
          
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
  
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   

    glColor3f(1,0,0);     
     {  
	        GLfloat default_a[]={0.6,0.6,0.6,1.0
		};
	        GLfloat default_d[]={   0.8,0.8,0.8,1.0
		};
	        GLfloat default_s[]={0.0,0.0,0.0,1.0
		};
	        GLfloat default_e[]={0.0,0.0,0.0,1.0
		};
	       
	        glMaterialfv(GL_FRONT,GL_AMBIENT,default_a);
	        glMaterialfv(GL_FRONT,GL_DIFFUSE,default_d);
	        glMaterialfv(GL_FRONT,GL_SPECULAR,default_s);
	        glMaterialfv(GL_FRONT,GL_EMISSION,default_e);
     }
#if 0
  for (x=0;x<source->xsize;x++)
            for (y=0;y<source->ysize;y++) {
	   *((Uint16 *)(t_pointer))=target_p->palette[*(s_pointer)];
	                  s_pointer++; t_pointer+=2;
	    }
#endif     

   
    glRotatef(rotation,1,1,1);
    rotation+=1.0;
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
   
    for(i=0;i<5;i++) {
       for(j=0;j<3;j++) {  /* make it 4, but fix so no out of bounds */
	  s_pointer=(source->memory)+(((j*64*320)+(i*64)));

          for(y=0;y<64;y++) {   
             for(x=0;x<64;x++) {    
	        temp_data[ ((y*64)+x)*4] =palette[*(s_pointer)][0];
	        temp_data[ ((y*64)+x)*4+1] =palette[*(s_pointer)][1];
	        temp_data[ ((y*64)+x)*4+2] =palette[*(s_pointer)][2];
	        temp_data[ ((y*64)+x)*4+3] =palette[*(s_pointer)][3];
	        s_pointer++;
	     }
             s_pointer+=(320-64);
	  }
   
	  glBindTexture(GL_TEXTURE_2D,texture_grid[i][j]);
   
          glTexSubImage2D(GL_TEXTURE_2D,0,0,0,64,64,GL_RGBA,GL_UNSIGNED_BYTE,
		          &temp_data);
   
	  glPushMatrix();
	  glTranslatef( i*4,-(j*4),0);
	  
          glBegin(GL_QUADS);
             glNormal3f(0.0,0.0,1.0);
             glTexCoord2f(0,0);
             glVertex3f(-2,2,0);
             glTexCoord2f(1,0);
             glVertex3f(2,2,0);
             glTexCoord2f(1,1);
             glVertex3f(2,-2,0);
             glTexCoord2f(0,1);
             glVertex3f(-2,-2,0);
          glEnd();
	  glPopMatrix();
       }
    }
    glDisable(GL_TEXTURE_2D);
#if 0   

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
#endif
    glFlush();
    SDL_GL_SwapBuffers();
    glDisable(GL_LIGHTING);
}
      

void openGL_closeGraphics() {
}
