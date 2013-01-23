;
;  tb1_snes.s -- Tom Bombem SNES in 65c816 Assembly v 0.1
;
;               by Vince Weaver  <vince _at_ deater.net>


; Page zero locations

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

	jsr	display_opening

	jsr	display_title

;	jsr	level_1

.include "title_screen.s"
.include "opening.s"
.include "level_1.s"
.include "svmwgraph.s"

;============================================================================

wram_fill_byte:
.byte $00

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
vblank_vector:
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

