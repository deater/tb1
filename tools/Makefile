

INCLUDE= -O2 -Wall -I/usr/local/include/SDL -I/usr/local/include -I..
LIBS= -lSDL -L/usr/X11R6/lib -lX11 -lpthread

all:	ppro_view

ppro_view:	ppro_view.o ../svmwgraph/libsvmwgraph.a
	gcc -o ppro_view ppro_view.o ../svmwgraph/libsvmwgraph.a $(LIBS)

../svmwgraph/libsvmwgraph.a:	
	cd ../svmwgraph && make

ppro_view.o:	ppro_view.c
	gcc -c ppro_view.c $(INCLUDE)

clean:
	rm -f ppro_view *.o *~
