

INCLUDE= -Wall -O2 -I/usr/local/include/SDL
LIBS= -lSDL -lSDL_mixer -L/usr/X11R6/lib -lX11 -lpthread

all:	libsvmwgraph.a
	
libsvmwgraph.a:	vmw_core.o vmw_flip.o vmw_font.o vmw_paintpro.o vmw_palette.o vmw_setup.o vmw_sprite.o sdl_svmwgraph.o
	ar cru libsvmwgraph.a vmw_core.o vmw_flip.o vmw_font.o vmw_paintpro.o vmw_palette.o vmw_setup.o vmw_sprite.o sdl_svmwgraph.o
	ranlib libsvmwgraph.a

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

vmw_setup.o:	vmw_setup.c
	gcc $(INCLUDE) -c vmw_setup.c
	
vmw_sprite.o:	vmw_sprite.c
	gcc $(INCLUDE) -c vmw_sprite.c

sdl_svmwgraph.o:	sdl_svmwgraph.c
	gcc $(INCLUDE) -c sdl_svmwgraph.c

clean:
	rm -f libsvmwgraph.a *.o *~
