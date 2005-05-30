
#include "wpc.h"


.area fastram

lamp_matrix:			.blkb NUM_LAMP_COLS
lamp_flash_select:	.blkb NUM_LAMP_COLS

lamp_flash_max:		.blkb 1
lamp_flash_count:		.blkb 1


.area ram

lamp_apply_delay:		.blkb 1


.area sysrom

proc(lamp_init)
	uses(a,x)
	ldx	#lamp_matrix
	lda	#NUM_LAMP_COLS * 2
	jsr	memset
	lda	#LAMP_DEFAULT_FLASH_RATE
	sta	lamp_flash_max
	sta	lamp_flash_count
	clr	lamp_apply_delay
endp


proc(lamp_rtt)
	lda	irq_shift_count
	sta	WPC_LAMP_COL_STROBE
	lda	irq_count
	anda	#0x07
	ldx	#lamp_matrix
	lda	a,x
	sta	WPC_LAMP_ROW_OUTPUT
endp



proc(lamp_on)	; A = lamp number
	uses(b,x)
	bsr	lamp_range_check
	ldx	#lamp_matrix
	bitshift
	orb	,x
	stb	,x
endp


proc(lamp_off)	; A = lamp number
	uses(b,x)
	bsr	lamp_range_check
	ldx	#lamp_matrix
	bitshift
	comb
	andb	,x
	stb	,x
endp


proc(lamp_range_check)
	cmpa	#64
	ifgt
		jsr	c_sys_error(ERR_INVALID_LAMP_NUM)
	endif
endp


proc(lamp_toggle)
	uses(b,x)
	bsr	lamp_range_check
	ldx	#lamp_matrix
	bitshift
	eorb	,x	
	stb	,x
endp


proc(lamp_test)
	uses(b,x)
	bsr	lamp_range_check
	ldx	#lamp_matrix
	bitshift
	bitb	,x	
endp


proc(lamp_flash)
	uses(b,x)
	bsr	lamp_range_check
	ldx	#lamp_flash_select
	bitshift
	orb	,x
	stb	,x
endp


proc(lamp_noflash)
	uses(b,x)
	bsr	lamp_range_check
	ldx	#lamp_flash_select
	bitshift
	comb
	andb	,x
	stb	,x
endp


proc(lamp_set_delay)
	requires(a)
	sta	lamp_apply_delay
endp


proc(lamp_apply_single)
	uses(a)
	lda	lamp_apply_delay
	jsr	task_sleep
	lda	saved(a)
	jsr	,x
endp

	; X = function to apply
	; Y = lampset pointer
proc(lamp_apply)
	uses(a,b,y)
	loop
		lda	,y+
		switch(a)
			case(LAMP_OP_SKIP_RANGE)
				lda	,y+
				ldb	,y+
				loop
					bsr	lamp_apply_single
					adda #2
					decb
				while(nz)
			endcase

			case(LAMP_OP_RANGE)
				lda	,y+
				ldb	,y+
				loop
					bsr	lamp_apply_single
					inca
					decb
				while(nz)
			endcase

			case(LAMP_OP_EXIT)
				return
			endcase

			bsr	lamp_apply_single
		endswitch
	endloop
endp


global(lampset_all)
	.db	LAMP_OP_RANGE, 0x00, 0x40, LAMP_OP_EXIT

global(lampset_first_half)
	.db	LAMP_OP_SKIP_RANGE, 0x00, 0x20, LAMP_OP_EXIT

global(lampset_second_half)
	.db	LAMP_OP_SKIP_RANGE, 0x01, 0x20, LAMP_OP_EXIT


proc(lamp_demo)
	loop /* main */

		lda	#TIME_33MS
		jsr	lamp_set_delay
	
		ldx	#lamp_toggle
		ldy	#lampset_all
		repeat(a,4)
			jsr	lamp_apply
			jsr	c_task_sleep(TIME_100MS * 2)
		endrep
	
		clr	lamp_set_delay
	
		ldx	#lamp_toggle
		ldy	#lampset_first_half
		jsr	lamp_apply
		repeat(a,4)
			ldy	#lampset_all
			jsr	lamp_apply
			jsr	c_task_sleep(TIME_100MS * 2)
		endrep

	endloop /* main */
endp


