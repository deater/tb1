C_FLAGS=-Wall -O2 -g

all:	      level3

level3:	level3.o
	gcc $(L_FLAGS) -o level3 level3.o

level3.o:   level3.c game_sprites.h
	gcc $(C_FLAGS) -c level3.c
	
clean:	
	rm -f *~ *.o level3 core
