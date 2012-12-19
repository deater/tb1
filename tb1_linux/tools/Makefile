include ../Makefile.inc

INCLUDE= $(INCLUDE_GLOBAL) -I../svmwgraph
LIBS= $(LIBS_GLOBAL)

all:	ppro_view pcx2ppp level2_editor pcx_slideshow

level2_editor:	level2_editor.o ../svmwgraph/libsvmwgraph.a
	$(CC) -o level2_editor level2_editor.o ../svmwgraph/libsvmwgraph.a $(LIBS)
	
benchmark:	benchmark.o ../svmwgraph/libsvmwgraph.a
	$(CC) -o benchmark benchmark.o ../svmwgraph/libsvmwgraph.a $(LIBS)

ppro_view:	ppro_view.o ../svmwgraph/libsvmwgraph.a
	$(CC) -o ppro_view ppro_view.o ../svmwgraph/libsvmwgraph.a $(LIBS)

pcx_slideshow:	 pcx_slideshow.o ../svmwgraph/libsvmwgraph.a
	$(CC) -o pcx_slideshow pcx_slideshow.o ../svmwgraph/libsvmwgraph.a $(LIBS)

pcx2ppp:	pcx2ppp.o ../svmwgraph/libsvmwgraph.a
	$(CC) -o pcx2ppp pcx2ppp.o ../svmwgraph/libsvmwgraph.a $(LIBS)

../svmwgraph/libsvmwgraph.a:	
	cd ../svmwgraph && make

level2_editor.o:	level2_editor.c
	$(CC) -c level2_editor.c $(INCLUDE)

benchmark.o:	benchmark.c
	$(CC) -c benchmark.c $(INCLUDE)

pcx2ppp.o:	pcx2ppp.c
	$(CC) -c pcx2ppp.c $(INCLUDE)

ppro_view.o:	ppro_view.c
	$(CC) -c ppro_view.c $(INCLUDE)

pcx_slideshow.o: pcx_slideshow.c
		 $(CC) -c pcx_slideshow.c $(INCLUDE)

clean:
	rm -f pcx_slideshow ppro_view pcx2ppp benchmark level2_editor *.o *~
