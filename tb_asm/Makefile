CC=gcc
CFLAGS = -Wall -O2
LFLAGS =

all:	tb_asm configure compress_data

./sstrip/sstrip:
		cd sstrip && make
	
compress_data:  compress_data.o lzss.o
		$(CC) $(LFLAGS) -o compress_data compress_data.o lzss.o
		
compress_data.o:    compress_data.c
		$(CC) $(CFLAGS) -c compress_data.c
		
			      
lzss.o:    lzss.c
	    gcc -O2 -Wall -c lzss.c
					 
lzss_new.o:    lzss_new.c
	                          gcc -O2 -Wall -c lzss_new.c


tb_asm:				  tb_asm.o	./sstrip/sstrip
				  ld -o tb_asm tb_asm.o
				  ./sstrip/sstrip tb_asm
		
tb_asm.o:			  tb_asm.s data.lzss data.labels
				  as -o tb_asm.o tb_asm.s
			
tb_asm.s:			  configure
				  ./configure

data.labels:			  compress_data
				  ./compress_data

data.lzss:			  compress_data data.inc
				  ./compress_data
				  
				  

configure:			  configure.c
				  gcc -O2 -Wall -o configure configure.c
					
clean:	
	rm -f *~ *.o tb_asm tb_asm.s core configure compress_data data.lzss data.header data.raw data.labels
	cd sstrip && $(MAKE) clean
