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

	rep	#$10	; X/Y = 16 bit
.i16
	sep	#$20	; mem/A = 8 bit
.a8

	lda     #0		; set data bank back to 0
	pha			;
	plb			;


;===============================================
;===============================================
; Display to the screen
;===============================================
;===============================================



	;==========================
	; Setup Background
	;==========================

        lda     #$04            ; BG1 Tilemap starts at VRAM 0400
	; 0000 0100
	; aaaa aass   a=0000 01 << 11 = 0800
	; ss = size of screen in tiles 00 = 32x32
        sta     $2107           ; bg1 src

	; 0000 0100
	; aaaa bbbb  a= BG2 tiles, b= BG1 tiles
	; bbbb<<13

	lda	#$04
        sta	$210b           ; bg1 tile data starts at VRAM 8000

	;==============
	; Load Palettes
	;==============
.a8
.i16
        stz     $2121           ; CGRAM color-generator read/write address

        ldy     #$0020          ; we only have 16 colors / 32 bytes

        ldx     #$0000          ; pointer
copypal:
;        lda     tile_palette, x	; load byte of palette
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
	lda     #$4000          ;
        sta     $2116           ; set adddress for VRAM read/write
				; multiply by 2, so 0x8000

        ldy     #$1690          ; Copy 361 tiles, which are 32 bytes each
                                ;  8x8 tile with 4bpp (four bits per pixel)
				; in 2-byte chunks, so
				; (361*32)/2 = 5776 = 0x1690

        ldx     #$0000
copy_tile_data:
        lda     f:tile_data, x
        sta     $2118           ; write the data
        inx                     ; increment by 2 (16-bits)
        inx
        dey                     ; decrement counter
        bne     copy_tile_data


	;=====================
	; Load TB_FONT Data
	;=====================

	; replace with DMA!


;	rep     #$20            ; set accumulator/mem to 16bit
;.a16
;.i16
;	lda     #$6000          ;
;        sta     $2116           ; set adddress for VRAM read/write
;				; multiply by 2, so 0xc000
;
 ;       ldy     #$600		; Copy 96 tiles, which are 32 bytes each
;                                ;  8x8 tile with 4bpp (four bits per pixel)
;				; in 2-byte chunks, so
;				; (96*32)/2 = 1536 = 0x600
;
 ;       ldx     #$0000
copy_font_data:
;        lda     f:tb_font, x
 ;       sta     $2118           ; write the data
  ;      inx                     ; increment by 2 (16-bits)
   ;     inx
    ;    dey                     ; decrement counter
     ;   bne     copy_font_data


	;===================================
	; clear background to linear tilemap
	;===================================
.a16
.i16

clear_linear_tilemap:

	lda	#$0400		; we set tilemap to be at VRAM 0x0400 earlier
	sta	$2116

        ldy     #$0000          ; clear counters
	ldx	#$ffff

				; store to VRAM
                                ; the bottom 8 bits is the tile to use
                                ; the top 8 bits is vhopppcc
                                ; vert flip, horiz flip o=priority
                                ; p = palette, c=top bits of tile#
	;
	; 0001 1000
	; vhop ppcc
	; so 1800 = v=0 h=0 o=0 ppp = 2
	;           c=0x0

.a16
.i16

fill_screen_loop:

	tya

        sta     $2118

	iny
	inx

	cpx	#30
	bne	no_skip

	lda	#$0
	sta	$2118
	sta	$2118

	ldx	#0

no_skip:

	cpy	#$0169			; 30x12 = 360 = 0x168

	bne     fill_screen_loop




        ; Write String to Background
put_string:

        lda     #$05a9          ; set VRAM address
                                ; 0400 = upper left (0,0)
                                ; 0420 =            (0,1)
                                ; 05a0 =            (0,13)
                                ; 05a9 =            (9,13)

        sta     $2116           ; set VRAM r/w address
                                ; 2116 = 05
                                ; 2117 = a9

        ldy     #$000d          ; length of string


        ldx     #$0000          ; string index

        lda     #$0200          ; clear A

copy_string:

        sep     #$20            ; set accumulator to 8 bit
                                ; as we only want to do an 8-bit load
.a8
        lda     hello_string, x       ; load string character
                                ; while leaving top 8-bits alone
        beq     done_copy_string

	sec
	sbc	#$20

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
	lda	#%00000001	; Enable BG1

	sta	$212c

	; disable subscreen
	stz	$212d

	; abcd efff
	; abcd = 8 (0) or 16 width for BG1234
	; e = priority for BG3
	; fff = background mode
	lda	#$01
	sta	$2105		; set Mode 1

	; a000 bbbb
	; a = screen on/off (0=on), ffff = brightness

	lda	#$0f
	sta	$2100		; Turn on screen, full Brightness


;	lda	#$81		; Enable NMI (VBlank Interrupt) and joypads
;	sta	$4200		;


main_loop:

	; repeat forever
	; stp?

	bra	main_loop


;============================================================================

wram_fill_byte:
.byte $00

hello_string:
        .asciiz "HELLO,_WORLD!"

;tb_font:
;.include "tbfont.inc"

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

tile_data:
.res	32
tile_data2:
.res (30*12)*32

output:
.res 4096

.segment "CARTINFO"
        .byte   "TOM BOMBEM 1          "        ; Game Title
        .byte   $01                             ; 0x01:HiRom, 0x30:FastRom(3.57MHz)
        .byte   $05                             ; ROM Size (2KByte * N)
        .byte   $00                             ; RAM Size (8KByte * N)
        .word   $0001                           ; Developper ID ?
        .byte   $00                             ; Version
        .byte   $7f, $73, $80, $8c              ; Security Key ?
        .byte   $ff, $ff, $ff, $ff              ; Security Key ?

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
        .word   Reset   ; Emulation:RESET
        .word   $0000   ; Emulation:IRQ/BRK

