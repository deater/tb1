##############################################################
#  Makefile for Seabattle 1.0  -- by Vince Weaver            #
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

all:	tb1 

clean:
	rm -f *.o
	rm -f tb1
	rm -f *~

tb1:	tb1.o svmwgrap.o tblib.o level1.o level2.o soundIt.o
	$(CC) $(C_OPTS) -o tb1 tb1.o svmwgrap.o tblib.o level1.o level2.o soundIt.o $(L_OPTS)
	@strip tb1

soundIt.o:	soundIt.c
	$(CC) $(C_OPTS) -c soundIt.c

tb1.o:	tb1.c
	$(CC) $(C_OPTS) -c tb1.c 

tblib.o:	tblib.c
	$(CC) $(C_OPTS) -c tblib.c

level1.o:	level1.c
	$(CC) $(C_OPTS) -c level1.c

level2.o:	level2.c
	$(CC) $(C_OPTS) -c level2.c
	
svmwgrap.o:	svmwgrap.c
	$(CC) $(C_OPTS) -c svmwgrap.c 
