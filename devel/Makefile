##############################################################
#  Makefile for TbLevelEdit    -- by Vince Weaver            #
#                                                            #
#  Written on Linux 2.1.35                                   #
#                                                            #
#  To modify for your configuration, add or remove the #     #
#                                                            #
##############################################################

#Your compiler.  If gcc doesn't work, try CC
CC = gcc
#CC = cc

#On Linux, uncomment the following
#
PLATFORM = Linux
C_OPTS = -O2 -Wall -DHAVE_LINUX_SOUND
L_OPTS = -lggi

######################################################################
#         THERE IS NO NEED TO EDIT ANYTHING BELOW THIS LINE          #
######################################################################

all:	lev2edit view_ppro

clean:
	rm -f *.o
	rm -f lev2edit
	rm -f *~

lev2edit:	lev2edit.o ../svmwgrap.o 
	$(CC) $(C_OPTS) -o lev2edit lev2edit.o ../svmwgrap.o ../gtblib.o $(L_OPTS)
	@strip lev2edit
	
view_ppro:	view_ppro.o ../svmwgrap.o
	$(CC) $(C_OPTS) -o view_ppro view_ppro.o ../svmwgrap.o $(L_OPTS)
	@strip view_ppro
	
view_ppro.o:	view_ppro.c
	$(CC) $(C_OPTS) -c view_ppro.c
	
lev2edit.o:	lev2edit.c
	$(CC) $(C_OPTS) -c lev2edit.c 
	
../svmwgrap.o:	../svmwgrap.c
	$(CC) $(C_OPTS) -c ../svmwgrap.c 
	
../gtblib.o:	../gtblib.c
	$(CC) $(C_OPTS) -c ../gtblib.c
