#include <stdio.h>
#include <string.h> /* strncmp */
#include <unistd.h> /* usleep */
#include <stdlib.h> /* free */

#include "../svmwgraph/svmwgraph.h"

#define ROW_WIDTH 12

/* #define DEBUG  1 */

struct sprite_type {
   int initialized;
   int type;
   int shoots;
   int explodes;
   vmwSprite *data;
};

struct level2_data {
    int xsize,ysize,rows,cols;
    int numsprites;
    struct sprite_type **sprites;
    int level_length,level_width;
    unsigned char **level_data;
};


#define UNDO_DEPTH 10
int undo_head=0;
int undo_tail=0;

int modified_since_save=0;

struct undo_type {
   int type;
   int x,y;
   int value;
   int row[ROW_WIDTH];
} undo_data[UNDO_DEPTH];



#define SPRITE_BACKGROUND    0
#define SPRITE_ENEMY_SHOOT   1
#define SPRITE_ENEMY_REFLECT 2
#define SPRITE_ENEMY_WEAPON  3
#define SPRITE_OBSTRUCTION   4
#define SPRITE_EXPLOSION     5
#define SPRITE_WEAPON        6
#define SPRITE_POWERUP       7


struct text_mapping_type {
   char name[30];
   int  size;
   int	type;
} text_mapping[] = { {"BACKGROUND",	10, SPRITE_BACKGROUND},
                     {"ENEMY_SHOOT",	11, SPRITE_ENEMY_SHOOT},
	             {"ENEMY_REFLECT",	13, SPRITE_ENEMY_REFLECT},
                     {"ENEMY_WEAPON",	12, SPRITE_ENEMY_WEAPON},
                     {"OBSTRUCTION",	11, SPRITE_OBSTRUCTION},
                     {"EXPLOSION",	9,  SPRITE_EXPLOSION},
                     {"WEAPON",		6,  SPRITE_WEAPON},
                     {"POWERUP",	7,  SPRITE_POWERUP},
                     {"DONE",		4,  0xff},
                     { }
};

int belongs_on_map(int type) {
   
   switch (type) {
    case SPRITE_BACKGROUND:
    case SPRITE_ENEMY_SHOOT:
    case SPRITE_ENEMY_REFLECT:
    case SPRITE_OBSTRUCTION: return 1;
    default: return 0;
   }
   return 0;
}
      

int map_string_to_type(char *string) {

    int i=0;
   
    while(text_mapping[i].type!=0xff) {
       if (!strncmp(text_mapping[i].name,string,text_mapping[i].size)) {
	  return text_mapping[i].type;
       }
       i++;
    }
    return -1;
}

int print_level(FILE *fff,struct level2_data *data) {
    
    int i,j;
    
    fprintf(fff,"%%DATALENGTH %i\n",data->level_length);
    fprintf(fff,"%%DATAWIDTH  %i\n",data->level_width);
    fprintf(fff,"%%DATABEGIN\n");
    
   
    for(j=0;j<data->level_length;j++) {
       for(i=0;i<data->level_width;i++) {
          fprintf(fff,"%3d ",*(data->level_data[j]+i));
       }
       fprintf(fff,"\n");
    }
    fprintf(fff,"%%DATAEND\n");
    return 0;
}


int save_data(char *filename,struct level2_data *data) {
 
    FILE *fff;
    char tempst[255];
    long filepos,good_pos=-1;
   
    fff=fopen(filename,"r+");

    do {
       filepos=ftell(fff);
       fgets(tempst,254,fff);
       if (!strncmp(tempst,"%DATA",5)) {
	  good_pos=filepos;
	  break;
       }
    } while(!feof(fff));

    if (good_pos<0) return -1;
   
    fseek(fff,good_pos,SEEK_SET);
   
    print_level(fff,data);
   
    return 0;
}





int print_level_reverse(struct level2_data *data) {
    
    int i,j;
    
    for(j=data->level_length-1;j>=0;j--) {
       for(i=0;i<data->level_width;i++) {
          printf("%3d ",*(data->level_data[j]+i));
       }
       printf("\n");
    }
    return 0;
}


struct level2_data *parse_data_file(char *filename) {
   
    
    FILE *fff;
     
    char tempst[255],sprite_file[255],throwaway[255];

    char type[255];
   
    int number,shoots,explodes,count,i,numsprites,tempint;
   
    struct level2_data *data;
   
   
    fff=fopen(filename,"r");
    if (fff==NULL) {
       printf("Cannot open %s\n",filename);
       return NULL;
    }
   
    data=(struct level2_data *)malloc(sizeof(struct level2_data));
   
   
   
       /* Pass 1 */
    do {
       fgets(tempst,254,fff);
       
       switch (tempst[0]) {
	  
	case '%': 
	  if (!strncmp(tempst,"%SPRITEFILE",11)) {
	     sscanf(tempst,"%s %s",throwaway,sprite_file);  
	  }
	  if (!strncmp(tempst,"%SPRITEXSIZE",11)) {
	     sscanf(tempst,"%s %d",throwaway,&data->xsize);
	  }
	  if (!strncmp(tempst,"%SPRITEYSIZE",11)) {
	     sscanf(tempst,"%s %d",throwaway,&data->ysize);
	  }
	  if (!strncmp(tempst,"%SPRITEROWS",11)) {
	     sscanf(tempst,"%s %d",throwaway,&data->rows);
	  }
	  if (!strncmp(tempst,"%SPRITECOLS",11)) {
	     sscanf(tempst,"%s %d",throwaway,&data->cols);
	  }
       }
	       
    } while (!feof(fff));

       /* Pass 2 */

    numsprites=(data->rows) * (data->cols);
    data->numsprites=numsprites;
   
    data->sprites=calloc( numsprites, sizeof(struct sprite_type*));
    for (i=0; i< numsprites;i++)  {
        data->sprites[i]=calloc(1,sizeof(struct sprite_type)); 
        data->sprites[i]->initialized=0;
    }
   
    rewind(fff);
    do {
       fgets(tempst,254,fff);
       switch(tempst[0]) {
	  
	case '%': if (!strncmp(tempst,"%SPRITE ",8)) {
	             count=sscanf(tempst,"%s %d %s %d %d",
				  throwaway,&number,type,&shoots,&explodes);
	             
	             if (count > 2) {
			data->sprites[number]->type=map_string_to_type(type);
			data->sprites[number]->initialized=1;
	             }
	             if (count > 3) {
			data->sprites[number]->shoots=shoots;
		     }
	             if (count > 4) {
			data->sprites[number]->explodes=explodes;
		     }
	          }
	          if (!strncmp(tempst,"%DATALENGTH",10)) {
		     sscanf(tempst,"%s %d",throwaway,&data->level_length);
		  }   
	          if (!strncmp(tempst,"%DATAWIDTH",9)) {
		     sscanf(tempst,"%s %d",throwaway,&data->level_width);
		  }
		  
	          break;
	 
	  
       }
    } while (!feof(fff));
   
   
    /* Pass 3 */
    data->level_data=calloc(data->level_length, sizeof(char *));
   
    data->level_data[0]=calloc(data->level_length * data->level_width,sizeof(char));
    for(i=1;i<data->level_length;i++) {
       data->level_data[i]=data->level_data[0]+ (i*data->level_width*sizeof(char));
    }
      
    rewind(fff);
   
    
    do {
	fgets(tempst,254,fff);       
    } while (strncmp(tempst,"%DATABEGIN",10));
   
    i=0;
    while(i<data->level_length*data->level_width) {
       /* Grrrrr */
//       fscanf(fff,"%d",(int *)data->level_data[0]);
       fscanf(fff,"%d",&tempint);
       *(data->level_data[0]+i)=(char)tempint;
       i++;
    }
#ifdef DEBUG     
    print_level(data);
   

    printf("Sprite File: %s\n",sprite_file);
    printf("Sprite size:  %ix%i\n",data->xsize,data->ysize);
    printf("Sprite array: %ix%i\n",data->rows,data->cols);
    printf("Level length: %i\n",data->level_length);
#endif   
    fclose(fff);
    return data;
}
   
int free_level2_data(struct level2_data *data) {
   /* IMPLEMENT THIS */
   return 0;
}

    
int draw_all_sprites(int y_offset,struct level2_data *data,
		     vmwVisual *virtual_1){

    int i,j;
   
    for(i=0;i<12;i++) { 
       for(j=0;j<20;j++) {
          vmwPutSpriteNonTransparent(data->sprites[(int) *(data->level_data[j+y_offset]+i)
				     ]->data,i*20,j*10,virtual_1);
 
       }
    }
    return 0;
}




int delete_line(struct level2_data *data, int y) {
   
    unsigned char **temp_data;
    int i;
   
        /* Overly Complicated way of deleting line */
   
        /* Create new data, 1 line shorter */
    temp_data=calloc(data->level_length-1, sizeof(char *));
    temp_data[0]=calloc( (data->level_length-1) * data->level_width,sizeof(char));
    
        /* Copy old data over, removing the one line */
    memcpy(temp_data[0],data->level_data[0],(y*ROW_WIDTH));
    memcpy(temp_data[0]+(y*ROW_WIDTH),data->level_data[0]+((y+1)*ROW_WIDTH),((data->level_length-y)-1)*ROW_WIDTH);
      
        /* Free the old data */
    free(data->level_data[0]);
    free(data->level_data);
	    
        /* We are now one line shorter */  
    data->level_length--;     

        /* New data magically becomes real data */
    data->level_data=temp_data;

        /* Setup pointers so we can pretend we have an array */
    for(i=1;i<data->level_length;i++) {
       data->level_data[i]=data->level_data[0]+ (i*data->level_width*sizeof(char));
    }

    return 0;
}

int insert_line(struct level2_data *data, int y) {

   
    unsigned char **temp_data;
    int i;
   
        /* Overly Complicated way of inserting line */
   
        /* Create new data, 1 line longer */
    temp_data=calloc(data->level_length+1, sizeof(char *));
    temp_data[0]=calloc( (data->level_length+1) * data->level_width,sizeof(char));
    
        /* Copy old data over, adding one line */
    memcpy(temp_data[0],data->level_data[0],(y*ROW_WIDTH));
    memcpy(temp_data[0]+((y+1)*ROW_WIDTH),data->level_data[0]+(y*ROW_WIDTH),((data->level_length-y))*ROW_WIDTH);
      
        /* Free the old data */
    free(data->level_data[0]);
    free(data->level_data);
	    
        /* We are now one line shorter */  
    data->level_length++;     

        /* New data magically becomes real data */
    data->level_data=temp_data;

        /* Setup pointers so we can pretend we have an array */
    for(i=1;i<data->level_length;i++) {
       data->level_data[i]=data->level_data[0]+ (i*data->level_width*sizeof(char));
    }

    return 0;   
   
}


#define CHANGE_SPRITE 0
#define INSERT_LINE   1
#define DELETE_LINE   2


int undo(struct level2_data *data) {

    int i;
   
      /* Cicrular buffer.  Avoid overflows.  Any data older than */
      /* UNDO_DEPTH is lost */
   
   if (undo_data[undo_head].type==CHANGE_SPRITE) {
      *(data->level_data[undo_data[undo_head].y]+undo_data[undo_head].x)=
	undo_data[undo_head].value;
   }

   if (undo_data[undo_head].type==INSERT_LINE) {
      delete_line(data,undo_data[undo_head].y);
   }
       
   if (undo_data[undo_head].type==DELETE_LINE) {
          /* Re-insert line */
      insert_line(data,undo_data[undo_head].y);
          /* Restore data */
      for(i=0;i<ROW_WIDTH;i++) {
         *(data->level_data[undo_data[undo_head].y]+i)=undo_data[undo_head].row[i];	 
      }
   }
   
   
   if (undo_head!=undo_tail) {
      undo_head--;
      if (undo_head<0) undo_head=UNDO_DEPTH;
   }
   
   return 0;
   
}



int modify_level(struct level2_data *data,int x,int y,int type,int value) {

   int i;

     
   modified_since_save++;
      /* Protect if you've made more than 2billion changes */
      /* Unlikely, but the fix is shorter than the comment */
      /* I was originally going to make about "broken if you*/
      /* change more than 2 billion things" */
   if (modified_since_save<0) modified_since_save=1;
   
      /* Cicrular buffer.  Avoid overflows.  Any data older than */
      /* UNDO_DEPTH is lost */
   undo_head++;
   if (undo_head>=UNDO_DEPTH) undo_head=0;
   if (undo_head==undo_tail) { 
      undo_tail=undo_head+1;
      if (undo_tail>=UNDO_DEPTH) undo_tail=0;
   }
   
   if (type==CHANGE_SPRITE) {
      undo_data[undo_head].type=CHANGE_SPRITE;
      undo_data[undo_head].x=x;
      undo_data[undo_head].y=y;
      undo_data[undo_head].value= *(data->level_data[y]+x);
      
      *(data->level_data[y]+x)=value;    
   }
   
   if (type==DELETE_LINE) {
      undo_data[undo_head].type=DELETE_LINE;
      undo_data[undo_head].y=y;
      for(i=0;i<ROW_WIDTH;i++) 
	 undo_data[undo_head].row[i]=*(data->level_data[y]+i);

      delete_line(data,y);

      
   }
   
   if (type==INSERT_LINE) {
      undo_data[undo_head].type=INSERT_LINE;
      undo_data[undo_head].y=y;
      
      insert_line(data,y);
      
   }
   
   
   return 0;
   
}


int main(int argc, char **argv) {
   
    struct level2_data *data;
    vmwSVMWGraphState *graph_state;
    vmwVisual *virtual_1;
    int ch,i,j,y_offset=0;
    int x=0,y=0;
    char tempst[10];
    int sprite_array[16];
    int sprite_offset=0;
    int doublesize=1,fullscreen=0;
    int argsfound=0;
    
    char filename[255],spritefile[255];
   
       /* Parse command line arguments */
    i=1;
    while(i<argc) {
       if (argv[i][0]=='-') {
	  if (argv[i][1]=='d') doublesize=2;
	  if (argv[i][1]=='f') fullscreen=1;
       }
       else {
	  argsfound++;
	  if (argsfound==1) strncpy(filename,argv[i],254);
	  if (argsfound==2) strncpy(spritefile,argv[i],254);
       }
       i++;
    }
   
    if (argsfound<2) {
       printf("\nUSAGE:  %s [-f] [-d] data_file sprite_file\n\n",argv[0]);
       return -1;
    }
       
    printf("Editing datafile=%s spritefile=%s\n",filename,spritefile);
   
   
    data=parse_data_file(filename);
   
    graph_state=vmwSetupSVMWGraph(VMW_SDLTARGET,doublesize*320,doublesize*200,0,doublesize,fullscreen,1);
    graph_state->default_font=vmwLoadFont("../data/tbfont.tb1",8,16,256);
   
    virtual_1=vmwSetupVisual(320,200);
   
    if (vmwLoadPicPacked(0,0,virtual_1,1,1,spritefile,graph_state)!=0) {
       printf("ERROR LOADING %s\n",spritefile);
       return -1;
    }
   
    for(j=0;j<data->rows;j++)
       for(i=0;i<data->cols;i++)
          data->sprites[j*10+i]->data=vmwGetSprite(1+i*21,1+j*11,20,10,virtual_1);

    vmwDrawBox(0,0,319,199,0,virtual_1);
    
    draw_all_sprites(0,data,virtual_1);
    vmwBlitMemToDisplay(graph_state,virtual_1);

    ch=' ';
    while(1) {

       switch(ch) {  
	case 'A'...'P' : if (sprite_array[ch-0x41]!=-1) 
	                    modify_level(data,x,y+y_offset,CHANGE_SPRITE,
					 sprite_array[ch-0x41]);
	                 break;
	case 'a'...'p' : if (sprite_array[ch-0x61]!=-1) 
		            modify_level(data,x,y+y_offset,CHANGE_SPRITE,
					 sprite_array[ch-0x61]);

	                 break;
	  
	case 's': case 'S': save_data(filename,data);
	                    modified_since_save=0;
	                    break;
	case 'u': case 'U': undo(data);
	                    break;
	  
	  
	case 'q': case 'Q': if (modified_since_save) {
	                       if (vmwAreYouSure(graph_state,
					  graph_state->default_font,
					  virtual_1,
					  "FILE HAS CHANGED",
					  "QUIT W/O SAVING?",
					  "YES-QUIT ANYWAY",
					  "NO!-LET ME SAVE")) return 0;
	                    }
	                    else return 0;
	                    break;  
	case '>': case '.': i=0; j=sprite_offset;
	                    while ( (i<16) && (j<data->numsprites)) {
			          if ((data->sprites[j]->initialized) 
			              && (belongs_on_map(data->sprites[j]->type))) {
				     i++;
				  }
			          j++;
			    }
	                    if (j<data->numsprites) sprite_offset=j; 
	                    break;
	  
	case '<': case ',': i=0; j=sprite_offset;
	                    while ( (i<16) && (j>0)) {
			       if ((data->sprites[j]->initialized) &&
				   (belongs_on_map(data->sprites[j]->type))) {
				  i++;
			       }
			       j--;
			    }
	                    if (j>=0) sprite_offset=j;
	                    break;
	  
	   case VMW_UP: if (y>0) y--;
	                else if (y_offset>0) y_offset--;
	                break;
	   case VMW_DOWN: if (y>=19) { 
	                     if (y_offset<data->level_length-21) y_offset++;
	                }
	                else y++;
	                break;
	case VMW_RIGHT: if (x<11) x++; break;
	case VMW_LEFT:  if (x>0) x--;  break;
	case VMW_PGUP: if (y_offset>20) y_offset-=20;
	               else (y_offset=0);
	               break;
	case VMW_PGDN: if (y_offset<data->level_length-41) y_offset+=20;
	               else y_offset=data->level_length-20;
		       break;
	case VMW_HOME: y_offset=0; break;
	case VMW_END:  y_offset=data->level_length-20; break;
	case VMW_INS:  modify_level(data,0,y+y_offset,INSERT_LINE,0);
	               break;
	case VMW_DEL:  modify_level(data,0,y+y_offset,DELETE_LINE,0); 
		       break;
	  
       }
       
          /* Make sure if we delete a line it doesn't make us */
          /* try to display off the end of the screen */
       if (y_offset>=data->level_length-20) y_offset=data->level_length-20;
       
       draw_all_sprites(y_offset,data,virtual_1);
       
          /* Draw cursor box */
       vmwDrawHLine(x*20,y*10,20,15,virtual_1);
       vmwDrawHLine(x*20,(y*10)+9,20,15,virtual_1);
       vmwDrawVLine(x*20,y*10,10,15,virtual_1);
       vmwDrawVLine((x*20)+19,y*10,10,15,virtual_1);
       
       vmwDrawBox(240,0,79,199,0,virtual_1);
       sprintf(tempst,"X: %4d",x);
       vmwTextXY(tempst,245,5,9,0,0,graph_state->default_font,virtual_1);
       sprintf(tempst,"Y: %4d",y+y_offset);
       vmwTextXY(tempst,245,15,9,0,0,graph_state->default_font,virtual_1);
       sprintf(tempst,"L: %4d",data->level_length);
       vmwTextXY(tempst,245,25,9,0,0,graph_state->default_font,virtual_1);

       vmwDrawHLine(243,38,74,12,virtual_1);
       vmwDrawHLine(243,150,74,12,virtual_1);
       
       sprintf(tempst,"%c",'<');
       vmwTextXY(tempst,275,40,11,0,0,graph_state->default_font,virtual_1);
       sprintf(tempst,"%c",'>');
       vmwTextXY(tempst,275,140,11,0,0,graph_state->default_font,virtual_1);
       
       if (sprite_offset>=data->numsprites) sprite_offset=0;
       if (sprite_offset<0) sprite_offset=0;
       
       j=sprite_offset;       

       for(i=0;i<16;i++) {
	  sprintf(tempst,"%c",0x41+i);
	  
          while ((j<data->numsprites) && 
		 
		 ((!data->sprites[j]->initialized) || (!belongs_on_map(data->sprites[j]->type)))) {
	      j++;
	  }

	  if (j<data->numsprites) { 
	     sprite_array[i]=j;
	     vmwTextXY(tempst,((i/8)*61)+245,((i%8)*11)+50,11,0,0,graph_state->default_font,virtual_1);
	     vmwPutSpriteNonTransparent(data->sprites[j]->data,
		       ((i/8)*21)+260, ((i%8)*11)+50,virtual_1);
	     j++;
	  }
	  else sprite_array[i]=-1;
	  
       }
       
       
       vmwTextXY("INS DEL",245,155,11,0,0,graph_state->default_font,virtual_1);
       vmwTextXY("   ,   ",245,155,9,0,0,graph_state->default_font,virtual_1);
       vmwTextXY(" ndo",245,165,9,0,0,graph_state->default_font,virtual_1);
       vmwTextXY("U",245,165,11,0,0,graph_state->default_font,virtual_1);      
       vmwTextXY(" ave",245,175,9,0,0,graph_state->default_font,virtual_1);
       vmwTextXY("S",245,175,11,0,0,graph_state->default_font,virtual_1);
       if (modified_since_save) 
	  vmwTextXY("     *",245,175,12,0,0,graph_state->default_font,virtual_1);
       vmwTextXY(" uit",245,185,9,0,0,graph_state->default_font,virtual_1);
       vmwTextXY("Q",245,185,11,0,0,graph_state->default_font,virtual_1);
       
       vmwBlitMemToDisplay(graph_state,virtual_1);

              
       while( ((ch=vmwGetInput())==0)) {
	  usleep(30);
       }
    }
   
    return 0;
}
