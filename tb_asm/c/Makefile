C_FLAGS=-Wall -O2 -g

all:	      tb_asm

tb_asm:	tb_asm.o
	gcc $(L_FLAGS) -o tb_asm tb_asm.o

tb_asm.o:   tb_asm.c game_sprites.h
	gcc $(C_FLAGS) -c tb_asm.c
	
clean:	
	rm -f *~ *.o tb_asm core
