#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "svmwgraph/svmwgraph.h"
#include "tb1_state.h"

  /* MAKE SURE YOU DO A COOLBOX TO 319,199, not 320,200!!! */
  /* This will cause HARD TO DEBUG BUGS!!! */
  /* I should add a check here I suppose */

void coolbox(int x1,int y1,int x2,int y2,int fill,vmwVisual *target)
{
    int i;
   
    for(i=0;i<5;i++) {
       vmwDrawHLine(x1+i,y1+i,(x2-x1-i-i),31-i,target);
       vmwDrawHLine(x1+i,y2-i,(x2-x1-i-i),31-i,target);
       vmwDrawVLine(x1+i,y1+i,(y2-y1-i-i)+1,31-i,target);
       vmwDrawVLine(x2-i,y1+i,(y2-y1-i-i)+1,31-i,target);
    }
    if (fill) {
       for(i=y1+5;i<y2-4;i++) vmwDrawHLine(x1+5,i,(x2-x1-9),7,target);
     }  
}

void shadowrite(char *st,int x5,int y5,int forecol,int backcol,
		vmwFont *tb1_font,vmwVisual *target)
{
    vmwTextXY(st,x5+1,y5+1,backcol,0,0,tb1_font,target);
    vmwTextXY(st,x5,y5,forecol,0,0,tb1_font,target);
}

char *check_for_tb1_directory(tb1_state *game_state,int try_to_create)
{
 
    char ch;
    struct stat buf;
    char *dir_name;
    vmwFont *tb1_font;
    vmwVisual *vis;   
   
    dir_name=calloc(300,sizeof(char)); /* Hope their home directory path */
                                       /* is not huge */
   
    tb1_font=game_state->graph_state->default_font;
    vis=game_state->virtual_3;
   
       /* First see if ~/.tb1 exists, and if it doesn't see if */
       /* They want it to be created */
   
    coolbox(0,0,319,199,1,vis);
 //   if (read_only_mode) {
 //      vmwTextXY("SORRY!  CANNOT SAVE GAME",40,40,4],0],0,tb1_font,vis);
 //      vmwTextXY("WHEN IN READ ONLY MODE",50,50,4],0],0,tb1_font,vis);
 //   }
//    else {

  
    sprintf(dir_name,"%s/.tb1",getenv("HOME"));
    stat(dir_name,&buf);
    if (S_ISDIR(buf.st_mode)) return dir_name; /* It Exists */
     
    else { /* It doesn't exist */
       if (!try_to_create) return NULL;
       vmwTextXY("DIRECTORY:",20,20,4,0,0,tb1_font,vis);
       if (strlen(dir_name)>18)
	  vmwTextXY("~/.tb1",120,20,4,0,0,tb1_font,vis);
       else 
	  vmwTextXY(dir_name,120,20,4,0,0,tb1_font,vis);
       
       vmwTextXY("THE ABOVE DIRECTORY DOES NOT",20,30,4,0,0,tb1_font,vis);
       vmwTextXY("EXIST.  DO YOU WANT TO ",20,40,4,0,0,tb1_font,vis);
       vmwTextXY("TO CREATE THIS DIRECTORY?",20,50,4,0,0,tb1_font,vis); 
       vmwTextXY("   PLEASE ANSWER [Y or N]",20,60,4,0,0,tb1_font,vis);
       vmwTextXY("NOTE IF YOU ANSWER \"N\" YOU",20,80,12,0,0,tb1_font,vis);
       vmwTextXY("WILL NOT BE ABLE TO SAVE",20,90,12,0,0,tb1_font,vis);
       vmwBlitMemToDisplay(game_state->graph_state,vis);
       ch='M';
       vmwClearKeyboardBuffer();
       while( (ch!='Y') && (ch!='y') && (ch!='N') && (ch!='n') && (ch!=VMW_ESCAPE)) {
          while((ch=vmwGetInput())==0) usleep(30);
       }
       if ((ch=='Y') || (ch=='y')) {
	  if (!mkdir(dir_name,S_IRUSR | S_IWUSR | S_IXUSR)) return dir_name; /* Now it exists */
	  else {
	     coolbox(0,0,319,199,1,vis);
	     vmwTextXY("WARNING! COULD NOT CREATE",30,30,4,0,0,tb1_font,vis);
	     vmwTextXY("DIRECTORY!  ABANDONING SAVE!",20,40,4,0,0,tb1_font,vis);
	     vmwBlitMemToDisplay(game_state->graph_state,vis);
	     vmwClearKeyboardBuffer();
	     while (!vmwGetInput()) usleep(30);
	     return NULL;
	  }
       }
    }
    return NULL;
}

char *vmwGrInput(tb1_state *game_state,int x,int y,int howlong,int forecol,int backcol,
		 vmwFont *tb1_font,vmwVisual *vis) {

    char *tempst;
    int ch;
    int num_chars=0;
    
    tempst=calloc(howlong+3,sizeof(char));
   
    ch=-1;
   
    while (ch!=VMW_ENTER) {
       while ((ch=vmwGetInput())==0) usleep(30);

       if ((ch==VMW_BACKSPACE) && (num_chars>0)) {
          vmwTextXY("          ",x,y,forecol,backcol,1,tb1_font,vis);
          num_chars--;
	  tempst[num_chars]=0;
       }
       if (num_chars<10) {
          if ((ch>31) && (ch<128)) { 
	     tempst[num_chars]=(char)ch;
	     tempst[num_chars+1]=0;
	     num_chars++;
	  }
       }
	     
       vmwTextXY(tempst,x,y,forecol,backcol,1,tb1_font,vis);
       vmwBlitMemToDisplay(game_state->graph_state,vis);
    }
    tempst[num_chars+1]=0;
	   
    return tempst;
}
