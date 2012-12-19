#  Tom Bombem in x86 Assmbler version 0.32
#
#  by Vince Weaver <vince@deater.net>
#
#  assemble with    "as -o tb_asm.o tb_asm.ix86.s"
#  link with        "ld -o tb_asm tb_asm.o"


# GAME CONSTANTS

.equ SCREEN_WIDTH,	40
.equ SCREEN_HEIGHT,	24
.equ BYTES_PER_PIXEL,	 2
.equ NUM_MISSILES,	 2
.equ NUM_ENEMIES,	 6
.equ SCROLL_SPEED,	 2
.equ UP_SHIELDS,	32
.equ WAVE_SIZE,		16 	# power of 2
.equ WAVES_TILL_BOSS,	 6	# plus level*2	

        # from ~/linux/include/asm/unistd.h
.equ SYSCALL_READ,	    3	
.equ SYSCALL_WRITE,   	    4
.equ SYSCALL_OPEN,	    5
.equ SYSCALL_CLOSE,	    6
.equ SYSCALL_IOCTL,	   54
.equ SYSCALL_FCNTL,	   55
.equ SYSCALL_GETTIMEOFDAY, 78
.equ SYSCALL_NANOSLEEP,	  162

        # from ~/linux/include/asm/ioctls.h
.equ TCGETS,   0x5401
.equ TCSETS,   0x5402

        # from ~/linux/include/asm/fcntl.h
.equ F_GETFL,  	      3
.equ F_SETFL,         4
.equ O_NONBLOCK, 0x0800
.equ O_RDONLY,	  0x000
.equ O_WRONLY,    0x001
.equ O_CREAT,	  0x040
.equ O_TRUNC,	  0x200
.equ SIXSIXSIX,	   0666



        # from ~/linux/include/asm/termbits.h
.equ C_LFLAG_OFF, 12	
.equ VMIN_OFF,	  23
.equ ICANON,	   2
.equ ECHO,	   8

       # struct offsets
.equ ENEMIES_OUT,   		0
.equ ENEMIES_EXPLODING,		1
.equ ENEMIES_XADD,		2
.equ ENEMIES_YADD,		3
.equ ENEMIES_XMIN,		4
.equ ENEMIES_XMAX,		5
.equ ENEMIES_KIND,		6
.equ ENEMIES_X,			7
.equ ENEMIES_Y,			9
.equ ENEMIES_SIZE,		11

.equ MISSILE_OUT,		0
.equ MISSILE_X,			1
.equ MISSILE_Y,			2


      # game_flags bit
.equ PAUSED,	     0x01
.equ SOUND,	     0x02
.equ SOUND_PENDING,  0x04 
.equ PERFECT_SHIELD, 0x08
.equ PERFECT_ENEMY,  0x10
.equ PERFECT_SHOT,   0x20
.equ ALL_THREE,	     PERFECT_SHIELD|PERFECT_ENEMY|PERFECT_SHOT

.include "data.header"

	.globl _start
_start:	

	#####################################
	# decompress data segment
		
# LZSS decompression algorithm implementation
# by Stephan Walter 2002, based on LZSS.C by Haruhiko Okumura 1989
# optimized some more by Vince Weaver

        # we used to fill the buffer with FREQUENT_CHAR
	# but, that only gains us one byte of space in the lzss image.
        # the lzss algorithm does automatic RLE... pretty clever
	# so we compress with NUL as FREQUENT_CHAR and it is pre-done for us


	mov     $(N-F), %bp   	     	# R
	mov  	$tb_data_begin, %esi	# %esi points to logo (for lodsb)
	mov	$DATA_OFFSET, %edi	# point to out_buffer

decompression_loop:	
	lodsb			# load in a byte

	mov 	$0xff, %bh	# re-load top as a hackish 8-bit counter
	mov 	%al, %bl	# move in the flags

test_flags:
	cmp	$tb_data_end, %esi # have we reached the end?
	jge	done_decompress    # if so, exit

	shr 	$1, %ebx	# shift bottom bit into carry flag
	jc	discreet_char	# if set, we jump to discreet char

offset_length:
	lodsw                   # get match_length and match_position
	mov %eax,%edx		# copy to edx
	    			# no need to mask dx, as we do it
				# by default in output_loop
	
	shr $(P_BITS),%eax	
	add $(THRESHOLD+1),%al
    
	mov %al,%cl             # cl = (ax >> P_BITS) + THRESHOLD + 1
                                #                       (=match_length)
		
output_loop:
        and 	$POSITION_MASK,%dh  	# mask it
	mov 	text_buf(%edx), %al	# load byte from text_buf[]
	inc 	%edx	    		# advance pointer in text_buf
store_byte:	
	stosb				# store it
	
	mov     %al, text_buf(%ebp)	# store also to text_buf[r]
	inc 	%ebp 			# r++
	and 	$(N-1), %bp		# mask r

	loop 	output_loop		# repeat until k>j
	
	or	%bh,%bh			# if 0 we shifted through 8 and must
	jnz	test_flags		# re-load flags
	
	jmp 	decompression_loop

discreet_char:
	lodsb				# load a byte
	inc	%ecx			# we set ecx to one so byte
					# will be output once
					# (how do we know ecx is zero?)
					
        jmp     store_byte              # and cleverly store it

done_decompress:
# end of LZSS code


   	#####################################
        # setup non-echo mode 
        #
     
         			    	# ioctl(0,TCGETS,&old_tty)  
				    	# AKA tcgetattr(0,&old_tty); 

 	mov	$old_tty,%edx	    	# save values to retstore at end
	call	tcgetattr
	
	mov	$new_tty,%edx	    	# get values to modify
	call	tcgetattr

        		 	    	# turn off "canonical mode" 
				    	# by clearing ICANON bit
        			    	# new_tty.c_lflag&=~ICANON;
	
	andl    $(~ICANON),(new_tty+C_LFLAG_OFF)
									
				    	# turn off "echo mode" 
				    	# by clearing ECHO bit
				    	# new_tty.c_lflag&=~ECHO;

        andl   $(~ECHO),(new_tty+C_LFLAG_OFF)

	       			        # set minimum number of characters 
				    	# to read as 1
        			    	#   new_tty.c_cc[VMIN]=1;

	movb 	$1,(new_tty+VMIN_OFF)

				     	# apply our new tty values
	call	tcsetattr		# %edx still points to new_tty
	
	
	##################################
        # make stdin non-blocking
        # 
	  				# use fctl rather than fctl64
	push    $SYSCALL_FCNTL		# for compatibility (no need of 2GB)
	pop	%eax
	xor	%ebx,%ebx		# stdin = 0
	push	$F_GETFL		# we want to get flags
	pop	%ecx
	int	$0x80

	mov	%eax,%edx		# copy result of GETFL to edx
	orw	$O_NONBLOCK,%dx		# set the NONBLOCK bit
	
	push	$SYSCALL_FCNTL
	pop	%eax	      		# use fctl again
	xor	%ebx,%ebx		# stdin = 0
	push	$F_SETFL		# want to set flags this time
	pop	%ecx
	int	$0x80

	###############################
        # clear the framebuffer
	
	call	clear_framebuffer

	###############################
        # clear the screen 

	mov     $clear_screen,%ecx	# send the escape code to clear screen
	call 	write_stdout

	##############################
        # draw VMW logo

	mov     $vmw_sprite,%esi        # Load in sprite
	mov	$((0x8<<8)+0x6),%ax     # Load in x and y co-ords
	call	blit_219

		    			# put "A VMW Software Production"
	            			# white, x=7, y=15
	mov	$vmw_string,%esi
	call	put_text_line_inline

        call	dump_to_screen	        # dump framebuffer to screen

	push	$3
	pop	%ecx		        # Pause 3 seconds
	call	pause_a_while
   
   	################################
	# Main Menu Loop
   
opening_screen:  

	###############################
        # clear the framebuffer

	call	clear_framebuffer
 
 	##############################
        # title screen

 	mov     $opener_sprite,%esi    # load the title sprite
	xor	%eax,%eax	       # at 0,0
	call	blit_219
		    		       # Yellow, title_string,x=0 y=5
	mov	$title_string,%esi     # Malicious Marketers
	call	put_text_line_inline
	   
        call	dump_to_screen	       # copy to screen

	#########################
	# pause for 10 seconds
	# or less if key pressed
	
	push    $10
	pop	%ecx
	call	pause_a_while

	#########################
	# set variables

	xor	%ebp,%ebp    	       # menu-item=0

	#########################
	# draw attention block
	#

	xor	%bl,%bl		       # color = 0
	call	put_attention_block

	####################################
	# put the "press F1 for help" string
	
	mov	$menu_help_string,%esi      	# color grey x=0 y=20
	call	put_text_line_inline
	
        ####################################
	# draw the menu
	
	mov	$((5<<16)+(10<<8)+6),%eax       # x=10 y=6
	call 	do_menu

        cmp	$0,%bp
	jne	check_1
	call	new_game
	jmp	opening_screen	
check_1:	
	cmp	$1,%bp
	jne	check_2
	call	do_about
check_2:	
	cmp	$2,%bp
	jne	check_3
	call	do_story
	jmp	opening_screen
check_3:	
	cmp	$3,%bp
	jne	check_4
	call	do_hi_score
	call	wait_until_keypressed        

check_4:		
	cmp	$4,%bp
	jne	check_5
	call	verify_quit
	jnz	exit
	jmp	opening_screen
check_5:	
	cmp	$5,%bp
	jne	end_handle_lf
	call	display_help

end_handle_lf:
	jmp	opening_screen	 
 
 
        #################################
	# Exit
	#################################
exit:

     	mov	$default_colors,%ecx	# restore from line-draw mode
	call	write_stdout

	andb	$(~SOUND),game_flags	# turn sound off
	mov	$(750<<16)+125,%ebx	# default sound=750Hz 125ms
	call	play_sound		# No way I know of to save/restore 
					# actual values

	mov	$old_tty,%edx		# restore the saved tty values
	call	tcsetattr

     	xor     %ebx,%ebx		# set return value as zero
	xor     %eax,%eax
	inc     %eax 	 		# put exit syscall number (1) in eax
        int     $0x80                   # and exit



	#################################
	# verify quit
	#################################
	# zero flag clear if we do want to quit
verify_quit:

	mov 	$15,%bl
	call	put_attention_block	# put a white attention block

	mov     $quit_line,%esi
	call	put_text_line_inline	# put the text

	xor	%ebp,%ebp
	mov	$(5<<24)+(2<<16)+(9<<8)+8,%eax
					# offset 5, 2 lines
					# at 9,8

					# slide into do_menu


	#########################
	# Do menu
	#########################
	# ax= x,y   dx=label_offset,num_options
	# returns %bp=option
	# destroys %ecx
do_menu:	
	push	%eax

write_menu_loop:
	xor	%ecx,%ecx	        # clear line counter
	pop	%eax		        # restore x,y,label_offset,num_options
	push	%eax

do_menu_loop:	
	mov	%eax,%edx
	ror	$16,%edx 	        # move label_offset,num_options -> dx
	
	cmp	%ecx,%ebp	        # are we the selected menu?
	jne	menu_no_arrow	        # if not, no_arrow
	
	mov	$menu_arrow,%esi        # if so, arrow
	mov	$13,%bh		        # set text color pink

	jmp	write_menu_text
	
menu_no_arrow:
        mov	$menu_blank,%esi        # not selected, so blank
        mov	$5,%bh		        # set text color purple
	
write_menu_text:
	mov	$14,%bl		        # yellow arrow
	call	put_text_xy	        # write out the arrow or blank
	
	mov	%bh,%bl		        # swap in text color
	
			                # point to the proper string
				       
	add	%dh,%cl
	lea	menu_new_string(%ecx,%ecx,8),%esi
	sub	%dh,%cl
	
	add	$5,%ah	 	        # text 5 right of arrow

	call	put_text_xy	        # put the text
	
	inc	%al		        # increment y
	sub	$5,%ah		        # restore x
	inc	%ecx		        # increment the count

	cmp	%dl,%cl		        # have we shown them all?
	jne	do_menu_loop	        # if not, loop

        call	dump_to_screen	        # dump results to screen
  
menu_key_loop:  
        call	get_a_char	        # get a character
	jnc	menu_check_key	        # if valid key, check it
	call	milisec_nanosleep
	jmp	menu_key_loop
       
menu_check_key:       
	pop    %edx   		        # convoluted way
	push   %edx		       	# to get num_options into %dl
	shr    $16,%edx	
	xor    %dh,%dh
	
check_q:
	cmp     $'q',%al	        # are we 'q'?
	jne	check_h
	mov	%dx,%bp			# if so move to last menu item
	dec	%bp
check_h:
	cmp	$'h',%al		# are we 'h'?
	jne	check_i
	mov	$5,%bp 			# if so move beyond and 
	mov	$'\n',%al		# trigger enter
	
check_i:
	cmp	$'i',%al		# are we up?
	jne	check_m
dec_menu:
	sub	$1,%bp
	jns	write_menu_loop		# if not, ok
	mov	%dx,%bp			# if so, wrap
	dec	%bp
	jmp	write_menu_loop
	
check_m:
	cmp	$'m',%al		# are we down?
	jne	check_j
inc_menu:
	inc	%bp			# move down
	cmp	%dx,%bp			# are we too far?
	jne	write_menu_loop		# if not ok
	xor	%bp,%bp			# if so, wrap
	jmp	write_menu_loop
	
check_j:
	cmp	$'j',%al		# are we right?
	je	dec_menu		# then pretend we are down
	
check_k:
	cmp	$'k',%al		# are we left?
	je	inc_menu		# then pretend we are up
	
check_LF:
	cmp	$'\n',%al		# check for enter/space
	je	exit_menu
	cmp	$'\r',%al
	je	exit_menu
	cmp	$' ',%al
	je	exit_menu

	jmp 	write_menu_loop		# unknown key, loop
	
exit_menu:
	pop	%eax			# clean up stack
	xor	$0,%ebp			# set zero flag with result
		
	ret





	######################
	# NEW GAME
	######################
	#
	
new_game:
	 
	# re-clear all bss variables to zero

	xor	%eax,%eax
	mov	$(game_vars),%edi
	mov	$(end_game_vars-game_vars),%ecx

	rep 	
	stosb

	call	load_hi_score	        # load hi_score

	###########################
	# init vars that need to be initialized

        movw	$(18<<8),x		# ship's x=18
	movb	$8,shields		# shields start as 8
	movb	$1,level		# level starts as 1

	##########################
	# Starting a new level!

new_level:
	call	clear_framebuffer	# clear screen

	mov	$8,%bl
	call	put_attention_block	# put attention block

	mov	$menu_blank,%esi
	mov	$15,%bl
	mov	$8,%al
	mov	%bl,%ah
	call	put_text_xy
	mov	$menu_blank,%esi
	mov	$20,%ah
	call	put_text_xy		# clear an opening
	

	mov	$level_line+3,%esi
	mov	$16,%ah
	call	put_text_xy		# print "LEVEL:"

	xor	%eax,%eax
        movb    level,%al               # Convert level to ascii
	mov     $level_string,%edi
	call    num_to_ascii
			
	mov     $level_string,%esi      # print level_string
	mov     $((23<<8)+8),%eax
	mov     $15,%bl
	call    put_text_xy
							
	call	dump_to_screen	   	
	
        push	$2	     		# Pause 2 seconds or until keypress
     	pop	%ecx
	call	pause_a_while

	##############################
	# set new_level variables
	# hackish but decreses code size
	
	mov	$enemies_spawned,%edi
	xor     %eax,%eax
	inc	%eax	 		# eax = 1
	stosw				# enemies_spawned = 1
	neg     %ax			# ax=-1
	stosw				# current_init_x = -1
	stosb				# current_enemy_kind = -1
	push 	$30
	pop	%eax			# al=30
	stosb				# enemy_wait=30
	xor  %eax,%eax			# al=0
	
#	add  $6,%al
	
	stosb				# current_enemy_type=0

#	xor  %eax,%eax	
	stosb				# enemy_wave=0
		
	orb	$ALL_THREE,game_flags	# set the three bonuses to ON
	
	movb	level,%al		# mave waves_till_boss
	add	%al,%al			# equal WAVES_TILL_BOSS + 2*level
	addb	$WAVES_TILL_BOSS,%al
	mov	%al,waves_till_boss

	########################
     	# RANDOMIZE TIMER
	
	call randomize_timer

	# Draw the Stars

	xor    	%eax,%eax		# clear eax
	
	mov	$1600,%ecx		# clear the background
	mov	$background,%edi
	push	%edi
	rep	
	stosb
	
	mov     $100,%ecx		# we want to draw 100 stars
draw_stars:
	pop	%edi			# point to "background"
	push	%edi
	push	$40
	pop	%ebx			# move 40 into bx
	call 	random			# call rand()%40 (y)
	imulb	%bl			# multiply result by 40 (screen width)
	add	%eax,%edi		# add to background pointer
	
	call	random			# call rand()%40 again (x)
	add	%eax,%edi		# add this to background pointer
	
	push	$2
	pop	%ebx			# call rand()%2
	call	random
	
	add	$7,%al			# put either light or dark grey star
	
	stosb				# and store it
	
	loop 	draw_stars		# loop
	pop	%edi

main_game_loop:

	############
	# clear screen
	############

	call	clear_framebuffer	# clear the screen
       
        ###########
	# Scroll the Stars
	###########

        mov	scroll_frames,%bx	 # get scroll val from memory
	mov	scroll,%ax		 # get scroll value
	inc	%bx			 # increment frames
	cmp	$SCROLL_SPEED,%bx	 # have we had enough frames to scroll?
	jge	scroll_it      	 	 # if so move ahead
	mov	%bx,scroll_frames	 # else store scroll back
	jmp	show_stars		 # and jump to the stars
	
scroll_it:
        movw	$0,scroll_frames	 # clear scroll_frames
	
	sub	$40,%ax			 # decrement it
	jns	write_scroll		 # is it less than zero?
	mov	$(40*40-40),%ax	    	 # if so wrap to (40*40-40)
write_scroll:
	movw  	%ax,scroll		 # store new scroll value to RAM
        
show_stars:
	mov	$background,%esi	# background as src
        mov	$40,%bl	     		# 40 wide
	mov	$'.',%dl		# stars are dots
		
	cmp	$(20*40),%ax		# do we have to wrap?
	jg	half_scroll		# if so we need 2 blits

	#########
	# plain 40x20 blit

	add	%eax,%esi		# else just offset into background
	xor	%eax,%eax		# put at 0,0
	mov	$20,%dh			# 20 high

	call	blit_already_know_xsize_ysize
	jmp	done_scrolling		# done putting stars

half_scroll:

	##############
	# top half of wrap blit
	
	add	%eax,%esi      		# offset into background
	idiv	%bl			# divide offset by 40 to get scroll
	
	push	%eax			# store this offset
	
	# scroll=%al
	# we want x=0,y=0
	# we want xsize=40,ysize=40-scroll
	# we want framebuffer=background+40*scroll
	
	mov  	$40,%dh
	sub  	%al,%dh  		# ysize=40-scroll
	
	xor	%eax,%eax		# x=0,y=0

	call	blit_already_know_xsize_ysize	

	################
	# bottom half of wrap bit

	mov	$background,%esi 	# start at top of background
	pop	%eax			# restore scroll value
	
	# scroll=%al
	# we want y=40-scroll
	# we want ysize=scroll-20
	
	mov  	%al,%dh
	sub  	$20,%dh  		# ysize=scroll-20
	
	neg	%al
	add	$40,%al			# y=40-scroll		
	xor	%ah,%ah			# x=0

	call	blit_already_know_xsize_ysize	
	
 
done_scrolling:

 	#############
 	# Put out new enemies 
	#############
		
	#################################################
    	# Keep a count of how close to place enemies 
     	# Don't want all to appear at once           
	
	incb	between_enemy_delay 	# between_enemy_delay++
	movb	enemy_wait,%al		# get time to wait
	cmp	%al,between_enemy_delay	# is delay>wait?
	jne	move_enemies		# if not, no enemies this frame

	movb	$0,between_enemy_delay	# reset between_enemy_delay

	###########################
	# See if we want to change the current enemy type

	cmpb    $7,current_enemy_type
	jne	not_boss
	movw	$1,enemies_spawned	# don't keep enemy count w boss
not_boss:	
	  
	# each wave is WAVE_SIZE-1.  Should we make this random?
	  
	mov     enemies_spawned,%ax
	test	$(WAVE_SIZE-1),%al      # See if multiple of 16
	jnz	same_enemy_type
	  
	  
	###############################
	# CHANGE THE ENEMY TYPE
	  
	incw	enemies_spawned		# if we keep at multiple of 16
					# we have problems if enemy
					# not output right away. so inc
	
	push	$6
	pop	%ebx
	call	random	
	mov	%al,current_enemy_type	# current_enemy_type=rand()%6
	
	incb	enemy_wave		# increment which wave we are on
		
	movb	waves_till_boss,%al			
	cmpb	%al,enemy_wave		# have we reached boss yet?
	jl	not_boss_yet		# if not, no boss yet
	
	movb	$6,current_enemy_type	# if so, set type to prepare_for_boss
	
not_boss_yet:	

	#####################################
	# Set some constants
	# These can be over-ridden later?
	
set_enemy_constants:

	#################
	# issue speed
	
	mov	$25,%ah			# enemy wait standard is 25

	mov	level,%al
	dec	%al	 		# 1 -> 0 for better math
	shl	$3,%al
	sub	%al,%ah			# adjust for level
	
	cmp	$5,current_enemy_type	
	jne	check_wait_underflow
	sub	$10,%ah
check_wait_underflow:	
	cmp	$0,%ah
	jge	write_out_enemy_wait
	xor	%ah,%ah
	
write_out_enemy_wait:	
	movb	%ah,enemy_wait


	movb	$-1,current_enemy_kind
	movw	$-1,current_init_x
	
same_enemy_type:

	#############################
	# Find an empty enemy slot
	
	mov	$enemy_0,%ebp		# point to first enemy
	push	$NUM_ENEMIES
	pop	%ecx	    		# loop through all enemies
	
find_enemy_slot:	
	mov	%ebp,%esi
	lodsb				# get enemy[i].out	
	test	$1,%al			# is the enemy out?
	jz	add_enemy		# if not keep going

	add	$ENEMIES_SIZE,%ebp	# move to next enemy struct
	loop	find_enemy_slot		# and loop

	jmp	move_enemies

add_enemy:

	##################
	# first see if need to wait till all enemies gone
	# (types 2 and 6 do this)

	cmpb	$2,current_enemy_type	
	jne	check_type_6
	
	movb	total_enemies_out,%al	# are total enemies_out >0?
	cmp	$0,%al
	jne	move_enemies		# if so, wait longer


	movb	$3,current_enemy_type	# all gone, so move on to type 3

	push	$6
	pop	%ebx
	call	random
	mov	%al,current_enemy_kind	# current_enemy_kind=rand()%6;
	
	push	$36
	pop	%ebx
	call	random
	mov	%al,%ah			# ah is the significant part
	mov	%ax,current_init_x	# current_init_x=rand()%36
	jmp	setup_enemy_defaults

check_type_6:
	cmpb	$6,current_enemy_type	
	jne	check_type_7
	
	################
	# before boss stuff
	
	movb	total_enemies_out,%al		# are total enemies_out >0?
	cmp	$0,%al
	jne	move_enemies			# if so, wait longer
	
	mov	$9,%bl
	call	put_attention_block		# put up a text_box
	
	mov	$bonus_points_line,%esi		# print bonus point title
	call	put_text_line_inline
	
	testb	$(ALL_THREE),game_flags		# see if we have a bonus
	jz	no_bonus_at_all
	
	testb	$(PERFECT_SHIELD),game_flags	# do we have perfect shields?
	jz	no_pshield_bonus

	mov	$no_shield_line,%esi		# if so print message
	call	put_text_line_inline
	addl	$1000,score	    		# add 1000 to score
	
no_pshield_bonus:

	testb	$(PERFECT_ENEMY),game_flags	# did we kill all enemies?
	jz	no_penemy_bonus
	
	mov	$shot_every_line,%esi		# if so print message
	call	put_text_line_inline		
	addl	$5000,score			# add 5000 to score
	
no_penemy_bonus:

	testb	$(PERFECT_SHOT),game_flags	# did we never miss a shot?
	jz	no_pshot_bonus
	
	mov	$perfect_shot_line,%esi		# if so, print message
	call	put_text_line_inline
	addl	$5000,score	    		# add 5000 to score

no_pshot_bonus:
	jmp     done_showing_bonus
	
no_bonus_at_all:
	mov	$no_bonus_line,%esi		# print no bonus message
	call	put_text_line_inline
	
done_showing_bonus:
	call	dump_to_screen
	
        call	wait_long_time
	call	wait_until_keypressed

	#################
	# setup boss

	mov	$boss_x,%edi
	xor	%eax,%eax
	mov	$13,%ah
	stosw	       			# movw $(13<<8),boss_x
	mov	$1,%ah
	stosw	      			# movw	$(1<<8),boss_xadd

	push	$40
	pop	%ebx
	call	random
	add	$10,%ax
	stosw	       			# movw	%ax,boss_count

	xor     %al,%al
	stosb	       			# movb 	$0,boss_smoke
	stosb				# movb	$0,boss_exploding
	inc	%al
	stosb	   			# movb    $1,boss_waiting
	
	mov	level,%al
	shl	$2,%al
	add	$20,%al
	stosb	       			# movb	%al,boss_hits

	movb	$7,current_enemy_type
	movb	$20,enemy_wait
	jmp	move_enemies
	
check_type_7:

	# if boss, and he's waiting, don't produce enemies

	cmpb  	$7,current_enemy_type
	jne	setup_enemy_defaults
	cmpb	$0,boss_waiting
	jne	move_enemies
	 
	
	###############################
	# setup enemy defaults
setup_enemy_defaults:
 	incw	enemies_spawned		    # enemies_spawned++
        incw	total_enemies_out	    # total_enemies_out++;

	mov	%ebp,%edi
	mov	$1,%al
	stosb	      		        # movb	$1,ENEMIES_OUT(%ebp)        
					# enemies[i].out=1
	xor	%al,%al
	stosb  				# movb	$0,ENEMIES_EXPLODING(%ebp)  
					# enemies[i].exploding=0;


	add	$2,%edi			# skip xadd,yadd

	stosb				# movb	$0,ENEMIES_XMIN(%ebp)	    
					# enemies[i].xmin=0;
	mov	$37,%al
	stosb	       			# movb	$37,ENEMIES_XMAX(%ebp)	    
					# enemies[i].xmax=37;
					
	
	
	##########################################
	# if enemy_kind < 0 then kind=random()%6
	# else use enemy_kind value

	movb    current_enemy_kind,%al	# get enemy_kind
	cmp	$0,%al			# is it less than 0?
	jge	no_random_kind		# if not move ahead
	push	$6
	pop	%ebx			# get rand()%6
	call	random
no_random_kind:
	stosb  				# movb	%al,ENEMIES_KIND(%ebp)  
					# enemies[i].kind=enemy_kind

	######################################
	# if init_x < 0 then x=random()%37
	# else, use the "init_x" value

	movw     current_init_x,%ax   	# get init_x
	cmp	 $0,%ax			# is it less than zero?
	jge	 no_random_x		# if so skip ahead
random_x:	
        push	 $36
	pop	 %ebx			# get random%36
	call	 random
	mov	 %al,%ah		# high byte is significant
no_random_x:
	stosw				# movw    %ax,ENEMIES_X(%ebp)	
					# store enemies[i].x
	xor	 %eax,%eax
	stosw	 	  		# movw	$0,ENEMIES_Y(%ebp)	    
					# enemies[i].y=0;
	

	#################################
	# enemy type specific inits

	movb	current_enemy_type,%bl
	movb	level,%dl
	shl	$4,%dl
	cmp	$0,%bl
	jne	enemy_type_1

	#####################
	# ENEMY_TYPE 0
	# diagonal, no wait, movement proportional to level
	
enemy_type_0:	
	movb	%dl,ENEMIES_XADD(%ebp)	# xadd = level*8
	movb	%dl,ENEMIES_YADD(%ebp)	# yadd = level*8
	jmp	move_enemies

	##########################
	# ENEMY_TYPE 1
	# Horizontal, then fall.
	
enemy_type_1:
	sub  	$1,%bl
	jnz	enemy_type_3

	mov	$5*8,%al
	movb	%al,ENEMIES_XADD(%ebp)	# enemies[i].xadd=5*8; 

	push	$100
	pop	%ebx
	call	random
	neg	%al
	movb	%al,ENEMIES_YADD(%ebp)	# enemies[i].yadd=-(rand()%100);

	jmp	move_enemies

	##########################
	# Enemy Type 3
	# type two waits for clear
	# then diagonal, all of same type

enemy_type_3:  		     	     	# skip type 2
	sub  	$2,%bl
	jnz	enemy_type_4

	mov	$5*8,%al
	movb	%al,ENEMIES_XADD(%ebp)	# enemies[i].xadd=5; 

	movb	%dl,ENEMIES_YADD(%ebp)  # enemies[i].yadd=level;

	jmp	move_enemies


	###########################
	# Enemy Type 4
	# "wiggle"

enemy_type_4:
	sub  	$1,%bl
	jnz	enemy_type_5
	
	mov	$4*8,%al
	movb	%al,ENEMIES_XADD(%ebp)	# enemies[i].xadd=4;

	movb	%dl,ENEMIES_YADD(%ebp)  # enemies[i].yadd=level;

	push	$34
	pop	%ebx
	call	random
	add	$2,%al

	movb	%al,ENEMIES_XMIN(%ebp)	# enemies[i].xmin=2+rand()%34;
	shl	$8,%ax
	movw	%ax,ENEMIES_X(%ebp)	# enemies[i].x=enemies[i].xmin*8;
	
	push	$20
	pop	%ebx
	call	random
	mov	%al,%dl
	mov	ENEMIES_XMIN(%ebp),%al
	add	%dl,%al
	
	cmp	$35,%al
	jle	xmax_ok
	mov	$35,%al
	
xmax_ok:
	mov	%al,ENEMIES_XMAX(%ebp)	# enemies[i].xmax=enemies[i].xmin+(rand()%20);
	
	jmp	move_enemies

	#########################
	# Enemy Type 5
	# "rain"

enemy_type_5:
        sub  	$1,%bl
	jnz	enemy_type_7

	push	$4
	pop	%ebx
	call	random 			# get rand()%4
		
	test	$3,%al			# see if bottom two bits 00
	jnz	no_use_own_x		# ak, one chance in 4
	
	mov	x,%ax			# one chance in 4 use 
	mov	%ax,ENEMIES_X(%ebp)	# enemies[i].x=x;
	
no_use_own_x:	
	
	movb	$0,ENEMIES_XADD(%ebp)		# enemies[i].xadd=0;
	movb	%dl,ENEMIES_YADD(%ebp)		# enemies[i].yadd=2;

	jmp	move_enemies

	#########################
	# Enemy Type 7
	# things flung by boss

enemy_type_7:
	movb    $0,ENEMIES_XADD(%ebp)		# enemies[i].xadd=0; 
	movb	%dl,ENEMIES_YADD(%ebp)		# enemies[i].yadd=2;
	
	mov	boss_x,%ax
	add	$5,%ah
	mov	%ax,ENEMIES_X(%ebp)	# enemies[i].x=(boss.x+5)*8;
	movw	$(3<<8),ENEMIES_Y(%ebp)	# enemies[i].y=3*8;

  	#######################
	# Move Enemies
	#######################
	
move_enemies:       
       
	mov	$enemy_0,%ebp		# point to first enemy
	push	$NUM_ENEMIES
	pop	%ecx	    		# loop through all enemies
	
move_enemies_loop:	
	mov	%ebp,%esi
	lodsb				# get enemy[i].out	
	test	$1,%al			# is the enemy out?
	jz	loop_move_enemies	# if so keep looping

move_enemies_x:
	xor     %eax,%eax
	inc	%esi	 		# point to xadd
	
	lodsb
	cmp	$0,%al			# is it zero?
	je	move_enemies_y		# if so, move along to y

	cbw				# sign extend al to ax
	addw	%ax,ENEMIES_X(%ebp)	# enemies[i].x+=enemies[i].xadd
		
	# check x bounds
	
	mov	ENEMIES_X(%ebp),%dx
	cmp	ENEMIES_XMAX(%ebp),%dh	# is enemies[i].x>enemies[i].xmax?
	jg	switch_x_dir		# if so, switch direction
	cmp	ENEMIES_XMIN(%ebp),%dh	# is enemies[i].x<enemies[i].xmin?
	jl	switch_x_dir		# if so, switch direction
	jmp	move_enemies_y
	
switch_x_dir:	
	neg	%al
	mov	%al,ENEMIES_XADD(%ebp)
	cbw
	addw	%ax,ENEMIES_X(%ebp)
	
move_enemies_y:

	xor     %eax,%eax
	lodsb
	
	# check for special case II 
	# if yadd<0 do back and forth until yadd positive
	
	cmp	$0,%al			    # is it less than zero?
	jge	no_special_case		    # if not, do normal thing
	
	addb    $1,ENEMIES_YADD(%ebp)	    # enemies[i].yadd++
#	cmpb	$0,ENEMIES_YADD(%ebp)	    # are we zero?
	jnz     ship_enemy_collision_detect # if not, keep going
	
	mov	level,%al		    # done <-> now fall
	shl	$4,%al			    # 
	mov	%al,ENEMIES_YADD(%ebp)	    # enemies[i].yadd=2*level;
	movb    $0,ENEMIES_XADD(%ebp)	    # enemies[i].xadd=0;

	jmp	ship_enemy_collision_detect

no_special_case:

	add	%ax,ENEMIES_Y(%ebp)	    # enemies[i].y+=yadd

	cmpb	$18,ENEMIES_Y+1(%ebp)	    # is y>18?
	jle	ship_enemy_collision_detect # if not move ahead
	
	decb	total_enemies_out	    # we are off screen.  remove
	movb	$0,ENEMIES_OUT(%ebp)	    # enemies[i].out=0
	andb	$~PERFECT_ENEMY,game_flags

ship_enemy_collision_detect:

	##########################################
	# Ship <-> Enemies Collision Detection
	##########################################
	
	cmpb	$0,ENEMIES_EXPLODING(%ebp)  # if enemy is exploding
	jne	loop_move_enemies	    # don't check for collision
	
	movw	x,%ax			    # check x
	mov	%ah,%al
	add	$6,%al 			    # through x+6

	movw	ENEMIES_X(%ebp),%dx	    # check enemies[i].x
	mov	%dh,%dl
	add	$2,%dl 			    # through enemies[i].x+2
	
	call	check_inside
	jnc	loop_move_enemies
	
	mov	$16,%ah			    # check y
	mov	%ah,%al
	add	$2,%al 			    # through y+2

	movw	ENEMIES_Y(%ebp),%dx	    # check enemies[i].y
	mov	%dh,%dl
	add	$1,%dl 			    # through enemies[i].y+1
	
	call	check_inside
	jnc	loop_move_enemies
	
	movb	$1,ENEMIES_EXPLODING(%ebp)
	decb	shields

	andb	$~PERFECT_SHIELD,game_flags

	movl	$(120<<16+50),sound_freq
	orb	$SOUND_PENDING,game_flags	# set_sound(120,50);
      
loop_move_enemies:
	add	$ENEMIES_SIZE,%ebp	# move to next enemy struct
	sub	$1,%ecx			# loop, but more than 128 away
	jnz	move_enemies_loop	# so have to do it by hand
	
        ###################
	# Draw the Enemies
	###################

	# Make out,exploding flags?
	# out,exploding,kind,x,y
        # xadd,yadd,xmin,xmax
	
	mov	$enemy_0,%ebp		# point to first enemy
	push	$NUM_ENEMIES
	pop	%ecx	    		# loop through all enemies
	
draw_enemies:	
	mov	%ebp,%esi
	lodsb				# get enemy[i].out	
	test	$1,%al			# is the enemy out?
	jz	loop_draw_enemies	# if so keep looping

	xor	%eax,%eax
	lodsb				# load "exploding" info
	cmp	$0,%al
	je	not_exploding	
	
handle_exploding:
	push	%eax
	shr	$1,%al			
					#     *2 for slow-down reasons)
	lea	explosion_sprites(,%eax,8),%edx
					# base+offset*8
	pop	%eax			# restore exploding 
	inc	%al			# increment exploding
	mov	%al,ENEMIES_EXPLODING(%ebp)		# store back exploding
	
	cmp	$7,%al			# is exploding > 7?
	jle	cont_handle_exploding
	
	movb	$0,0(%ebp)	        # enemy[i].out=0
	decb	total_enemies_out	# enemies_out--
	
cont_handle_exploding:
	add	$5,%esi			# skip to enemies_x   
	jmp  	enemies_xy
	
not_exploding:
	xor     %eax,%eax
	add	$4,%esi	 		# skip to kind
	lodsb 				# get kind
	lea	enemy_sprites(,%eax,8),%edx
					# kind muliplied by 8 and added on

enemies_xy:
	lodsw				# load xh
	lodsb				# 
	lodsb				# load yh (little endian) 

	mov	%edx,%esi		# proper sprite

	push	%ecx
	call	blit_219			# actually blit things
	pop	%ecx
		
loop_draw_enemies:
	add	$ENEMIES_SIZE,%ebp	# move to next enemy struct
	loop	draw_enemies		# and loop
	

 	##########################
	# Draw the Boss
	##########################
draw_the_boss:

	cmpb    $7,current_enemy_type	# are we at end-of-level boss?
	jne	done_with_boss		# if not skip all this nonsense
	
	
	cmpb	$1,boss_exploding	# if boss is exploding
	je	skip_draw_boss		# don't draw it
	
	mov	$boss_sprite,%esi
	movw	boss_x,%ax
        xor	%al,%al
	call	blit_219		# blit(boss_sprite,framebuffer,219,boss.x,0,13,3);
	
skip_draw_boss:

	########################
	# Draw Smoke

	cmpb    $0,boss_smoke		# is the boss smoking?
	je	skip_draw_smoke		# if not skip ahead


	xor	%eax,%eax
	movb	boss_smoke,%al
	shr	$2,%al
	lea	smoke_sprites(%eax,%eax,4),%esi
	push	%esi
	movw	boss_x,%ax
	add	$5,%ah
	mov	$3,%al
	push	%eax
	call	blit			# blit(smoke_sprites[boss.smoke/4],framebuffer,219,boss.x+5,3,3,1);
	
	pop	%eax
	subw	boss_xadd,%ax
	mov	$4,%al
	pop	%esi
	call	blit			# blit(smoke_sprites[boss.smoke/4],framebuffer,219,boss.x+5-boss.xadd,4,3,1);
	
	decb 	boss_smoke	        # boss.smoke--;


skip_draw_smoke:

	########################
	# Boss Laser Shoot

	cmpb    $0,boss_shooting	# is the boss shooting?
	je	skip_boss_shooting	# if not skip ahead
	
	decb	boss_shooting		# boss.shooting--

	xor	%eax,%eax
	movb	boss_shooting,%al	# move boss.shooting into %eax
	push	%eax			# saveon stack
	
	add	$200,%ax		# add 200
        movw    %ax,sound_freq+2	# save as sound_freq
	movw    $20,sound_freq	
	orb     $SOUND_PENDING,game_flags 	 # set sound to play

	pop	%eax		       # restore boss_shooting

	and	$1,%eax		       # get low bit of boss shooting
	lea	laser_sprites(,%eax,4),%esi  	 # point to proper laser sprite
	
	xor	%ecx,%ecx	       # clear ecx counter
	
boss_shoot_loop:	

	mov	boss_x,%ax	       # get boss_x into ax
	mov	%cl,%al
	shl	$1,%al
	add	$3,%al
	
	push	%eax
	push	%ecx	
	push	%esi
	call	blit_219  		# blit(laser_sprite[boss.shooting%2],framebuffer,219,boss.x,3+(i*2),1,2);
	pop	%esi
	pop	%ecx
	pop	%eax
	
	add	$12,%ah
	push	%ecx
	push	%esi
	call	blit   			# blit(laser_sprite[boss.shooting%2],framebuffer,219,boss.x+12,3+(i*2),1,2);
	pop	%esi
	pop	%ecx

	add   	$1,%ecx
        cmp     $8,%ecx
	jl	boss_shoot_loop

skip_boss_shooting:

	#######################
	# boss dead

	cmpb    $0,boss_exploding
	je	boss_is_not_exploding

	movb	$1,boss_waiting

	push	$30	       			 # 30 random smokes
	pop	%ecx
big_explosion:
	xor     %eax,%eax
	movb	boss_exploding,%al		 # get exploding amount
	shr	$3,%al				 # div 3
	lea   	smoke_sprites(%eax,%eax,4),%esi	 # and that much smoke

	push	$4
	pop	%ebx
	call	random
	mov	%al,%dl				 # y=rand()%4
	
	push	$11
	pop	%ebx
	call	random
	mov	%al,%dh				 # x=rand()%11
	
	movw	boss_x,%ax
	add	%dh,%ah	  			 # boss_x+x
	mov	%dl,%al
      
	push	%ecx
	push	%esi
	call	blit_219
	pop	%esi
	pop	%ecx

	loop    big_explosion
	
	movl	$(120<<16+20),sound_freq
	orb	$SOUND_PENDING,game_flags	# set_sound(120,20);

	decb	boss_exploding

	cmpb	$0,boss_exploding		# are we done exploding?
	jne	not_dead_yet

	cmpb	$1,level    			# only show ending
	jne	no_ending			# after level1
	
	call 	do_ending
no_ending:
	  
	addl	$1000,score			# 1000 points for killing boss
	
	cmpb	$7,level
	je	new_level			# can't go past level 7!
	
	incb	level				# level++
	
	
	jmp	new_level			# start new level

not_dead_yet:

	jmp	move_boss

boss_is_not_exploding:

        decw	boss_count
	cmpw	$0,boss_count
	jge	move_boss

	#############################################
	# toggle boss waiting state if count is up

	cmpb	$0,boss_waiting
	je	make_boss_wait	
stop_boss_waiting:
	movb	$0,boss_waiting		# boss.waiting=0;
	mov	$320,%ebx
	call	random
	add	$40,%ax
	mov	%ax,boss_count		# boss.count=40+rand()%320;
	jmp	move_boss
	
make_boss_wait:
	movb   	$1,boss_waiting
	push	$40
	pop	%ebx
	call	random
	add	$30,%ax
	mov	%ax,boss_count		# boss.count=30+rand()%40;
	movb	$30,boss_shooting	# boss.shooting=30;

move_boss:
	cmpb	$0,boss_waiting
	jne	laser_collision

	movw	boss_x,%ax
	addw	boss_xadd,%ax		# boss.x+=boss.xadd;
	mov	%ax,boss_x
	
	cmp	$26,%ah
	jg	boss_reverse
boss_under:
	cmp	$0,%ah
	jge	laser_collision

boss_reverse:
	negw 	boss_xadd
	addw	boss_xadd,%ax
	mov	%ax,boss_x
	
laser_collision:
	#######################
	# Collision detection for lasers

	cmpb	$0,boss_shooting
	je	done_with_boss

	movw	boss_x,%dx
	mov	%dh,%dl
	
	movw	x,%ax
	mov	%ah,%al
	add	$6,%al
	call	check_inside
	
	jc	laser_hit
	
	add	$12,%dh
	mov	%dh,%dl
	
	movw	x,%ax
	mov	%ah,%al
	add	$6,%al
	call	check_inside
	
	jnc	done_with_boss
	
laser_hit:
	testb	$3,boss_shooting
	jnz	done_with_boss
	decb	shields	      		# if (boss.shooting%7==0) shields--;

done_with_boss:



    	########################
	# Move the Missiles
	########################

	mov	$missile_0,%ebp		# point to first missile
	push	$NUM_MISSILES
	pop	%ecx	     		# loop through all missiles
	
move_missiles:
	mov	%ebp,%esi
	lodsb				# get missile[i].out	
	test	$1,%al			# is the missile out?
	jz	loop_move_missiles	# if not keep looping

	lodsb				# load x
	lodsb				# load y
	dec  	%al			# y--
	mov	%esi,%edi
	dec	%edi
	stosb	    			# store y back out
	
	cmp	$0,%al				# is y<0?
	jg	missile_collision_detection	# if not keep going

	andb	$(~PERFECT_SHOT),game_flags	# not a perfect shot
	mov	%ebp,%edi		  	# destroy missile
	xor	%al,%al
	stosb
	jmp  	loop_move_missiles

	#################################
	# Missile Collision Detection 
	
missile_collision_detection:	
	push	%ecx			# save missile count
	mov	MISSILE_X(%ebp),%dh	# move missile[i].x -> dh
	mov	MISSILE_Y(%ebp),%dl	# move missile[i].y -> dl
	push	%edx			# save missile info for later
	
	mov	$enemy_0,%ebx
	mov	$NUM_ENEMIES,%ecx
check_missile_enemy:
	cmpb	$0,ENEMIES_OUT(%ebx)
	je	loop_missile_enemy

	cmpb	$0,ENEMIES_EXPLODING(%ebx)
	jne	loop_missile_enemy
	
	pop	%edx		  	    # restore missile x,y
	push	%edx
	
	mov	%dh,%dl			    # missile x is 1 wide
	
	movw	ENEMIES_X(%ebx),%ax	    # get enemies x
	mov	%ah,%al
	add	$2,%al 			    # enemy x is two wide
	call	check_inside	
	jnc	loop_missile_enemy
	
	pop	%edx
	push	%edx
	mov	%dl,%dh			    # only check head of missile?
	
	movw	ENEMIES_Y(%ebx),%ax
	mov	%ah,%al
	add	$1,%al
	call	check_inside
	jnc	loop_missile_enemy
	
	
	# Collision!
	
	movb	$1,ENEMIES_EXPLODING(%ebx)
	addl	$10,score

        movl    $(150<<16+40),sound_freq
	orb     $SOUND_PENDING,game_flags 	 # set_sound(150,40);

	addb	$1,shield_up_count		 # add 1 to shield bonus count
	cmpb	$UP_SHIELDS,shield_up_count	 # have we hit limit?
	jl	no_shield_bonus			 # if not move on
	
	movb	$0,shield_up_count		 # reset shield_up_count to 0
	addb	$1,shields			 # add one to shields
	
	movl    $(220<<16+50),sound_freq
	orb     $SOUND_PENDING,game_flags      	 # set_sound(220,50);
	
	cmpb	$10,shields			 # are we greater than 10?
	jle	no_shield_bonus
	movb	$10,shields     		 # if so, set to 10
	
no_shield_bonus:	

	movb	$0,MISSILE_OUT(%ebp)		 # set missile out to 0
	
loop_missile_enemy:
        add	$ENEMIES_SIZE,%ebx
	dec	%ecx
	cmp	$0,%ecx
	jnz	check_missile_enemy


check_missile_boss:

	pop     %edx			# missile[i].x,missile[i].y
	pop	%ecx			# missile count
	
	# Missile <-> Boss Collision Detection 

	cmpb	$7,current_enemy_type  # see if boss is out
	jne	done_missile_collision
	
	cmpb	$1,boss_exploding      # be sure boss is not exploding
	je	done_missile_collision

	movw	boss_x,%ax
	mov	%ah,%al
	add	$13,%al
	
	push	%edx
	mov	%dh,%dl
	call	check_inside
	pop	%edx
	
	jnc	done_missile_collision

check_boss_y:

	xor  	%eax,%eax
	add	$2,%al
	
	mov	%dl,%dh
	call	check_inside
	
	jnc	done_missile_collision

	movl    $(150<<16+50),sound_freq
	orb     $SOUND_PENDING,game_flags      	 # set_sound(150,50);

	movb	$0,MISSILE_OUT(%ebp)
	movb	$11,boss_smoke
	
	decb	boss_hits     			 # decrement hits left
	cmpb	$0,boss_hits			 # are we done?
	jge	done_missile_collision		 # if not, go on
	
	movb	$23,boss_exploding		 # start boss exploding
	movb	$0,boss_shooting		 # have him stop shooting
	
	# destroy all extant missiles

	push	%ecx
	
	mov	$enemy_0,%ebx		# point to first missile
	mov	$NUM_ENEMIES,%ecx	# loop through all missiles
destroy_enemies:	
	movb	$2,ENEMIES_EXPLODING(%ebx)
	add	$ENEMIES_SIZE,%ebx
	loop	destroy_enemies	
	
	pop	%ecx
	
done_missile_collision:


loop_move_missiles:
	add	$3,%ebp
	dec	%ecx
	cmp	$0,%ecx
	jnz	move_missiles

done_move_missiles:



    	########################
	# Draw the Missiles
	########################

	mov	$missile_0,%ebp		# point to first missile
	push	$NUM_MISSILES
	pop	%ecx	     		# loop through all missiles
	
draw_missiles:	
	mov	%ebp,%esi
	lodsb				# get missile[i].out	
	test	$1,%al			# is the missile out?
	jz	loop_draw_missiles	# if so keep looping

	lodsb				# load x
	shl	$8,%ax
	lodsb				# load y
	
	mov	$missile_sprite,%esi	# missile sprite
	push	%ecx
	call	blit_219			# actually blit things
	pop	%ecx
		
loop_draw_missiles:
	add	$3,%ebp			# move to next missile struct
	loop	draw_missiles		# and loop


	#############################
	# Read the keyboard
	#############################
	
game_read_keyboard:      
 
        call	get_a_char
   	jc	move_ship

game_q:
   	cmp	$'q',%al		# are we 'q'?
	jne	game_j			# if not, move on
	
	call	verify_quit		# pop up "sure you want to quit"
	jnz	done_game		# then game over
	
	jmp	set_pause_flag		# if not act like paused then cont
	
game_j:   
	cmp	$'j',%al		# are we "j" [right] ?
	jne	game_k			# if not keep going
	
	mov	xadd,%bx		# load in xadd from RAM
	cmp	$0,%bx			# are we less than zero?
	jg	game_j_0		# if not go ahead
	add	$(-128),%bx		# add -128 to xadd
	jmp	game_j_done		
game_j_0:	
	xor	%ebx,%ebx		# switching direction, make xadd 0
game_j_done:
	mov 	%bx,xadd		# store back to RAM
	jmp 	game_read_keyboard	# done
	
		
game_k:
        cmp	$'k',%al		# are we 'k'? [left]
        jne	game_p			# if not move ahead

	mov	xadd,%bx		# load in xadd from RAM
	cmp	$0,%bx			# are we less than zero?
	jl	game_j_0		# then changing direction
					# set to 0 ( OPT reuse game_j)
	
	add	$(128),%bx		# add 128 to xadd
	jmp	game_j_done		# (OPT reuse game_j code)
       
game_p:
        cmp	$'p',%al		# did we press 'p'?
	jne	game_s			# if not, move on

	mov	$14,%bl			# put attention block
	call	put_attention_block

	mov	$game_paused_line,%esi	# print "GAME PAUSED!"

	push	$1
	pop	%ecx

	call	text_screen_keypressed_ret
				        # call	put_text_inline
        				# call	dump_to_screen
					# call	wait_until_keypressed
	
	
	jmp	set_pause_flag
	
game_s:
        cmp	$'s',%al	       	# did we press 's'?
	jne	game_h			# if not, move on
	xorb	$SOUND,game_flags	# toggle sound bit
	jmp	game_read_keyboard

game_h:
        cmp	$'h',%al		# Did we press 'h'?
	jne	game_space		# if not, move on
       	call 	display_help		# display help
set_pause_flag:	
	orb	$PAUSED,game_flags	# set the "paused" flag
	jmp	game_read_keyboard

game_space:
	cmp	$' ',%al		# Did we press ' '?
	jne	game_unknown		# If not, move on
	
	mov	$missile_0,%esi		# point to first missile
	push	$NUM_MISSILES
	pop	%ecx	     		# loop through all missiles
fire_missiles:	
	lodsb				# get missile[i].out	
	cmp	$0,%al			# is it empty?
	jne	end_fire_loop		# if not keep looping
	
	mov	%esi,%edi		# point to missile[i].out
	dec	%edi
	mov	$1,%al			# indicate that it's out
	stosb
	
	mov  	x,%ax
	shr	$8,%ax
	add	$3,%al
	stosb	      			# missile[i].x=shipx+3
	
	mov  	$16,%al
	stosb	       			# missile[i].y=shipy
	
	movl    $(110<<16+30),sound_freq
	orb     $SOUND_PENDING,game_flags # set_sound(110,30);
	
	jmp	game_read_keyboard	# finished shooting
	
end_fire_loop:
	add   	$2,%esi			# point to next missile
	loop	fire_missiles		# loop
	
game_unknown:
 	jmp	game_read_keyboard


	########################
	# move the ship
	########################

move_ship:
	mov	x,%ax			# restore x from memory
	mov	xadd,%bx		# restore xadd from memory

	add	%bx,%ax			# x+=xadd

check_x_under:	
	jns	check_x_over		# if not go ahead
	xor	%ax,%ax			# if under clear x and xadd
	xor	%bx,%bx
check_x_over:	
	cmp	$33,%ah			# are we over 33?
	jle	blit_ship		# if not draw ship	
	mov	$33,%ah			# x=33, xadd=0
	xor	%bx,%bx
blit_ship:	
	mov	%ax,x			# write updated values out to RAM
	mov	%bx,xadd

        mov     $ship_sprite,%esi     	# load the ship-centric pic
	mov	$16,%al		      	# x already in ah, 16=y
	call    blit_219
  
 	###########################
	# draw the info bar
 	###########################
	

	cmpb     $0,shields	   	# are shields <0?
	jl	 done_game		# if so, GAME OVER
	
	movb	 $12,shields_line	# make SHIELDS line red
	jg	 draw_shields_bar	# are we at zero shields?
	movb	 $14,shields_line	# if so make SHIELDS line yellow
		
draw_shields_bar:
        xor	 %ecx,%ecx		# clear count
	movb	 shields,%dl		# move shields into dl
	mov	 $21,%al		# y=21
	mov	 $10,%ah		# x starts at 10
	
shield_bar_loop:
        cmp	 %dl,%cl		# is count greater than shields?
	jge      shield_empty		# if so, put empty
	
	mov	 $block,%esi		# if not, put a block
	
	mov	 $4,%bl			# color=dark red
	cmp	 $4,%ecx		# if less than 4
	jl	 draw_shield
	
	mov	 $12,%bl    		# color=light red
	cmp	 $8,%ecx		# if 4<shields<8
	jl	 draw_shield
	
	mov	 $15,%bl   		# color=white if shields>8

	jmp	 draw_shield
	
shield_empty:	

        mov	 $underscore,%esi	# put a dark grey underscore
	mov	 $8,%bl
	
draw_shield:

	call	put_text_xy		# put the char
	add	$2,%ah			# move x over by 2

	inc	%ecx			# increment counter
	cmp	$10,%ecx		# are we less than 10?
	jne	shield_bar_loop		# if so keep looping
  
  
   	#  Should convert to ascii at score changetime
  	#  And not update score string here
  
  	mov	$shields_line,%esi      # print "SHIELDS:","SCORE:",
	push	$4			#       "HISCORE:","LEVEL:"
	pop	%ecx
	call	put_text_inline
	
	mov	score,%eax     		# Convert score to ascii
	mov	$score_string,%edi
	call    num_to_ascii
		
	mov	$score_string,%esi	# print score_string
	mov	$((8<<8)+22),%eax
	mov	$15,%bl
	call	put_text_xy
	
	xor	%eax,%eax
	movb	level,%al     		# Convert level to ascii
	mov	$level_string,%edi
	call    num_to_ascii
		
	mov	$level_string,%esi	# print level_string
	mov	$((8<<8)+23),%eax
	mov	$15,%bl
	call	put_text_xy
	
	mov	hiscore,%eax     	# Convert hiscore to ascii
	mov	$hiscore_string,%edi
	call    num_to_ascii
		
	mov	$hiscore_string,%esi	# print hiscore_string
	mov	$((32<<8)+22),%eax
	mov	$15,%bl
	call	put_text_xy

	testb	$SOUND_PENDING,game_flags   # is a sound pending? 
	jz	no_sound_effect		    # if not, move ahead	
	andb	$~SOUND_PENDING,game_flags  # clear sound_pending flag
	movl	sound_freq,%ebx
	call	play_sound	  	    # play the sound

	
no_sound_effect:
    
        #################################################
	# Finally draw it all to the screen
    	#################################################
	
        call	dump_to_screen
	
	##################################################
	#  Frame limit... keep it at ~30 frames/sec
	##################################################
     	# Assumes we don't lag more than a second 
     	# Seriously, if we lag more than 10ms we are screwed anyway 

frame_limit:
      	mov     $game_time,%ebx               # get current time
        call    gettimeofday
		
	mov	game_time+4,%ebx	      # move usecs to %ebx
  	mov	game_old_time,%eax	      # mov old usecs to %eax

	mov	%ebx,%ecx

  	sub	%eax,%ebx		      # time delta in %ebx
	
	cmp	$0,%ebx
	jge	time_no_adjust
  	add	$1000000,%ebx 		      # correct for underflow
  
time_no_adjust:

	cmp    $30000,%ebx		      # have we waited 30ms?
	jg     done_frame_limit		      # if so, move on

	call   hundred_usec_nanosleep  	      # else, sleep a bit

	jmp    frame_limit


done_frame_limit:
  	mov	%ecx,game_old_time	      # store as old time
  
      
	testb	$PAUSED,game_flags	      # were we paused?
	jz	game_not_paused	

	andb	$(~PAUSED),game_flags	      # clear PAUSED flag
      	mov     $game_time,%ebx               # get current time
        call    gettimeofday
		
	mov	game_time+4,%ebx	      # move usecs to %ebx
	
game_not_paused:

	########################
	# LOOP until FINISHED
	########################

        jmp	main_game_loop		# LOOP until finished

done_game:

	########################
	# GAME OVER
	########################

	mov	$14,%bl			# put attention block
	call	put_attention_block

	mov	$game_over_line,%esi    # print "GAME OVER!"
	call	put_text_line_inline

        call	dump_to_screen

	call	wait_long_time
	
	#############################
	# Play sad music

	cmpb   $0,shields
	jg     no_bum_bum

	testb	$SOUND,game_flags
	jz	no_bum_bum

	call	wait_long_time
	  
        mov     $(164<<16+500),%ebx
	call	play_sound	   	# set_sound(164,500);

	call	dump_to_screen

	call	wait_long_time
	      
        mov     $(130<<16+500),%ebx
	call	play_sound	   	# set_sound(130,500);
   
   	call	dump_to_screen
   
        jmp	show_hi_score   
	
no_bum_bum:
	push	$3
	pop	%ecx			# Pause 3 seconds or until keypress
     	call	pause_a_while
    
show_hi_score:

	call	clear_framebuffer	# clear screen

	mov	score,%eax	 	# get score
	mov	hiscore,%ebx		# get hiscore
	cmp	%ebx,%eax		# is score less than hiscore?
	jle	no_new_hiscore		# if so skip ahead
	
	mov	%eax,hiscore		# score is the new hiscore
	
        call	new_hi_score
	
no_new_hiscore:
	call    save_hi_score
      	call	do_hi_score		# show the hiscore
	call	wait_long_time
    
        push	$5
	pop	%ecx	     		# Pause 5 seconds or until keypress
     	call	pause_a_while

        ret



	######################
	# RANDOMIZE TIMER
	######################
	# We want a seed that is not divisible by 2 or 5
	#    to enhance the quality of subsequent pseudo-random
	#    numbers
	###############
	# destroys eax,ebx,ecx,edx
	
randomize_timer:
	mov	$random_seed,%ebx	# get current time
	call	gettimeofday        

	mov	random_seed,%eax        # use seconds since 1970 as seed

	push	$5
	pop	%ecx			# we will be dividing by 5 later
						
try_again:
	inc	%eax			# hackish way, but keeps logic simple
					# and we only have 50/50 of even anyway
					
	test	$0x1,%al		# check if even
	jz	try_again		# if so inc and try again
	
	xor	%edx,%edx		# clear out top 32 bits
	idivl	%ecx	 		# divide edx:eax by 5
	
	cmp	$0,%edx			# check if remainder is zero
	je	try_again		# if so inc by 1 and try again
	
	and	$0xffffff,%eax		# we're doing fixed point so
					# trim some high-order digits
					
	mov	%eax,random_seed	# store back to memory
	
	ret
	

	######################
	# RANDOM
	######################
	# pseudo-random number generator
	######################
	# bx = mod [aka high range]
	# ax returned with random number

random:

        # The following algorithm from the Hewlett Packard HP-20S 
	# Scientific Calculator manual, September 1998, p75-76    
	#
	# The algorithm  r     = FractionalPart (997r )           
	#                 i+1                        i            
	#                                                         
	# Where r0 is a starting value between 0 and 1.           
	# The generator passes the chi-square frequency test for  
	# uniformity, and serial and run tests for randomness.    
	# The more significant digits are more random than the    
	# less significant.  If r0 * 10e7 is not divisible        
	# by 2 or 5 you get 500,000 numbers w/o repeating.        
	#
	# modified to be fixed-point integer only by Vince Weaver 

	push	%edx
	push	%ecx
	push	%ebx
	
	mov	random_seed,%eax     	# get current seed
		
	mov	$997,%ecx		
	mull	%ecx			# multiply by 997
	
	and	$0xffffff,%eax		# keep to ~7 digits
	mov	%eax,random_seed	# store back the seed


	shr	$8,%eax			# high order most random
	and	$0xffff,%eax		# we limit random nums to 0-65535 range

	pop	%ebx			# get high limit
	cdq				# clear dx
	divw	%bx			# divide by limit
	mov	%dx,%ax			# move remainder to ax

	pop	%ecx
	pop	%edx

    	ret

	########################
	# wait_long_time
	########################
wait_long_time:	
	push	$5
	pop	%ecx
wait_loop:	
	push    %ecx
	call	inflection_point
	pop	%ecx
	loop	wait_loop
	ret


	########################
	# inflection_point
	########################
	# pause hard, and clear keypresses
	# used in a sudden stop of action
inflection_point:
	mov	$150,%ecx
sleep_250ms:	
	call	milisec_nanosleep
	loop	sleep_250ms
	
			   		# slide in and also
					# clear keyboard buffer
	
	##########################
	# clear_keyboard_buffer
	##########################
clear_keyboard_buffer:
	
	call	get_char
	jnc	clear_keyboard_buffer
	
	ret
	

	#################################
	# do_ending
	#################################
do_ending:
	call	wait_long_time		# pause a sop up keyboard in case
					# still firing keys at boss

	call	clear_framebuffer	# clear screen

	push	$16			# want to write 16 lines of text
	pop	%ecx
	
	mov	$ending_line,%esi
	call	put_text_inline
	
	mov	$earth_sprite,%esi	# Draw Picture of Earth
	mov	$(32<<8),%ax
	call	blit_219
	
		    			# draw picture of Susie
					# %esi follows that of earth
	mov	$(20<<8+15),%ax
	call	blit
	
	call	dump_to_screen
	
	call	wait_long_time
	
	call	wait_until_keypressed
	   
	call	clear_framebuffer

	mov	$tom_head_sprite,%esi	# put head picture
	xor	%eax,%eax		# at 0,0
	call	blit_219


	mov	$ending_line_2,%esi
	
	push	$2  			# put two lines of text
	
	jmp	pop_text_screen_keypressed_ret
	
					# insane code reuse
					# pop	%ecx
				      	# call	put_text_inline	   
 					# call    dump_to_screen
					# call	wait_until_keypressed
 					# ret


	#################################
	# do_about
	#################################
do_about:

	call	clear_framebuffer
   
        mov     $vince_sprite,%esi    # load the ego-centric pic
	mov     $(24<<8),%eax         # at 24,0
	call    blit_219
				   
	mov	$about_line_0,%esi
	push	$15
	
	jmp	pop_text_screen_keypressed_ret	
	
					# insane code re-use
				        #pop	%ecx
					#call	put_text_inline
					#call	dump_to_screen
					#call   wait_until_keypressed
					#ret



	#################################
	# do_story
	#################################
do_story:

	call	clear_framebuffer

        mov     $phobos_sprite,%esi   	# load the mars pic
	xor	%eax,%eax             	# at 0,0
	inc	%eax
	call    blit_219

	mov	$story_line_0,%esi    	# write the story text
	push	$3
	pop	%ecx
	call	put_text_inline

	push	%esi
	call	dump_to_screen
	pop	%esi

	push	$8
	pop	%ecx

	call	pause_a_while	        # pause for up to 8 seconds
	
    					# esi points at story_line_1
					# saved from before
	push    $3
	pop	%ecx
	call	put_text_inline

	call	dump_to_screen

	push	$4
	pop	%ecx
	call	pause_a_while		# pause for up to 4 seconds

	mov	$(19<<8+9),%ax
	push	%eax
	
phobos_loop:
        mov     $evil_ship_sprite,%esi  # load the evil ship
	call    blit_219		# and draw it to the screen
	
	call	dump_to_screen

	push	$30
	pop	%ecx
	
phobos_delay:	
	call	milisec_nanosleep	# delay 30ms
	loop	phobos_delay

	mov     $menu_blank+2,%esi	# erase the ship
	pop	%eax
	push	%eax
	call	put_text_xy
	
	pop	%eax	   		# increment x
	inc	%ah
	push	%eax

     	cmp	$37,%ah			# have we reached end of screen?
	jne	phobos_loop		# if not, loop

	pop	%eax			# restore stack

	push	$4
	pop	%ecx
	call	pause_a_while		# pause for up to 4 seconds
	  
	call	clear_framebuffer

        mov     $tom_sprite,%esi      	# load the tom pic
	mov	$(24<<8)+1,%eax	      	# at 24,1
	call    blit_219

	mov	$story_line_2,%esi      # put the tom related story
	push	$9
	pop	%ecx
	call	put_text_inline

	call	dump_to_screen
	
	push	$20
	pop	%ecx			# pause for up to 20 seconds

					# cheat and slide into
					# pause a while


        ############################
	# Pause a While
	############################
	# ecx = seconds
	#
pause_a_while:
	mov	$time_of_day,%ebx	# get starting time
	call	gettimeofday
	
	mov	(time_of_day),%eax	# store usecs in eax
	
pause_loop:
	push	%eax
	push	%ebx
	push	%ecx
	call	get_char
	pop	%ecx
	pop	%ebx
	pop	%eax
	jnc	pause_end		# if keypressed, end
	
	call	milisec_nanosleep	# sleep a bit

	mov	$time_of_day,%ebx	# get current time
	call	gettimeofday
	
	mov	(time_of_day),%ebx	# put usecs in ebx
	
	sub	%eax,%ebx		# ebx=new-old

	cmp	%ebx,%ecx
	jg	pause_loop
pause_end:

        ret


	######################
	# Help
	######################
	#

display_help:
	call	clear_framebuffer

	mov	$help_line_0,%esi      
	push	$18
	
pop_text_screen_keypressed_ret:	
	pop	%ecx
text_screen_keypressed_ret:	
	call	put_text_inline
	
	call	dump_to_screen
	
			      		# cheat and slide into
					# wait until keypressed
						
	#######################
	# Wait until keypressed
	#######################
	#
wait_until_keypressed:

	call	get_char
	jnc	wait_exit
	call	milisec_nanosleep
	jmp	wait_until_keypressed
	
wait_exit:	
	ret	


	##################
	# mov_edi_x_y
	##################
	# makes edi point to framebuffer
	# plus proper x,y offset
	#
	# ah=x al=y
	# %eax,%ecx,%edi destroyed
	
mov_edi_x_y:	
        mov     $framebuffer,%edi	    # point to frame buffer
	xor	%ecx,%ecx		    # clear cx
	mov	%al,%cl			    # want to offset by y
	
	cmp	$0,%cl			    # if at x,0 don't offset
	je	offset_x
	
offset_y:
        add	$(SCREEN_WIDTH*2),%edi	    # offset by y*2
	loop	offset_y
	
offset_x:	
	and	$0xffff,%eax		    # make sure top of eax clear
	shr	$7,%eax			    # add in x*2
	add	%eax,%edi
	
	ret

	##################
	# BLIT
	##################
	# esi=source
	# ah=x al=y	
	# (for already_know_only) bl=xsize,dh=ysize
	# dl=char to write	
	# destroyed: %eax,%ebx,%ecx,%edx,%edi

blit_219:
	mov	$219,%dl

blit:
	push	%eax			# save x,y
        lodsb				# get x-size from sprite
        mov     %al,%bl
        lodsb				# get y-size from sprite
        mov	%al,%dh
	pop	%eax   			# restore x,y
	
blit_already_know_xsize_ysize:	

	and     $0xff,%ebx		# clear high bits of ebx
	
     	call	mov_edi_x_y		# point edi to proper place in framebuf
	
blit_y_loop:
	mov     %bl,%cl			# restore x-size counter

blit_x_loop:

        lodsb				# get color info
	cmp     $0,%al			# is it zero?
	jz	no_color		# if so skip the write

	mov	%dl,%ah			# get character
	stosw				# store char/color
	jmp  	check_x

no_color:	
	inc	%edi			# increment pointer
	inc	%edi
check_x:	
        loop blit_x_loop		# do while x<0

	add    $(SCREEN_WIDTH*2),%edi	# move to next line
	sub    %ebx,%edi		# by adding y-size then
	sub    %ebx,%edi		# subtracting x-offset
	
	sub    $1,%dh			# decrement y counter
	jnz    blit_y_loop		# if not zero we loop

        ret						





	#####################
	# do_hi_score
	#####################

do_hi_score:
	call	load_hi_score		# load hi-score from disk
	
	mov	$12,%bl			# color light-red
	call	put_attention_block	# put an attention block
	
	mov	$high_score_line,%esi	# write "HIGH SCORE"
	call	put_text_line_inline
	
	mov	$hiscore_buffer,%edi	# point to temp buffer
	mov	$' ',%al		# space
	stosb
	
	mov  	$hi_player,%esi		# put the high-players name
	mov	$3,%ecx
	rep	
	movsb
	
	mov	$' ',%al		# pad with '  '
	stosb
	stosb
	
	mov  	hiscore,%eax		# Convert hiscore to ascii 
	call	num_to_ascii
	mov	$' ',%ax
	stosw
	
	mov	$hiscore_buffer,%esi	# point to buffer again
	mov	$15,%bl			# white
	mov	$9,%al			# y=9
	
	mov	$40,%ah			# Center... start with 40
	mov	%esi,%edx
strlen:		
	cmp	$0,0(%edx)		# repeat till end
	je	done_strlen
	inc	%edx
	dec	%ah 			# subtracting from the 40
	
	jmp	strlen
done_strlen:	
	shr	$1,%ah			# divide by 2
       
	call	put_text_xy		# put hiscore name/score

					# cheat and slide into 
					# dump_to_screen


	##################
	# dump_to_screen
	##################
	# eax,ebx,ecx,edx,esi,edi trashed
	# ebp= ^[
	# eax = scratch
	# bl = block_mode
	# dl = old_color
	# dh = ysize
	# ecx = xsize
	
dump_to_screen:	
	push	%ebp			# we save ebp
	
	mov     $framebuffer,%esi       # setup pointers
	mov	$out_buffer,%edi

	mov	$('['<<8+27),%bp	# store ^[[ as it will be useful later
	
	
				        # write ^[[0;0H to home the cursor
	mov	 %ebp,%eax
	stosw
	mov	 $( ('H'<<24)+('0'<<16)+ (';'<<8)+('0')),%eax
	stosl

	xor	 %bl,%bl	        # set block_mode
	mov	 $17,%dl	        # set old_color

	mov	 $(SCREEN_HEIGHT),%dh   # load in y size

dump_y_loop:
       push     $SCREEN_WIDTH
       pop	%ecx	     	        # restore x size
dump_x_loop:

	lodsw			        # load ascii/color
	
	cmp     %dl,%al		        # compare to current color
	je	blit_block	        # if the same, skip ahead

	mov	%al,%dl		        # store color as old_color

	push	%eax		        # save ascii/color on stack

	mov	%ebp,%eax
	stosw			        # print ^[[
	
	mov  	%dl,%al		        # restore color

	shr  	$3,%al		        # get intensity bit
	add	$0x30,%al	        # convert to ascii
	stosb			        # store
	
	mov	%dl,%ah		        # restore color
	and	$7,%ah		        # mask off intensity bit
	
				        # swap bits 3 and 1
				        # we use rgb, ansi does bgr.  annoying
				        # There's probably a better way.
				        # Maybe a lookup table?
	xor	%al,%al
	rcr	$1,%ah
	rcl	$1,%al
	
	rcr	$1,%ah
	rcl	$1,%al
	
	rcr	$1,%ah
	rcl	$1,%al
	
	add	$0x30,%al	        # convert to ascii
		
	mov  	$'m',%ah	        # tack on trailing m
	
	shl	$16,%eax
	mov	$(('3'<<8)+';'),%ax
      
	stosl			        # finish writing out color

	pop	%eax		        # restore ascii/color

blit_block:		
	cmp     $219,%ah	        # are we the block char?
	jne 	regular_char	        # if not skip ahead

	cmp	$1,%bl		        # are we in block mode?
	je	change_block_zero       # if yes, skip ahead
	
	mov	$14,%al		        # switch to vt102 line-draw mode
	stosb			        # with ^N
	mov	$1,%bl		        # set block mode flag

change_block_zero:
	mov	$'0',%ah	        # 0 = block in vt102 line-draw
	jmp	blit_char
	
regular_char:
        cmp    $1,%bl		        # are we in block mode?
	jne    blit_char	        # if we aren't, skip ahead
	
	push   %eax		        # switch to normal mode
	mov    $15,%al		        # with ^O
	stosb
	xor     %bl,%bl		        # clear block mode flag	
	pop     %eax

blit_char:
	shr     $8,%ax		        # write out the ascii char
	stosb

blit_char_done:

	loop    dump_x_loop	        # loop until x done

	mov	$10,%al		        # write a line-feed
	stosb

	sub	$1,%dh		        # loop until y done
	jnz	dump_y_loop

	mov	$0,%al	   	        # store terminating 0 char
	stosb
	
	mov  	$out_buffer,%ecx        # write to stdout
	
	pop	%ebp
	
		    			# hijack our way into
					# write_stdout
					
	

        #################################
	# WRITE_STDOUT
	#################################
	# ecx has string
        # eax,ebx,ecx,edx trashed
write_stdout:

	cdq                             # clear edx (depends on high bit
					#    of eax being 0)
		
str_loop1:
        inc     %edx
	cmpb    $0,(%ecx,%edx)          # repeat till zero
	jne     str_loop1

write_stdout_dx_set:

	push	$SYSCALL_WRITE          # put 4 in eax (write syscall)
        pop     %eax                    # in 3 bytes of code
	
	xor     %ebx,%ebx               # put 1 in ebx (stdout)
	inc     %ebx                    # in 3 bytes of code
	
	int     $0x80                   # run the syscall

	ret




        #########################
	# Clear Framebuffer
	#########################
	# eax,ecx,edi trashed

clear_framebuffer:
	xor	%eax,%eax
	mov	$' ',%ah 			      # clear with black spaces
	mov	$framebuffer,%edi
	mov	$(SCREEN_WIDTH*SCREEN_HEIGHT),%ecx
	rep	
	stosw
	ret

	############################
	# gettimeofday()
	############################
	# ebx points to buffer
gettimeofday:
	push 	%eax
	push	%ecx
	push	$SYSCALL_GETTIMEOFDAY
	pop	%eax
	xor	%ecx,%ecx		# NULL timezone
	int     $0x80                   # run the syscall
	pop	%ecx
	pop	%eax
	ret

	###########################
	# nanosleep()
	###########################
	# time to sleep in buffer pointed by ebx
	
milisec_nanosleep:	
	push	%ebx
	mov	$milisecond,%ebx        
	jmp	mid_nanosecond
	
hundred_usec_nanosleep:
	push    %ebx
	mov	$hundred_microsecond,%ebx
mid_nanosecond:	
	call	nanosleep
	pop	%ebx
	ret
	
nanosleep:
	push	%eax
	push 	%ecx
	push	$SYSCALL_NANOSLEEP
	pop	%eax	
					# ebx=req or &time_we_want_to_sleep
	xor	%ecx,%ecx		# ecx=rem or &remainder (if interrupted)
	int     $0x80                   # run the syscall
	pop	%ecx
	pop	%eax
	ret

	############################
	# tcgetattr
	############################
	# pointer to result in edx
	# eax,ebx,ecx trashed
tcgetattr:
	mov	$TCGETS,%ecx
	jmp	do_ioctl


	############################
	# tcsetattr
	############################
	# pointer to parameters in edx
	# eax,ebx,ecx trashed
tcsetattr:	
	mov	$TCSETS,%ecx		# TCSETS (or SNDCTL_TMR_TIMEBASE :( )
do_ioctl:	
	push	$SYSCALL_IOCTL
        pop	%eax	      		# someone set up us the ioctl
       	xor	%ebx,%ebx	     	# STDIN
	int	$0x80			# syscall!
	ret



	#############################
	# put_text_inline
	#############################
	# esi = data
	# ecx = number of strings
	#
	# first color, then y, then x, then string

put_text_line_inline:
	push	$1   			# convenience function
	pop	%ecx			# for those only wanting 1 line of text
put_text_inline:
	lodsb				# get color
	mov  	%al,%bl
	lodsw	       			# then get x,y
	call 	put_text_xy
	loop	put_text_inline
	ret


	#############################
	# put_text_xy
	#############################
	# esi = string
	# ah=x al=y
	# bl=color

	# ebx trashed  esi points to end of string
	
put_text_xy:
	push	%eax
	push 	%ecx			# edi_x_y destroys this
	call	mov_edi_x_y		# point to x,y in framebuffer
	pop	%ecx
p_loop:
	lodsb		 		# load char
	cmp     $0,%al			# is it zero?
	je	p_done			# if so we are done

        push    %eax
	mov     %bl,%al			# output color
	stosb
	pop  	%eax
	stosb	    			# output character
	jmp p_loop
p_done:	
	pop	%eax
        ret

	#######################
	# get_char
	#######################
	#
	# returns char in al.  carry set if invalid
	# eax,ebx,ecx,edx all trashed
	
get_char:

	push	$SYSCALL_READ
	pop	%eax
	xor	%ebx,%ebx	       # stdin
	mov	$in_char,%ecx	       # in_char
	push	$1
	pop	%edx		       # try to read 1 byte
	int	$0x80

	cmp	$1,%eax
	jl	no_char
	
	movb	(in_char),%al	
	clc	
	ret
no_char:
	stc
	ret


	#######################
	# get_a_char
	#######################
	# cooks up get_char for arrows and F keys
	# carry set it invalid,  or cooked char in al

get_a_char:
	call	get_char	       # grab a character
	jc	get_a_char_unknown

	cmp	$27,%al		       # see if escape
	jne	get_a_char_over	       # if not we are done
	
	call	get_char	       # get another character
	jnc	extended_char
	
	mov	$'q',%al     	       # if was just esc return q
	jmp	get_a_char_over

extended_char:
        cmp	$'[',%al       	       # check if was ^[[
	jne	get_a_char_unknown
	
	call	get_char	       # get yet another character
	jc	get_a_char_unknown	
check_up:
	cmp	$'A',%al
	jne	check_down
	mov	$'i',%al
	jmp	get_a_char_over
	
check_down:
	cmp	$'B',%al
	jne	check_right
	mov	$'m',%al
	jmp	get_a_char_over
	
check_right:
	cmp 	$'C',%al
	jne	check_left
	mov	$'k',%al
	jmp	get_a_char_over
	
check_left:
	cmp	$'D',%al
	jne	check_f1
	mov	$'j',%al
	jmp	get_a_char_over
	
check_f1:	
	cmp	$'[',%al
	jne	get_a_char_unknown
	call	get_char	 	# get yet _another_ char
	jc	get_a_char_unknown
	cmp	$'A',%al
	jne	get_a_char_unknown
	mov	$'h',%al
	jmp	get_a_char_over
get_a_char_over:	
	clc
	ret
	
get_a_char_unknown:
        stc
        ret




 	#######################
	# put_attention_block
	#######################
	# color is in bl
	
put_attention_block:	
	push	$5
	pop	%eax			# x=0 y=5
	call	mov_edi_x_y
    
	mov	$'*',%ah
	mov	%bl,%al

	mov	$(40*7),%ecx

	rep 
	stosw
    
	ret
	

	####################
	# num_to_ascii
	####################
	# eax = value to print
	# edi points to where we want it
	# ebx,ecx,edx trashed
	

num_to_ascii:
	push    $10
	pop     %ebx
	xor     %ecx,%ecx		# clear ecx
 div_by_10:
        cdq				# clear edx
	div     %ebx			# divide
	push    %edx            	# save for later
	inc     %ecx            	# add to length counter
	or      %eax,%eax       	# was Q zero?
	jnz     div_by_10       	# if not divide again

write_out:
        pop     %eax            	# restore in reverse order
	add     $0x30, %al      	# convert to ASCII
	stosb                  		# save digit
	loop    write_out       	# loop till done
	ret

	##########################
	# check_inside
	##########################
	# see if line dh -> dl ovelaps bigger line ah -> al
	# used as half of 2d rectangular collision detection
	# carry flag set if true, unset if not
check_inside:

	cmp  	%ah,%dl			
	jl	ar_not_in
	cmp	%al,%dl
	jle	inside
ar_not_in:
	cmp	%ah,%dh
	jl	outside
	cmp	%al,%dh
	jg	outside
inside:
        stc
	ret
outside:
	clc
	ret


	########################
	# play_sound
	########################
	# ebx=freq(hertz) / duration (ms)
play_sound:	

	##########################
	# To set frequency
	# ESC[10;FFF]   where FFF=frequency in hertz
	######################
	# To set duration
	# ESC[11;DDD] where DDD = Duration

	mov	$sound_buffer,%edi
	push	$'0'
	pop	%ecx

freq_loop:
	mov	%cl,%al
	shl	$24,%eax
	add	$('1'<<16 + '['<<8 + 27),%eax
	stosl
	
	mov  	$';',%al
	stosb

	xor  	%eax,%eax
	ror  	$16,%ebx	
	mov	%bx,%ax

	push	%ebx
	push	%ecx
	call	num_to_ascii
	pop	%ecx
	pop	%ebx

	mov	$']',%al
	stosb

	inc	%cl
	cmp	$'2',%cl
	jne	freq_loop
		
	testb   $SOUND,game_flags	# Check if sound on
	jz	no_bell
	
	
	##################
	# and ^G is BEL as always
	mov     $7,%al
	stosb
no_bell:
	xor	%al,%al
	stosb	       			# NUL string termination
		
	mov  	$sound_buffer,%ecx
	
	jmp 	write_stdout	  	# cheat and jmp instead of call
					# write_stdout will ret for us
	
	


	#########################
	# Load Hi Score
	#########################
	#
load_hi_score:	

	call	check_existing_hi_score

	jnc	done_load_hiscore	# if invalid/no file, jump ahead

	btl	$29,%eax		# is 4th char "S" or "s"?
	jc	hiscore_clear_sound	# if "s" we turn off sound
	
hiscore_set_sound:	
	orb	$SOUND,game_flags	# and turn on sound
	jmp	hiscore_get_initials
	
hiscore_clear_sound:	
       	andb	$(~SOUND),game_flags	# and turn off sound
hiscore_get_initials:	

	movl	hiscore_buffer+4,%eax	# get 4 bytes
	movl	%eax,hi_player		# first 3 are initials
	
	movl	hiscore_buffer+8,%eax	# get next 4 bytes
	movl	%eax,hiscore		# which is 32 bit int

done_load_hiscore:	
	ret


	#####################
	# Check Existing Hi Score
	#####################
	# cf=1 if valid previously existing file
	# cf=0 file problem
	# dl=0 file not found
	# d1=1 file_invalid

check_existing_hi_score:

	push	$SYSCALL_OPEN
	pop	%eax
	mov	$score_file,%ebx	# "/tmp/tb_asm.hsc"
	xor	%ecx,%ecx		# 0 = O_RDONLY <bits/fcntl.h>
	cdq				# clear edx
	int	$0x80
	
	cmp	$0,%eax			# see if fd opened 
	jl	file_not_found		# if not, exit
	
	mov	%eax,%ebx     		# move fd to %ebx
	
	push	$SYSCALL_READ		# read
	pop	%eax
	mov	$hiscore_buffer,%ecx	# into hiscore_buffer
	mov	$12,%dx			# 12 bytes
	int	$0x80
	
	push	%eax
		
	call    close_file		# close the file

	pop	%eax
	
	cmp	$12,%eax		# did we read 12 bytes?
	jne	invalid_file		# if not, invalid

	movl	hiscore_buffer,%eax	# get first 4 bytes into eax
	cmp	$('b'<<8 + 't'),%ax	# are first 2 chars "tb"?
	jne     invalid_file		# if not, close
	
	stc				# valid file
	ret

file_not_found:
	xor     %dl,%dl			# file_not_found
	jmp	file_done
invalid_file:	
	mov	$1,%dl			# invalid file	
file_done:
	clc				# file problem
	ret			     


	###############################
	# New High Score
	###############################
	#
	
new_hi_score:

	mov     $9,%bl
	call	put_attention_block	# make a bright blue attention block

	mov	$new_high_line,%esi	# output the high score directions
	push	$3
	pop	%ecx
	call	put_text_inline
	
	xor	%edx,%edx      		# we use dx as a counter
	movl	$('A'<<16+'A'<<8+'A'),hi_player
					# clear the hi-score initials
	
 hiscore_loop:
 	mov    $menu_blank,%esi		# clear the initials line
 	mov    $15,%bl
        mov    $17,%ah
 	mov    $10,%al
	push   %esi
	call   put_text_xy

 	pop    %esi	  		# clear the pointer line
 	inc    %al			# y=11
	call   put_text_xy
	   
	mov    $hi_player,%esi		# put the initials
	inc    %ah			# x=18
	dec    %al			# y=10
	call   put_text_xy
  
        mov	$menu_pointer,%esi	# "^"
	mov	%dl,%ah			# dl is x
	add	$18,%ah
	inc	%al			# y=11
  	call	put_text_xy
   
   	push	%edx
        call    dump_to_screen		# print to screen
	pop	%edx
	
hiscore_get_char:	
	call	hundred_usec_nanosleep	# sleep so don't use all CPU
	push	%edx
	call	get_a_char		# call get_a_char
	pop	%edx

	jc	hiscore_get_char	# then loop

hiscore_k:
	cmp	$'k',%al		# were we a right?
	jne	hiscore_j		# if not move on
	
	inc	%dl			# x++
	cmp	$2,%dl			# is x > 2?
	jle	hiscore_loop		# if not keep going
	mov	$0,%dl			# otherwise wrap so x=0
	
hiscore_j:	
   	cmp	$'j',%al		# are we left?
	jne	hiscore_i		# if not move on
   
   	sub	$1,%dl			# x--
					# is x < 0?
	jns	hiscore_loop		# if not keep going
	mov	$2,%dl			# otherwise wrap so x=2
   
hiscore_i:    
        cmp     $'i',%al		# are we up?
	jne	hiscore_m		# if not move on
	
	movb	hi_player(%edx),%bl	# get initial
	dec	%bl			# decrement
	cmp	$64,%bl			# are we less than '@'?
	jge	write_initial
	mov	$126,%bl		# if so wrap to '~'
write_initial:
	movb    %bl,hi_player(%edx)	# write back inital
	jmp	hiscore_loop
   
hiscore_m:   
        cmp     $'m',%al		# are we down?
	jne	hiscore_enter		# if not, move on
   
    	movb	hi_player(%edx),%bl	# get initial
	inc	%bl			# incrememnt it
	cmp	$126,%bl		# are we > "~"?
	jle	write_initial
	mov	$64,%bl	     		# if so wrap to "@"
	jmp	write_initial
	   
hiscore_enter:       
        cmp     $'\n',%al		# once we hit enter we are done
        jne	hiscore_loop

	ret

	######################
	# Save Hi Score 
	######################
	# also saves sound on/off
	
save_hi_score:
        call  	check_existing_hi_score
	jc	ok_to_write
	
	cmp	$1,%dl		       	# we won't write if invalid file
	je	done_save_hi		# we will if ours, or not there

ok_to_write:

	push	$SYSCALL_OPEN
	pop	%eax
	mov	$score_file,%ebx	# "/tmp/tb_asm.hsc"
	mov	$(O_WRONLY|O_CREAT|O_TRUNC),%ecx
	mov	$(SIXSIXSIX),%edx
	int	$0x80
	
	cmp	$0,%eax			# see if fd opened 
	jl	done_save_hi		# if not, exit

	mov	%eax,%ebx		# save fd
	
	movl	$('s'<<24+'a'<<16+'b'<<8+'t'),%eax
	
	testb	$SOUND,game_flags
	jz	no_save_sound
	btrl	$29,%eax
no_save_sound:

	mov	%eax,hiscore_buffer
	movl	hi_player,%eax
	mov	%eax,hiscore_buffer+4
	movl	hiscore,%eax
	mov	%eax,hiscore_buffer+8
	
	push	$SYSCALL_WRITE          # put 4 in eax (write syscall)
        pop     %eax                    # in 3 bytes of code

	mov	$hiscore_buffer,%ecx
	mov	$12,%edx
	
	int     $0x80                   # run the syscall
	
					# cheat and run into close_file
	
	
	#####################
	# Close File
	#####################
	# fd=ebx
	
close_file:
	push	$SYSCALL_CLOSE		# close the file
	pop	%eax
	int	$0x80
done_save_hi:	
	ret



################################################
# DATA SEGMENT
################################################

# .data

.include "data.labels"
data_compressed:
.include "data.lzss"


##################################################
# BSS SEGMENT
##################################################

#.bss
.lcomm  text_buf, (N+F-1)
.lcomm  DATA_OFFSET, TB_DATA_SIZE

    		# +1 for any wrap-around blits
.lcomm	framebuffer,	SCREEN_WIDTH*(SCREEN_HEIGHT+1)*BYTES_PER_PIXEL
.lcomm  out_buffer,	SCREEN_WIDTH*(SCREEN_HEIGHT+5)*10

.lcomm	sound_buffer,	12
.lcomm	sound_freq,	 4

.lcomm	hiscore_buffer,	16

.lcomm	in_char, 	1

.lcomm	random_seed,	8

.lcomm  old_time,	8
.lcomm	time_of_day,	8

.lcomm game_old_time,	8
.lcomm game_time,	8


.lcomm game_vars,1

.lcomm	x,		2
.lcomm	xadd,		2
.lcomm	scroll,		2
.lcomm	scroll_frames,	2

.lcomm	missile_0,	3*NUM_MISSILES	# out,x,y
.lcomm	enemy_0,	ENEMIES_SIZE*NUM_ENEMIES  
					# out,exploding,kind,xw,yw
				        # xadd,yadd,xmin,xmax
				       

.lcomm	score,		4
.lcomm  score_string,  10
.lcomm  hiscore_string,10
.lcomm  level,		1
.lcomm	level_string,  10

.lcomm shields,	        1
.lcomm shield_up_count, 1


.lcomm total_enemies_out, 1

# next bunch in order to smallen init code
.lcomm enemies_spawned,  	2
.lcomm current_init_x,	  	2
.lcomm current_enemy_kind,	1
.lcomm enemy_wait,		1
.lcomm current_enemy_type,	1
.lcomm enemy_wave,		1


.lcomm between_enemy_delay,1
.lcomm waves_till_boss,1


# FIXME move some of these to be flags?
.lcomm boss_x,2
.lcomm boss_xadd,2
.lcomm boss_count,2
.lcomm boss_smoke,1
.lcomm boss_exploding,1
.lcomm boss_waiting,1
.lcomm boss_hits,1
.lcomm boss_shooting,1

.lcomm	  background,1600

.lcomm	  end_game_vars,1


# From ~/linux/include/asm/termbits.h  
# struct termios {
#    unsigned int c_iflag;               /* input mode flags */
#    unsigned int c_oflag;               /* output mode flags */
#    unsigned int c_cflag;               /* control mode flags */
#    unsigned int c_lflag;               /* local mode flags */
#    unsigned char c_line;               /* line discipline */
#    unsigned char c_cc[19];             /* control characters */
# };

.lcomm  old_tty	       40
.lcomm  new_tty	       40
