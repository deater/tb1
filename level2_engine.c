#include <stdio.h>
#include <string.h> /* strncmp */
#include <unistd.h> /* usleep */
#include <stdlib.h> /* free */
#include <sys/time.h> /* gettimeofday */

#include "./svmwgraph/svmwgraph.h"
#include "tb1_state.h"
#include "tblib.h"
#include "graphic_tools.h"
#include "sidebar.h"
#include "sound.h"
#include "help.h"
#include "loadsave.h"

#define ROW_WIDTH 12
#define COL_HEIGHT 20

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
//	  printf("%s %i\n",string,text_mapping[i].type);
	  return text_mapping[i].type;
       }
       i++;
    }
    return -1;
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


    /* Define this to get a frames per second readout */
/* #define DEBUG_ON */

    /* The sounds */

#define NUM_SAMPLES 8
#define SND_AHH    0
#define SND_CC     1
#define SND_KAPOW  2
#define SND_SCREAM 3
#define SND_BONK   4
#define SND_CLICK  5
#define SND_OW     6
#define SND_ZRRP   7

struct enemyinfo {
     int x,y;
     int kind;
     int out,exploding,boundarycheck,dead;
     int explodeprogress;
     int minx,maxx,xspeed,yspeed;
     int hitsneeded;
};

struct bullet_info_type {
   int out,type,x,y;
   struct bullet_info_type *next,*prev;
};

struct bullet_root_type {
   int howmany,allowed;
   struct bullet_info_type *head,*tail;
};

struct obstruction_type {
    int x,y;
    int type;
    int exploding;
    int explodeprogress;
    int howmanyhits;
    int lastshot;
    struct obstruction_type *next;
};


#define BULLETS_MAX 3

    struct bullet_root_type *bullet_root;
    struct bullet_info_type **bullet_info;
    struct bullet_info_type *temp_bullet=NULL;


void remove_bullet() {
   
	  temp_bullet->out=0;
	  bullet_root->howmany--;

	     /* Remove bullet from linked list */
	  if (bullet_root->head==temp_bullet) {
	     bullet_root->head=temp_bullet->next;
	  }
	  
	  if (bullet_root->tail==temp_bullet) {
	     bullet_root->tail=temp_bullet->prev;
	  }
	  
	  if (temp_bullet->prev!=NULL) {
	     if (temp_bullet->next!=NULL) 
		temp_bullet->prev->next=temp_bullet->next;
             else temp_bullet->prev->next=NULL;
	  }
	  
	  
	  if (temp_bullet->next!=NULL) {
	     if (temp_bullet->prev!=NULL) 
	        temp_bullet->next->prev=temp_bullet->prev;
	     else temp_bullet->next->prev=NULL;
	  }
	
}

	

void leveltwoengine(tb1_state *game_state, char *shipfile, char *levelfile,
		    char *spritefile,char *level_num, char *level_desc,
		    void *close_function) 
{
    int ch,i;
    char tempst[BUFSIZ];
    int k,game_paused=0,speed_factor=1;
    int shipx=36,shipy;
    int whatdelay=1;
    int levelover=0,j,offscreen_row=0;
//    struct enemyinfo *enemy;
    struct timeval timing_info;
    struct timezone dontcare;
   
    long oldsec,oldusec,time_spent;
    int howmuchscroll=200; /* there is a reason for this */
    struct obstruction_type *obstruction_head[20];
    struct obstruction_type obstructions[20][12];
    struct obstruction_type *temp_obstruct;
    int shipadd=0,shipframe=1;
    int our_row,rows_goneby=0;
    int grapherror;
    int done_waiting,type;
   
    vmwFont *tb1_font;
    vmwVisual *virtual_1;
    vmwVisual *virtual_2;
   
    vmwSprite *ship_shape[3];
//    int enemies_drawn[200];
   
    struct level2_data *data;
   
       /* For convenience */
    tb1_font=game_state->graph_state->default_font;
    virtual_1=game_state->virtual_1;
    virtual_2=game_state->virtual_2;
   
       /* Set this up for Save Game */
    game_state->begin_score=game_state->score;
    game_state->begin_shields=game_state->shields;
    
          /* Load Sprite Data */
    grapherror=vmwLoadPicPacked(0,0,virtual_1,1,1,
			        tb1_data_file(shipfile,
					      game_state->path_to_data),
				game_state->graph_state);
   
    ship_shape[0]=vmwGetSprite(0,0,48,30,virtual_1);
    ship_shape[1]=vmwGetSprite(0,32,48,30,virtual_1);
    ship_shape[2]=vmwGetSprite(0,64,48,30,virtual_1);

   
       /* Load Level Data */
    data=parse_data_file(tb1_data_file(levelfile,game_state->path_to_data));
   
      vmwLoadPicPacked(0,0,virtual_1,1,1,
		     tb1_data_file(spritefile,game_state->path_to_data),
					       game_state->graph_state);
      
    for(j=0;j<data->rows;j++)
       for(i=0;i<data->cols;i++) 
          data->sprites[j*10+i]->data=vmwGetSprite(1+i*21,1+j*11,20,10,virtual_1);


       /* Initialize Structures for background */
    bullet_info=calloc(5,sizeof(struct bullet_info_type *));
    bullet_info[0]=calloc(5,sizeof(struct bullet_info_type));
    for (i=1;i<5;i++) bullet_info[i]=bullet_info[0]+i*sizeof(struct bullet_info_type);
    bullet_root=malloc(sizeof(struct bullet_root_type));
    bullet_root->howmany=0;
    bullet_root->allowed=5;
    bullet_root->head=NULL;
    bullet_root->tail=NULL;
    for(i=0;i<bullet_root->allowed;i++) {
       bullet_info[i]->out=0;
    }
   
     
      /* Initialize obstruction heap to 0 */
   for(j=0;j<20;j++) {
      for(i=0;i<12;i++) {
         obstructions[j][i].next=NULL;
      }
   }
   for(i=0;i<12;i++) {
      obstruction_head[i]=NULL;
   }
   
   
	
  
   
   
   
   
   

      /* Announce the Start of the Level */
    vmwDrawBox(0,0,320,200,0,virtual_1);
    coolbox(70,85,240,120,1,virtual_1);
    vmwTextXY(level_num,84,95,4,7,0,tb1_font,virtual_1);
    vmwTextXY(level_desc,84,105,4,7,0,tb1_font,virtual_1);
    
    vmwBlitMemToDisplay(game_state->graph_state,virtual_1);
    vmwClearKeyboardBuffer();
    pauseawhile(5);

       /* Setup and draw the sidebar */
    setupsidebar(game_state,virtual_2);
    vmwFlipVirtual(virtual_1,virtual_2,320,200);
    sprintf(tempst,"%d",game_state->level);
    vmwDrawBox(251,52,63,7,0,virtual_2);
    vmwTextXY(tempst,307,51,12,0,0,tb1_font,virtual_2);
     
    change_shields(game_state);

   
       /* Ready the timing loop */
    gettimeofday(&timing_info,&dontcare);
    oldsec=timing_info.tv_sec; oldusec=timing_info.tv_usec;

   
       /* Get the initial background ready */
    offscreen_row=data->level_length-(COL_HEIGHT*2);
    for(i=0;i<ROW_WIDTH;i++) {
       for(j=0;j<40;j++) {
	  vmwPutSpriteNonTransparent(data->sprites[(int) *(data->level_data[offscreen_row+j]+i)]->data,
		       i*20,j*10,virtual_2);
       }
    }
    offscreen_row-=20;

       /* Get the initial obstructions ready */
    our_row=data->level_length-20;
    for(j=0;j<20;j++) {
       for(i=0;i<12;i++) {

	  type=data->sprites[*(data->level_data[our_row]+i)]->type;
	  
          if ((type==SPRITE_OBSTRUCTION) || 
	      (type==SPRITE_ENEMY_SHOOT) || 
	      (type==SPRITE_ENEMY_REFLECT)) {
	     k=0;
	     obstruction_head[j]=obstructions[j][0];
	     temp_obstruct=obstructions[j][0];
	     while(temp_obstruct->next!=NULL) temp_obstruct=temp_obstruct->next;
	     temp_obstruct->next=temp_obstruct+1;
	     temp_obstruct=temp_obstruct->next;
	     temp_obstruct->x=i*20;
	     temp_obstruct->y=j*20;
	     temp_obstruct->type=type;
	     temp_obstruct->exploding=0;
	     temp_obstruct->lastshot=0;
	     temp_obstruct->next=NULL;
	  }
       }
    }
   
    vmwArbitraryCrossBlit(virtual_2,0,0+howmuchscroll,240,200,
                          virtual_1,0,0);   
   
       /**** GAME LOOP ****/
    while (!levelover) { 
       ch=0;
       
          /* Scroll the Background */
       if (speed_factor>1) howmuchscroll-=speed_factor;
       else howmuchscroll--;
       
          /* If used up all the buffered background, draw some more */
       if (howmuchscroll<0) {
	  howmuchscroll=200+howmuchscroll;
	     /* Copy half of old downward */
	  vmwArbitraryCrossBlit(virtual_2,0,0,240,200,virtual_2,0,200);
	     /* Load 20 rows of data preceding it */
	  for(i=0;i<12;i++) {
	     for(j=0;j<20;j++) {
		vmwPutSprite(data->sprites[(int) *(data->level_data[offscreen_row+j]+i)]->data,
			     i*20,j*10,virtual_2);
	     }
	  }
	  offscreen_row-=20;
       }
       

          /* Setup Obstructions */
       if (rows_goneby%10==0) {

	  /* move all rows down by one, dropping old off end */
	  memmove(obstructions[1],obstructions[0],
		  19*12*sizeof(struct obstruction_type));
	  memmove(obstruction_head[1],obstruction_head[0],
		  19*sizeof(struct *obstruction_type));
	  obstructions[0]->next=NULL;
	  our_row--;
//	  printf("ROW %i\n",our_row);       
	  temp_obstruct=obstructions[0];
	  for(i=0;i<12;i++) {
             type=data->sprites[*(data->level_data[our_row]+i)]->type;
//	     printf("%3i ",type);
             if ((type==SPRITE_OBSTRUCTION) || 
	         (type==SPRITE_ENEMY_SHOOT) || 
	         (type==SPRITE_ENEMY_REFLECT)) {
//	         printf("*");
//		 printf("Enemy %i on row %i\n",type,our_row);
//		 fflush(stdout);
	         while(temp_obstruct->next!=NULL) temp_obstruct=temp_obstruct->next;
	         temp_obstruct->next=temp_obstruct+1;
		 temp_obstruct=temp_obstruct+1;
		 temp_obstruct->x=i*20;
	         temp_obstruct->y=j*20;
	         temp_obstruct->type=type;
	         temp_obstruct->exploding=0;
	         temp_obstruct->lastshot=0;
	         temp_obstruct->next=NULL;
	     }
	  }
//	  printf("\n"); fflush(stdout);
       }


#if 0
       if (!enemies_drawn[our_row]) {
	  enemies_drawn[our_row]=1;
	  for(i=0;i<12;i++) {
//	     our_shape=background[our_row][i];
	     if ((our_shape>9)&&(our_shape<20)) {
		   k=0;
		   while ((!passive[k].dead) && (k<40)) k++;
		   passive[k].dead=0;
		   passive[k].kind=our_shape;
		   if ((our_shape>10) && (our_shape<15)) passive[k].shooting=1;
		   else passive[k].shooting=0;
		   passive[k].exploding=0;
		   passive[k].x=i*20;
		   passive[k].y=(rows_goneby%10)-9;
		   passive[k].howmanyhits=1;
		   passive[k].lastshot=0;
	     }
	  }
       }
#endif
       
          /* Flip the far background to regular background */
       vmwArbitraryCrossBlit(virtual_2,0,0+howmuchscroll,240,200,
                             virtual_1,0,0);

          /***Collision Check***/
       for(i=0;i<bullet_root->howmany;i++) {
	  temp_obstruct=obstructions[ (bullet_info[i]->y)/10];
//	  printf("%p %p\n",temp_obstruct,temp_obstruct->next);
//	  printf("Row %i\n",(bullet_info[i]->y)/10); fflush(stdout);
	  while (temp_obstruct->next!=NULL) {
	     temp_obstruct=temp_obstruct->next;
	     if ( ( (bullet_info[i]->x-3) >= temp_obstruct->x) &&
	          ( bullet_info[i]->x <= (temp_obstruct->x+20))) {
		
		  if (temp_obstruct->type!=SPRITE_ENEMY_REFLECT) {
		       if ((game_state->sound_possible)&&(game_state->sound_enabled)) 
			  playGameFX(SND_KAPOW);
                       temp_obstruct->exploding=1;
                       temp_obstruct->explodeprogress=0;
		       temp_bullet=bullet_info[i];
		       remove_bullet();
                       game_state->score+=10;
                       changescore(game_state);
		    }
                    else {
  //                     bullet[j].out=0;
    //                   k=0;
//		       while ((!enemy[k].out) && (k<10)) k++;
//		       if (k<9) {
  //                        enemy[k].out=1;
    //                      enemy[k].y=bullet[j].y;
      //                    enemy[k].x=bullet[j].x;
        //                  enemy[k].yspeed=7;
          //                enemy[k].kind=21;
	//	       }
		    }
		
	//	  printf("BOOM\n"); fflush(stdout);
	     }	  
	  }
	  
       }
	  
	  
	  
       
	  
       
#if 0       
       for(i=0;i<40;i++) 
	  if ((!passive[i].dead) && (!passive[i].exploding)) {
	     for(j=0;j<3;j++) {
                if ((bullet[j].out) && 
		    (collision(bullet[j].x,bullet[j].y,3,4,passive[i].x,
						  passive[i].y,10,5))){
                    if (passive[i].kind!=10) {
		       if ((game_state->sound_possible)&&(game_state->sound_enabled)) 
			  playGameFX(SND_KAPOW);
                       passive[i].exploding=1;
                       passive[i].explodeprogress=0;
                       bullet[j].out=0;
                       game_state->score+=10;
                       changescore(game_state);
		    }
                    else {
                       bullet[j].out=0;
                       k=0;
		       while ((!enemy[k].out) && (k<10)) k++;
		       if (k<9) {
                          enemy[k].out=1;
                          enemy[k].y=bullet[j].y;
                          enemy[k].x=bullet[j].x;
                          enemy[k].yspeed=7;
                          enemy[k].kind=21;
		       }
		    }
		 }
	     }
	  
             /* See if ship is hitting any Obstructions*/
          if ((passive[i].y>155) && (passive[i].kind!=10)) {
             if ((collision(passive[i].x,passive[i].y,10,5,shipx+16,165,5,5))||
		(collision(passive[i].x,passive[i].y,10,5,shipx+6,175,18,8))) {
                if ((game_state->sound_possible)&&(game_state->sound_enabled))
		   playGameFX(SND_BONK);
                passive[i].dead=1;
                game_state->shields--;
                if(game_state->shields<0) levelover=1;
//		vmwPutSprite(shape_table[34],
//			     passive[i].x,passive[i].y+howmuchscroll,
//			     virtual_1);
		change_shields(game_state);
		}
	  }
       }
          /* See if hit by lasers */
       for (i=0;i<10;i++) 
	 if (enemy[i].out) {
            if ((collision(enemy[i].x,enemy[i].y,2,5,shipx+16,165,5,5)) ||
                (collision(enemy[i].x,enemy[i].y,2,5,shipx+6,175,18,8))) {
                if ((game_state->sound_possible)&&(game_state->sound_enabled))
		   playGameFX(SND_BONK);
                enemy[i].out=0;
                game_state->shields--;
                if (game_state->shields<0) levelover=1;
	        change_shields(game_state);
	    }
	 }
       
          /***DO EXPLOSIONS***/
       for(i=0;i<40;i++) 
          if (passive[i].exploding) { 
             passive[i].explodeprogress++;
//	     vmwPutSprite(shape_table[35+passive[i].explodeprogress],
  //                        passive[i].x,passive[i].y+howmuchscroll,
//			  virtual_2);
             if (passive[i].explodeprogress>3) {
                passive[i].dead=1;
                passive[i].exploding=0;
//	        vmwPutSprite(shape_table[34],
//			     passive[i].x,passive[i].y+howmuchscroll,
//			     virtual_2);   
           }
        }
#endif       
       /***MOVE BULLETS***/
    
    temp_bullet=bullet_root->head;
    while(temp_bullet!=NULL) {
       if (speed_factor>1) temp_bullet->y-=(5*speed_factor);
       else temp_bullet->y-=5;
       if (temp_bullet->y<5) {

	  remove_bullet();
	  

       }	  
       else vmwPutSprite(data->sprites[temp_bullet->type]->data,
			 temp_bullet->x,temp_bullet->y,virtual_1);
       temp_bullet=temp_bullet->next;
    }
    
#if 0       
       /***MOVE ENEMIES***/
    for(j=0;j<40;j++) {
       if (!passive[j].dead) {
          if (speed_factor==1) passive[j].y++;
	  else passive[j].y+=speed_factor;
          if (passive[j].y>190) passive[j].dead=1;
       }
       if (passive[j].lastshot>0) passive[j].lastshot--;
       if ((!passive[j].dead) && (passive[j].shooting)
          && (!passive[j].lastshot) && (passive[j].y>0)) {
          k=0;
	  while ((enemy[k].out) && (k<10)) k++;
	  if (k<9) {
             passive[j].lastshot=30;
             enemy[k].out=1;
             enemy[k].y=passive[j].y;
             enemy[k].x=passive[j].x+5;
             enemy[k].yspeed=5;
             enemy[k].kind=25;
             if (passive[j].kind==11) enemy[k].kind=26;
	  }
       }
    }
    
    for(j=0;j<10;j++) {
       if (enemy[j].out) {
//	  vmwPutSprite(shape_table[enemy[j].kind],
//		       enemy[j].x,enemy[j].y,
//		       virtual_1);
          if (speed_factor==1) enemy[j].y+=enemy[j].yspeed;
	  else enemy[j].y+=(enemy[j].yspeed*speed_factor);
          if (enemy[j].y>189) enemy[j].out=0;
       }
    }
#endif
       
       /***READ KEYBOARD***/
    if ((ch=vmwGetInput())!=0) { 
       switch(ch) {
	case VMW_ESCAPE: levelover=1; break;
	case VMW_RIGHT: if (shipadd>=0) shipadd+=3; else shipadd=0; break;
	case VMW_LEFT:  if (shipadd<=0) shipadd-=3; else shipadd=0; break;
	case VMW_F1: game_paused=1; help(game_state); break;      
        case '+': whatdelay++; break;
	case 'P': case 'p': game_paused=1;
	                    coolbox(65,85,175,110,1,virtual_1);
	                    vmwTextXY("GAME PAUSED",79,95,4,7,
					            0,tb1_font,virtual_1);
	                    vmwBlitMemToDisplay(game_state->graph_state,virtual_1);
	                    while (vmwGetInput()==0) usleep(30000);
			    break;
	case '-': whatdelay--; break;
	case 'S':
	case 's': game_state->sound_enabled=!(game_state->sound_enabled); break;
        case VMW_F2: game_paused=1; 
	             savegame(game_state);
	             break;
	case ' ':  if (bullet_root->howmany<BULLETS_MAX) {  
	              /* Find empty slot */
	              for (j=0;j<bullet_root->allowed;j++) {
			  if (!bullet_info[j]->out) {
			     temp_bullet=bullet_info[j];
			     break;
			  }
		      }
	              bullet_root->howmany++;
	              temp_bullet->out=1;
	              temp_bullet->type=20;  /* FIXME */
		      if ((game_state->sound_possible)&&(game_state->sound_enabled))
			 playGameFX(SND_CC);
		      temp_bullet->x=shipx+21;
		      temp_bullet->y=165;
	              temp_bullet->next=NULL;
	                 /* Insert in linked list */
	              if (bullet_root->head==NULL) {
		         bullet_root->head=temp_bullet;
			 bullet_root->tail=temp_bullet;
			 temp_bullet->prev=NULL;
		      }
	              else {
			 temp_bullet->prev=bullet_root->tail;
			 bullet_root->tail->next=temp_bullet;
			 bullet_root->tail=temp_bullet;
		      } 
	                 /* Draw Bullet */

		       vmwPutSprite(data->sprites[temp_bullet->type]->data,
			            temp_bullet->x,temp_bullet->y,virtual_1);

	}
	  
       }
    }
	  

       /***MOVE SHIP***/
//    if (speed_factor>1) {
//       shipx+=(shipadd*speed_factor);
//       rows_goneby+=(speed_factor);
//    }
//    else {
       shipx+=shipadd;
       rows_goneby++;
//    }
    if (shipx<1) shipx=1;
    if (shipx>190) shipx=190;
    switch(shipframe) {
       case 1: vmwPutSprite(ship_shape[0],
			    shipx,165,virtual_1); break;
       case 3: vmwPutSprite(ship_shape[1],
			    shipx,165,virtual_1); break;
       case 2:
       case 4: vmwPutSprite(ship_shape[2],
			    shipx,165,virtual_1); break;
    }
    shipframe++;
    if (shipframe==5) shipframe=1;
   
       /* Flip Pages */
    vmwBlitMemToDisplay(game_state->graph_state,virtual_1);   

       
       /* If time passed was too little, wait a bit */
       /* 33,333 would frame rate to 30Hz */
       /* Linux w 100Hz scheduling only gives +- 10000 accuracy */
    done_waiting=0;
    while (!done_waiting) {
       
       gettimeofday(&timing_info,&dontcare);
       time_spent=timing_info.tv_usec-oldusec;
       
          /* Assume we don't lag more than a second */
          /* Seriously, if we lag more than 10ms we are screwed anyway */
       if (time_spent<0) time_spent+=1000000;
       if (time_spent<30000) usleep(100);
       else (done_waiting=1);                       
    }
    oldusec=timing_info.tv_usec;
    oldsec=timing_info.tv_sec;
       
       /* If game is paused, don't keep track of time */
      
    if (game_paused) {
       gettimeofday(&timing_info,&dontcare);
       oldusec=timing_info.tv_usec;
       oldsec=timing_info.tv_sec;
       game_paused=0;
    }
    

    /*printf("%i\n",rows_goneby);*/
    if (rows_goneby>1950) {
//       printf("%i\n",rows_goneby);
//       coolbox(35,85,215,110,1,virtual_1);
//       vmwTextXY("TO BE CONTINUED...",55,95,4,7,0,tb1_font,virtual_1);
//       vmwBlitMemToDisplay(game_state->graph_state,virtual_1);
//       pauseawhile(10);
       
	 
  
       vmwClearKeyboardBuffer();
       pauseawhile(5);
       vmwLoadPicPacked(0,0,game_state->virtual_3,0,1,
			tb1_data_file("level1/viewscr.tb1",game_state->path_to_data),
			game_state->graph_state);
       vmwClearScreen(game_state->virtual_1,0);
       vmwArbitraryCrossBlit(game_state->virtual_3,0,79,58,37,
			     game_state->virtual_1,10,10);
       vmwClearKeyboardBuffer();
       vmwSmallTextXY("UNIDENTIFIED SPACECRAFT!",70,10,2,0,1,tb1_font,game_state->virtual_1);
       vmwSmallTextXY("DO YOU WISH TO DEACTIVATE ",70,20,2,0,1,tb1_font,game_state->virtual_1);
       vmwSmallTextXY("THIS SHIP'S SECURITY SYSTEMS? (Y/N)",70,30,2,0,1,tb1_font,game_state->virtual_1);
       vmwBlitMemToDisplay(game_state->graph_state,virtual_1);
       vmwClearKeyboardBuffer();
      
       ch='!';
       while ((ch!='Y') && (ch!='y') && (ch!='N') && (ch!='n')) {
          while(!(ch=vmwGetInput())) usleep(1000);
       }
       
       if ((ch=='N') || (ch=='n')) {
	  vmwArbitraryCrossBlit(game_state->virtual_3,0,79,58,37,
				game_state->virtual_1,10,50);
          vmwSmallTextXY("NO?  AFFIRMATIVE. ",70,50,9,0,1,tb1_font,game_state->virtual_1);
          vmwSmallTextXY("ARMING REMOTE DESTRUCTION RAY.",70,60,9,0,1,tb1_font,game_state->virtual_1);
          vmwSmallTextXY("GOOD-BYE.",70,70,9,0,1,tb1_font,game_state->virtual_1);
          vmwBlitMemToDisplay(game_state->graph_state,virtual_1);
	  pauseawhile(4);
       }
      

       if ((ch=='Y') || (ch=='y')) {
	  vmwArbitraryCrossBlit(game_state->virtual_3,0,79,58,37,
				game_state->virtual_1,10,50);
          vmwSmallTextXY("'Y'=CORRECT PASSWORD. ",70,50,2,0,1,tb1_font,game_state->virtual_1);
          vmwSmallTextXY("WELCOME SUPREME TENTACLEE COMMANDER.",70,60,2,0,1,tb1_font,game_state->virtual_1);
          vmwSmallTextXY("INITIATING TRACTOR BEAM AND AUTOMATIC",70,70,2,0,1,tb1_font,game_state->virtual_1);
          vmwSmallTextXY("LANDING PROCEDURE.",70,80,2,0,1,tb1_font,game_state->virtual_1);
          vmwSmallTextXY("WE WILL BE DEPARTING FOR THE PLANET",70,90,2,0,1,tb1_font,game_state->virtual_1);
          vmwSmallTextXY("EERM IN THREE MICROCYCLE UNITS.",70,100,2,0,1,tb1_font,game_state->virtual_1);
          vmwBlitMemToDisplay(game_state->graph_state,virtual_1);
	  pauseawhile(5);
	  
          game_state->level=3;
          vmwClearKeyboardBuffer();
	  
          vmwArbitraryCrossBlit(game_state->virtual_3,0,42,58,37,
				game_state->virtual_1,10,110);
	  
          vmwSmallTextXY("Wha? Wait!",70,110,9,0,1,tb1_font,game_state->virtual_1);
          vmwSmallTextXY("What's happening?",70,120,9,0,1,tb1_font,game_state->virtual_1);
          vmwBlitMemToDisplay(game_state->graph_state,virtual_1);
	  pauseawhile(6);
       }
       
       vmwLoadPicPacked(0,0,game_state->virtual_3,0,1,
			tb1_data_file("level3/tbtract.tb1",game_state->path_to_data),
			game_state->graph_state);

       
       vmwArbitraryCrossBlit(game_state->virtual_3,0,0,240,50,
			     game_state->virtual_2,0,0);
       vmwClearScreen(game_state->virtual_1,0);
       
       setupsidebar(game_state,virtual_1);
       
       
       for(howmuchscroll=50;howmuchscroll>0;howmuchscroll--) {
	  vmwArbitraryCrossBlit(virtual_2,0,howmuchscroll,240,200,virtual_1,0,0);
	  usleep(30000);
	  vmwPutSprite(ship_shape[0],shipx,165,virtual_1);
          vmwBlitMemToDisplay(game_state->graph_state,virtual_1);    
       }

       if ((ch=='N') || (ch=='n')) {
          vmwClearKeyboardBuffer();
          vmwLine(7,6,shipx+10,180,4,virtual_1);
          vmwLine(shipx+37,180,231,6,4,virtual_1);
          vmwBlitMemToDisplay(game_state->graph_state,virtual_1);
	  pauseawhile(1);
          vmwClearKeyboardBuffer();
          for(i=shipx;i<shipx+48;i++) {
             vmwDrawVLine(i,165,30,4,virtual_1);
	  }
	  vmwBlitMemToDisplay(game_state->graph_state,virtual_1);
          pauseawhile(2);
	  vmwArbitraryCrossBlit(virtual_2,0,howmuchscroll,240,200,virtual_1,0,0);
          vmwBlitMemToDisplay(game_state->graph_state,virtual_1);
	  pauseawhile(2);      
       }

       else {
	  if (shipx-95==0) shipadd=0;
	  if (shipx-95>0) shipadd=1;
	  if (shipx-95<0) shipadd=-1;
	  shipy=165;
          while ((shipx!=95) || (shipy>10)) {
             if (shipx!=95) shipx-=shipadd;
             if (shipy>10) shipy--;
	     vmwArbitraryCrossBlit(virtual_2,0,howmuchscroll,240,200,virtual_1,0,0);
	     
             vmwLine(7,6,shipx+12,shipy+15,2,virtual_1);
             vmwLine(shipx+37,shipy+15,231,6,2,virtual_1);
             vmwPutSprite(ship_shape[0],shipx,shipy,virtual_1);
             vmwBlitMemToDisplay(game_state->graph_state,virtual_1);
	     usleep(30000);
	  }
          vmwClearKeyboardBuffer();
          pauseawhile(8);
          vmwClearScreen(virtual_1,0);
       }
/*
     while keypressed do ch:=readkey;
     if level=4 then begin
       vmwSmallTextXY('THE PLANET EERM?',20,20,10,0,1,tb1_font,game_state->virtual_1);
       vmwSmallTextXY('XENOCIDE FLEET?',20,30,10,0,1,tb1_font,game_state->virtual_1);
       vmwSmallTextXY('WHAT'S GOING ON?',20,40,10,0,1,tb1_font,game_state->virtual_1);
       vmwSmallTextXY('A MAJOR GOVERNMENT CONSPIRACY?  MASS HALUCINATIONS?',20,50,10,0,1,tb1_font,game_state->virtual_1);

       vmwSmallTextXY('WATCH FOR TOM BOMBEM LEVEL 3 (CURRENTLY IN THE DESIGN PHASE).',10,70,12,0,1,tb1_font,game_state->virtual_1);
       vmwSmallTextXY('ALL THESE QUESTIONS WILL BE ANSWERED!',10,80,12,0,1,tb1_font,game_state->virtual_1);
       vmwSmallTextXY('ALSO MORE FEATURES WILL BE ADDED:',10,90,12,0,1,tb1_font,game_state->virtual_1);
       vmwSmallTextXY('     BETTER GRAPHICS, SOUND AND SPEED.',10,100,12,0,1,tb1_font,game_state->virtual_1);

       vmwSmallTextXY('TO HASTEN COMPLETION, SEND QUESTIONS/COMMENTS/DONATIONS TO ',9,120,9,0,1,tb1_font,game_state->virtual_1);
       vmwSmallTextXY('THE AUTHOR (SEE THE REGISTER MESSAGE FOR RELEVANT ADDRESSES).',9,130,9,0,1,tb1_font,game_state->virtual_1);

       vmwSmallTextXY('THANK YOU FOR PLAYING TOM BOMBEM',80,150,14,0,1,tb1_font,game_state->virtual_1);
       unfade;
       pauseawhile(1800);
     end; */
          levelover=1;
    }
    }

}

  

void littleopener2(tb1_state *game_state) {
   
    vmwDrawBox(0,0,319,199,0,game_state->virtual_1);
    vmwLoadPicPacked(0,0,game_state->virtual_1,1,1,
		     tb1_data_file("level2/tbl2ship.tb1",game_state->path_to_data),
		     game_state->graph_state);
    vmwTextXY("Hmmmm... ",10,10,4,0,0,game_state->graph_state->default_font,
	      game_state->virtual_1);
    vmwTextXY("This Might Be Harder Than I Thought.",10,20,4,0,0,
	      game_state->graph_state->default_font,game_state->virtual_1);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(13); 
    vmwDrawBox(0,0,319,199,0,game_state->virtual_1);
 
}
