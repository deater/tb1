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
	; 1111 0000

        lda     #$f0            ; BG1 Tilemap starts at VRAM $F000
        sta     $2107           ; bg1 src


	; aaaa bbbb  a= BG2 tiles, b= BG1 tiles
	; bbbb<<13
	; 0000 0000
	; our BG1 tiles are stored starting in VRAM $0000

	lda	#$00
        sta	$210b           ; bg1 tile data starts at VRAM 0000

	;==============
	; Load Palettes
	;==============
	stz	$2121		; start with color 0
        ldy     #(256*2)	; we have 256 colors
	lda	#^title_screen_palette
	ldx	#.LOWORD(title_screen_palette)
	jsr	svmw_load_palette

	;=====================
	; Load Tile Data
	;=====================

	; replace with DMA!


	rep     #$20            ; set accumulator/mem to 16bit
.a16
.i16
	lda     #$0000          ;
        sta     $2116           ; set adddress for VRAM read/write
				; multiply by 2, so 0x0000

        ldy     #$3bc0		; Copy 478 tiles, which are 64 bytes each
				;  8x8 tile with 8bpp (four bits per pixel)
				; in 2-byte chunks, so
				; (478*64)/2 = 15296 = 0x3bc0

        ldx     #$0000
copy_tile_data:
	lda     f:title_screen_tile_data, x
	sta     $2118           ; write the data
	inx                     ; increment by 2 (16-bits)
	inx
	dey                     ; decrement counter
	bne     copy_tile_data


	;===================================
	; Load Tile Map
	;===================================
.a16
.i16

clear_linear_tilemap:

	lda	#$f000		; we set tilemap to be at VRAM 0xf000 earlier
	sta	$2116

        ldx	#$0000          ; clear offset

fill_screen_loop:

	lda	f:title_screen_tilemap,X
        sta     $2118
	inx
	inx

	cpx	#$0700			; 32x28 = 896 * 2 = 0x700
	bne     fill_screen_loop


setup_video:

        sep     #$20            ; set accumulator to 8 bit
                                ; as we only want to do an 8-bit load
.a8
.i16


	; Enable sprite
	; sssnnbbb
	; ss = size (8x8 in our case)
	; nn = name
	; bb = base selection, VRAM >> 14
;	lda	#%00000010	; point at 0x4000 in VRAM
;	sta	$2101

	; 000abcde
	; a = object, b=BG4 c=BG3 d=BG2 e=BG1
;	lda	#%00010001	; Enable BG1
	lda	#%00000001	; Enable BG1

	sta	$212c

	; disable subscreen
	stz	$212d

	; abcd efff
	; abcd = 8 (0) or 16 width for BG1234
	; e = priority for BG3
	; fff = background mode
	lda	#$03
	sta	$2105		; set Mode 3

	; a000 bbbb
	; a = screen on/off (0=on), ffff = brightness

	lda	#$0f
	sta	$2100		; Turn on screen, full Brightness


;	lda	#$81		; Enable NMI (VBlank Interrupt) and joypads
	lda	#$01		; Enable joypad
	sta	$4200		;

title_joypad_read:
        lda     $4212           ; get joypad status
        and     #%00000001              ; if joy is not ready
        bne     title_joypad_read     ; wait

        lda     $4219           ; read joypad (BYSTudlr)

        and     #%11110000      ; see if a button pressed

        beq     title_joypad_read

        lda     #$80
        sta     $2100           ; Turn off screen

        rts


;============================================================================

.segment "HIGHROM"

;tb_font:
;.include "tbfont.inc"

.include "tb1_title.tiles"



