include ../Makefile.inc

INCLUDE= $(INCLUDE_GLOBAL) -I../svmwgraph
LIBS= $(LIBS_GLOBAL)

all:	ppro_view pcx2ppp level2_editor

level2_editor:	level2_editor.o ../svmwgraph/libsvmwgraph.a
	gcc -o level2_editor level2_editor.o ../svmwgraph/libsvmwgraph.a $(LIBS)
	
benchmark:	benchmark.o ../svmwgraph/libsvmwgraph.a
	gcc -o benchmark benchmark.o ../svmwgraph/libsvmwgraph.a $(LIBS)

ppro_view:	ppro_view.o ../svmwgraph/libsvmwgraph.a
	gcc -o ppro_view ppro_view.o ../svmwgraph/libsvmwgraph.a $(LIBS)

pcx2ppp:	pcx2ppp.o ../svmwgraph/libsvmwgraph.a
	gcc -o pcx2ppp pcx2ppp.o ../svmwgraph/libsvmwgraph.a $(LIBS)

../svmwgraph/libsvmwgraph.a:	
	cd ../svmwgraph && make

level2_editor.o:	level2_editor.c
	gcc -c level2_editor.c $(INCLUDE)

benchmark.o:	benchmark.c
	gcc -c benchmark.c $(INCLUDE)

pcx2ppp.o:	pcx2ppp.c
	gcc -c pcx2ppp.c $(INCLUDE)

ppro_view.o:	ppro_view.c
	gcc -c ppro_view.c $(INCLUDE)

clean:
	rm -f ppro_view pcx2ppp benchmark level2_editor *.o *~
