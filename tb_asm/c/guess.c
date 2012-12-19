
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv) {
    int result,guess,secret,tries;
   
    srand(time(NULL));

new_game:
    secret=rand()%65536;   
    tries=0;
    printf("Guess a number between 0 and 65536!\n");
    printf("  Or -1 to quit\n");
guess_again:   
    result=scanf("%d",&guess);
    if (result<1) {
       scanf("%*s");
       goto guess_again;
    }
    if (guess==-1) goto end;
    tries++;
    if (guess<secret) {
       printf("%i is too low!  Try again!\n",guess);
       goto guess_again;
    }
    if (guess>secret) {
       printf("%i is too high!  Try again!\n",guess);
       goto guess_again;
    }
   
    if (guess==secret) {
       printf("Correct!  It took you %i tries!\n",tries);
       goto new_game;
    }
   
end:    
    printf("Thanks for playing!\n\n"); 
    return 0;
}
