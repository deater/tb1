AS=cl65
LD=ld65


all:	tb1_snes.sfc

tb1_snes.sfc:	tb1_snes.o snes-hirom.cfg
	$(LD) -o tb1_snes.sfc --config snes-hirom.cfg --obj tb1_snes.o

tb1_snes.o:	tb1_snes.s snes_init.s \
	tb1_title.tiles
	$(AS) -t none -o tb1_snes.o -l tb1_snes.lst -c tb1_snes.s

tb1_title.tiles:	graphics/tb1_title.pcx ./tools/pcx_to_tiles_8bpp
	./tools/pcx_to_tiles_8bpp < graphics/tb1_title.pcx > tb1_title.tiles

tools/pcx_to_tiles_8bpp:	
	cd tools && make

clean:
	rm -f *~ *.o *.lst *.sfc
	cd tools && make clean
