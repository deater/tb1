#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> /* exit() */

#include "svmwgraph/svmwgraph.h"
#include "tb1_state.h"
#include "graphic_tools.h"
#include "sound.h"
#include "tblib.h"

int quit(tb1_state *game_state) {

   int result=0;
   
   result=are_you_sure(game_state,"QUIT??? ARE YOU",
                                  "ABSOLUTELY SURE?",
                                  "YES-RIGHT NOW!",
                                  "NO--NOT YET.");

   if (result) { 
      shutdownSound();
      vmwCloseGraphics();
      exit(1);
   }

   return 6;
}
