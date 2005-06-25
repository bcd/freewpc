
#include <freewpc.h>


.globl _lamp_matrix
.globl _lamp_flash_select
.globl _lamp_flash_max
.globl _lamp_flash_count
.globl _lamp_apply_delay


.area sysrom


proc(lamp_rtt)
	lda	_irq_shift_count
	sta	WPC_LAMP_COL_STROBE
	lda	_irq_count
	anda	#0x07
	ldx	#_lamp_matrix
	lda	a,x
	sta	WPC_LAMP_ROW_OUTPUT
endp



proc(lamp_set_delay)
	requires(a)
	sta	_lamp_apply_delay
endp


proc(lamp_apply_single)
	uses(a)
	lda	_lamp_apply_delay
	jsr	task_sleep
	lda	saved(a)
	sta	,-s
	jsr	,x
	leas	1,s
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


proc(_lamp_demo)
	loop /* main */

		lda	#TIME_33MS
		jsr	lamp_set_delay
	
		ldx	#_lamp_toggle
		ldy	#lampset_all
		repeat(a,4)
			jsr	lamp_apply
			jsr	c_task_sleep(TIME_100MS * 2)
		endrep
	
		clr	lamp_set_delay
	
		ldx	#_lamp_toggle
		ldy	#lampset_first_half
		jsr	lamp_apply
		repeat(a,4)
			ldy	#lampset_all
			jsr	lamp_apply
			jsr	c_task_sleep(TIME_100MS * 2)
		endrep

	endloop /* main */
endp


