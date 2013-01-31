AS=cl65
LD=ld65


all:	checksum.inc


tb1_snes.sfc:	tb1_snes.o snes-hirom.cfg
	$(LD) -o tb1_snes.sfc --config snes-hirom.cfg --obj tb1_snes.o

tb1_snes.o:	tb1_snes.s \
	snes_init.s opening.s title_screen.s level_1.s \
	svmwgraph.s \
	tb1_title.tiles tb1_opening.tiles level1_background.tiles \
	star_background.tiles \
	level1_pal0.sprites level1_pal2.sprites \
	tbfont.inc
	$(AS) -t none -o tb1_snes.o -l tb1_snes.lst -c tb1_snes.s


level1_pal0.sprites:	graphics/level1_pal0.pcx tools/pcx_to_tiles_4bpp
	./tools/pcx_to_tiles_4bpp level1_pal0 < graphics/level1_pal0.pcx > level1_pal0.sprites

level1_pal2.sprites:	graphics/level1_pal2.pcx tools/pcx_to_tiles_4bpp
	./tools/pcx_to_tiles_4bpp level1_pal2 < graphics/level1_pal2.pcx > level1_pal2.sprites


level1_background.tiles:	graphics/level1_background.pcx ./tools/pcx_to_compressed_tilemap
	./tools/pcx_to_compressed_tilemap level1_background 4 0 < graphics/level1_background.pcx > level1_background.tiles

star_background.tiles:	graphics/star_background.pcx ./tools/pcx_to_compressed_tilemap
	./tools/pcx_to_compressed_tilemap star_background 4 1 < graphics/star_background.pcx > star_background.tiles

tb1_opening.tiles:	graphics/vmw_logo.pcx ./tools/pcx_to_compressed_tilemap
	./tools/pcx_to_compressed_tilemap vmw_logo 8 0 < graphics/vmw_logo.pcx > tb1_opening.tiles

tb1_title.tiles:	graphics/tb1_title.pcx ./tools/pcx_to_compressed_tilemap
	./tools/pcx_to_compressed_tilemap title_screen 8 0< graphics/tb1_title.pcx > tb1_title.tiles

checksum.inc:	./tools/snes_checksum tb1_snes.sfc
	./tools/snes_checksum < tb1_snes.sfc > checksum.inc

tbfont.inc:	./tools/tbfont.tb1 ./tools/convert_font_4bpp
	./tools/convert_font_4bpp < ./tools/tbfont.tb1 > tbfont.inc

tools/snes_checksum:	
	cd tools && make

tools/pcx_to_tiles_8bpp:	
	cd tools && make

clean:
	rm -f *~ *.o *.lst *.sfc *.tiles *.sprites tbfont.inc
	cd tools && make clean
