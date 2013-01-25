;
;	title_screen.s
;               by Vince Weaver  <vince _at_ deater.net>
;
;	Draws the title screen in Mode 3
;


display_title:

	rep	#$10	; X/Y = 16 bit
.i16
	sep	#$20	; mem/A = 8 bit
.a8


;===============================================
;===============================================
; Display to the screen
;===============================================
;===============================================

	;==========================
	; Setup Background
	;==========================

	; we want the BG1 Tilemap to start at VRAM $F000 (60k)
	; Format is
	; aaaa aass   a is shifted by 10 for address
	;             ss = size of screen in tiles 00 = 32x32
	;
	; 0111 0000

        lda     #$70		; BG1 Tilemap starts at VRAM $e000/2
        sta     $2107           ; bg1 src

	; aaaa bbbb  a= BG2 tiles, b= BG1 tiles
	; bbbb<<13
	; 0000 0000
	; our BG1 tiles are stored starting in VRAM $0000

	lda	#$00
        sta	$210b           ; bg1 tile data starts at VRAM 0000

	;==========================
	; Load Title Screen Palette
	;==========================
	stz	$2121		; start with color 0
	ldy	#(256*2)	; we have 256 colors
	lda	#^title_screen_palette
	ldx	#.LOWORD(title_screen_palette)
	jsr	svmw_load_palette

	;=====================
	; Load Title Tile Data
	;=====================
	ldx	#$0000		;
	stx	$2116		; set adddress for VRAM read/write
				; multiply by 2, so 0x0000

	lda	#^title_screen_tile_data
	ldx	#.LOWORD(title_screen_tile_data)
	ldy	#$7780		; Copy 478 tiles, which are 64 bytes each
				;  8x8 tile with 8bpp
				; (478*64) = 30592 = 0x7780

	jsr	svmw_load_vram

	;===================================
	; Load Title Tile Map
	;===================================
	ldx	#$7000		;
	stx	$2116		; set adddress for VRAM read/write
				; multiply by 2, so 0xe000

	lda	#^title_screen_tilemap
	ldx	#.LOWORD(title_screen_tilemap)
	ldy	#$0700		; 32x28 = 896 * 2 = 0x700
	jsr	svmw_load_vram


	;===================================
	; Setup Video
	;===================================

title_setup_video:

	; abcd efff
	; abcd = 8 (0) or 16 width for BG1234
	; e = priority for BG3
	; fff = background mode
	lda	#$03
	sta	$2105		; set Mode 3


	; 000abcde
	; a = object, b=BG4 c=BG3 d=BG2 e=BG1
	lda	#$01		; Enable BG1
	sta	$212c

	; disable subscreen
	stz	$212d


	; a000 bbbb
	; a = screen on/off (0=on), ffff = brightness

	lda	#$0f
	sta	$2100		; Turn on screen, full Brightness


	lda	#$81		; Enable NMI (VBlank Interrupt) and joypads
	sta	$4200		;

	jsr	svmw_fade_in

	jsr	svmw_repeat_until_keypressed

	jsr	svmw_fade_out


        rts


;============================================================================

.segment "HIGHROM"

.include "tb1_title.tiles"

