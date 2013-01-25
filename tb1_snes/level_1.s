; Level 1

; Page zero locations
joypad1 = $0000
joypad2 = $0001
shipx   = $0002

.setcpu "65816"

.segment "STARTUP"

;================================================
; LEVEL 1
;================================================

level_1:

	sep	#$20	; mem/A = 8 bit
.a8
	rep	#$10	; X/Y = 16 bit
.i16

	;==========================
	; Setup Background
	;==========================

	; we want the BG1 Tilemap to start at VRAM $e000
	; Format is
	; aaaa aass   a is shifted by 10 for address
	;             ss = size of screen in tiles 00 = 32x32
	;
	; 0111 0000

        lda	#$70		; BG1 Tilemap starts at VRAM $e000/2
        sta	$2107		; bg1 src

	; aaaa bbbb  a= BG2 tiles, b= BG1 tiles
	; bbbb<<13
	; 0000 0000
	; our BG1 tiles are stored starting in VRAM $0000

	lda	#$02
	sta	$210b		; bg1 tile data starts at VRAM 4000/2


        ;===============================
        ; Load Level1 Background Palette
        ;===============================
	stz	$2121		; start with color 0
	ldy	#(16*2)		; we have 16 colors
	lda	#^level1_background_palette
	ldx	#.LOWORD(level1_background_palette)
	jsr	svmw_load_palette

	;======================
	; Load Level1 Tile Data
	;======================
	ldx	#$2000		;
	stx	$2116		; set adddress for VRAM read/write
				; multiply by 2, so 0x4000

	lda	#^level1_background_tile_data
	ldx	#.LOWORD(level1_background_tile_data)
	ldy	#1824		; Copy 57 tiles, which are 32 bytes each
				;  8x8 tile with 4bpp
				; (57*32) = 1824 = ????

	jsr	svmw_load_vram

	;===================================
	; Load Level1 Tile Map
	;===================================
	ldx	#$7000		;
	stx	$2116		; set adddress for VRAM read/write
				; multiply by 2, so 0xe000

	lda	#^level1_background_tilemap
	ldx	#.LOWORD(level1_background_tilemap)
	ldy	#$0700		; 32x28 = 896 * 2 = 0x700
	jsr	svmw_load_vram



	;==========================
	; Setup Sprite
	;==========================

	; Load Palette for our sprite

	; Sprite Palettes start at color 128

	lda	#128		; start with color 128
	sta	$2121		;
	ldy	#(16*2)		; we have 16 colors
	lda	#^level1_pal0_palette
	ldx	#.LOWORD(level1_pal0_palette)
	jsr	svmw_load_palette

	;=========================
	; Load sprite data to VRAM
	;=========================

	lda	#$80		; increment after writing $2119
	sta	$2115

	ldx	#$0000		;
	stx	$2116		; set adddress for VRAM read/write
				; multiply by 2, so 0x0000

	lda	#^level1_pal0_data
	ldx	#.LOWORD(level1_pal0_data)
	ldy	#$0800		; 32 bytes * 64 tiles

	jsr	svmw_load_vram

	; Init sprites to be offscreen

	jsr	svmw_move_sprites_offscreen

	; Set our sprite active
	; assume 544 byte sprite table in $0200

	sep	#$20	; mem/A = 8 bit
.a8
	lda	#104
	sta	$0200		; set sprite 0 X to 0

	lda	#192		; set sprite 0 Y to 100
	sta	$0201

	; Xxxxxxxxx yyyyyyy cccccccc vhoopppN
	;

	stz	$0202		; set sprite 0

	; 00000000
	; no flip, priority 0, N=0 palette=0 (128)

	lda	#$00
	sta	$0203

	; X high bit = 0 for sprite 0
	; sprite size = 0 (smaller)

	lda	#%01010110
	sta	$0400

	; Enable sprite
	; sssnnbbb
	; sss = size (16x16 and 32x32 in our case)
	; nn = name
	; bbb = base selection
	lda	#%01100000
	sta	$2101

	jsr	svmw_transfer_sprite


	;======================
	; Setup the Video Modes
	;======================
level1_setup_video:

	sep	#$20	; mem/A = 8 bit
.a8
	rep	#$10	; X/Y = 16 bit
.i16

	lda	#$01		; 8x8 tiles, Mode 1
	sta	$2105

	lda	#$11		; Enable BG1 and sprites
	sta	$212c

	stz	$212d		; disable subscreen



	lda	#$00		; Disable NMI (VBlank Interrupt) and joypads
	sta	$4200		;

	;==========================
	; Setup VBLANK routine
	;==========================

	ldx	#level1_vblank
	stx	vblank_vector

	lda	#$81		; Enable NMI (VBlank Interrupt) and joypads
	sta	$4200		;

	jsr	svmw_fade_in

;	lda	#$0f
;	sta	$2100		; Turn on screen, full Brightness


	; init vars
	lda	#104
	sta	shipx

level1_loop:

	wai			; wait for interrupt

	; handle keypress
	lda	joypad1
check_left:
	bit	#$02
	beq	check_right

	dec	shipx

	bpl	check_right
	stz	shipx

check_right:
	bit	#$01
	beq	no_keypress

	inc	shipx

no_keypress:


	; update OAM

	lda	shipx
	sta	$0200		; set sprite 0 X to shipx


	bra	level1_loop


;=============================
; Level 1 VBLank Routine
;=============================

level1_vblank:
	php		; save status register
	rep	#$30	; Set A/mem=16 bits, X/Y=16 bits (to push all 16 bits)
.a16			; tell assembler the A is 16-bits
	phb		; save b
	pha		; save A
	phx		; save X
	phy		; save Y
	phd		; save zero page

	sep #$20        ; A/mem=8 bit
.a8

l1_joypad_read:
	lda	$4212		; get joypad status
	and	#$01		; if joy is not ready
	bne	l1_joypad_read	; wait

	lda	$4219		; read joypad (BYSTudlr)
	sta	joypad1

	lda	$4218		; read joypad (AXLRiiii)
	sta	joypad2

done_joypad:

	;=======================================
	; Update the sprite info structure (OAM)
	;=======================================

	jsr	svmw_transfer_sprite

done_vblank:

	lda	$4210	; Clear NMI flag

	rep	#$30	; A/Mem=16 bits, X/Y=16 bits
.a16
	pld		; restore saved vaules from stack
	ply
	plx
	pla
	plb

	plp
	rti		; return from interrupt


;============================================================================
; Sprite Data
;============================================================================

.segment "HIGHROM"

; sprite data
.include "level1_pal0.sprites"
.include "level1_background.tiles"

