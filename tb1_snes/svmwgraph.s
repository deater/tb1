; Various Common Routines
; Called the Super-VMW Graphics Library for various sentimental reasons

.segment "STARTUP"

;====================================
;====================================
; Move Sprites Offscreen
;====================================
;====================================
; assumes we have RAM copy of OAM at $0200

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
	sta $0200,X	; assume at $0200
	inx
	inx
	inx
	inx		; skip 4 bytes
	cpx #$0200	; are we 512 bytes yet? (bottom part of OAM)
	bne clear_xy

	ldx #$0000
	lda #$5555	; set high bit of X, effective setting X to 256
set_top:
	sta $0400, X	; writing to top of screen
	inx		; increment
	inx
	cpx #$0020	; put all 32 values
	bne set_top

	plp


;========================================
;========================================
; transfer sprite
;  assumes sprite_data starts at $7e:0200
;========================================
;========================================

svmw_transfer_sprite:
	php			; save status word

	rep	#$10	; 16 bit X/Y
.i16
	sep	#$20	; 8 bit A
.a8

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

	ldx	#$0200
	stx	$4302		; source offset in RAM (low/high)
				; $0200

	ldy	#$0220
	sty	$4305		; number of bytes to transfer

	lda	#$7E
	sta	$4304		; bank address = $7E  (work RAM)

	lda	#$01
	sta	$420B		; start DMA transfer channel 0

	plp			; restore status word

	rts

;==============
;==============
; Load Palette
;   Bank in A
;   Address in X
;   Colors in Y*2
;   Color offset written to $2121 in advance
;==============
;==============

svmw_load_palette:
	php		; save status on stack

	rep	#$10	; 16 bit X/Y
.i16
	sep	#$20	; 8 bit A
.a8
	stx     $4302   ; Store data offset into DMA source offset
        sta     $4304   ; Store data bank into DMA source bank
        sty     $4305   ; Store size of data block

        stz     $4300   ; Set DMA Mode (byte, normal increment)
        lda     #$22    ; Set destination register ($2122 - CGRAM Write)
        sta     $4301
        lda     #$01    ; Initiate DMA transfer
        sta     $420b

	plp		; restore status
	rts

;===================
;===================
svmw_load_vram:
;===================
;===================
; set 2116/2117 for destination in advance
; A:X = address of data
; Y = length

	php		; save status on stack

	rep	#$10	; 16 bit X/Y
.i16
	sep	#$20	; 8 bit A
.a8

	stx	$4302	; Store data offset into DMA source offset
        sta	$4304	; Store data bank into DMA source bank
        sty	$4305	; Store size of data block

	lda	#$01	; Set DMA Mode (word, normal increment)
        sta	$4300   ;

        lda	#$18	; Set destination register ($2118 - VRAM Write)
        sta	$4301

        lda     #$01	; Initiate DMA transfer
        sta     $420b

	plp		; restore status
	rts


	;=========================
        ;=========================
        ; Fade in
        ;=========================
	;=========================
svmw_fade_in:

	php			; save status registers

        sep     #$20            ; set accumulator to 8 bit
.a8


        lda     #$00
fade_in_loop:
				; a000 bbbb a=!enable bbbb=brightness
        sta     $2100           ; Turn on screen, update brightnes

        wai			; wait until next interrupt

        cmp     #$0f		; are we at full brightness?
        beq     done_fade_in

        ina			; increment brightness and loop
        bra     fade_in_loop

done_fade_in:

	plp			; restore status

	rts			; return



	;=========================
        ;=========================
        ; Fade out
        ;=========================
	;=========================
svmw_fade_out:

	php			; save status registers

        sep     #$20            ; set accumulator to 8 bit
.a8


        lda     #$0f
fade_out_loop:
				; a000 bbbb a=!enable bbbb=brightness
        sta     $2100           ; Turn on screen, update brightnes

        wai			; wait until next interrupt

        cmp     #$00		; are we at full darkness?
        beq     done_fade_out

        dea			; increment brightness and loop
        bra     fade_out_loop

done_fade_out:

	lda	#$80
	sta	$2100		; disable screen

	plp			; restore status

	rts			; return


;========================
;========================
; repeat until keypressed
;========================
;========================
; this assumes auto-joypad read enabled

svmw_repeat_until_keypressed:
	php			; save status register

        sep     #$20            ; set accumulator to 8 bit
.a8

wait_for_keypress:
	wai			; wait until next interrupt

wait_for_joypad_ready:
	lda     $4212		; get joypad status
	and     #$01		; loop if joypad is not ready
	bne	wait_for_joypad_ready

        lda     $4219           ; read joypad (BYSTudlr)
	bne	found_keypress

	lda	$4218		; read joypad (axlriiii)
	bne	found_keypress

        bra	wait_for_keypress

found_keypress:

	plp			; restore status
	rts


;===========================
;===========================
; get_sprite_mask
;===========================
;===========================
; only meant to be called by the routines below
; sprite number is Y/4
get_sprite_mask:

	rep	#$30		; A=16bit X/Y=16bit
.i16
.a16

	phx			; save X on stack

;	tyx			; copy Y to X

	; address=$0400 + Y/16
	tya			; copy Y to A

	lsr			; divide by 16
	lsr
	tax
	lsr
	lsr
	tay			; copy back to Y

	txa			; copy original Y to A
	and	#$3		; mask to get low 2 bits
	tax			; copy back to X

	sep	#$20		; A=8bit
.a8
	lda	SPRITE_HIGH_LOOKUP,X
				; get bitmask for X
	plx
	rts

;===========================
;===========================
; is_sprite_active
;===========================
;===========================
; assumes high sprite table at $0400
; sets carry if active
; clears carry if not
; sprite number is Y/4
is_sprite_active:

	php			; save status
	phy			; save Y on stack

	jsr	get_sprite_mask
.a8
	; mask is in A

	and	$0400,Y		; sprite on screen when bit is 0

	beq	sprite_is_active

sprite_is_not_active:
	ply
	plp
	clc
	rts

sprite_is_active:
	ply
	plp
	sec
	rts


;===========================
;===========================
; activate_sprite
;===========================
;===========================
; assumes high sprite table at $0400
; sprite number is Y/4
activate_sprite:

	php				; store status
	phy				; store Y on stack

	jsr	get_sprite_mask
.a8
	eor	#$ff			; negate it

	and	$0400,Y			; sprite on screen when bit is 0
	sta	$0400,Y			; store it back out

	ply				; restore values from stack
	plp
	rts				; return


;===========================
;===========================
; deactivate_sprite
;===========================
;===========================
; assumes high sprite table at $0400
; sprite number is Y/4
deactivate_sprite:

	php					; save status on stack
	phy					; save Y

	jsr	get_sprite_mask
.a8

	ora	$0400,Y			; sprite off screen when bit is 1
	sta	$0400,Y			; save value back out

	ply				; restore Y
	plp				; restore status
	rts				; return
