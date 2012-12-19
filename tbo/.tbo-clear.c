
/* todo, make look like game */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> /* usleep */
#include <termios.h>
#include <fcntl.h>

char ch;
static int ri;

   /* pseudo-random generator */
int int_random(int seed) {
               
   if (seed!=0) {
      ri=seed;
      while ((ri%2==0) || (ri%5==0)) ri++;
      ri=ri&0xffffff;
      return ri;
   }
   
   
   ri*=997;
   ri&=0xffffff;
   
   return ri>>4;
}

   /* clear the screen */
void cls() {
      printf("%c[2J%c[H\n",27,27);
}

   

  /* display help */
void help() {

   cls();
   
   printf("HELP\n");
   printf("~~~~\n");
   printf(", moves left\n");
   printf(". moves right\n");
   printf("spacebar shoots\n");
   printf("s toggles sound\n");
   printf("p pauses\n");
   printf("q quits\n");
   while(read(0,&ch,1)<0);
}




int main(int argc, char **argv) {

   FILE *debug;
   char j,x=7,keypressed;
   int i,s=0,sh=5,ra,wait,frames=0,shots=0,sound=1,xb=0,done=0;   
   short missile[8],enemy[8],stars[8],mask;

   for(i=0;i<8;i++) enemy[i]=missile[i]=stars[i]=0;
   
   static struct termios new_tty,old_tty;
   
   debug=fopen("bob","w");
   
      /* Setup Term */
   tcgetattr(0,&old_tty);   
   new_tty=old_tty;
   new_tty.c_lflag&=~ICANON;
   new_tty.c_cc[VMIN]=1;
   new_tty.c_lflag&=~ECHO;
   tcsetattr(0,TCSANOW,&new_tty);   
   fcntl(0,F_SETFL,fcntl(0,F_GETFL) | O_NONBLOCK);

   cls();

     /* do the opening */
   i=8;
   while(i>0) {	
   cls();
      for(j=0;j<i;j++) printf("\n");
      if(j<7) printf("   TOM BOMBEM\n");
      if(j<6) printf("   OBFUSCATED\n");
      if(j<5) printf("       by\n");
      if(j<4) printf("  vmw5@cornell\n");
      if(j<3) printf("\n");
      if (j<2) printf("   h for help\n");
      i--;
      usleep(300000);
     }
   
    while(read(0,&ch,1)<0);   
    if (ch=='h') help(); 
    ch=0;
   
   
   
          
   wait=(int_random(255)&0xf);
   
   while(!done) {
      frames++;
      ra=int_random(0);

         /* If wait time up, put a new enemy */
      if (!wait) {
         enemy[0]|=1<<( (int_random(0)&0xf));	   
         wait=(int_random(0)&0xf);
	 /* Go a bit faster the more points you get */
	 wait-=(s/15);
	 if (wait<1) wait=1;
      }
      else wait--;
      
         /* check the keyboard */
      keypressed=read(0,&ch,1);
      if (keypressed) {
	 if (ch=='q') done=1;
         if (ch==',') x--; 
         if (ch=='.') x++; 
	 if (ch=='h') help();
	 if (ch=='s') sound=!sound;
	 if (ch=='p') {
	    while(read(0,&ch,1)<0);
	    ch=0;
	 }	 
	 if (ch==' ') { 
	    missile[7]|=(1<<(x+1));
	    shots++;
	    ri+=frames;
	 }
	    /* Handle arrow keys */
	 if (ch=='\033') {
	    read(0,&ch,1);
	    if (ch=='[') 
	      {
		 read(0,&ch,1);
		 if (ch=='D') x--;
		 if (ch=='C') x++;
	      }
	 }
	 if (x<1) x=1;
	 if (x>13) x=13;
	      
	 ch=0;
      }

         /* draw the action */
      cls();
      for(i=0;i<8;i++) {
	 for(j=0;j<16;j++) {
	    mask=1<<j;
	    if ((missile[i]&mask) && (enemy[i]&mask)) 
	      {
		 printf("*");
		 missile[i]&=~mask;
		 enemy[i]&=~mask;
		 s++;
		 if ((s&31)==0) sh++;
	      }
	     else
	    
	    if (missile[i]&mask) printf("!");
	    else
	    if (enemy[i]&mask) printf("T");
	    else if (stars[i]&mask) printf(".");
	    else printf(" ");
	 }
         printf("\n");  
      }
      printf("%c[%iC/#\\\n",27,x);
      printf("SCORE:   %i\n",s);
      printf("SHIELDS: %i\n",sh);

 
         /* collision detect with ship */
      mask=0x7<<(x);
      if (enemy[7]&mask) {
	 sh--;
	 if (sh<0) {
	    done=1;
	 }
	 if (sh==0) {
	    xb=5;
	 }
	   
	 if (sound) printf("%c",7);
	 enemy[7]&=~mask;
      }
      
   
         /* Scroll the stars */
      if ((frames&0x7)==1) {
	  for(i=7;i>0;i--) stars[i]=stars[i-1];
	   stars[0]=1<<(int_random(0)&0xf);
	}
      
         /* scroll the missiles */
      if (frames&1) {  
         for(i=0;i<7;i++) missile[i]=missile[i+1];   
            missile[7]=0;
	 }
      
         /* scroll the enemies */
      if ((frames&0x3)==2) {     
         for(i=7;i>0;i--) enemy[i]=enemy[i-1];   
            enemy[0]=0;      
      }

         /* delay */
      usleep(33000);
      
         /* extra beep */
      if (xb) {
         xb--;
         if (xb==0) 
         if (sound) printf("%c\n",7);
    
      }
      
      
   }
   
      /* end screen */
    cls();
    printf("\n\n\nGAME OVER!\n");
    printf("SCORE: %i\n",s);
    if (!shots) printf("HIT RATIO: %i%%\n",
	   s*100/shots);
	   
   return 0;   
   
}

   
