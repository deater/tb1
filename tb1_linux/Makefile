include Makefile.inc

INCLUDE= $(INCLUDE_GLOBAL) -g
LIBS= $(LIBS_GLOBAL)

all:	tb1

tb1:	tb1.o ./svmwgraph/libsvmwgraph.a about.o credits.o graphic_tools.o \
		help.o hiscore.o level_1.o level2_engine.o level_3.o \
		loadsave.o options.o playgame.o quit.o sidebar.o sound.o\
		story.o tblib.o
	$(CC) -o tb1 tb1.o about.o credits.o graphic_tools.o help.o hiscore.o\
		level_1.o level2_engine.o level_3.o loadsave.o options.o\
		playgame.o quit.o sidebar.o sound.o story.o tblib.o\
		./svmwgraph/libsvmwgraph.a $(LIBS)

./svmwgraph/libsvmwgraph.a:	
	cd svmwgraph && make

tb1.o:	tb1.c
	$(CC) $(CFLAGS) -c tb1.c $(INCLUDE)

about.o:	about.c
	$(CC) $(CFLAGS) -c about.c $(INCLUDE)

credits.o:	credits.c
	$(CC) $(CFLAGS) -c credits.c $(INCLUDE)

graphic_tools.o:	graphic_tools.c
	$(CC) $(CFLAGS) -c graphic_tools.c $(INCLUDE)

help.o:	help.c
	$(CC) $(CFLAGS) -c help.c $(INCLUDE)

hiscore.o:	hiscore.c
	$(CC) $(CFLAGS) -c hiscore.c $(INCLUDE)

level_1.o:	level_1.c
	$(CC) $(CFLAGS) -c level_1.c $(INCLUDE)

level2_engine.o:	level2_engine.c
	$(CC) $(CFLAGS) -c level2_engine.c $(INCLUDE)

#level_2.o:	level_2.c
#	$(CC) -c level_2.c $(INCLUDE)

level_3.o:	level_3.c
	$(CC) $(CFLAGS) -c level_3.c $(INCLUDE)

loadsave.o:	loadsave.c
	$(CC) $(CFLAGS) -c loadsave.c $(INCLUDE)

options.o:	options.c
	$(CC) $(CFLAGS) -c options.c $(INCLUDE)

playgame.o:	playgame.c
	$(CC) $(CFLAGS) -c playgame.c $(INCLUDE)

quit.o:	quit.c
	$(CC) $(CFLAGS) -c quit.c $(INCLUDE)

sidebar.o:	sidebar.c
	$(CC) $(CFLAGS) -c sidebar.c $(INCLUDE)

sound.o:	sound.c
	$(CC) $(CFLAGS) -c sound.c $(INCLUDE) $(SDL_MIXER_FLAGS)

story.o:	story.c
	$(CC) $(CFLAGS) -c story.c $(INCLUDE)

tblib.o:	tblib.c
	$(CC) $(CFLAGS) -c tblib.c $(INCLUDE)

clean:
	rm -f tb1 *.o *~
	cd svmwgraph && make clean
	cd tools && make clean
