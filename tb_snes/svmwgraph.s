; Various Common Routines
; Called the Super-VMW Graphics Library for various sentimental reasons

.segment "STARTUP"


;====================================
; Move Sprites Offscreen
;====================================

svmw_move_sprites_offscreen:

	php		; store status on stack

	rep	#$30	; 16bit mem/A, 16 bit X/Y
.a16
.i16
	ldx	#$0000	; offset
	lda	#$0001

	; OAM format.  Low part is:
	; Format is xxxxxxx yyyyyyy  cccccccc  vhoopppN
	; Upper part is X/Y packed
	; Xxxxxxxxx = X pos
	; Yyyyyyyyy = Y pos
	;  cccccccc = first tile of sprite
	;  v/h      = vert/horiz flip
	;  oo       = sprite priority
	;  ppp      = palette
	;  s        = sprite size
clear_xy:
	sta $0000,X	; page 0?
	inx
	inx
	inx
	inx		; skip 4 bytes
	cpx #$0200	; are we 512 bytes yet? (bottom part of OAM)
	bne clear_xy

	ldx #$0000
	lda #$5555	; set high bit of X, effective setting X to 256
set_top:
	sta $0200, X	; writing to top of screen
	inx		; increment
	inx
	cpx #$0020	; put all 32 values
	bne set_top

	plp


	;=====================================
	; transfer sprite
	;  assumes sprite_data starts at $7e:0000
	;=====================================

svmw_transfer_sprite:
	php			; save status word

	rep	#$30	; 16bit mem/A, 16 bit X/Y
.a16
.i16

	; p------b	p=special object priority
	; aaaaaaaa	baaaaaaaa = OAM address

	stz	$2102	; set OAM address to 0
	stz	$2103

	; setup DMA transfer

	; $4300
	; da-ifttt	d=direction  0 CPU->PPU  1 PPU->CPU
	;		a = HDMA address mode
	;		i = DMA address increment 0=+1 1=-1
	;		f = fixed transfer  1=no inc, 0=increment
	;		ttt = transfer mode

	; $4301
	; pppppppp	destination register $2100

	ldy	#$0400
	sty	$4300		; CPU -> PPU, auto increment,
				; write 1 reg, $2104 (OAM Write)

	stz	$4302
	stz	$4303		; source offset in RAM (low/high)
				; $0000

	ldy	#$0220
	sty	$4305		; number of bytes to transfer

	lda	#$7E
	sta	$4304		; bank address = $7E  (work RAM)

	lda	#$01
	sta	$420B		; start DMA transfer channel 0

	plp			; restore status word

	rts
