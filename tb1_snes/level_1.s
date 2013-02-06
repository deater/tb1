; Level 1

;; GAME PARAMETERS
NUM_MISSILES    =  2
NUM_ENEMIES     =  6
UP_SHIELDS      = 32
WAVE_SIZE       = 16
WAVES_TILL_BOSS =  5

;; BONUS_FLAGS
PERFECT_SHIELDS = $80
PERFECT_KILLS   = $40
PERFECT_AIM     = $1


; Page zero locations
shipx		= $02
star_scroll	= $03
star_scroll_h	= $04	; 0004 too, 16-bit
RANDOM_SEED     = $05
YSAV		= $06
YSAV_H		= $07

joypad1_raw	= $08
joypad1_raw_h	= $09
joypad1_pressed = $0a
joypad1_pressed_h	= $0b
joypad1_held	= $0c
joypad1_held_h	= $0d

sprite_l	= $10
sprite_h	= $11

SPRITE_HIGH_LOOKUP = $20
SHL2		= $21
SHL3		= $22
SHL4		= $23

HISCORE_1	= $cb
HISCORE_2	= $cc
HISCORE_3	= $cd
HISCORE_H	= $ce
HISCORE_L	= $cf

BOSS_X			= $d0
BOSS_XADD		= $d1
BOSS_COUNT		= $d2
BOSS_SMOKE		= $d3
BOSS_EXPLODING		= $d4
BOSS_WAITING		= $d5
BOSS_HITS		= $d6
BOSS_SHOOTING		= $d7
ENEMIES_SPAWNED		= $d8
ENEMY_TYPE		= $d9
ENEMY_WAVE		= $da
CURRENT_INIT_X		= $db
CURRENT_ENEMY_KIND	= $dc
TOTAL_ENEMIES_OUT	= $dd
SCROLL			= $de
SOUND_ON		= $df

SHIPXADD	= $e1
ENEMY_PL	= $e2
ENEMY_PH	= $e3
MISSILE_PL	= $e4
MISSILE_PH	= $e5
GR_PAGE		= $e6
LEVEL		= $e7
SHIELDS		= $e8
SCOREL		= $e9
SCOREH		= $ea
BONUS_FLAGS	= $eb
BCD_BYTEH	= $ec
BCD_BYTE	= $ed

COL_X1		= $ec
COL_X2		= $ed
COL_X3		= $ee
COL_X4		= $ef

ENEMY_EXPLODING	= $f0
ENEMY_KIND	= $f1
ENEMY_X		= $f2
ENEMY_Y		= $f3
ENEMY_XADD	= $f4
ENEMY_YADD	= $f5
ENEMY_XMIN	= $f6
ENEMY_XMAX	= $f7

BETWEEN_DELAY	= $f8
ENEMY_WAIT	= $f9

STRINGL		= $fa
STRINGH		= $fb
PARAM2		= $fc
RESULT		= $fd
LASTKEY		= $fe
TEMP		= $ff


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

	; we want the BG1 Tilemap to start at VRAM $f000
	; Format is
	; aaaa aass   a is shifted by 10 for address
	;             ss = size of screen in tiles 00 = 32x32
	;
	; 0111 1000

        lda	#$78		; BG1 Tilemap starts at VRAM $f000/2
        sta	$2107		; bg1 src

	; we want the BG2 Tilemap to start at VRAM $d000
	; Format is
	; aaaa aass   a is shifted by 10 for address
	;             ss = size of screen in tiles 10 = 32x64
	;
	; 0110 1010

        lda	#$6a		; BG2 Tilemap starts at VRAM $d000/2
        sta	$2108		; bg2 src


	; aaaa bbbb  a= BG2 tiles, b= BG1 tiles
	; bbbb<<13
	; 0000 0000
	; our BG1 tiles are stored starting in VRAM $0000

	lda	#$32		; bg2 tile data starts at VRAM 5000/2
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
	ldx	#$7800		;
	stx	$2116		; set adddress for VRAM read/write
				; multiply by 2, so 0xe000

	lda	#^level1_background_tilemap
	ldx	#.LOWORD(level1_background_tilemap)
	ldy	#$0700		; 32x28 = 896 * 2 = 0x700
	jsr	svmw_load_vram




        ;===============================
        ; Load Level1 Star Palette
        ;===============================
	lda	#16
	sta	$2121		; start with color 16
	ldy	#(16*2)		; we have 16 colors
	lda	#^star_background_palette
	ldx	#.LOWORD(star_background_palette)
	jsr	svmw_load_palette

	;======================
	; Load Level1 Star Data
	;======================
	ldx	#$3000		;
	stx	$2116		; set adddress for VRAM read/write
				; multiply by 2, so 0x4000

	lda	#^star_background_tile_data
	ldx	#.LOWORD(star_background_tile_data)
	ldy	#$4420		; Copy 545 tiles, which are 32 bytes each
				;  8x8 tile with 4bpp
				; (545*32) = 17,440 = 0x4420

	jsr	svmw_load_vram

	;===================================
	; Load Level1 Star Tile Map
	;===================================
	ldx	#$6800		;
	stx	$2116		; set adddress for VRAM read/write
				; multiply by 2, so 0xd000

	lda	#^star_background_tilemap
	ldx	#.LOWORD(star_background_tilemap)
	ldy	#$1000		; 32x64 = 2048 * 2 = 0x1000
	jsr	svmw_load_vram



	;==========================
	; Setup Sprites
	;==========================

	; Load Palette for our sprite

	; Sprite Palettes start at color 128

	; Load palette 0

	lda	#128		; start with color 128
	sta	$2121		;
	ldy	#(16*2)		; we have 16 colors
	lda	#^level1_pal0_palette
	ldx	#.LOWORD(level1_pal0_palette)
	jsr	svmw_load_palette

	; Load palette 2

	lda	#160		; start with color 128
	sta	$2121		;
	ldy	#(16*2)		; we have 16 colors
	lda	#^level1_pal2_palette
	ldx	#.LOWORD(level1_pal2_palette)
	jsr	svmw_load_palette


	;=========================
	; Load sprite data to VRAM
	;=========================

	lda	#$80		; increment after writing $2119
	sta	$2115

	; Pal0 Sprites

	ldx	#$0000		;
	stx	$2116		; set adddress for VRAM read/write
				; multiply by 2, so 0x0000

	lda	#^level1_pal0_data
	ldx	#.LOWORD(level1_pal0_data)
	ldy	#$0800		; 32 bytes * 64 tiles

	jsr	svmw_load_vram

	; Pal2 Sprites

				; 0x0000 + 32*0x60 / 2 = 0x600
	ldx	#$0600		;
	stx	$2116		; set adddress for VRAM read/write

	lda	#^level1_pal2_data
	ldx	#.LOWORD(level1_pal2_data)
	ldy	#$0400		; 32 bytes * 32 tiles

	jsr	svmw_load_vram


	;=============================
	; Init sprites to be offscreen

	jsr	svmw_move_sprites_offscreen

	;=============================
	; Set our sprite active
	; assume 544 byte sprite table in $0200

	sep	#$20	; mem/A = 8 bit
.a8

	;==================
	; setup ship sprite

	; Xxxxxxxxx yyyyyyy cccccccc vhoo pppN
	;

	lda	#104		; set shipx to 104
	sta	$0200		;

	lda	#192		; set shipy to 192
	sta	$0201

	stz	$0202		; set ship sprite to 0

	lda	#$20		; no flip, priority 2, N=0 palette=0 ($20)
	sta	$0203

	; X high bit = 0 for sprite 0
	; sprite size = 1 (larger)
	lda	#%01010110
	sta	$0400

	;=====================================
	; set sprites 8,9,10,11 to be missiles

	lda	#12		; sprite location 13
	sta	$0202 + 4*8
	sta	$0202 + 4*9
	sta	$0202 + 4*10
	sta	$0202 + 4*11

	lda	#$20		; pal=0, priority=2, flip=none
	sta	$0203 + 4*8
	sta	$0203 + 4*9
	sta	$0203 + 4*10
	sta	$0203 + 4*11


	;=========================
	;=========================
	; Enable sprites
	;=========================
	;=========================
	; sssnnbbb
	; sss = size (011 = 16x16 and 32x32)
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

	lda	#$13		; Enable BG1,BG2, and sprites
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


	;===========================
	; init lookup tables
	;===========================

	lda	#$01
	sta	SPRITE_HIGH_LOOKUP
	lda	#$04
	sta	SHL2
	lda	#$10
	sta	SHL3
	lda	#$40
	sta	SHL4

  ;====================
  ; setup the high score
  ;=====================

        lda	#$01
	sta	HISCORE_H
	lda	#$00
	sta	HISCORE_L

	; modestly make me the high scorer
	lda     #'V'
	sta	HISCORE_1
	lda	#'M'
	sta	HISCORE_2
	lda	#'W'
	sta	HISCORE_3


;==========================================================
; START NEW GAME
;==========================================================


do_new_game:

	;; Clear BSS

;	ldx	#$0
;clear_bss:
;	stz	start_bss,X
;	inx
;	cpx	#end_bss-start_bss
;	bne	clear_bss

	;; Init one-time vars

	stz	star_scroll
	stz	star_scroll_h

	lda	#$8
	sta	SHIELDS			; shields start at 8
;	jsr	update_shields

	lda	#104
	sta	shipx			; shipx at start is 104

	lda	#$1
	sta	LEVEL			; start at level 1

	stz	SCOREL
	stz	SCOREH
;	jsr	print_score

new_level:

	;========================
	; Setup various variables
	;========================

	lda	#(PERFECT_AIM|PERFECT_SHIELDS|PERFECT_KILLS)
	sta	BONUS_FLAGS		; set perfect shot/shield/enemies

	lda	#$14
	sta	ENEMY_WAIT		; ???

	stz	BETWEEN_DELAY
	stz	SHIPXADD		; clear shipxadd
	stz	ENEMY_WAVE
	stz	TOTAL_ENEMIES_OUT
	stz	ENEMIES_SPAWNED
	stz	ENEMY_TYPE

	;======================
	; Print level on screen
	;======================

;	lda	#>(level_string_xy)
;	sta	STRINGH
;	lda	#<(level_string_xy)
;	sta	STRINGL

;	jsr	print_text_xy

;	ldx	#20
;	jsr	wait_X_100msec	       ; pause for 3 seconds
;	bit	KEYRESET	       ; clear keyboard


        ;/========================\
	;+                        +
	;+    MAIN LEVEL1 LOOP    +
	;+                        +
	;\========================/
.a8
.i16

level1_loop:

	wai			; wait for interrupt

	; ================================
	;  put out new enemies (if needed)
	; ================================

	inc	BETWEEN_DELAY	; inc how long we've delayed
	lda	BETWEEN_DELAY	; load it in
	cmp	ENEMY_WAIT	; have we waited long enough?
	beq     reset_delay

	jmp	move_enemies	; if not, go on to movement

reset_delay:

	; delay==wait, so attempt to put out new enemy

	lda	BETWEEN_DELAY
	and	#$1
	sta	BETWEEN_DELAY          ; reset delay

	; special case for boss

	lda	#$9			; if boss, don't keep track of
	cmp	ENEMY_TYPE		; how many enemies were spawned
	bne	not_boss_dont_clear

	lda	#$1			; store 1 so we don't increment wave
	sta	ENEMIES_SPAWNED

not_boss_dont_clear:

	; see if we are at a new wave
	; basically, if 16 have been spawned, change

	lda	ENEMIES_SPAWNED
	and	#$0f
	bne	same_enemy_type		; if not 16 gone by, move on

	;=======================
	; change  the enemy type

	inc	ENEMIES_SPAWNED

	jsr	random_number
	and	#$7			; get a random number 0-7
	sta	ENEMY_TYPE

	inc	ENEMY_WAVE

	lda	ENEMY_WAVE		; have we enough waves to reach boss?
	cmp	#WAVES_TILL_BOSS
	bne	not_boss_yet

	lda	#$8
	sta	ENEMY_TYPE

not_boss_yet:

	; set various constants
	; these may be overriden later

	lda	#20
	sec
	sbc	LEVEL
	sta	ENEMY_WAIT		; enemy_wait=20-level

	; set kind and init x to be random by default

	lda	#$ff
	sta	CURRENT_ENEMY_KIND
	sta	CURRENT_INIT_X

same_enemy_type:

	; find empty enemy slot

	ldy	#$80			; point to enemies[0] $20 * 4
find_empty_enemy:
	jsr	is_sprite_active
	bcc	add_enemy

	iny
	iny
	iny
	iny
	cpy	#($80+4*NUM_ENEMIES)
	bne	find_empty_enemy

	jmp	move_enemies		; no empty, slots, move on

add_enemy:

	;==============================================
	; First see if we must wait for enemy to clear
	; types 2 and 8

	lda	ENEMY_TYPE
	cmp	#$2

	bne	check_type_8

	lda	TOTAL_ENEMIES_OUT
	beq     change_to_type_3
	jmp	move_enemies
change_to_type_3:
	lda	#$3
	sta	ENEMY_TYPE

	jsr	random_number
	and	#$8
	sta	CURRENT_ENEMY_KIND

	jsr	random_number
	and	#$1f			; mask off so 0-31
	clc
	adc	#$2
	asl	A
	sta	CURRENT_INIT_X
	jmp	setup_enemy_defaults

check_type_8:

	cmp	#$8
	beq	before_boss_stuff
	jmp	check_type_9

before_boss_stuff:

	;======================
	; before boss stuff

	lda	TOTAL_ENEMIES_OUT
	beq	prepare_for_boss
	jmp	move_enemies

prepare_for_boss:

	;===============
	; HANDLE BONUSES
	;===============

	; Print "BONUS POINTS"

;	lda	#>bonus_string
;	sta	STRINGH
;	lda	#<bonus_string
;	sta	STRINGL

;	jsr	print_text_xy          ; print text

	; Check to see if we had any bonuses

;	lda	#$C1
;	bit	BONUS_FLAGS
;	beq	no_bonus


	; Check if we had no hits on shields

perfect_shields:
;	bpl 	perfect_kills

;	jsr	score_plus_50

;	lda	#>bonus_shields
;	sta	STRINGH
;	lda	#<bonus_shields
;	sta	STRINGL

;	jsr	print_text_xy          ; print text


	; See if we killed all the enemies

perfect_kills:
;	bit	BONUS_FLAGS
;	bvc	perfect_aim

;	jsr	score_plus_50

;	lda	#>bonus_kills
;	sta	STRINGH
;	lda	#<bonus_kills
;	sta	STRINGL

;	jsr	print_text_xy          ; print text

	; See if no missiles missed

perfect_aim:
;	lda 	#$01
;	bit	BONUS_FLAGS
;	beq	done_bonus

;	jsr	score_plus_50

;	lda	#>bonus_aim
;	sta	STRINGH
;	lda	#<bonus_aim
;	sta	STRINGL

;	jsr	print_text_xy          ; print text

;	jmp 	done_bonus

	; we had no bonuses

no_bonus:
;	lda	#>no_bonus_string
;	sta	STRINGH
;	lda	#<no_bonus_string
;	sta	STRINGL

;	jsr	print_text_xy          ; print text

	; Wait until a keypress, and return to graphics mode

done_bonus:

;	ldx	#30
;	jsr	wait_X_100msec	       ; pause for 3 seconds
;	bit	KEYRESET	       ; clear keyboard

;	jsr	wait_until_keypressed
;	jsr     set_page0_gr


	;======================
	; setup boss

	lda	#$0C
	sta	BOSS_X			; boss_x = 13

	lda	#$1
	sta	BOSS_XADD		; boss_xadd=1
	sta	BOSS_WAITING		; boss_waiting=1

	jsr	random_number
	and	#$1f
	sta	BOSS_COUNT		; boss_count = rand%32

	lda	#$0
	sta	BOSS_SMOKE		; boss_smoke=0
	sta	BOSS_EXPLODING		; boss_exploding=0
	sta	BOSS_SHOOTING		; boss_shooting=0

	lda	LEVEL
	asl	A
	clc
	adc	#$10
	sta	BOSS_HITS		; boss_hits=(level*2)+20

	lda	#$9
	sta	ENEMY_TYPE		; enemy_type=9

	jmp	move_enemies


check_type_9:

	; if boss, and he's waiting,
	; don't produce enemies

	cmp	#$9
	bne	setup_enemy_defaults
	lda	BOSS_WAITING
	beq	setup_enemy_defaults
	jmp     move_enemies


	;========================
	; setup enemy defaults
setup_enemy_defaults:
	inc	ENEMIES_SPAWNED
	inc	TOTAL_ENEMIES_OUT

	jsr	activate_sprite		; enemy[i].out=1

;	lda	#$0
;	iny	; exploding
;	sta	(ENEMY_PL),Y	       ; enemy[i].exploding=0

	lda	CURRENT_ENEMY_KIND     ; if kind <0 then random
	bpl	store_enemy_kind

	jsr	random_number
	and	#$8

store_enemy_kind:

	; Xxxxxxxxx yyyyyyy cccccccc vhoo pppN

	clc
	adc	#$60		; Use pal2 enemies for now

	pha			; store type for later

				; Y = $20+enemies

				; want to store to $200 + (Y*4)

;	rep	#$20		; set A to 16-bit
;.a16

	tyx			; copy Y to A
;	asl
;	asl			; multiply by 4
;	tax			; put in X


;	sep	#$20		; set A back to 8-bit
;.a8

	; determine enemy _x
	; if < 0, make random between 2->34

	lda     CURRENT_INIT_X
	bpl	store_init_x

	jsr	random_number
	and	#$1f
	clc
	adc	#$2
	asl

store_init_x:

	sta	$200,X		; store x value at $200+(Y*4)

	; enemy_y is always 0 by default

	inx			; store y value at $201+(Y*4)
	lda	#$0
	sta	$200,X

	pla			; pull enemy type off stack

	inx			; store SPRITE value to $202+(Y*4)
	sta	$200,X

	lda	#$24		; noflip, pal2, priority=2
	inx
	sta	$200,X		; store paramaters to $203+(Y*4)

	; X = (Y*4)+3

	lda	#$24
	sta	extra_sprite,X	       ; ymin
	inx

	sta	extra_sprite,X	       ; xmin
	inx

	stz	extra_sprite,X	       ; yadd
	lda	#$2

	inx

	stz	extra_sprite,X	       ; xadd


	;===========================================
	; Enemy specific inits

;	lda	ENEMY_TYPE
;	beq	enemy_type_0
;	cmp	#$1
;	beq	enemy_type_1
;	jmp	enemy_type_2

enemy_type_0:
enemy_type_1:

	;================================
	; ENEMY TYPE 0 and 1
	; diagonal, no wait
	; movement proportional to level

	lda	LEVEL			; xadd = level
	sta	extra_sprite,X

	inx				; point to yadd

	lsr	A
	ora	#$1
	sta	extra_sprite,X		; yadd = level/2
	jmp	move_enemies

enemy_type_2:
	;=====================
	; Enemy Type 2
	; just a place-holder
	; waits for enemies to die then moves on to 3

;	cmp	#$2
;	bne	enemy_type_3
;	jmp	move_enemies


enemy_type_3:

;	cmp  	#$3
;	bne	enemy_type_4

	;======================
	; Enemy type 3

;	lda	#$1
;	sta	(ENEMY_PL),Y		; xadd=1

;	iny

;	lda	LEVEL
;	sta	(ENEMY_PL),Y		; yadd=level

;	jmp	move_enemies

enemy_type_4:

;	cmp	#$4
;	bne	enemy_type_5


	;=========================
	; Enemy Type 4
	; Horizontal, then fall

;	lda	#$2
;	sta	(ENEMY_PL),Y		; xadd = 2

;	iny

;	jsr	random_number
;	ora	#$80	     		; set negative
;	sta	(ENEMY_PL),y		; yadd = -(random%128)
					; this means bop back and forth a random
					; time, then drop

;	jmp	move_enemies


enemy_type_5:
;	cmp     #$5
;	bne	enemy_type_6

	;========================
	; Enemy Type 5
	; "wiggle"

;	lda	#$1
;	sta	(ENEMY_PL),y		; xadd=1

;	iny
;	lda	LEVEL
;	sta	(ENEMY_PL),y		; yadd=2

;	iny
;	jsr	random_number
;	and	#$0f
;	clc
;	adc	#$2
;	sta	(ENEMY_PL),y		; xmin=(rand%16)+2


;	dey 	; yadd
;	dey 	; xadd
;	dey 	; y
;	dey 	; x
;	asl    A
;	sta    (ENEMY_PL),y
;	iny 	; y
;	iny 	; xadd
;	iny 	; yadd
;	iny 	; xmin

;	jsr	random_number
;	and	#$0f
;	clc
;	adc	(ENEMY_PL),Y
;	adc	#$02
;	iny
;	sta	(ENEMY_PL),Y		; xmax = xmin+(rand%16)+2

;	jmp	move_enemies


enemy_type_6:
;	cmp     #$6
;	beq	enemy_type_7
;	cmp	#$7
;	beq	enemy_type_7
;	jmp	enemy_type_8
enemy_type_7:
	;=====================
	; Enemy Types 6+7
	; "Rain"

;	jsr	random_number
;	and	#6
;	bne	no_use_own_x

;	dey 	; y
;	dey 	; x

;	lda	shipx
;	cmp	#$2
;	bpl	shipx_ok
;	lda	#$2			; stupid bug where gets stuck is < xmin

shipx_ok:

;	asl	A
;	sta	(ENEMY_PL),Y		; one-in-four chance we use shipx as X

;	iny 	; y
;	iny 	; xadd

no_use_own_x:

;	lda	#$0
;	sta	(ENEMY_PL),Y		; xadd=0
;	iny
;	lda	#$1
;	sta	(ENEMY_PL),Y		; yadd = 1

;	jmp	move_enemies

enemy_type_8:
enemy_type_9:

	;======================
	; Things flung by boss


;	dey	       	      		; y
;	dey				; x

;	lda	BOSS_X
;	clc
;	adc	#$5
;	asl	A
;	sta	(ENEMY_PL),Y		; enemy_x=boss_x+5

;	iny
;	lda	#$3
;	asl	A
;	asl	A
;	sta	(ENEMY_PL),Y		; enemy_y=3


;	iny
;	lda	#$0
;	sta	(ENEMY_PL),Y		; xadd=0

;	iny
;	lda	#$2
;	sta	(ENEMY_PL),Y		; yadd=2



move_enemies:

  ;==============================================
  ; Move Enemies!  (first thing, if no new added)
  ;==============================================

	ldy	#$20			; point to enemies[0]
handle_enemies:

	jsr	is_sprite_active	; set if enemy[y].out
	bcs	load_enemy_zero_page	; if enemy.out then we are good

	jmp	skip_to_next_enemy	; enemy is not out, so skip to next


   ;==========================================
   ; load this enemy stuff into zero page for
   ; easier access
   ;==========================================

load_enemy_zero_page:
;	ldx	#ENEMY_EXPLODING
load_to_zero_page:
;	iny			       ; point to exploding
;	lda	(ENEMY_PL),Y
;	sta	0,X	    	       ; store to zero page
;	inx
;	cpx	#(ENEMY_XMAX+1)	       ; see if reached end
;	bne	load_to_zero_page      ; if not keep copying

   ;================================
   ; skip all movement and collision
   ; if exploding
   ;================================

;	lda	ENEMY_EXPLODING
;	beq     move_enemy_x
;	jmp	draw_enemy

   ;================================
   ; Start the enemy movement engine
   ;================================


     ;========
     ; Move X
     ;========

move_enemy_x:
;	clc
;	lda	ENEMY_X	       	       ; X
;	adc	ENEMY_XADD	       ; x+=xadd
;	sta	ENEMY_X

;	lsr	A

;	cmp	ENEMY_XMIN	       ; are we less than xmin?
;	bmi	switch_dir_enemy_x     ; if so, switch direction

;	cmp	ENEMY_XMAX	       ; are we greater than xmax?
;	bpl	switch_dir_enemy_x     ; if so, switch direction

;	jmp	move_enemy_y


switch_dir_enemy_x:

	; switch X direction

;	lda	#$0	    	       ; load zero
;	sec
;	sbc	ENEMY_XADD	       ; 0 - ENEMY_XADD
;	sta	ENEMY_XADD	       ; store it back out, negated
;	jmp	move_enemy_x	       ; re-add it in

     ;========
     ; Move Y
     ;========

move_enemy_y:

;	lda	#$0		       ; load in zero
;	cmp	ENEMY_YADD	       ; compare to YADD

;	bmi	no_y_special_case      ; if minus, we have special case

;	inc	ENEMY_YADD
;	bne	done_enemy_y

;	lda	#$0
;	sta	ENEMY_XADD
;	lda	#$2
;	sta	ENEMY_YADD

	; increment y
	; is it > 0?
	; if not keep going
	; if so, yadd=level*2

;	jmp	done_enemy_y

no_y_special_case:
;	clc
;	lda	ENEMY_Y		       ; get Y
;	adc	ENEMY_YADD	       ; y+=yadd
;	sta	ENEMY_Y	       	       ; store back out

;	lsr	A
;	lsr	A

;	cmp	#$12		       ; is y<=12?
;	bmi	done_enemy_y	       ; if so no need to do anything
;	beq	done_enemy_y

	; off screen

;	pla   	    		       ; pop saved Y off stack
;	tay
;	pha			       ; push y back on stack

;	lda	#$0
;	sta	(ENEMY_PL),Y	       ; set enemy[i].out=0

;	dec	TOTAL_ENEMIES_OUT

;	lda	BONUS_FLAGS
;	and	#<(~PERFECT_KILLS)
;	sta	BONUS_FLAGS

;	jmp	skip_to_next_enemy     ; skip to next enemy


done_enemy_y:

   ;===============
   ; Done Movement
   ;===============

   ;======================
   ; Check for Collisions
   ;======================


     ;==================================
     ; Check ENEMY <> MISSILE collision
     ;==================================

check_enemy_missile_collision:

;	ldy     #$0
;	sty	YSAV
check_missile_loop:
;	lda	(MISSILE_PL),Y
;	beq	missile_not_out

;	iny		       	       ; point to missile.x
;	lda	(MISSILE_PL),Y	       ; load missile.x

;	sta	COL_X1
;	sta	COL_X2

;	lda	ENEMY_X
;	lsr	A
;	sta	COL_X3
;	clc
;	adc	#3
;	sta	COL_X4

;	jsr	check_inside

;	bcc	missile_done

x_in_range:

;	iny
;	lda	(MISSILE_PL),Y	       ; load missile.y

;	sta	COL_X3
;	clc
;	adc	#2
;	sta	COL_X4

;	lda	ENEMY_Y
;	lsr	A
;	lsr	A
;	sta	COL_X1
;	clc
;	adc	#1
;	sta	COL_X2

;	jsr	check_inside

;	bcc	missile_done

horrible_explosion:

	; clear missile

;	ldy	YSAV
;	lda	#$0
;	sta	(MISSILE_PL),Y

	; clear enemy

;	lda	#$1
;	sta	ENEMY_EXPLODING
;	lda	#$40
;	sta	ENEMY_KIND

;	jsr	inc_score

;	jmp	draw_enemy


missile_done:
missile_not_out:
;	ldy	YSAV
;	iny
;	iny
;	iny
;	sty	YSAV
;	cpy     #(NUM_MISSILES*3)
;	bne	check_missile_loop

     ;=================================
     ; Done missile <> enemy collision
     ;=================================

     ;====================================
     ; check for ship <-> enemy collision
     ;====================================

;	lda     shipx
;	sta	COL_X3
;	clc
;	adc	#8
;	sta	COL_X4		       ; big check is shipx - shipx+8

;	lda	ENEMY_X
;	lsr	A
;	sta	COL_X1
;	clc
;	adc	#2
;	sta	COL_X2		       ; small check enemy_x - enemy_x+2

;	jsr	check_inside	       ; check if overlap

;	bcc	draw_enemy	       ; if not, move ahead

;	lda	#16
;	sta	COL_X3
;	lda	#18
;	sta	COL_X4		       ; big check is 16 - 18

;	lda	ENEMY_Y
;	lsr	A
;	lsr	A
;	sta	COL_X1
;	clc
;	adc	#$1
;	sta	COL_X2		       ; little check is enemy_y - enemy_y+1

;	jsr	check_inside	       ; check if overlap

;	bcc	draw_enemy  	       ; if not, move ahead

	; make the enemy explode

;	lda	#$1
;	sta	ENEMY_EXPLODING
;	lda	#$40
;	sta	ENEMY_KIND

;	dec     SHIELDS
;	jsr 	update_shields	       ; move shields down

;	lda	#<(~PERFECT_SHIELDS)   ; (~PERFECT_SHIELDS)
;	and    	BONUS_FLAGS	       ; remove perfect shield bonus
;	sta	BONUS_FLAGS


     ;=====================================
     ; Done ship <> enemy collision detect
     ;=====================================


draw_enemy:

	; See if the enemy is currently exploding
	; if so, do explosion stuff

check_enemy_explode:
;	lda	ENEMY_EXPLODING	       ; load enemy[i].exploding
;	beq	not_exploding	       ; if 0 then not exploding

handle_exploding:

;	jsr	click		       ; make some noise

;	clc
;	lda	ENEMY_KIND	       ; move to next step in explosion
;	adc	#$4
;	sta	ENEMY_KIND

;	cmp	#$58		       ; have we cycles through explosion?
;	bne	draw_enemy_sprite      ; if not, we are still exploding

;	dec	TOTAL_ENEMIES_OUT      ; total_enemies_out--

;	pla
;	tay			       ; load y
;	pha

;	lda	#$0		       ; enemy[i].out=0
;	sta	(ENEMY_PL),Y

;	jmp	skip_to_next_enemy


        ; point to enemies_x
	; goto enemies_xy


not_exploding:



draw_enemy_sprite:

        ; point to proper sprite

;        lda	#>enemy_sprite0       ; point to the missile sprite
;	sta	STRINGH
;	lda	#<enemy_sprite0
;	sta	STRINGL

;        lda	ENEMY_KIND	      ; get kind
;	and	#$F8

;	clc
;	adc	STRINGL

;	sta	STRINGL
;	lda	#0
;	adc	STRINGH
;	sta	STRINGH


enemies_xy:
;	lda	ENEMY_X	       	       ; get X
;	lsr	A
;	sta	CH
;	lda	ENEMY_Y	       	       ; load it
;	lsr	A
;	lsr	A
;	sta	CV

;	jsr	blit   		       ; blit the missile sprite




save_zp_enemy_back:
	; save zero page copy back to RAM

;	ldx	#ENEMY_EXPLODING

;	pla
;	tay			       ; restore y pointer
;	pha

save_from_zero_page:


;	iny			       ; point to exploding
;	lda	0,X
;	sta	(ENEMY_PL),Y

;	inx
;	cpx	#(ENEMY_XMAX+1)	       ; see if reached end
;	bne	save_from_zero_page    ; if not keep copying


skip_to_next_enemy:

;	pla	  		       ; get saved value of Y

	iny

	cpy	#($20+NUM_ENEMIES)	; have we looped through them all?
	beq	draw_the_boss		; if not, loop
	jmp	handle_enemies


	;===================================================
	;===================================================
	; BOSS STUFF


draw_the_boss:

	;=======================
	; if enemy_type==9
	; we have a boss out

;	lda     ENEMY_TYPE
;	cmp	#$9

;	beq	check_boss_exploding
;	jmp	done_with_boss

check_boss_exploding:
	;================================
	; if not exploding, draw the boss

;	lda	BOSS_EXPLODING
;	bne	skip_draw_boss

;	lda     #$0    	       	       ; boss_y=0
;	sta	CV
;	lda	BOSS_X
;	sta	CH   		       ; boss_x

;	lda	#>boss_sprite
;	sta	STRINGH
;	lda	#<boss_sprite
;	sta	STRINGL
;	jsr	blit   		       ; blit the ship sprite

skip_draw_boss:
	;================================
	; Draw Smoke

;	lda     BOSS_SMOKE
;	beq	skip_draw_smoke

        ; point to proper sprite

;        lda	#>smoke_sprite0       ; point to the missile sprite
;	sta	STRINGH
;	lda	#<smoke_sprite0
;	sta	STRINGL

;	lda	BOSS_SMOKE	      ; get kind

;	and	#$fc		      ; mask off bottom 2 bits

;	clc
;	adc	STRINGL

;	sta	STRINGL
;	lda	#0
;	adc	STRINGH
;	sta	STRINGH

;	lda	BOSS_X	       	       ; get X
;	clc
;	adc	#$5
;	sta	CH
;	lda	#$3	       	       ; load it
;	sta	CV

;	jsr	blit   		       ; blit the missile sprite

;	lda	BOSS_X
;	clc
;	adc 	#$5
;	sec
;	sbc	BOSS_XADD
;	sta	CH

;	lda	#$4
;	sta	CV

;	jsr	blit

	dec	BOSS_SMOKE

skip_draw_smoke:

	;======================
	; BOSS Laser Shoot

;	lda     BOSS_SHOOTING
;	beq	skip_boss_shooting

;	dec	BOSS_SHOOTING

;	jsr	click


;	ldx	#$0
boss_shoot_loop:
;	txa
;	pha

	; point to proper sprite

;        lda	#>laser_sprite0       ; point to the missile sprite
;	sta	STRINGH
;	lda	#<laser_sprite0
;	sta	STRINGL

;	lda	BOSS_SHOOTING	      ; get shooting
;	and	#$1

;	beq	got_right_laser

;	clc
;	lda	#$5
;	adc	STRINGL

;	sta	STRINGL
;	lda	#0
;	adc	STRINGH
;	sta	STRINGH

got_right_laser:

;	lda	BOSS_X
;	sta	CH

;	txa
;	asl

;	clc
;	adc	#$3
;	sta	CV

;	jsr	blit   		       ; blit the laser sprite

;	lda	BOSS_X
;	clc
;	adc	#$C
;	sta	CH

;	pla
;	pha
;	asl
;	clc
;	adc	#$3
;	sta	CV

;	jsr	blit

;	pla
;	tax
;	inx
;	cpx	#$8
;	bne	boss_shoot_loop
;	jsr	click


skip_boss_shooting:

	;=============================
	; boss is dead

;	lda     BOSS_EXPLODING
;	beq	boss_is_not_exploding

;	lda	#$1
;	sta	BOSS_WAITING

;	ldx	#$20
big_explosion:
;	txa
;	pha

	; point to proper sprite

;        lda	#>smoke_sprite0       ; point to the missile sprite
;	sta	STRINGH
;	lda	#<smoke_sprite0
;	sta	STRINGL

 ;       lda	BOSS_EXPLODING	      ; get kind
;	lsr	A
;	lsr	A
;	and	#$0c		      ; mask off bottom 2 bits

;	clc
;	adc	STRINGL

;	sta	STRINGL
;	lda	#0
;	adc	STRINGH
;	sta	STRINGH

;	jsr	random_number
;	and	#$03
;	sta	CV

;	jsr	random_number
;	and	#$07
;	clc
;	adc	BOSS_X
;	sta	CH

;	jsr	blit   		       ; blit the missile sprite

;	pla
;	tax
;	dex
;	bne	big_explosion

;	jsr	click
;	dec	BOSS_EXPLODING

;	bne	not_dead_yet

;	lda	LEVEL	    		; only show ending after level 1
;	cmp	#$1
;	bne	no_ending

;	jmp	do_ending

no_ending:

;	inc	SCOREH			; add 100 to score
;	jsr	print_score

;	lda	LEVEL
;	cmp	#$7  			; level can't be higher than 7
;	beq	start_new_level
;	inc	LEVEL

;	inc	SHIELDS
;	inc	SHIELDS

;	lda	#$A
;	cmp	SHIELDS
;	bpl	start_new_level
;	sta	SHIELDS

start_new_level:
;	jsr	update_shields
;	jmp	new_level

not_dead_yet:
;	jmp  	move_boss

boss_is_not_exploding:

;	dec     BOSS_COUNT
;	bne	move_boss

        ;=========================================
	; Toggle boss waiting state if count is up

;	lda	BOSS_WAITING
;	beq	make_boss_wait
stop_boss_waiting:
;	lda	#$0
;	sta	BOSS_WAITING		; boss_waiting=0
;	jsr	random_number
;	sta	BOSS_COUNT   		; boss_count=rand%256
;	jmp	move_boss

make_boss_wait:
;	lda     #$1
;	sta	BOSS_WAITING

;	jsr	random_number
;	and	#$01f
;	clc
;	adc	#$30
;	sta	BOSS_COUNT

;	lda	#$20
;	sta	BOSS_SHOOTING

move_boss:
;	lda	BOSS_WAITING
;	bne	laser_collision

;	lda	BOSS_X
;	clc
;	adc	BOSS_XADD
;	sta	BOSS_X

;	cmp	#26
;	bpl	boss_reverse

boss_under:
;	cmp	#$0
;	bpl	laser_collision

boss_reverse:

;	lda	#$0	    	       ; load zero
;	sec
;	sbc	BOSS_XADD	       ; 0 - ENEMY_XADD
;	sta	BOSS_XADD	       ; store it back out, negated
;	jmp	move_boss	       ; re-add it in


laser_collision:

        ;================================
        ; Collision detection for lasers
	;

;	lda	BOSS_SHOOTING
;	beq	done_with_boss

left_laser:
;	lda	BOSS_X
;	sta	COL_X1
;	sta	COL_X2
;	lda	shipx
;	sta	COL_X3
;	lda	#$6
;	clc
;	adc	shipx
;	sta	COL_X4
;	jsr	check_inside
;	bcs	laser_hit
;
right_laser:
;	lda  	BOSS_X
;	clc
;	adc	#$C
;	sta	COL_X1
;	sta	COL_X2
;	jsr	check_inside
;	bcc	done_with_boss

laser_hit:
;	lda	BOSS_SHOOTING
;	and	#$3	     		; only take damage 1/8 the time
;	bne	done_with_boss

;	dec	SHIELDS
;	jsr	update_shields




done_with_boss:

	;====================
	; Move Missiles
	;====================
	; Missiles are 8,9,10,11 in sprite table


	ldy	#$20			; point to missile[0] (8*4)
move_missiles:
	jsr	is_sprite_active
	bcc	loop_move_missiles	; if not active, skip

;	phy
;	tya
;	asl
;	asl
;	tay

	lda	$0201,Y
	dea
	sta	$0201,Y

;	ply

	cmp	#200

	; bge
	bcc	missile_collision_detection	; if not off screen, contine

	jsr	deactivate_sprite

	lda	#<(~PERFECT_AIM)		; shot missed!
	and	BONUS_FLAGS			; clear perfect shot flag
	sta	BONUS_FLAGS

	jmp	loop_move_missiles		; continue

missile_collision_detection:

check_missile_boss:
;	lda	ENEMY_TYPE
;	cmp	#$9
;	bne	done_missile_collision
;	dey	; missile x
;	lda	(MISSILE_PL),Y
;	iny		      		; fix y
;	sta	COL_X1
;	sta	COL_X2
;	lda	BOSS_X
;	sta	COL_X3
;	clc
;	adc	#$0d
;	sta	COL_X4
;	jsr	check_inside

;	bcc	done_missile_collision

check_boss_y:
;	lda  	(MISSILE_PL),Y
;	sta	COL_X1
;	clc
;	adc	#$2
;	sta	COL_X2
;	lda	#$0
;	sta	COL_X3
;	lda	#$3
;	sta	COL_X4
;	jsr	check_inside

;	bcc	done_missile_collision

hit_the_boss:
;	dey
;	dey
;	lda	#$0
;	sta	(MISSILE_PL),Y		; missile_out=0
;	lda	#$B
;	sta	BOSS_SMOKE

;	dec	BOSS_HITS
;	bne	loop_move_missiles

;	lda	#$2F
;	sta	BOSS_EXPLODING
;	lda	#$0
;	sta	BOSS_SHOOTING

;	jmp	loop_move_missiles

done_missile_collision:

;	jmp	loop_move_at_y

loop_move_missiles:
	iny
	iny
	iny
	iny
	cpy	#($20+4*NUM_MISSILES)	; have we checked all missiles?
	bne	move_missiles		; if not, loop

done_move_missiles:

	;===============================
	; handle joypad input
	;===============================

game_read_keyboard:
	ldx	joypad1_pressed
	bne	key_start
	jmp	move_ship		; if no keypressed, move the ship

key_start:
	lda	joypad1_pressed_h
	bit	#$10			; start button
	beq	key_left

	; pause game
	; ask for resume/save/toggle_sound/help/quit

key_left:
	bit	#$02
	beq	key_right

	lda	SHIPXADD       	       ; load xadd
	beq	game_l_sub
	bpl	game_l_0	       ; is switch dir, then 0 it
game_l_sub:
	dec	SHIPXADD	       ; else, dec XADD
	jmp	move_ship
game_l_0:
	lda	#$0
	sta	SHIPXADD
	jmp	move_ship

key_right:
	bit	#$01
	beq	key_A

	lda	SHIPXADD	       ; load xadd
	bmi	game_l_0	       ; if we are switching dirs, set to zero
	inc	SHIPXADD	       ; else xadd++
	jmp	move_ship

key_A:
	lda	joypad1_pressed
	bit	#$80
	beq	game_unknown

	ldy	#$20			; point to missile[y] (8*4)
fire_missiles:
	jsr	is_sprite_active
	bcs	end_fire_loop		; if active, skip to next

;	phy
;	tya
;	asl
;	asl
;	tay

	lda	#8
	clc
	adc	shipx			; missile[y].x=shipx+8
	sta	$0200,Y			; $220 + 4*(Y-8)

	lda	#190			; set to 190
	sta	$0201,Y


;	ply
	jsr	activate_sprite

	bra	done_fire_missiles	; done firing missile

end_fire_loop:
	iny				; point to next missile
	iny
	iny
	iny

	cpy	#($20+4*NUM_MISSILES)	; see if we have more missiles
	bne	fire_missiles		; if so, loop

done_fire_missiles:

	jmp	move_ship

game_unknown:


move_ship:
	clc			       ; Clear carry
	lda	shipx		       ; load ship_x
	adc	SHIPXADD	       ; ship_x+=xadd
	sta	shipx		       ; store it back

check_x_under:
	bpl	check_x_over	       ; if positive, keep going
	lda	#$0		       ; we were below zero
	sta	shipx		       ; so shipx=0
	sta	SHIPXADD	       ; xadd=0
	bra	update_shields2	       ; go to blit

check_x_over:
	cmp	#$70		       ; are we over 33?
	bmi	update_shields2	       ; if not, blit ship
	lda	#$0		       ; shipxadd=0
	sta	SHIPXADD
	lda	#$70		       ; shipx=33
	sta	shipx

	; Update shields
update_shields2:

;	jsr	clear_bottom

;	lda	#>shields_string
;	sta	STRINGH
;	lda	#<shields_string
;	sta	STRINGL

;	clc
;	lda	#$2
;	adc	GR_PAGE
;	sta	BASH
;	lda	#$50
;	sta	BASL
;	ldy	#$0

shield_print_loop:

        ;=============================
	; CHECK TO SEE IF GAME IS OVER
	;=============================

;	lda	SHIELDS
;	bmi	done_game

;	lda	(STRINGL),Y
;	sta	(BASL),Y
;	iny
;	cpy	#$1E		       ; string is 30 long
;	bne	shield_print_loop

;	inc	BASH		       ; move to line 23

;	lda	#>score_string
;	sta	STRINGH
;	lda	#<score_string
;	sta	STRINGL

;	ldy	#$0

score_print_loop:
;	lda	(STRINGL),Y
;	sta	(BASL),Y
;	iny
;	cpy	#$26		       ; string is 38 long
;	bne	score_print_loop

;	clc
;	lda	#$80
;	adc	BASL
;	sta	BASL		       ; move to line 24

;	lda	#>level_string
;	sta	STRINGH
;	lda	#<level_string
;	sta	STRINGL

;	ldy	#$0
level_print_loop:
;	lda	(STRINGL),Y
;	sta	(BASL),Y
;	iny
;	cpy	#$C		       ; string is 12 long
;	bne	level_print_loop


	;========================
	; scroll background
	;========================

	lda	star_scroll
	sta	$2110
	lda	star_scroll_h
	sta	$2110

	rep	#$20        ; A/mem=16 bit
.a16
	dec	star_scroll
	sep	#$20        ; A/mem=8 bit
.a8


	;========================
	; update OAM
	;========================

	lda	shipx
	sta	$0200		; set sprite 0 X to shipx


	jmp	level1_loop


done_game:


;	ldx	#30
;	jsr	wait_X_100msec	       ; pause for 3 seconds
;	bit	KEYRESET	       ; clear keyboard

;	jsr	set_page0_text
;	jsr	HOME

;	lda	#>game_over_string
;	sta	STRINGH
;	lda	#<game_over_string
;	sta	STRINGL

;	jsr	print_text_xy

;	jsr	wait_until_keypressed

;	jsr	set_page0_text

see_if_new_hi_score:

	sed
	lda	SCOREH
	cmp	HISCORE_H
	cld

	beq	too_close
	bpl	new_high
	jmp	do_hi_score


too_close:
	sed
        lda	SCOREL
	cmp	HISCORE_L
	cld

	bpl	new_high
	jmp	do_hi_score

new_high:

	; Actually set the high score

	lda	SCOREL
	sta	HISCORE_L
	lda	SCOREH
	sta	HISCORE_H

;	lda	#>(score_string+31)
;	sta	STRINGH
;	lda	#<(score_string+31)
;	sta	STRINGL
	jsr	print_high_score

;	jsr	HOME

	; print new high score message

;	lda	#>new_high_score_string
;	sta	STRINGH
;	lda	#<new_high_score_string
;	sta	STRINGL

;	jsr	print_text_xy
;	jsr	print_text_xy

	; set high score to AAA

	lda	#$C1
	sta	HISCORE_1
	sta	HISCORE_2
	sta	HISCORE_3


	; load initials address

	lda	#$06
	sta	STRINGH
	lda	#$12
	sta	STRINGL


	; load pointer address
;	lda     #$06
;	sta	BASH
;	lda	#$92
;	sta	BASL

	ldy	#$0
	ldx	#$0

initials_loop:

	; erase old pointer

	ldy	#$0
	lda	#$A0
;	sta	(BASL),Y
;	iny
;	sta	(BASL),Y
;	iny
;	sta	(BASL),Y

	txa
	tay

	; draw pointer

;	lda     #$DE			; '^'
;	sta	(BASL),Y


	ldy	#$0

	; draw initials

	lda	HISCORE_1
	sta	(STRINGL),Y

	iny
	lda	HISCORE_2
	sta	(STRINGL),Y

	iny
	lda	HISCORE_3
	sta	(STRINGL),Y

in_key:
;	jsr	wait_until_keypressed
	lda	LASTKEY
	beq	in_key

	cmp	#$D
	beq	do_hi_score

	cmp	#'I'
	bne	in_down
	inc	HISCORE_1,X

in_down:

	cmp	#'M'
	bne	in_left
	dec	HISCORE_1,X

in_left:
	cmp	#'K'
	bne	in_right
	inx

in_right:
	cmp	#'J'
	bne	fix_limits
	dex
fix_limits:

	; Make sure X is between 0 and 2

	cpx     #$0
	bpl	x_high
	ldx	#$2
x_high:
	cpx	#$3
	bmi	x_lo
	ldx	#$0
x_lo:

	jmp	initials_loop


do_hi_score:

;	jsr	HOME

;	lda	#>high_score_string
;	sta	STRINGH
;	lda	#<high_score_string
;	sta	STRINGL

;	jsr	print_text_xy


	; go to screen co-ords 15x12

	lda	#$06
	sta	STRINGH
	lda	#$37
	sta	STRINGL

	; put the initials up

	lda	HISCORE_1
	ldy	#$0
	sta	(STRINGL),Y

	lda	HISCORE_2
	iny
	sta	(STRINGL),Y

	lda	HISCORE_3
	iny
	sta	(STRINGL),Y

	; Print score to screen
	;

	lda	#$06
	sta	STRINGH
	lda	#$3D
	sta	STRINGL

	jsr	print_high_score
;
;	jsr 	wait_until_keypressed

;	jmp 	opener



;=============================
; Level 1 VBLank Routine
;=============================

level1_vblank:
	php		; save status register
	rep	#$30	; Set A/mem=16 bits, X/Y=16 bits (to push all 16 bits)
.a16			; tell assembler the A is 16-bits
.i16
	phb		; save b
	pha		; save A
	phx		; save X
	phy		; save Y
	phd		; save zero page

	sep #$20        ; A/mem=8 bit
.a8

joypad_read:
	lda	$4212		; get joypad status
	and	#$01		; if joy is not ready
	bne	joypad_read	; wait

	rep	#$20		; A 16-bit
.a16

	ldx	joypad1_raw	; load previous raw value

	lda	$4218		; load 2 bytes of joypad status
	sta	joypad1_raw	; save new raw value

	txa			; transfer old raw into A

	eor	joypad1_raw	; set the bits that have changed

	and	joypad1_raw	; set the bits just pressed

	sta	joypad1_pressed	; store the pressed value
	txa			; transfer back prev value
	and	joypad1_raw	; find buttons being held
	sta	joypad1_held	; store held down buttons

	sep	#$20		; A=8bit
.a8


done_joypad:

	;=======================================
	; Update the sprite info structure (OAM)
	;=======================================

	jsr	svmw_transfer_sprite

done_vblank:

	lda	$4210	; Clear NMI flag

	rep	#$30	; A/Mem=16 bits, X/Y=16 bits
.a16
.i16
	pld		; restore saved vaules from stack
	ply
	plx
	pla
	plb

	plp
	rti		; return from interrupt



;==========================================================
; "Random" Number Generator
;==========================================================
        ; from dlyons@Apple.COM (David A Lyons)
        ; posting to comps.sys.apple2 24 November 1992
        ; algorithm from Pop Science ~1980s
        ; when seeded with non-zero, will generate all 255 values
        ; before repeating
.a8
random_number:
	lda	RANDOM_SEED
	bne	random_not_zero
	lda	#$D
random_not_zero:
	asl	A
	bcc	random_num_done
	eor	#$87
random_num_done:
	sta	RANDOM_SEED
	rts


;==========================================================
; Update Shields
;==========================================================
	;

update_shields:

;	lda	#>shields_string
;	sta	STRINGH
;	lda	#<shields_string
;	sta	STRINGL


	ldy	#0

	lda	SHIELDS
	bne	normal_shields


flash_shields:
	lda	#$7f
	and	(STRINGL),Y
	sta	(STRINGL),Y
	iny
	cpy	#$7
	bne	flash_shields
	jmp	shields_line

normal_shields:
	lda	#$80
	ora	(STRINGL),Y
	sta	(STRINGL),Y
	iny
	cpy	#$7
	bne	normal_shields

shields_line:

	ldy  	#$A
	ldx	#$0
shield_line_loop:

	cpx	SHIELDS
	bmi	shield_box
	lda	#'_'+128
	jmp	shield_char
shield_box:
	lda	#' '
shield_char:
	sta	(STRINGL),Y
	iny
	iny
	inx
	cpx	#$A
	bne	shield_line_loop

	rts



;==========================================================
; score_plus_50
;==========================================================
        ;
score_plus_50:

	sed				; enter decimal mode
	clc
	lda	SCOREL
	adc	#$50
	sta	SCOREL			; score+=50

	lda	SCOREH
	adc	#$0
	sta	SCOREH			; carry into high byte if needed

	cld	      			; leave decimal mode

	jmp	print_score

;==========================================================
; inc_score
;==========================================================
        ;
inc_score:
	sed				; enter decimal mode
	clc
	lda	SCOREL
	adc	#$5
	sta	SCOREL			; score+=5

	lda	SCOREH
	adc	#$0
	sta	SCOREH			; carry into high byte if needed

	cld	      			; leave decimal mode


	lda	SCOREL			; if score /100 =0 then inc shields
	bne	print_score

	lda	SHIELDS

	cmp	#$0A   			; don't raise shields higher than 10
	bpl	print_score

	inc	SHIELDS
	jsr	update_shields

print_score:
;	lda	#>(score_string+7)
;	sta	STRINGH
;	lda	#<(score_string+7)
;	sta	STRINGL

;	tya
;	pha				; save Y on stack

;	ldy     #$0

;	lda	SCOREH
;	sta	BCD_BYTEH

;	lda	SCOREL
;	sta	BCD_BYTE
;	jsr	print_bcd_word

;	pla				; restore Y
;	tay

	rts

;======================
; print high_score
;======================
   ; location to output to in STRINGH/STRINGL

print_high_score:
;	tya
;	pha				; save Y on stack

;	ldy     #$0

;	lda	HISCORE_H
;	sta	BCD_BYTEH

;	lda	HISCORE_L
;	sta	BCD_BYTE
;	jsr	print_bcd_word

;	pla				; restore Y
;	tay

	rts

;==========================================================
; check inside
;==========================================================
	; Simple collision detection.  Have small line x1<->x2 
	; Want to see if it overlaps long line x3<---------->x4
	; so:
	;    if ((x1>x3)&&(x1<x4)) || ((x2>x3) && (x2<x4)) inside
	;    else outside

check_inside:

	lda	COL_X1
	cmp	COL_X3
	bmi	check_higher
	cmp	COL_X4
	bmi	inside
check_higher:
	lda	COL_X2
	cmp	COL_X3
	bmi	outside
	cmp	COL_X4
	bpl	outside

inside:
	sec
	rts
outside:
	clc
	rts




;; *********************
;; BSS
;; *********************
.bss

start_bss:
extra_sprite:	  .res 256
end_bss:


;============================================================================
; Sprite/Tile Data
;============================================================================

.segment "HIGHROM"

; sprite data
.include "level1_pal0.sprites"
.include "level1_pal2.sprites"
.include "level1_background.tiles"
.include "star_background.tiles"





