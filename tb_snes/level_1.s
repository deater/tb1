; Page zero locations
ball_x = $0000

.setcpu "65816"

.segment "STARTUP"

;================================================
; LEVEL 1
;================================================

level_1:

	rep	#$20	; mem/A = 16 bit
.a16


	;==========================
	; Setup VBLANK routine
	;==========================

	sei		; disable interrupts

	lda	#level1_vblank
	sta	vblank_vector

	cli		; enable interrupts

	sep	#$20	; mem/A = 8 bit
.a8


	;==========================
	; Setup Background
	;==========================


	;==========================
	; Setup Sprite
	;==========================

	; Load Palette for our sprite

	; Sprite Palettes start at color 128

	lda	#128
	sta	$2121       		; Start at START color
	lda	#^sprite_palette        ; Using ^ before the parameter gets its bank.
	ldx	#sprite_palette         ;
	ldy	#(16 * 2)   		; 2 bytes for every color

	; In: A:X  -- points to the data
	;      Y   -- Size of data

	pha
	phx
	phb
	php         ; Preserve Registers

	sep	#$20

	stx	$4302   ; Store data offset into DMA source offset
	sta	$4304   ; Store data bank into DMA source bank
	sty	$4305   ; Store size of data block

	stz	$4300   ; Set DMA Mode (byte, normal increment)
	lda	#$22    ; Set destination register ($2122 - CGRAM Write)
	sta	$4301
	lda	#$01    ; Initiate DMA transfer
	sta	$420B

	plp         ; Restore registers
	plb
	plx
	pla

	; Load sprite data to VRAM
	lda	#$80
	sta	$2115
	ldx	#$0000		; DEST
	stx	$2116		; $2116: Word address for accessing VRAM.
	lda	#^sprite_data	; SRCBANK
	ldx	#sprite_data	; SRCOFFSET
	ldy	#$0100		; SIZE

	; In: A:X  -- points to the data
	;     Y     -- Number of bytes to copy (0 to 65535)  (assumes 16-bit index)

	phb
	php         ; Preserve Registers

	sep	#$20

	stx	$4302   ; Store Data offset into DMA source offset
	sta	$4304   ; Store data Bank into DMA source bank
	sty	$4305   ; Store size of data block

	lda	#$01
	sta	$4300   ; Set DMA mode (word, normal increment)
	lda	#$18    ; Set the destination register (VRAM write register)
	sta	$4301
	lda	#$01    ; Initiate DMA transfer (channel 1)
	sta	$420B

	plp         ; restore registers
	plb

	;
	; Init sprites to be offscreen

	jsr	svmw_move_sprites_offscreen

	; Set our sprite active
	; assume 544 byte sprite table in $0000

	sep	#$20	; mem/A = 8 bit
.a8
	lda	#($0)
	sta	$0000

	lda	#(224/2 - 16)
	sta	$0001

	stz	$0002
	lda	#%01110000
	sta	$0003

	lda	#%01010100
	sta	$0200

	jsr	svmw_transfer_sprite


SetupVideo:
	rep #$10
	sep #$20

;    stz $2102
;    stz $2103

	;=============================
	;*********transfer sprite data
	;=============================

	stz	$2102		; set OAM address to 0
	stz	$2103

	ldy	#$0400
	sty	$4300		; CPU -> PPU, auto increment, write 1 reg, $2104 (OAM Write)
	stz	$4302
	stz	$4303		; source offset
	ldy	#$0220
	sty	$4305		; number of bytes to transfer
	lda	#$7E
	sta	$4304		; bank address = $7E  (work RAM)
	lda	#$01
	sta	$420B		; start DMA transfer

	; Enable sprite
	; sssnnbbb
	; ss = size (8x8 in our case)
	; nn = name
	; bb = base selection
	lda	#%00000000
	sta	$2101

	lda	#%00010001	; Enable BG1 and sprites
	sta	$212C
	stz	$212d

	lda	#$0F
	sta	$2100		; Turn on screen, full Brightness


;	lda	#$81		; Enable NMI (VBlank Intterupt) and joypads
;	sta	$4200		;


main_loop:

	; all work done in interrupt handler

	bra	main_loop


;=============================
; VBLank Routine
;  All the action happens here
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
	and #%00000001		; if joy is not ready
	bne l1_joypad_read		; wait

	lda	$4219		; read joypad (BYSTudlr)

	and	#%11110000  	; see if a button pressed

	bne	done_vblank	; if so, skip and don't move ball

done_joypad:

	lda	#^x_direction	; get bank for x_direction var (probably $7E)
	pha			;
	plb			; set the data bank to the one containing x_direction

	lda	ball_x		; get current ball X value
				; in the zero page, which is mirrored on SNES

	ldx	x_direction	; get x_direction  0=right, 1=left
	bne	ball_left	; if 1 skip ahead to handle going left

	ina			; ball_x += 2
	ina

	cmp	#248		; have we reached right side?

	bne	done_moving	; if not, keep moving right

	ldx	#1		; if so, switch to moving left
	bra	done_moving

ball_left:
	dea			; ball_x -= 2
	dea
	bne	done_moving	; if not at zero, keep moving left

	ldx	#0		; hit wall, switchto moving right

done_moving:
	sta	ball_x		; save ball_x co-ord
	stx	x_direction	; save x_direction


	;=======================================
	; Update the sprite info structure (OAM)
	;=======================================
.a8

	lda	#$0		; set data bank back to 0
	pha
	plb

	; Setup DMA transfer to copy our OAM structure in the zero page
	; into the actual OAM

	stz	$2102		; set OAM address to 0
	stz	$2103

	ldy	#$0400
	sty	$4300		; CPU -> PPU, auto increment, write 1 reg, $2104 (OAM Write)
	stz	$4302
	stz	$4303		; source offset
	ldy	#$0220
	sty	$4305		; number of bytes to transfer
	lda	#$7E
	sta	$4304		; bank address = $7E  (work RAM)
	lda	#$01
	sta	$420B		;start DMA transfer


done_vblank:

	lda	$4210	; Clear NMI flag

	rep	#$30	; A/Mem=16 bits, X/Y=16 bits
.a16
	pld		; restore saved vaules from stack
	ply
	plx
	pla
	plb

	sep #$20
	plp
	rti		; return from interrupt


;============================================================================
; Sprite Data
;============================================================================

; sprite data
    .include "sphere.sprite"

; tile data
;tile_pal:
;.include "tb1.pal"

.segment "BSS"
x_direction:	.word 0

