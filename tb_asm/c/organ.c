#include <stdio.h>
#include <math.h>  /* pow */
#include <fcntl.h> /* fcntl */
#include <termios.h>



int main(int argc, char **argv) {
   
   int frequency[127],i,ch,note=60;

      static struct termios new_tty,old_tty;
   
      
       /* setup non-blocking non-echo mode */
   
         tcgetattr(0,&old_tty);
   
         new_tty=old_tty;
         new_tty.c_lflag&=~ICANON;
         new_tty.c_cc[VMIN]=1;
         new_tty.c_lflag&=~ECHO;
         tcsetattr(0,TCSANOW,&new_tty);
   
         fcntl(0,F_SETFL,fcntl(0,F_GETFL) | O_NONBLOCK);
   
   
   
   for (i=0;i<127;i++) frequency[i]=(440.0/32.0)*(pow(2.0,(i-9.0)/12.0));
   
   for(i=0;i<127;i++) printf("%i\n",frequency[i]);

   while ((ch=getchar())!='q') {
	if (ch==-1) goto loop;
	switch(ch)
	  {
	     
             case 'a': note=60; break;
	     case 'w': note=61; break;
	     case 's': note=62; break;
	     case 'e': note=63; break;
	     case 'd': note=64; break;
	     case 'f': note=65; break;
	     case 't': note=66; break;
	     case 'g': note=67; break;
	     case 'y': note=68; break;
	     case 'h': note=69; break;
	     case 'u': note=70; break;
	     case 'j': note=71; break;
	     case 'k': note=72; break;
	     
	  }
	
   
	
	printf("%c[10;%i]\n",27,frequency[note]);
//      write(1, "\33[11;200]", 9) length = 200
	printf("%c",ch);
	printf("\a");
      	usleep(150000);
      loop:

	
     }

        tcsetattr(0,TCSANOW,&old_tty);
   
   
   
   
}
