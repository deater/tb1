#
# Edit the below to fit your configuration.  I should automate this 
# (maybe with a configure file?) One of these days.


#
# Compiler to use
#
CC= gcc

#
# Uncomment the following for SDL_mixer sound
#

SDL_MIXER_FLAGS= -DSDL_MIXER_SOUND
SDL_MIXER_LIBS= -lSDL_mixer

#
# Uncomment the following for SDL
#
SDL_TARGET= sdl_svmwgraph.o
SDL_LIBS= `sdl-config --libs`
SDL_FLAGS= -DSDL_TARGET
SDL_INCLUDE= `sdl-config --cflags`

#
# Uncomment the following for ncurses
#
#CURSES_TARGET= curses_svmwgraph.o
#CURSES_FLAGS= -DCURSES_TARGET=1
#CURSES_LIBS= -lncurses

#
# On machines w/ curses instead of ncurses [i.e. solaris, Irix, etc] 
# You'll want to use the following instead
#
#CURSES_TARGET= curses_svmwgraph.o
#CURSES_FLAGS= -DCURSES_TARGET=2
#CURSES_LIBS= -lcurses


#
# Uncomment the following for opengGL
#
#OPENGL_TARGET= opengl_svmwgraph.o
#OPENGL_FLAGS=-DOPENGL_TARGET
#OPENGL_LIBS= -L/usr/X11R6/lib -lX11 -lICE -lXmu -lGL -lGLU


##############################
# DO NOT EDIT BELOW THIS LINE
##############################

INCLUDE_GLOBAL= -Wall -Werror -O2 $(SDL_INCLUDE)
LIBS_GLOBAL= -lm $(SDL_MIXER_LIBS) $(SDL_LIBS) $(CURSES_LIBS) $(OPENGL_LIBS)
