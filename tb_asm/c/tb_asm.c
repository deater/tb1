#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <time.h>
#include <sys/time.h>

#define SCREEN_WIDTH   40
#define SCREEN_HEIGHT  24
#define BYTES_PER_PIXEL 2
#include "vmwsoft.h"
#include "opener.h"
#include "vince.h"
#include "tom.h"
#include "phobos.h"
#include "game_sprites.h"
#include "boss.h"
#include "ending.h"

int play_sound=0;
char hi_player[4]="KRG";
int hiscore=100;
int dump_screen=0;

void load_hi_score() {
    FILE *hi_score;
   
    hi_score=fopen("/tmp/tb_asm.hsc","r");
    if (hi_score!=NULL) {
       fscanf(hi_score,"%i\n",&hiscore);
       fgets(hi_player,4,hi_score);
       fclose(hi_score);
    }
}



int clear_framebuffer(char *buffer,char color,char value) {
   
    int i=0;
    
    while(i<SCREEN_WIDTH*SCREEN_HEIGHT*BYTES_PER_PIXEL) {
       buffer[i]=color;
       buffer[i+1]=value;
       i+=BYTES_PER_PIXEL;
    }

    return 0;
}

int blit(char *sprite, char *framebuffer, char ascii,int x, int y,int xsize,int ysize) {

   int xx=0,yy=0,offset;
     
   offset=((y*SCREEN_WIDTH)+x)*BYTES_PER_PIXEL;
   
   while(yy<ysize) {
      if (sprite[(yy*xsize)+xx]) {
	 framebuffer[offset]=sprite[(yy*xsize)+xx];
         framebuffer[offset+1]=ascii;
      }
      
      offset+=BYTES_PER_PIXEL;
      xx++;
      if (xx>=xsize) {
	   offset+=((SCREEN_WIDTH-xsize)*BYTES_PER_PIXEL);
	   xx=0;
	   yy++;
      }
   }
   
   return 0;   
}

int put_text_xy(char *text,int x,int y,int color, char *framebuffer) {
   
   int offset,i;
   
   offset=(y*SCREEN_WIDTH+x)*BYTES_PER_PIXEL;
   
   i=0;
   
   while(i<strlen(text)) {
      framebuffer[offset]=(char)color;
      framebuffer[offset+1]=text[i];
      i++;
      offset+=2;
   }
   return 0;
 
   
}


int to_buffer(char *buffer,char *string) {
   
   strncat(buffer,string,BUFSIZ);
   return strlen(string);
   
}

int to_buffer_color(char *buffer,int bold, int color) {
   
   char temp_string[BUFSIZ];
   
   sprintf(temp_string,"\033[%i;%im",bold,color);
   strncat(buffer,temp_string,BUFSIZ);
   return strlen(temp_string);
}


int dump_to_screen(char *framebuffer) {

    static char out_buffer[40*44*10];
    char colors[8]={30,34,32,36,31,35,33,37};   

    unsigned char temp_string[2]={0,0};
   
    int size=0,i,j,color,old_color=-1;
    int block_mode;
   
   
    block_mode=0;
    out_buffer[0]='\0';
   
       /* home cursor */
    size+=to_buffer(out_buffer,"\033[0;0H");

       
    for(j=0;j<SCREEN_HEIGHT;j++) {
       for(i=0;i<SCREEN_WIDTH;i++) {
	  color=framebuffer[(i+j*SCREEN_WIDTH)*BYTES_PER_PIXEL];
	  if (color!=old_color) { 
	     size+=to_buffer_color(out_buffer,color>7,colors[color&7]);
	     old_color=color;
	  }
	  temp_string[0]=framebuffer[((i+j*SCREEN_WIDTH)*BYTES_PER_PIXEL)+1];
	  if (temp_string[0]==219) {
	     if (!block_mode){
		block_mode=1;
	        size+=to_buffer(out_buffer,"\016");
	     }
	     temp_string[0]='0';
	  }
	  else {
	     if (block_mode) {
		block_mode=0;
		size+=to_buffer(out_buffer,"\017");
				
	     }
	  }
   
	  size+=to_buffer(out_buffer,temp_string);
       }       
       size+=to_buffer(out_buffer,"\n");
    }
    
    if (size>40*44*10) {	
	
       printf("\a\a\a\n");
       exit(10);
    }
   
   
    write(1,out_buffer,size);
   if (dump_screen) {
      int fd;
      fd=creat("screen",0x666);
      write(fd,out_buffer,size);
      close(fd);
      dump_screen=0;
   }
   
//    fdatasync(1);       
   
    return 0;
}

int get_timer() {
   
   static int initialized=0;
   static int startseconds,startusecs;
   
   struct timeval our_time;
   
   int temp_time;
   
   if (!initialized) {
      gettimeofday(&our_time,NULL);
      
      startseconds=time(NULL);
      startusecs=our_time.tv_usec;	
      
      initialized=1;
   }
   gettimeofday(&our_time,NULL);
   temp_time=(time(NULL)-startseconds)*1000;
   temp_time+=(our_time.tv_usec/1000);
   
   return temp_time;
 
}


void clear_keyboard_buffer() 
{
   int ch;
   
   while(read(0,&ch,1)==1);
}


void pause_a_while(int msecs) {
   
   int ch,our_time;
   
   our_time=get_timer();
   
   while ( read(0,&ch,1)!=1) {
      usleep(1);
  
      if ( (get_timer()-our_time) > msecs ) break;
   }
   clear_keyboard_buffer();
   
}

int get_char() {
   
   char ch;
   int result;
   
   result=read(0,&ch,1);
   
   if (result<1) return 0;
   else return ch;
}


int get_a_char() {
   
    char ch,ch1,ch2,ch3;
   
    ch=get_char();
		
       if (ch==27) {
	  ch1=get_char();
	  ch2=get_char();
	  ch3=get_char();
	  
	  if ((ch1==0) && (ch2==0)) return 'q';
	  if ((ch1==91) && (ch2==66)) return 'm';
	  if ((ch1==91) && (ch2==65)) return 'i';
	  if ((ch1==91) && (ch2==67)) return 'k';
	  if ((ch1==91) && (ch2==68)) return 'j';
	  if ((ch1==91) && (ch2==91)) {   
	     if (ch3==65) return 'h';
	  }
	  	    {
	       FILE *fff;
	       fff=fopen("bob","w");
	       fprintf(fff,"%i %i %i\n",ch1,ch2,ch3);
	       fclose(fff);
	    }
//	 }
       }
    return ch;
}




void null()
{
}


void put_attention_block(char *framebuffer,int color) {
   
    int i,j;
   
    for(i=5;i<12;i++) 
       for(j=0;j<40;j++) 
          put_text_xy("*",j,i,color,framebuffer);
    
}


int verify_quit(char *framebuffer) {
 
    int quit_bar=0;
    char ch;
   
    put_attention_block(framebuffer,14);
    put_text_xy(" QUIT?  Are you sure? ",9,7,12,framebuffer);
    put_text_xy("     NO!",9,8,12,framebuffer);
    put_text_xy("     YES!",9,9,12,framebuffer);
 quit_loop:   
   
    if (quit_bar==0) {
       put_text_xy(" --> NO!  ",9,8,13,framebuffer);
       put_text_xy("     YES  ",9,9,5,framebuffer);
    }
    else {
       put_text_xy("     NO   ",9,8,5,framebuffer);
       put_text_xy(" --> YES! ",9,9,13,framebuffer);
    }
   
    dump_to_screen(framebuffer);	      
    while ((ch=get_a_char())==0) usleep(5000);
   
    if ((ch=='i') || (ch=='j') || (ch=='k')  || (ch=='m')) quit_bar=!quit_bar;
   
    if (ch=='y') quit_bar=1;
    if (ch=='n') quit_bar=0;
   
    if (ch=='\n') {
       return quit_bar;
    }
   
    goto quit_loop;	
   
    return 0;
   
}

int ending(char *framebuffer) {

    sleep(3);    
   
    clear_keyboard_buffer();
   
    clear_framebuffer(framebuffer,0,' ');

    blit(earth_sprite,framebuffer,219,32,0,5,6);
    put_text_xy("INCOMING MESSAGE FROM",0,0,15,framebuffer);
    put_text_xy("     ** EARTH **",0,1,15,framebuffer);
   
    put_text_xy("Congratulations Tom!",0,3,14,framebuffer);
    put_text_xy("You've destroyed the",0,4,14,framebuffer);
    put_text_xy("  Marketing Menace!!",0,5,14,framebuffer);
   
    put_text_xy("But wait...",0,7,14,framebuffer);
    put_text_xy("Our sensors detect another batch of",0,8,14,framebuffer);
    put_text_xy("  enemies, moving even faster!",0,9,14,framebuffer);
    put_text_xy("Feel free to quit at any time, but",0,11,14,framebuffer);
    put_text_xy("  remember no paycheck bonus unless",0,12,14,framebuffer);
    put_text_xy("  all are destroyed.",0,13,14,framebuffer);
    put_text_xy("PS Your pet guinea",0,15,14,framebuffer);
    put_text_xy("   pig is doing",0,16,14,framebuffer);
    put_text_xy("   well.       -->",0,17,14,framebuffer);
    blit(susie_sprite,framebuffer,219,20,15,19,7);   
    put_text_xy("** END TRANSMISSION **",0,22,15,framebuffer);
       
    dump_to_screen(framebuffer);

    while(get_char()==0) usleep(30);
   
    clear_framebuffer(framebuffer,0,' ');

    blit(tom_head_sprite,framebuffer,219,0,0,7,7);
    put_text_xy("Tom: *Sigh*",10,0,9,framebuffer);
   
    put_text_xy("Press Any Key to Resume Fighting",0,10,15,framebuffer);
   
    dump_to_screen(framebuffer);
   
    while(get_char()==0) usleep(30);
   
    return 0;
}

int help(char *framebuffer) {

    clear_framebuffer(framebuffer,0,' ');

    put_text_xy("TOM BOMBEM",10,0,15,framebuffer);
   
    put_text_xy("by",14,1,7,framebuffer);
    put_text_xy("Vince Weaver",9,2,15,framebuffer);
   
    put_text_xy("Key Bindings:",0,4,15,framebuffer);
    put_text_xy("UP or 'i'    : Move menu up",2,6,7,framebuffer);
    put_text_xy("DOWN or 'm'  : Move menu down",2,7,7,framebuffer);
    put_text_xy("ENTER        : Selects current option",2,8,7,framebuffer);
   
    put_text_xy("RIGHT or 'k' : Move ship right",2,10,7,framebuffer);
    put_text_xy("LEFT or 'j'  : Move ship left",2,11,7,framebuffer);
    put_text_xy("SPACEBAR     : Shoots",2,12,7,framebuffer);

    put_text_xy("F1 or 'h'    : Displays help",2,14,7,framebuffer);
    put_text_xy("ESC or 'q'   : Quits",2,15,7,framebuffer);
    put_text_xy("'p'          : Pauses",2,16,7,framebuffer);
    put_text_xy("'s'          : Toggles sound",2,17,7,framebuffer);
   
    dump_to_screen(framebuffer);

    while(get_char()==0) usleep(30);
   
    return 0;
}



int hi_score(char *framebuffer) {

    char temp_string[BUFSIZ];
   
    load_hi_score();
   
    put_attention_block(framebuffer,12);
    put_text_xy("HIGH SCORE",15,7,14,framebuffer);
    
    sprintf(temp_string," %s   %i ",hi_player,hiscore);
    put_text_xy(temp_string,(40-strlen(temp_string))/2,9,15,framebuffer);
   
    dump_to_screen(framebuffer);	      


   
    return 0;
}

int new_hi_score(char *framebuffer) {

    FILE *hi_score;
    char temp_name[4]="AAA";
    int char_num=0,ch;
   
    put_attention_block(framebuffer,9);
    put_text_xy("NEW HIGH SCORE",13,5,13,framebuffer);
    put_text_xy("Use the arrows to pick your initials",2,7,14,framebuffer);
    put_text_xy("Press ENTER when you are done",5,8,14,framebuffer);

 hi_score_loop:
    put_text_xy("     ",17,10,15,framebuffer);
    put_text_xy("   ",18,11,15,framebuffer);
   
    put_text_xy(temp_name,18,10,15,framebuffer);
    put_text_xy("^",18+char_num,11,15,framebuffer);   
   
    dump_to_screen(framebuffer);   
   
    while ((ch=get_a_char())==0) usleep(5000);   
   
    if (ch=='k') {
       char_num++;
       if (char_num>2) char_num=0;
    }
    if (ch=='j') {
       char_num--;
       if (char_num<0) char_num=2;
    }
    
    if (ch=='i') {
       temp_name[char_num]--;
       if (temp_name[char_num]<64) temp_name[char_num]=126;
    }
   
    if (ch=='m') {
       temp_name[char_num]++;
       if (temp_name[char_num]>126) temp_name[char_num]=64;
    } 
       
    if (ch!='\n') goto hi_score_loop;
   
    strncpy(hi_player,temp_name,3);
    
    hi_score=fopen("/tmp/tb_asm.hsc","w");
    if (hi_score!=NULL) {
       fprintf(hi_score,"%i\n",hiscore);
       fprintf(hi_score,"%s\n",hi_player);
       fclose(hi_score);    
    }
   
    return 0;
}




void set_sound(int freq,int duration) {
    printf("%c[10;%i]%c[11;%i]\n",27,freq,27,duration);
    play_sound=1;
}



#define UP_SHIELDS 200

int new_game(char *framebuffer) {

         
    int i,j,xadd=0,scroll=0,quit=0,x=20,bigx=20<<8,y=16;
    int sound=1,shields=8,score=0;
    int game_paused=0,current_enemy_type=0 /*0*/,current_enemy_kind=-1,current_init_x=-1,
        total_enemies_out=0,level=1,enemies_spawned=1,enemy_wave=0;

    char temp_string[BUFSIZ];
   
    int done_waiting;
    struct timeval timing_info;
    int time_spent,oldusec=0,oldsec=0;

    struct boss_type {
	int x,xadd,count;
        int waiting,hits,smoke,exploding;
        int shooting;
    } boss;
   
    struct missile_type {
       int out,x,y;
    } missile[2];

    struct enemy_type {
       int x,y,out,kind;
       int xadd,yadd,xmin,xmax;
       int hitsneeded;
       int exploding;
    } enemies[5];

    int enemy_wait=30,enemy_count=0;
   
   
    char ch;
   
    char *background;

    FILE *debug;
   
    debug=fopen("debug","w");

   
    load_hi_score();
   
    clear_keyboard_buffer();
    clear_framebuffer(framebuffer,0x0,' ');

    background=calloc(1600,sizeof(char));
   
    missile[0].out=0;
    missile[1].out=0;
   
    for(i=0;i<5;i++) enemies[i].out=0;
   

       /* RANDOMIZE TIMER */
    srand(time(NULL));
   
       /* Draw the Stars */
    for(i=0;i<50;i++) {
       *(background+(rand()%40)*40+rand()%40)=7;
       *(background+(rand()%40)*40+rand()%40)=8;
    }
   
   
    while(!quit) {

       
       clear_framebuffer(framebuffer,0x0,' ');
       
          /* Scrolling Stars */
       scroll-=1;
       if (scroll<0) scroll=78;
       
       if (scroll/2<20) {
	  blit(background+(scroll/2*40),framebuffer,'.',0,0,40,20);
       }
       else {
	  blit(background+(scroll/2*40),framebuffer,'.',0,0,40,40-scroll/2);
	  blit(background,framebuffer,'.',0,40-scroll/2,40,scroll/2-20);
       }


          /* Put out new enemies */

          /* Type 0: "Diagonal No Wait" x_init=rand, xadd=1, yadd=1
	   * Type 1: "horiz then fall" xadd=1. After random, xadd=0 yadd=1
	   * Type 2: "diagonal, wait" wait(all_free) then 0 with same type
	   * Type 4: "Wiggle" x=rand()
	   * Type 5: "Rain" x=rand() xadd=0 yadd=1 
	   * Type 6: Wait for Boss
	   * Type 7: BOSS
	   * 
	   * 
	   * After enemies_destroyed>20 switch types
	   * After enemies_destroyed>500? Wait till all destroyed then BOSS
	   * After boss keep going, knocking up speed each 100
	   */
       
           /* Keep a level indicator? */
       
          /* Keep a count of how close to place enemies */
          /* Don't want all to appear at once           */

       
       enemy_count++;
       if (enemy_count==enemy_wait) {
	  enemy_count=0;
	  
	if (current_enemy_type==7) enemies_spawned=1;  
	  
	  /* make length random? */
       if (enemies_spawned%15==0) {
	  
	  enemies_spawned++; /* hack to keep from getting stuck */
	  current_enemy_type=rand()%6;
	  
	  enemy_wave++;
	  if (enemy_wave==15) current_enemy_type=6;
	  

	  if ((current_enemy_type==0) || (current_enemy_type==1))
	     enemy_wait=30;
	  else enemy_wait=10;
	  current_enemy_kind=-1;
	  current_init_x=-1;
	  
       }	  
	  
	         
          i=0;
          while (i<5) {
	     if (!enemies[i].out) break;
	     i++;
	  }
	  
	  if (current_enemy_type==2) {
	     if (total_enemies_out>0) goto move_enemies;
	     current_enemy_type=3;
	     current_enemy_kind=rand()%6;
	     current_init_x=rand()%300;
	     enemy_wait=10;
	  }
	  
	  if (current_enemy_type==6) {
	     if (total_enemies_out>0) goto move_enemies;
	     boss.x=13;
	     boss.xadd=1;
	     boss.count=10+rand()%40;
	     boss.waiting=1;
	     boss.hits=20+(5*level);
	     boss.smoke=0;
	     boss.exploding=0;
	     current_enemy_type=7;
	     enemy_wait=10;
	  }
	  
	  if ((current_enemy_type==7) && (boss.waiting)) goto move_enemies;
       
          if (i<5) {
	     enemies_spawned++;
	     enemies[i].out=1;
	     enemies[i].exploding=0;
             total_enemies_out++;
	     enemies[i].y=0;
	     enemies[i].hitsneeded=0;
	     enemies[i].xmin=0;
	     enemies[i].xmax=37;
	     
	     if (current_enemy_kind<0) enemies[i].kind=rand()%6;
	     else enemies[i].kind=current_enemy_kind;
	     
             if (current_init_x<0) enemies[i].x=rand()%300;
	     else enemies[i].x=current_init_x;
	     
	     switch(current_enemy_type) {
	        case 0: enemies[i].xadd=level; 
		        enemies[i].yadd=level;
		        break;
		case 1: enemies[i].xadd=5; 
		        enemies[i].yadd=-(rand()%100);
		        break;
		case 3: enemies[i].xadd=3; 
		        enemies[i].yadd=level;
		        break;
		case 4: enemies[i].xadd=4; 
		        enemies[i].yadd=level;
		        enemies[i].xmin=2+rand()%34;
		        enemies[i].xmax=enemies[i].xmin+(rand()%20);
		        if (enemies[i].xmax>35) enemies[i].xmax=35;
		        enemies[i].x=enemies[i].xmin*8;
//		        fprintf(debug,"%i %i\n",enemies[i].xmin,enemies[i].xmax)
		        break;
	        case 5: enemies[i].xadd=0; enemies[i].yadd=2;
		        if (rand()%3==0) enemies[i].x=x;
		        break;
		case 7: enemies[i].xadd=0; enemies[i].yadd=2;
		        enemies[i].x=(boss.x+5)*8;
		        enemies[i].y=3*8;
		        break;
	     }
	     
	  }
	  	  
       }
       
move_enemies:       
       
       
          /* Move the enemies */
       for(i=0;i<5;i++) {
	  if (enemies[i].out) {
	     	     
	        /* Move on x */
	     if (enemies[i].xadd!=0) {
		enemies[i].x+=enemies[i].xadd; 
	     
                if ((enemies[i].x/8>37) || (enemies[i].x/8<0)) {
	           enemies[i].xadd=-enemies[i].xadd;
	           enemies[i].x+=enemies[i].xadd;
		}     
		
		if (current_enemy_type==4) {
		   if ((enemies[i].x/8>enemies[i].xmax) || (enemies[i].x/8<enemies[i].xmin)) {
	              enemies[i].xadd=-enemies[i].xadd;
//	              enemies[i].x+=enemies[i].xadd;
		   }
		}       
	     }
	     
	        /* check for special case II */
	     if (enemies[i].yadd<0) {
		enemies[i].yadd++;
		if (enemies[i].yadd==0) {
		   enemies[i].yadd=2*level;
		   enemies[i].xadd=0;
		}
		   
	     }
	     
	        /* less than zero signifies extra info */
	     if (enemies[i].yadd>0) {  
	        enemies[i].y+=enemies[i].yadd;
	        if (enemies[i].y/8>18) {
		   enemies[i].out=0;
		   total_enemies_out--;
		}
		
	     }
	     
	     
	        /* Ship <-> Enemies Collision Detection */
	     if (!enemies[i].exploding) {
		if ( ( (enemies[i].x/8+2 >= x) && (enemies[i].x/8+2 <= x +6) )
		     || ( (enemies[i].x/8 >=x) && (enemies[i].x/8 <= x+6))) {
		   
		   if ( ((enemies[i].y/8 >= y) && (enemies[i].y/8 <=y+2))
		      || ( (enemies[i].y/8+1 >=y) && (enemies[i].y/8+1 <=y+2)) ) {
			
		      enemies[i].exploding=1;
		      shields--;
		      set_sound(120,50);
				      
		   }
		   
		}
		
		     
	     }
	     
		  
	     
	  }
       }
       
     
       
       
       
       
          /* Draw the enemies */
       for(i=0;i<5;i++) {
          if (enemies[i].out) {	    
	     if (enemies[i].exploding) {
		blit(explosion_sprites[enemies[i].exploding/2-1],framebuffer,219,enemies[i].x/8,enemies[i].y/8,3,2);
		enemies[i].exploding++;
		if (enemies[i].exploding>7) {
		   enemies[i].out=0;
		   total_enemies_out--;
		}
		
		     
	     }
	     else {
		blit(enemy_sprites[enemies[i].kind],framebuffer,219,enemies[i].x/8,enemies[i].y/8,3,2);  
	     }	  
	  }
	  
       }

       
         /* Draw the Boss */
       if (current_enemy_type==7) {
	  if (boss.exploding!=1) blit(boss_sprite,framebuffer,219,boss.x,0,13,3);
	  if (boss.smoke) {
	     blit(smoke_sprites[boss.smoke/4],framebuffer,219,boss.x+5,3,3,1);
	     blit(smoke_sprites[boss.smoke/4],framebuffer,219,boss.x+5-boss.xadd,4,3,1);
	     boss.smoke--;
	  }

	  if (boss.shooting) {
	     boss.shooting--;
	     for(i=0;i<8;i++) {
	        blit(laser_sprite[boss.shooting%2],framebuffer,219,boss.x,3+(i*2),1,2);
		blit(laser_sprite[boss.shooting%2],framebuffer,219,boss.x+12,3+(i*2),1,2);
		set_sound(200+boss.shooting,20);
	     }
	  }
	  
	  if (boss.exploding) {
	     boss.waiting=1;
	     for(i=0;i<30;i++) {
		blit(smoke_sprites[boss.exploding/8],framebuffer,219,boss.x+rand()%11,rand()%4,3,1);
	     }
	     set_sound(120,20);
	     boss.exploding--;
	     if (boss.exploding==0) {
		level++;
		boss.shooting=0;
		for(i=0;i<5;i++) enemies[i].exploding=2;
		ending(framebuffer);
		current_enemy_type=2;
		enemy_wave=0;
	     }	  
	  }
	  
	  else {     
	       
	     boss.count--;
	     if (boss.count<0) {
	        if (boss.waiting) {
		   boss.waiting=0;
		   boss.count=40+rand()%320;
		}
		
	        else {
		   boss.waiting=1;
		   boss.count=30+rand()%40;
		   boss.shooting=30;

		}
		
	     }   
	  }
	  
	  
	  if (!boss.waiting) {
	     boss.x+=boss.xadd;
	     if ((boss.x>26) || (boss.x<0)) {
	        boss.xadd=-boss.xadd;
	        boss.x+=boss.xadd;
	     }

	  }
	  
	     /* Collision detection for lasers */
	  if (boss.shooting) {
	     if (((boss.x < x+6) && (boss.x>=x)) ||
	        ((boss.x+12<x+6) && (boss.x+12>=x))) {
                if (boss.shooting%7==0) shields--;
		
	     }  
	  }	  
       }
       
       
       

          /* Move the missiles */
       for(i=0;i<2;i++) {  
          if (missile[i].out) {
	     missile[i].y--;
	     if (missile[i].y<0) missile[i].out=0;

             /* Missile Collision Detection */
	     for(j=0;j<5;j++) {

	        /* Missile <-> Enemy Collision Detection */		     
		if ((enemies[j].out)&&(!enemies[j].exploding)) {
	           if ((missile[i].x >=enemies[j].x/8) && (missile[i].x < enemies[j].x/8+3)) {
		      if ((missile[i].y >= enemies[j].y/8) && (missile[i].y <=enemies[j].y/8+1) ) {
		         missile[i].out=0;
		         enemies[j].exploding=2;
		         set_sound(150,40);
		         score+=10;
		         if (score%UP_SHIELDS==0) {
			    shields++;
		 	    set_sound(220,50);
			    if (shields>9) shields=9;
			 }
			 
		      }
		   }
		}
	     }
	     
		/* Missile <-> Boss Collision Detection */
	     if ((current_enemy_type==7) && (!boss.exploding)) {
		if ((missile[i].x >= boss.x) && (missile[i].x < boss.x+13)) {
		   if ((missile[i].y>= 0) && (missile[i].y <=2)) {
		      missile[i].out=0;
		      boss.smoke=11;
		      set_sound(150,50);
		      boss.hits--;
		      
		      if (boss.hits<0) boss.exploding=23;
		   }      
		}
	     }
	  }
       }      
       
          /* Draw the missiles */
       for(i=0;i<2;i++) {
	  if (missile[i].out) blit(missile_sprite,framebuffer,219,missile[i].x,missile[i].y,1,3);
       }
   
		  
	    
       
       ch=0;
       
       while( (ch=get_a_char()) ) {
	    	    
	  switch(ch) {
	   case 'd': dump_screen=1; break;
	   case 'q': case 'Q': 
	             if (verify_quit(framebuffer)) {
			quit=1; 
		     }
	             else {
		        game_paused=1;
		     }
	             break;
	   case 'j': if (xadd<=0) xadd-=128; else xadd=0; break;
	   case 'k': if (xadd>=0) xadd+=128; else xadd=0; break;
	   case 'p': 
	           put_attention_block(framebuffer,14);
	           put_text_xy(" GAME PAUSED! ",13,8,12,framebuffer);
	           dump_to_screen(framebuffer);
	      
	           while (get_char()==0) usleep(5000); 
	           game_paused=1; 
	           break;
	   case 's': sound=!sound; /*level++;*/ break;
	   case 'h': help(framebuffer); /*game_paused=1;*/ break;
	   case ' ': if (!missile[0].out) {
	                missile[0].out=1;
	                missile[0].x=x+3;
	                missile[0].y=y;
	      		set_sound(110,30);
	             } else if (!missile[1].out) {
			missile[1].out=1;
			missile[1].x=x+3;
			missile[1].y=y;
		        set_sound(110,30);
		      
		     }
	             break;
	  }	  
       }
       
       bigx+=xadd;
       if (bigx<0) {
	  bigx=0;
	  xadd=0;
       }
       
       if (bigx>(32<<8)) {
	  bigx=32<<8;
	  xadd=0;
       }

       x=bigx>>8;

       
       blit(ship_sprite,framebuffer,219,x,y,8,4);

       put_text_xy("SHIELDS: ",0,21,shields?12:14,framebuffer);       
       
       for(i=0;i<10;i++) {
	  if (i<shields) {
	       
	     if (i<4) put_text_xy("\333",10+(i*2),21,4,framebuffer);
	     else if (i<8) put_text_xy("\333",10+(i*2),21,12,framebuffer);
	     else put_text_xy("\333",10+(i*2),21,15,framebuffer);
	  }
	  
	  else put_text_xy("_",10+(i*2),21,8,framebuffer);
       }
       
       

          /* Should only print score/shields if they change */
          /* and not waste CPU each frame */
       sprintf(temp_string,"SCORE:   %i",score);
       put_text_xy(temp_string,0,22,15,framebuffer);
       
       sprintf(temp_string,"HISCORE:   %i",hiscore);
       put_text_xy(temp_string,20,22,15,framebuffer);

       sprintf(temp_string,"LEVEL:   %i",level);
       put_text_xy(temp_string,0,23,15,framebuffer);
       
       if ((sound) && (play_sound)) {
	  play_sound=0;
	  put_text_xy("\a",39,23,15,framebuffer);
       }
       
	    
       

       if (shields<0) {
	  quit=1;
       }
       
       
       
       dump_to_screen(framebuffer);

       done_waiting=0;
       while (!done_waiting) {
          gettimeofday(&timing_info,NULL);
	  time_spent=timing_info.tv_usec-oldusec;
	     /* Assume we don't lag more than a second */
	     /* Seriously, if we lag more than 10ms we are screwed anyway */
	  if (time_spent<0) time_spent+=1000000;
	  if (time_spent<30000) usleep(100);
	  else (done_waiting=1);  
       }
       oldusec=timing_info.tv_usec;
       oldsec=timing_info.tv_sec;
       
       if (game_paused) {         
	  gettimeofday(&timing_info,NULL);
	  oldusec=timing_info.tv_usec;
	  oldsec=timing_info.tv_sec;
	  game_paused=0;
       }       
     }

	  
    put_attention_block(framebuffer,14);
	         
    put_text_xy(" GAME OVER! ",14,8,12,framebuffer);
    dump_to_screen(framebuffer);
	  
    if (sound) {
       usleep(250000);
       set_sound(164,500);
       put_text_xy("\a",39,23,15,framebuffer);
       dump_to_screen(framebuffer);
       usleep(600000);
       set_sound(130,500);
       dump_to_screen(framebuffer);
    }	  
    else sleep(2);
	  
    clear_keyboard_buffer();
    pause_a_while(5000);
	  
    clear_framebuffer(framebuffer,0,' ');
	 
    if (score>hiscore) {
       hiscore=score;
       fprintf(debug,"score=%i hiscore=%i\n",score,hiscore);
       new_hi_score(framebuffer);
    }
    hi_score(framebuffer);
	  
    pause_a_while(5000);
	  
     return 0;
}

int about(char *framebuffer) {

    clear_framebuffer(framebuffer,0,' ');
   
    blit(vince_sprite,framebuffer,219,24,0,16,20);
    put_text_xy("TOM BOMBEM",7,0,13,framebuffer);
   
    put_text_xy("by",11,1,14,framebuffer);
    put_text_xy("Vince Weaver",6,2,9,framebuffer);
    put_text_xy("author if you squint ->",0,3,12,framebuffer);
   
    put_text_xy("* Testing my asm skills",0,5,11,framebuffer);
   
    put_text_xy("* Based on Tom Bombem: ",0,7,10,framebuffer);
    put_text_xy("  Invasion of Inanimate",0,8,10,framebuffer);
    put_text_xy("  Objects, a game I    ",0,9,10,framebuffer);
    put_text_xy("  started 10 years ago ",0,10,10,framebuffer);
    put_text_xy("  in Pascal/asm and is ",0,11,10,framebuffer);
    put_text_xy("  still languishing    ",0,12,10,framebuffer);
    put_text_xy("  uncompleted in C/SDL.",0,13,10,framebuffer);
   
    put_text_xy("Contact Vince:",0,17,5,framebuffer);
    put_text_xy("vince@deater.net",0,18,9,framebuffer);
    put_text_xy("http://www.deater.net/weave/",0,19,9,framebuffer);
   
    dump_to_screen(framebuffer);

    while(get_char()==0) usleep(30);
   
    return 0;
}


int story(char *framebuffer) {
   
   int x;
   
   clear_framebuffer(framebuffer,0,' ');
   blit(phobos_sprite,framebuffer,219,0,0,40,16);
            /*  0    5    0   5    0    5    0    5    0 */    
   put_text_xy("It is the year 2025.  All telemarketers",0,17,15,framebuffer);
   put_text_xy("and unsolicited bulk e-mailers have    ",0,18,15,framebuffer);
   put_text_xy("been exiled to Phobos.",0,19,15,framebuffer);
   
   dump_to_screen(framebuffer);
   pause_a_while(7000);
   
   put_text_xy("Right before being trapped forever they",0,17,10,framebuffer);
   put_text_xy("manage to launch one last marketing    ",0,18,10,framebuffer);
   put_text_xy("droid.                                 ",0,19,10,framebuffer);
   
   dump_to_screen(framebuffer);   
   pause_a_while(4000);
   
   x=19; 
   while(x<37) {
       blit(evil_ship_sprite,framebuffer,219,x,9,3,1); 
       dump_to_screen(framebuffer);
       pause_a_while(250);
       put_text_xy("   ",x,9,0,framebuffer);
       x++;
      
   }
   pause_a_while(2000);
   
   clear_framebuffer(framebuffer,0,' ');
   blit(tom_sprite,framebuffer,219,24,1,16,18);
   
              /*0    5    0    5    0   */
   put_text_xy("You are Tom Bombem.",0,0,9,framebuffer);
   
   put_text_xy("You drew the short straw.",0,2,9,framebuffer);
   
   put_text_xy("So it is up to you to",0,4,9,framebuffer);
   put_text_xy("  fight through the ads",0,5,9,framebuffer);
   put_text_xy("  and destroy the evil ",0,6,9,framebuffer);
   put_text_xy("  marketing robot, thus",0,7,9,framebuffer);
   put_text_xy("  restoring harmony to ",0,8,9,framebuffer);
   put_text_xy("  the space lanes.",0,9,9,framebuffer);
	       
   put_text_xy("       GOOD LUCK!",0,12,11,framebuffer);
   
   dump_to_screen(framebuffer);
   pause_a_while(20000);
 
   return 0;
}


int main(int argc, char **argv) {

   static struct termios new_tty,old_tty;
   
   char *framebuffer;

   
    int i,menu_item;
    char ch;
   
    framebuffer=calloc(SCREEN_WIDTH*SCREEN_HEIGHT*BYTES_PER_PIXEL,sizeof(char));
   
    /* setup non-blocking non-echo mode */
   
      tcgetattr(0,&old_tty);
   
      new_tty=old_tty;
      new_tty.c_lflag&=~ICANON;
      new_tty.c_cc[VMIN]=1;
      new_tty.c_lflag&=~ECHO;
      tcsetattr(0,TCSANOW,&new_tty);

      fcntl(0,F_SETFL,fcntl(0,F_GETFL) | O_NONBLOCK);


    /* clear screen */
    printf("\033[2J\n");
      
    clear_framebuffer(framebuffer,0,' ');
   
    blit(vmw_sprite,framebuffer,219,8,6,23,8);
    put_text_xy("A VMW Software Production",7,15,15,framebuffer);
    dump_to_screen(framebuffer);
    pause_a_while(3000);

   
 opening_screen:  

    clear_framebuffer(framebuffer,0,' ');
    blit(opener_sprite,framebuffer,219,0,0,40,20);
    put_text_xy("Merciless Marauding  Malicious Marketers",0,5,14,framebuffer); 

    dump_to_screen(framebuffer);
   
    pause_a_while(10000);

   
    menu_item=0;

    put_attention_block(framebuffer,0);
      
    while(1) {	
       put_text_xy("New Game",15,6,(menu_item==0)?13:5,framebuffer); 
       put_text_xy("About",15,7,(menu_item==1)?13:5,framebuffer); 
       put_text_xy("Story",15,8,(menu_item==2)?13:5,framebuffer); 
       put_text_xy("Hi Score",15,9,(menu_item==3)?13:5,framebuffer); 
       put_text_xy("Quit",15,10,(menu_item==4)?13:5,framebuffer); 
       put_text_xy("F1 or 'h' for HELP",0,20,7,framebuffer);   

       for(i=0;i<5;i++)
	  if (i==menu_item) put_text_xy("-->",12,6+menu_item,14,framebuffer);
          else put_text_xy("   ",12,6+i,0,framebuffer);
       dump_to_screen(framebuffer);
       
       while( !(ch=get_a_char()) ) usleep(30);
       
       switch(ch) {
	case 'q': case 'Q': menu_item=4; ch='\n'; break;
	
	case 'i': case 'j': menu_item--; 
	          if (menu_item<0) menu_item=4; 
	          break;
	case 'm': case 'k': menu_item++;
	          if (menu_item>4) menu_item=0; break;
	case 'h': menu_item=5; ch='\n'; break;
	
	 
	case 10: case 13: case 32: break;
	
	  
	    
       }
       
       if ((ch==10) || (ch==13) || (ch==32)) {
	  switch(menu_item) {
	     case 0: new_game(framebuffer); break;
	     case 1: about(framebuffer); break;
	     case 2: story(framebuffer); break;
	     case 3: hi_score(framebuffer); 
	             while(get_char()==0) usleep(30);
	             break;
	     case 4: if (verify_quit(framebuffer)) goto done_the_game; break;
	     case 5: help(framebuffer); break;
	  }
	  goto opening_screen;
       }
          
    }
   
   
   
   

 done_the_game:   
     printf("%c\n",15);
     set_sound(750,125); /* The default.  No way I know of to save/restore */
     tcsetattr(0,TCSANOW,&old_tty);   

    return 0;
}
