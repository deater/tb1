include Makefile.inc

INCLUDE= $(INCLUDE_GLOBAL)
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
	$(CC) -c tb1.c $(INCLUDE)

about.o:	about.c
	$(CC) -c about.c $(INCLUDE)
	
credits.o:	credits.c
	$(CC) -c credits.c $(INCLUDE)

graphic_tools.o:	graphic_tools.c
	$(CC) -c graphic_tools.c $(INCLUDE)

help.o:	help.c
	$(CC) -c help.c $(INCLUDE)

hiscore.o:	hiscore.c
	$(CC) -c hiscore.c $(INCLUDE)

level_1.o:	level_1.c
	$(CC) -c level_1.c $(INCLUDE)

level2_engine.o:	level2_engine.c
	$(CC) -c level2_engine.c $(INCLUDE)

#level_2.o:	level_2.c
#	$(CC) -c level_2.c $(INCLUDE)

level_3.o:	level_3.c
	$(CC) -c level_3.c $(INCLUDE)

loadsave.o:	loadsave.c
	$(CC) -c loadsave.c $(INCLUDE)

options.o:	options.c
	$(CC) -c options.c $(INCLUDE)

playgame.o:	playgame.c
	$(CC) -c playgame.c $(INCLUDE)

quit.o:	quit.c
	$(CC) -c quit.c $(INCLUDE)

sidebar.o:	sidebar.c
	$(CC) -c sidebar.c $(INCLUDE)

sound.o:	sound.c
	$(CC) -c sound.c $(INCLUDE) $(SDL_MIXER_FLAGS)

story.o:	story.c
	$(CC) -c story.c $(INCLUDE)

tblib.o:	tblib.c
	$(CC) -c tblib.c $(INCLUDE)

clean:
	rm -f tb1 *.o *~
	cd svmwgraph && make clean
