#include <stdio.h>

/* Converts SMIX (ethan brodsky) sound samples back to standard
 * raw audio format
 */


int main(int argc,char **argv)
{
FILE *fff;
int ch;

   fff=fopen(argv[1],"r");
   if(fff==NULL) {
      printf("\n\nUsage '%s filename'\n\n",argv[0]);
      return 1;  
   }
   while(!feof(fff)) {  
      ch=fgetc(fff);
      printf("%c",(ch+128));
   }
   fclose(fff);
   
}

