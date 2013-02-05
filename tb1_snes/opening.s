; Fade in
; Display the Opening VMW Logo
; eventually play music
; wait until button pressed
; Fade out

.segment "STARTUP"


display_opening:

        rep     #$10    ; X/Y = 16 bit
.i16
        sep     #$20    ; mem/A = 8 bit
.a8

        ;==========================
        ; Load Opening Screen Palette
        ;==========================
	stz	$2121		; start with color 0
	ldy	#(128*2)	; we have 128 colors
	lda	#^vmw_logo_palette
	ldx	#.LOWORD(vmw_logo_palette)
	jsr	svmw_load_palette

	;==========================
	; Load Opening Tile Data
	;==========================
	lda     #$0000          ;
        sta     $2116           ; set adddress for VRAM read/write
				; multiply by 2, so 0x0000

        lda     #^vmw_logo_tile_data
        ldx     #.LOWORD(vmw_logo_tile_data)
        ldy     #$12c0		; Copy 75 tiles, which are 64 bytes each
				;  8x8 tile with 8bpp (four bytess per pixel)
				; (75*64) = 4800 = 0x12c0

        jsr     svmw_load_vram


	;===================================
	; Load Opening Tilemap
	;===================================

	; 6 down, 0 right = (6*32) = 96 = 0xc0

	; 1111 0000 1100 0000
	ldx	#$78c0		; we set tilemap to be at VRAM 0xf000 earlier
				; f0c0
	stx	$2116

        lda     #^vmw_logo_tilemap
        ldx     #.LOWORD(vmw_logo_tilemap)
	ldy	#640		; 32*10*2 = 640 tiles
	jsr	svmw_load_vram


	;=======================
	; Load TB_FONT Tile Data
	;=======================
	ldx	#$7000		;
	stx	$2116           ; set adddress for VRAM read/write
				; multiply by 2, so 0xe000

	ldy	#$1000		; Copy 128 tiles, which are 32 bytes each
				; (128*32) = 4096 = 0x1000

        lda     #^tb_font
        ldx     #.LOWORD(tb_font)

        jsr     svmw_load_vram




	;=====================================
	; Set the BG1 and BG2 Tilemap Location
	;=====================================
	; Format is
	; aaaa aass   a is shifted by 10 for address
	;             ss = size of screen in tiles 00 = 32x32
	;

        lda     #$78            ; BG1 Tilemap starts at VRAM $F000 (/2)
        sta     $2107           ; bg1 src

	lda     #$7c            ; BG2 Tilemap starts at VRAM $F800 (/2)
	sta     $2108           ; bg2 src

	;========================================
	; Set the BG1 and BG2 Tile Data Locations
	;========================================

	; aaaa bbbb  a= BG2 tiles, b= BG1 tiles
	; bbbb<<13

	lda	#$70
	sta	$210b		; BG1 tile data starts at VRAM 0000
				; BG2 tile data starts at VRAM e000


	rep	#$20	; mem/A = 16 bit
.a16


        ; Write String to Background

opening_put_string:
        ; 1111 1101 0000 1000
	lda     #$7e44          ; set VRAM address
				; f800 = upper left
				; want 4x20, so (20 * 64) + 4*2 = 1288 = 0x508
				; fd08

	sta     $2116           ; set VRAM r/w address

	ldx     #$0000          ; string index

	; vhop ppcc cccc cccc
	lda     #$1c00          ; clear A

opening_copy_string:

	sep     #$20            ; set accumulator to 8 bit
                                ; as we only want to do an 8-bit load
.a8
	lda     opening_string, x       ; load string character
                                ; while leaving top 8-bits alone
	beq     opening_done_copy_string

	rep     #$20            ; set accumulator back to 16 bit
.a16
	sta     $2118           ; store to VRAM
                                ; the bottom 8 bits is the tile to use
                                ; the top 8 bits is vhopppcc
                                ; vert flip, horiz flip o=priority
                                ; p = palette, c=top bits of tile#

	inx                     ; increment string pointer

	bra     opening_copy_string
opening_done_copy_string:



opening_setup_video:

        sep     #$20            ; set accumulator to 8 bit
                                ; as we only want to do an 8-bit load
.a8


	; Enable Backgrounds
	; 000abcde
	; a = object, b=BG4 c=BG3 d=BG2 e=BG1
	lda	#%00000011	; Enable BG1 & BG2
	sta	$212c

	; disable subscreen
	stz	$212d

	; abcd efff
	; abcd = 8 (0) or 16 width for BG1234
	; e = priority for BG3
	; fff = background mode
	lda	#$03
	sta	$2105		; set Mode 3

	lda	#$81		; Enable NMI (VBlank Interrupt) and joypads
	sta	$4200		;


	jsr	svmw_fade_in

	jsr	svmw_repeat_until_keypressed

	jsr	svmw_fade_out

	rts


opening_string:
        .asciiz "A VMW SOFTWARE PRODUCTION"

.segment "HIGHROM"

tb_font:
.include "tbfont.inc"

.include "tb1_opening.tiles"

