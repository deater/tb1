/* Views paintpro files   */
/* Also will re-save them */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "svmwgraph/svmwgraph.h"
#include <string.h> /* for strdup */
#include <unistd.h> /* for usleep() */

int main(int argc,char **argv)
{
    int grapherror,i;
    int scale=1,fullscreen=0;
    vmwVisual *virtual_1; 
    vmwSVMWGraphState *graph_state;
    vmwSprite *temp_sprite;

    struct timeval our_tv;
    struct timezone our_tz;
   
    int start_sec,start_usec,total_sec,total_usec;
   
    int size;
   
    int size_to_try[]={5,10,20,50,75,-1
    };
      
    srand(time(NULL));
   
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
   
    size=0;
    while (size_to_try[size]!=-1) {
       
   
       grapherror=vmwLoadPicPacked(0,0,virtual_1,1,1,
				"../data/tbomb1.tb1",
				graph_state);

   
      temp_sprite=vmwGetSprite(0,100,size_to_try[size],
			       size_to_try[size],virtual_1);
       
       gettimeofday(&our_tv,NULL);
       start_sec=our_tv.tv_sec;
       start_usec=our_tv.tv_usec;
       
    for(i=0;i<100000;i++) {
       vmwPutSprite(temp_sprite,rand()%220,rand()%100,virtual_1);
    }
    gettimeofday(&our_tv,NULL);
    total_usec=our_tv.tv_usec-start_usec;
    total_sec=our_tv.tv_sec-start_sec;
    if (total_usec<0) {
       total_usec=-total_usec;
       total_sec--;
    }
   
    
    printf("Blitting %i %ix%i blocks took %is %ius\n",i,size_to_try[size],
	   size_to_try[size],
	   total_sec,total_usec);
    fflush(stdout);
    size++;
    }
    return 0;
}
