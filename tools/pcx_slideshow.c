/* Views paintpro files   */
/* Also will re-save them */

#include <stdio.h>
#include <string.h> /* for strdup */
#include <unistd.h> /* for usleep() */
#include <stdlib.h> /* for exit() */

#include "svmwgraph.h"


int main(int argc,char **argv)
{
    int grapherror;
    int scale=1,fullscreen=0;
    vmwVisual *virtual_1; 
    vmwPaintProHeader *ppro_header;
    char *filename;
    char ch=0;
    char save_string[BUFSIZ];
    char *extension,*temp_string1,*temp_string2;
    int xsize,ysize,type;
    int is_pcx=0,target=VMW_SDLTARGET;
    int whichfile;
    
    vmwSVMWGraphState *graph_state;

   
    if (argc<2) {
       printf("\nUsage: %s filename ...\n\n",argv[0]);
       return -1;
    }
   
    whichfile=1;
      
    while(whichfile<argc) {
	
   
   
   
       /* Hacky way to grab the extension.  I am sure this is a cleaner way */
    temp_string1=strdup(argv[whichfile]);
    temp_string2=strtok(temp_string1,".");
    extension=temp_string2;
    while( ( temp_string2=strtok(NULL,"."))!=NULL) 
         extension=temp_string2;
      
//    if (!strncmp(extension,"pcx",4)) {
       vmwGetPCXInfo(argv[whichfile],&xsize,&ysize,&type);
       if (type!=PCX_8BITPAL) {
	  printf("Unsupported PCX type!  Must be 256 colors!\n");
	  exit(1);
       }
	   
       printf("\nAttempting to load %s as a %ix%ix8bpp PCX file\n",
	      argv[whichfile],xsize,ysize);

       is_pcx=1;
  //  }
#if 0
    else { /* We assume paintpro file */
       ppro_header=vmwGetPaintProHeader(argv[whichfile]);
       printf("\nLoading file: %s\n",argv[whichfile]);
       if (strncmp(ppro_header->ppro_string,"PAINTPRO",8)) {
          printf("ERROR!  Not in paintpro format!\n");
          return 0;
       }
       if (strncmp(ppro_header->version,"V6.",3)) {
          printf("ERROR!  Not a version 6.x file!\n");
          return 0;
       }
       printf("  + Verified PaintPro v%c.%c file.\n",ppro_header->version[1],
	                                             ppro_header->version[3]);
       printf("  + Picture is %ix%i with %i colors.\n",
	      ppro_header->xsize,ppro_header->ysize,ppro_header->num_colors);
   

       if (ppro_header->version[3]=='0') {
          /* broken ppro 6.0 files sometimes were saved as 319x199 */
          ppro_header->xsize=320;
          ppro_header->ysize=205;
       }
       xsize=ppro_header->xsize;
       ysize=ppro_header->ysize;
    }
#endif	
       /* Setup Graphics */

    if ( (graph_state=vmwSetupSVMWGraph(target,
					xsize,
					ysize,
					0,scale,fullscreen,1))==NULL) {   
          fprintf(stderr,"ERROR: Couldn't get display set up properly.\n");
          return VMW_ERROR_DISPLAY;
    }

       /* Allocate Virtual screen */
    if ((virtual_1=vmwSetupVisual(xsize,ysize))==NULL) {
       fprintf(stderr,"ERROR: Couldn't get RAM for virtual screen 1!\n");
       return VMW_ERROR_MEM;
    }
          
    if (is_pcx) {
       grapherror=vmwLoadPCX(0,0,virtual_1,1,1,argv[whichfile],graph_state);
    }
    else { /* Paintpro */
       grapherror=vmwLoadPicPacked(0,0,virtual_1,1,1,
			           argv[whichfile],
				   graph_state);
    }
	
    vmwBlitMemToDisplay(graph_state,virtual_1);


    while ( (ch=vmwGetInput())==0) usleep(100);

    if ((ch=='Q') || (ch=='q') || (ch==VMW_ESCAPE)) break;

       whichfile++;
       if (whichfile>=argc) whichfile=1;
   }
   

    vmwCloseGraphics();
    return 0;
}
