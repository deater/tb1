AS=cl65
LD=ld65


all:	tb1_snes.sfc

tb1_snes.sfc:	tb1_snes.o 
	$(LD) -o tb1_snes.sfc --config snes.cfg --obj tb1_snes.o

tb1_snes.o:	tb1_snes.s snes_init.s
	$(AS) -t none -o tb1_snes.o -l tb1_snes.lst -c tb1_snes.s

#ll.tiles:	ll.ans ./tools/ansi_to_tile
#	./tools/ansi_to_tile < ll.ans > ll.tiles

#tools/ansi_to_tile:	
#	cd tools && make

clean:
	rm -f *~ *.o *.lst *.sfc
#	cd tools && make clean
