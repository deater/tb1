;
;  tb1_snes.s -- Tom Bombem SNES in 65c816 Assembly v 0.1
;
;               by Vince Weaver  <vince _at_ deater.net>


; Page zero locations

.define EQU =
;LOGOB     EQU $FA
;LOGOH     EQU $F9

.setcpu "65816"

.segment "STARTUP"

	; Called at Reset (Startup) Time
Reset:

	; Run the initialization code
	; as the hardware state is unknown at reset

.include "snes_init.s"

	;================================
	; Get Ready to Go
	;================================

	phk			;make sure Data Bank = Program Bank
	plb

	cli			;enable interrupts



;==============================================
;==============================================
; DONE INITIALIZATION
;==============================================
;==============================================

start_program:

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
	; Setup Backgrounds
	;==========================

	; we want the BG1 Tilemap to start at VRAM $F000 (60k)
	; Format is
	; aaaa aass   a is shifted by 10 for address
	;             ss = size of screen in tiles 00 = 32x32
	;
	; 1111 0000

        lda     #$f0            ; BG1 Tilemap starts at VRAM $F000
        sta     $2107           ; bg1 src

	; we want the BG2 Tilemap to start at VRAM $F800
	; Format is
	; aaaa aass   a is shifted by 10 for address
	;             ss = size of screen in tiles 00 = 32x32
	;
	; 1111 0000

        lda     #$f8            ; BG2 Tilemap starts at VRAM $F800
        sta     $2108           ; bg1 src



	; aaaa bbbb  a= BG2 tiles, b= BG1 tiles
	; bbbb<<13
	; 0000 0000
	; our BG1 tiles are stored starting in VRAM $0000
	; our BG2 tiles are stored starting in VRAM $e000

	lda	#$e0
        sta	$210b           ; bg1 tile data starts at VRAM 0000



	;==============
	; Load Palettes
	;==============
.a8
.i16
        stz     $2121           ; CGRAM color-generator read/write address

        ldy     #$0200          ; we have 256 colors / 512 bytes

        ldx     #$0000          ; pointer
copypal:
        lda     f:opening_palette, x	; load byte of palette
        sta     $2122           ; store to color generator
        inx
        dey
        bne     copypal


	;=====================
	; Load Tile Data
	;=====================

	; replace with DMA!


	rep     #$20            ; set accumulator/mem to 16bit
.a16
.i16
	lda     #$0020          ;
        sta     $2116           ; set adddress for VRAM read/write
				; multiply by 2, so 0x0000

        ldy     #$2580		; Copy 300 tiles, which are 64 bytes each
				;  8x8 tile with 8bpp (four bits per pixel)
				; in 2-byte chunks, so
				; (300*64)/2 = 9600 = 0x2580

        ldx     #$0000
copy_tile_data:
	lda     f:opening_tile_data, x
	sta     $2118           ; write the data
	inx                     ; increment by 2 (16-bits)
	inx
	dey                     ; decrement counter
	bne     copy_tile_data


	;=====================
	; Load TB_FONT Data
	;=====================

	; replace with DMA!

	rep	#$20            ; set accumulator/mem to 16bit
.a16
.i16
	lda     #$7000          ;
	sta	$2116           ; set adddress for VRAM read/write
				; multiply by 2, so 0xe000

	ldy	#$0800		; Copy 128 tiles, which are 32 bytes each
				;  8x8 tile with 4bpp (four bits per pixel)
				; in 2-byte chunks, so
				; (128*32)/2 = 2048 = 0x0800

	ldx	#$0000
copy_font_data:
	lda	f:tb_font, x
	sta	$2118		; write the data
	inx			; increment by 2 (16-bits)
	inx
	dey			; decrement counter
	bne	copy_font_data


	;===================================
	; clear background to linear tilemap
	;===================================
.a16
.i16

clear_linear_tilemap:

	; 6 down, 1 right = (6*32)+1 = 97 = 0xc1

	lda	#$f0c1		; we set tilemap to be at VRAM 0xf000 earlier
	sta	$2116

        ldy     #$0001          ; clear counters
				; store to VRAM
                                ; the bottom 8 bits is the tile to use
                                ; the top 8 bits is vhopppcc
                                ; vert flip, horiz flip o=priority
                                ; p = palette, c=top bits of tile#

	ldx	#$0001

	; 8-bit color so ppp is 0
	; and we have a linear tilemap
	; 0000 0000
	; vhop ppcc cccc cccc

.a16
.i16

fill_screen_loop:

	cpx	#31
	bne	not_thirty

	stz	$2118
	stz	$2118
	ldx	#$0000
	bra	check_end

not_thirty:
	tya

        sta     $2118

	iny

check_end:
	inx

	cpy	#$0140			; (10*32) = 0x140

	bne     fill_screen_loop

        ; Write String to Background

put_string:

        lda     #$fd08          ; set VRAM address
				; f800 = upper left
				; want 4x20, so (20 * 64) + 4*2 = 1288 = 0x508
				; fd08

        sta     $2116           ; set VRAM r/w address

        ldy     #$000d          ; length of string


        ldx     #$0000          ; string index

        lda     #$0200          ; clear A

copy_string:

        sep     #$20            ; set accumulator to 8 bit
                                ; as we only want to do an 8-bit load
.a8
        lda     opening_string, x       ; load string character
                                ; while leaving top 8-bits alone
        beq     done_copy_string

        sec
        sbc     #$20

        rep     #$20            ; set accumulator back to 16 bit
.a16
        sta     $2118           ; store to VRAM
                                ; the bottom 8 bits is the tile to use
                                ; the top 8 bits is vhopppcc
                                ; vert flip, horiz flip o=priority
                                ; p = palette, c=top bits of tile#

        inx                     ; increment string pointer

        bra     copy_string
done_copy_string:


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
	lda	#%00000011	; Enable BG1

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
;	sta	$4200		;


overflowing:

	; repeat forever
	; stp?

	bra	overflowing


;============================================================================

wram_fill_byte:
.byte $00

opening_string:
        .asciiz "A VMW SOFTWARE PRODUCTION"

.segment "HIGHROM"

tb_font:
.include "tbfont.inc"

.include "tb1_opening.tiles"

.segment "BSS"

fx:
.res 1
fy:
.res 1
offset:
.res 1
tile_offset:
.res 2
logo_pointer:
.res 2

screen_byte:
.res 8*4	; 8 bytes, times four

output:
.res 4096

.segment "CARTINFO"
        .byte   "TOM BOMBEM 1         " ; Game Title (21 bytes)
        .byte   $21			; ROM Layout
					;   0x20:LowRom
					;   0x21:HiRom
					;   0x30/0x31:FastRom
	.byte	$00			; Cartridge Type (00 == only ROM)
        .byte   $09			; ROM Size (2^N KB)
        .byte   $00			; RAM Size
        .byte   $01			; Country Code (1=NTSC,2=PAL)
	.byte	$00			; License Code (0=unassigned)
        .byte   $00			; Version
.include "checksum.inc"
	.word   $0,$0			; unknown?

	; Interrupt Vectors!

        .word   $0000	; Native:COP
        .word   $0000	; Native:BRK
        .word   $0000	; Native:ABORT
        .word   $0000	; Native:NMI
        .word   $0000	;
        .word   $0000	; Native:IRQ

        .word   $0000   ;
        .word   $0000   ;

        .word   $0000   ; Emulation:COP
        .word   $0000   ;
        .word   $0000   ; Emulation:ABORT
        .word   $0000   ; Emulation:NMI
        .word   .LOWORD(Reset)   ; Emulation:RESET
        .word   $0000   ; Emulation:IRQ/BRK

