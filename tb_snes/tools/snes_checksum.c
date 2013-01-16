#include <stdio.h>

int main(int argc, char **argv) {

   int byte;
   unsigned int total=0;
   unsigned short checksum;
   
   while(1) {
      byte=fgetc(stdin);
      if (byte<0) break;
	
      total+=byte;

   }
      
   checksum=total&0xffff;
   
   printf("Checksum: %hx\n",checksum);
   printf("Complement: %hx\n",~checksum);
   
   return 0;
   
   
}

   