

INCLUDE= -Wall -I/usr/local/include/SDL -I/usr/local/include
LIBS= -lSDL -lSDL_mixer -L/usr/X11R6/lib -lX11 -lpthread

all:	tb1

tb1:	tb1.o sdl_svmwgraph.o vmw_sprite.o tblib.o level_1.o level_2.o sound.o
	gcc -o tb1 tb1.o sdl_svmwgraph.o vmw_sprite.o tblib.o level_1.o level_2.o sound.o $(LIBS)
	
tb1.o:	tb1.c
	gcc -c tb1.c $(INCLUDE)

tblib.o:	tblib.c
	gcc -c tblib.c $(INCLUDE)

sdl_svmwgraph.o:	sdl_svmwgraph.c
	gcc -c sdl_svmwgraph.c $(INCLUDE)

vmw_sprite.o:	vmw_sprite.c
	gcc -c vmw_sprite.c $(INCLUDE)

level_1.o:	level_1.c
	gcc -c level_1.c $(INCLUDE)
	
level_2.o:	level_2.c
	gcc -c level_2.c $(INCLUDE)
	
sound.o:	sound.c
	gcc -c sound.c $(INCLUDE)

clean:
	rm tb1 *.o *~
