#include <stdio.h>

int main(int argc, char **argv) {
 
   int ch,vt102mode=0,inAnsi=0;
   
   while(!feof(stdin)) {
      
      ch=getchar();
      
      if (ch<0) break;
      
      if (ch==27) inAnsi=1;
      if ((inAnsi) && (ch=='m')) inAnsi=0;
      if ((inAnsi) && (ch=='H')) inAnsi=0;
      
      if (ch==14) {
	 vt102mode=1;
	 goto skip_print;
      }
      
      if (ch==15) {
	 vt102mode=0;
	 goto skip_print;
      }
      
      if ((vt102mode)&&(ch=='0')&&(!inAnsi)) putchar('\333');      
      else putchar(ch);
      
skip_print:
   }
   
}
