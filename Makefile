

INCLUDE= -O2 -Wall -I/usr/local/include/SDL -I/usr/local/include
LIBS= -lSDL -lSDL_mixer -L/usr/X11R6/lib -lX11 -lpthread

all:	tb1

tb1:	tb1.o ./svmwgraph/libsvmwgraph.a about.o credits.o help.o hiscore.o level_1.o level_2.o loadgame.o menu_tools.o options.o playgame.o quit.o savegame.o sidebar.o sound.o story.o tblib.o
	gcc -o tb1 tb1.o about.o credits.o help.o hiscore.o level_1.o level_2.o loadgame.o menu_tools.o options.o playgame.o quit.o savegame.o sidebar.o sound.o story.o tblib.o ./svmwgraph/libsvmwgraph.a $(LIBS)

tb1.o:	tb1.c
	gcc -c tb1.c $(INCLUDE)

about.o:	about.c
	gcc -c about.c $(INCLUDE)
	
credits.o:	credits.c
	gcc -c credits.c $(INCLUDE)

help.o:	help.c
	gcc -c help.c $(INCLUDE)

hiscore.o:	hiscore.c
	gcc -c hiscore.c $(INCLUDE)

level_1.o:	level_1.c
	gcc -c level_1.c $(INCLUDE)
	
level_2.o:	level_2.c
	gcc -c level_2.c $(INCLUDE)

loadgame.o:	loadgame.c
	gcc -c loadgame.c $(INCLUDE)

menu_tools.o:	menu_tools.c
	gcc -c menu_tools.c $(INCLUDE)

options.o:	options.c
	gcc -c options.c $(INCLUDE)

playgame.o:	playgame.c
	gcc -c playgame.c $(INCLUDE)

quit.o:	quit.c
	gcc -c quit.c $(INCLUDE)

savegame.o:	savegame.c
	gcc -c savegame.c $(INCLUDE)

sidebar.o:	sidebar.c
	gcc -c sidebar.c $(INCLUDE)

sound.o:	sound.c
	gcc -c sound.c $(INCLUDE)

story.o:	story.c
	gcc -c story.c $(INCLUDE)

tblib.o:	tblib.c
	gcc -c tblib.c $(INCLUDE)

clean:
	rm tb1 *.o *~
