#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "svmwgraph/svmwgraph.h"

void pauseawhile(int howlong) {
   
       struct timeval bob;
       struct timezone mree;
       long begin_s,begin_u;
   
       vmwClearKeyboardBuffer();
       gettimeofday(&bob,&mree);
       begin_s=bob.tv_sec; begin_u=bob.tv_usec;
       while ((bob.tv_sec-begin_s)<howlong) {
	         if (vmwGetInput()) return;
	         usleep(30);
	         gettimeofday(&bob,&mree);
       }
}


    /* The collision detection routine.  Optimize? */
    /* Detects if x1,y1 is within x2,y2            */
int collision(int x1,int y1,int xsize,int ysize,
	      int x2,int y2,int x2size,int y2size)
{
       if (abs((y1+ysize)-(y2+y2size))<(ysize+y2size)){
	         if (abs((x1+xsize)-(x2+x2size))<(xsize+x2size)) return 1;
       }
       return 0;
}

char *tb1_data_file(char *name,char *path)
{
   
    char tempst[BUFSIZ];
   
    sprintf(tempst,"%s/%s",path,name);
    return strdup(tempst);
}
