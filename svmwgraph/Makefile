

INCLUDE= -Wall -O2
LIBS= -lSDL -lSDL_mixer -L/usr/X11R6/lib -lX11 -lpthread

all:	svmwgraph.a
	
svmwgraph.a:	vmw_core.o vmw_flip.o vmw_font.o vmw_paintpro.o vmw_palette.o vmw_sprite.o
	echo "hello"

vmw_core.o:	vmw_core.c
	gcc $(INCLUDE) -c vmw_core.c
	
vmw_flip.o:	vmw_flip.c
	gcc $(INCLUDE) -c vmw_flip.c

vmw_font.o:	vmw_font.c
	gcc $(INCLUDE) -c vmw_font.c

vmw_paintpro.o:	vmw_paintpro.c
	gcc $(INCLUDE) -c vmw_paintpro.c

vmw_palette.o:	vmw_palette.c
	gcc $(INCLUDE) -c vmw_palette.c

vmw_sprite.o:	vmw_sprite.c
	gcc $(INCLUDE) -c vmw_sprite.c
	
clean:
	rm -f svmwgraph.a *.o *~
