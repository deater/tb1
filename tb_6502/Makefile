all:	TB_6502


TB_6502:	tb_6502 
		make_b ./tb_6502 ./TB_6502 0xC00

tb_6502:	tb_6502.o
	ld65 -C ./apple2_c00.inc -o tb_6502 tb_6502.o

tb_6502.o:	tb_6502.s
		ca65 -o tb_6502.o -l tb_6502.s

disk:		
		dos33 tb_6502.dsk SAVE B TB_6502
		

clean:		
		rm -f *.lst *.o tb_6502 *~ add_to_disk TB_6502 make_b a.out
