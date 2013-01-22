#include <stdio.h>

int main(int argc, char **argv) {

   int byte,offset=0;
   unsigned int total=0;
   unsigned short checksum;
   int debug=0;
   
   while(1) {
      byte=fgetc(stdin);
      if (byte<0) break;
	
      if (debug) {
         if (offset==0xffdc) printf("%x\n",byte);
         if (offset==0xffdd) printf("%x\n",byte);
         if (offset==0xffde) printf("%x\n",byte);
         if (offset==0xffdf) printf("%x\n",byte);      
      }
      
      total+=byte;
      offset++;

   }
      
   checksum=total&0xffff;
   
   printf("\t.word\t$%hx\t\t; Complement of checksum\n",~checksum);
   printf("\t.word\t$%hx\t\t; Unsigned 16-bit sum of ROM\n",checksum);
     
   return 0;
   
   
}

   
