/* Views paintpro files   */
/* Also will re-save them */

#include <stdio.h>
#include "svmwgraph/svmwgraph.h"
#include <string.h> /* for strdup */
#include <unistd.h> /* for usleep() */

int main(int argc,char **argv)
{
    int grapherror;
    int scale=1,fullscreen=0;
    vmwVisual *virtual_1; 
    vmwPaintProHeader *ppro_header;
    char *filename;
    char ch=0;
    char save_string[BUFSIZ];
   
    vmwSVMWGraphState *graph_state;
   
    if (argc<2) {
       printf("\nUsage: %s filename\n\n",argv[0]);
       return -1;
    }
   
    filename=strdup(argv[1]);
    
    ppro_header=vmwGetPaintProHeader(filename);
   
    printf("\nLoading file: %s\n",filename);
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
   
       /* Setup Graphics */

    if ( (graph_state=vmwSetupSVMWGraph(VMW_SDLTARGET,
					   ppro_header->xsize,
					   ppro_header->ysize,
					   0,scale,fullscreen,1))==NULL) {   
          fprintf(stderr,"ERROR: Couldn't get display set up properly.\n");
          return VMW_ERROR_DISPLAY;
    }

       /* Allocate Virtual screen */
    if ((virtual_1=vmwSetupVisual(ppro_header->xsize,
				  ppro_header->ysize))==NULL) {
       fprintf(stderr,"ERROR: Couldn't get RAM for virtual screen 1!\n");
       return VMW_ERROR_MEM;
    }
          
       /* Load palette */
    grapherror=vmwLoadPicPacked(0,0,virtual_1,1,1,
				filename,
				graph_state);

    vmwBlitMemToDisplay(graph_state,virtual_1);

    while ((ch!='Q') && (ch!='q') && (ch!=VMW_ESCAPE)) {
       while ( (ch=vmwGetInput())==0) usleep(100);
       
       if (ch=='s') {
	  printf("\nEnter file name to save as:\n");
	  scanf("%s",save_string);
	  vmwSavePicPacked(0,0,320,200,virtual_1,
			   graph_state->palette_size,
			   graph_state->actual_pal,save_string);
	  
       }
    }
   
    return 0;
}
