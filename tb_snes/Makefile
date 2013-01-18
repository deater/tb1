AS=cl65
LD=ld65


all:	checksum.inc


tb1_snes.sfc:	tb1_snes.o snes-hirom.cfg
	$(LD) -o tb1_snes.sfc --config snes-hirom.cfg --obj tb1_snes.o

tb1_snes.o:	tb1_snes.s snes_init.s title_screen.s \
	tb1_title.tiles tb1_opening.tiles
	$(AS) -t none -o tb1_snes.o -l tb1_snes.lst -c tb1_snes.s

tb1_opening.tiles:	graphics/vmw_logo.pcx ./tools/pcx_to_tiles_8bpp
	./tools/pcx_to_tiles_8bpp vmw_logo < graphics/vmw_logo.pcx > tb1_opening.tiles

tb1_title.tiles:	graphics/tb1_title.pcx ./tools/pcx_to_tiles_8bpp
	./tools/pcx_to_tiles_8bpp title_screen < graphics/tb1_title.pcx > tb1_title.tiles

checksum.inc:	./tools/snes_checksum tb1_snes.sfc
	./tools/snes_checksum < tb1_snes.sfc > checksum.inc

tools/snes_checksum:	
	cd tools && make

tools/pcx_to_tiles_8bpp:	
	cd tools && make

clean:
	rm -f *~ *.o *.lst *.sfc *.tiles
	cd tools && make clean
