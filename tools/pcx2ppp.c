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
    char *filename,*outfile;
   
    vmwSVMWGraphState *graph_state;
   
    if (argc<3) {
       printf("\nUsage: %s pcx_file ppro_file\n\n",argv[0]);
       return -1;
    }
   
    filename=strdup(argv[1]);
    outfile=strdup(argv[2]);    
   
       /* Setup Graphics */

    if ( (graph_state=vmwSetupSVMWGraph(VMW_NULLTARGET,
					   320,
					   200,
					   0,scale,fullscreen,1))==NULL) {   
          fprintf(stderr,"ERROR: Couldn't get display set up properly.\n");
          return VMW_ERROR_DISPLAY;
    }

       /* Allocate Virtual screen */
    if ((virtual_1=vmwSetupVisual(320,
				  200))==NULL) {
       fprintf(stderr,"ERROR: Couldn't get RAM for virtual screen 1!\n");
       return VMW_ERROR_MEM;
    }
          
       /* Load palette */
    grapherror=vmwLoadPCX(0,0,virtual_1,1,1,
				filename,
				graph_state);

  
    vmwSavePicPacked(0,0,320,200,virtual_1,
		     graph_state->palette_size,
		     graph_state->actual_pal,outfile);
   
    return 0;
}
