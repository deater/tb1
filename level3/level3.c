#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <time.h>
#include <sys/time.h>

#define SCREEN_WIDTH   24
#define SCREEN_HEIGHT  20
#define BYTES_PER_PIXEL 2
#include "game_sprites.h"



int dump_screen=0;


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

int map_ch;

void skip_line(FILE *fff) {
   
   map_ch=fgetc(fff);
   
   while((map_ch!='\n') && (map_ch!=-1)) map_ch=fgetc(fff);

}

int get_int(FILE *fff) {
   
   int result=0;
   
   map_ch=fgetc(fff);
   
   while(map_ch!=-1) {
    
      while ((map_ch<=' ')&&(map_ch!=-1)) map_ch=fgetc(fff);
      if (map_ch==';') skip_line(fff);
      
      if ((map_ch>='0') && (map_ch<='9')) {
         while ((map_ch>='0') && (map_ch<='9')) {
	    result=result*10;
	    result+=map_ch-'0';
	    map_ch=fgetc(fff);
	 }
	 return result;
      }
   }   
   return -1;
}

	
   
void panic_oom(char *string) {
   printf("Out of Memory: %s\n",string);
   exit(1);
}

   


int new_game(char *framebuffer) {

    int quit=0,game_paused=0,sound=1,done_waiting=0;
    int ch;
   
    int time_spent,oldusec=0,oldsec=0;
    struct timeval timing_info;

    
    struct tom_struct {
       int x,y,xadd,yadd;
       int roomx,roomy,roomz;
    } tom;

    struct enemies_struct {
       int x,y,xadd,yadd;
       char type;
    } enemies[2];
    
    int total_enemies=0;	
   

   
    int x,y,z,i,j,k,tempx,tempy;
    int roomx,roomy,roomz,offset;
    char *current_room;
    FILE *level_file;
   
    struct map_type {
       int xsize,ysize,zsize;
       int roomx,roomy;
       char **rooms;
    } map;
   

       tom.xadd=0; tom.yadd=0;
   
    /* Load Level */
   
    level_file=fopen("level3.map","r");
    if (level_file==NULL) {
       printf("File not found!\n");
       exit(1);
    }


    /* Get header info */
    map.xsize=get_int(level_file);
    map.ysize=get_int(level_file);
    map.zsize=get_int(level_file);
    map.roomx=get_int(level_file);
    map.roomy=get_int(level_file);
   
    printf("Found map of %i,%i,%i\n",map.xsize,map.ysize,map.zsize);
    printf("Roomsize of %ix%i\n",map.roomx,map.roomy);
    
    map.rooms=calloc(map.xsize*map.ysize*map.zsize,sizeof(char *));
    if (map.rooms==NULL) panic_oom("map.rooms");
   
    for(i=0;i<map.xsize;i++) {
       for(j=0;j<map.ysize;j++) {
	  for(k=0;k<map.zsize;k++) {
	     map.rooms[i+(j*map.xsize)+(k*map.xsize*map.ysize)]
	       =calloc(1,sizeof(char)*map.roomx*map.roomy);
	     if (map.rooms[i+(j*map.xsize)+(k*map.xsize*map.ysize)]==NULL)
	        panic_oom("map.rooms2");
	  }
       }
    }
   /*
                f f f
	      z z z f  
	    x x x z f
            x x x z
            x x x
*/	
       
    while(map_ch!=-1) {          
       
       roomx=get_int(level_file);
       roomy=get_int(level_file);
       roomz=get_int(level_file);
//       skip_line(level_file);
       printf("Loading room %i,%i,%i\n",roomx,roomy,roomz);
       if ((roomx>map.xsize) || (roomy>map.ysize) || (roomz>map.zsize)) {
          printf("Room horribly out of bounds!\n");
	  exit(1);
       }
       
       offset=roomx+(roomy*map.xsize)+(roomz*map.xsize*map.ysize);
       printf("Offset=%i\n",offset);

       current_room=map.rooms[offset];

       x=0;y=0;
       
       for(i=0;i<map.roomx;i++) {
	  for(j=0;j<map.roomy;j++) {
	     current_room[j*map.roomx+i]=' ';
	  }	  
       }

       while(map_ch<' ') map_ch=fgetc(level_file); // skip last '\n'  
       if (map_ch=='\n') map_ch=fgetc(level_file); // skip last '\n'  
//       printf("Next=%i %c\n",map_ch,map_ch);

       while(y<map.roomy) {

//	  printf("%c",map_ch);
          switch(map_ch) {
	     case 'T':  tom.x=x;  tom.y=y;
	                tom.roomx=roomx;
	                tom.roomy=roomy;
	                tom.roomz=roomz;
	                printf("Found Tom at %i,%i,%i\n",roomx,roomy,roomz);
	                map_ch=fgetc(level_file);
	   	        x++;
	                break;
	     case '#': current_room[y*map.roomx+x]='#';
	               map_ch=fgetc(level_file);
	               x++;
	               break;
	     case ' ': 
	               map_ch=fgetc(level_file);
	               x++;
	               break;
	     case '/': current_room[y*map.roomx+x]='/';
	               map_ch=fgetc(level_file);
	               x++;
	               break;  
	  
	     case 'A': case 'G': case 'S':  
	               map_ch=fgetc(level_file);
	               x++;
	               break;
	     case 'w': 
	               map_ch=fgetc(level_file);
/*	          enemies[total_enemies].x=x;
	          enemies[total_enemies].y=y;
	          enemies[total_enemies].xadd=0;
	          enemies[total_enemies].yadd=1;
	          enemies[total_enemies].type='*';
	          total_enemies++;
*/	  
	          x++;
	          break;	  

	     case '\n': y++; x=0; map_ch=fgetc(level_file);break;
	          
	     
	     default:
	          printf("Unknown char '%c' (%i)\n",map_ch,map_ch);
	          map_ch=fgetc(level_file);
	  }
	  
       
          if (x>=map.roomx) {	  
	     while(map_ch!='\n') map_ch=fgetc(level_file);
	     

	     map_ch=fgetc(level_file);
//	     printf("SKIP LINE, ch=%i\n",map_ch);
	     y++;
	     x=0;
	  }
	  
       }
       
       
    }
   
   
    clear_keyboard_buffer();
    clear_framebuffer(framebuffer,0x0,' ');

          for(i=0;i<map.roomx;i++) {
	  for(j=0;j<map.roomy;j++) {
	     printf("%c",current_room[i+j*map.roomx]);  
	  }
	     printf("\n");
	  }
   

       /* RANDOMIZE TIMER */
    srand(time(NULL));

    offset=tom.roomx+(tom.roomy*map.xsize)+(tom.roomz*map.xsize*map.ysize);
    printf("Tom starting in room offset %i\n",offset);
    current_room=map.rooms[offset];

   
    while(!quit) {
       
       clear_framebuffer(framebuffer,0x0,' ');
              
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
	   case 'j': if (tom.x>0) tom.xadd=-1; 
	             else {
	      	        tom.xadd=0;
			tom.x=(map.roomx-1);
			tom.roomx--;
			offset=tom.roomx+(tom.roomy*map.xsize)+(tom.roomz*map.xsize*map.ysize);
 //   printf("Tom starting in room offset %i\n",offset);
                        current_room=map.rooms[offset];
	             }
	             break;
	   case 'k': if (tom.x<SCREEN_WIDTH-1) tom.xadd=1; 
	             else {
			tom.xadd=0;
			tom.x=0;
			tom.roomx++;
			offset=tom.roomx+(tom.roomy*map.xsize)+(tom.roomz*map.xsize*map.ysize);
 //   printf("Tom starting in room offset %i\n",offset);
                        current_room=map.rooms[offset];
		     }
	     
	             break;
	     
	   case 'i': if (tom.y>0) tom.yadd=-1;
	             else {
			tom.yadd=0;
			tom.y=(map.roomy-1);
			tom.roomy--;
			offset=tom.roomx+(tom.roomy*map.xsize)+(tom.roomz*map.xsize*map.ysize);
 //   printf("Tom starting in room offset %i\n",offset);
                        current_room=map.rooms[offset];
			
		     }
	     
	             break;
	   case 'm': if (tom.y<SCREEN_HEIGHT-1) tom.yadd=1;
	             else {
			tom.yadd=0;
			tom.y=0;
			tom.roomy++;
			offset=tom.roomx+(tom.roomy*map.xsize)+(tom.roomz*map.xsize*map.ysize);
 //   printf("Tom starting in room offset %i\n",offset);
                        current_room=map.rooms[offset];
		     }
	     
	             break;
	     
	   case 'p': 
	           put_attention_block(framebuffer,14);
	           put_text_xy(" GAME PAUSED! ",13,8,12,framebuffer);
	           dump_to_screen(framebuffer);
	      
	           while (get_char()==0) usleep(5000); 
	           game_paused=1; 
	           break;
	   case 's': sound=!sound; /*level++;*/ break;
	   case 'h': help(framebuffer); /*game_paused=1;*/ break;
	   case ' ': 
	     
	             break;
	  }	  
       }

       
       /* Collision Detect Tom */
       tempx=tom.x+tom.xadd;
       tempy=tom.y+tom.yadd;
       
       if (current_room[tempx+tempy*map.roomx]==' ') {
	  tom.x=tempx;
	  tom.y=tempy;
       }
       
       tom.xadd=0; tom.yadd=0;
       
       /* Collision Detect Enemies */
       
       for(i=0;i<total_enemies;i++) {
	    
          tempx=enemies[i].x+enemies[i].xadd;
	  if (current_room[tempx+enemies[i].y*map.roomx]==' ') {
	     enemies[i].x=tempx;
	  }
	  else enemies[i].xadd= -enemies[i].xadd;
	  
	  tempy=enemies[i].y+enemies[i].yadd;
	  if (current_room[enemies[i].x+tempy*map.roomx]==' ') {
	     enemies[i].y=tempy;
	  }
	  else enemies[i].yadd= -enemies[i].yadd;
       }
       
       
       
       /* draw level */
       for(i=0;i<map.roomx;i++) {
	  for(j=0;j<map.roomy;j++) {
	     switch(current_room[i+j*map.roomx]) {
	      case '#':       blit( (char *)wall,framebuffer,'#',
	                           i,j,1,1);
		              break;
	      case '/':       blit( (char *)door,framebuffer,'/',
	                           i,j,1,1);
		              break;
	     }
	     
	  }
	  
       }
              
       
       /* draw items */
       
       /* draw enemies */
       
       for(i=0;i<total_enemies;i++) {
	         
          blit( (char *)wisp,framebuffer,enemies[i].type,
	    enemies[i].x,enemies[i].y,1,1);
       }
       
	    
       
       /* draw tom */
       
       blit( (char *)tom_sprite,framebuffer,'T',
	    tom.x,tom.y,1,1);
       
       
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
	  
    clear_keyboard_buffer();
	  
    clear_framebuffer(framebuffer,0,' ');

     return 0;
}




int main(int argc, char **argv) {

   static struct termios new_tty,old_tty;
   
   char *framebuffer;

   
   
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
    
   
    new_game(framebuffer);
   

// done_the_game:   
     printf("%c\n",15);
     tcsetattr(0,TCSANOW,&old_tty);   
   printf("%c[0m%c[255D\n",27,27);
    return 0;
}
