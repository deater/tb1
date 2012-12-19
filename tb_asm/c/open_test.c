#include <stdio.h>

int main(int argc, char **argv) {
   FILE *fff;
   
   fff=fopen("bob","w");
   if (fff!=NULL) {
      fprintf(fff,"HELLO\n");
      fclose(fff);
   }
   return 0;
}
