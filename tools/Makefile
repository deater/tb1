

INCLUDE= -O2 -Wall -I/usr/local/include/SDL -I/usr/local/include -I..
LIBS= -lSDL -L/usr/X11R6/lib -lX11 -lpthread -lncurses

all:	ppro_view pcx2ppp

ppro_view:	ppro_view.o ../svmwgraph/libsvmwgraph.a
	gcc -o ppro_view ppro_view.o ../svmwgraph/libsvmwgraph.a $(LIBS)

pcx2ppp:	pcx2ppp.o ../svmwgraph/libsvmwgraph.a
	gcc -o pcx2ppp pcx2ppp.o ../svmwgraph/libsvmwgraph.a $(LIBS)

../svmwgraph/libsvmwgraph.a:	
	cd ../svmwgraph && make

pcx2ppp.o:	pcx2ppp.c
	gcc -c pcx2ppp.c $(INCLUDE)

ppro_view.o:	ppro_view.c
	gcc -c ppro_view.c $(INCLUDE)

clean:
	rm -f ppro_view pcx2ppp *.o *~
