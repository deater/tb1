#include <stdio.h>
#include <time.h>

/* default beep is 750Hz/125ms */


/* The following algorithm from the Hewlett Packard HP-20S */
/* Scientific Calculator manual, September 1998, p75-76    */

/* The algorithm  r     = FractionalPart (997r )           */
/*                 i+1                        i            */
/*                                                         */
/* Where r0 is a starting value between 0 and 1.           */
/* The generator passes the chi-square frequency test for  */
/* uniformity, and serial and run tests for randomness.    */
/* The more significant digits are more random than the    */
/* less significant.  If r0 * 10e7 is not divisible        */
/* by 2 or 5 you get 500,000 numbers w/o repeating.        */

/* modified to be fixed-point integer only by Vince Weaver */

int int_random(int seed) {
   
   static int ri;
   
      /* Be sure doesn't end in multiple of 2, 5 and is ~<=7 digits */
   if (seed!=0) {
      ri=seed;
      while ((ri%2==0) || (ri%5==0)) ri++;
      ri=ri&0xffffff;
      return ri;
   }
      
   ri*=997;
   ri&=0xffffff;
   
   return ri;
}

int random_word() {
   int i;
   
   i=(int_random(0)>>8)&0xffff;
   return i;
}

int main(int argc,char **argv) {
   
   int i;
   
   int_random(time(NULL));
   
   for(i=0;i<1000;i++) printf("%i\n",random_word()%1000);
   
   return 0;
}
