

#include "wpc.h"


.area fastram

irq_count::			.blkb 1
irq_shift_count::	.blkb	1
tick_count::		.blkb 1

.area ram

#ifdef DEBUG
checkpoint_value:		.blkb 1
#endif
sys_init_complete:	.blkb 1
errno:					.blkb 1
err_pc:					.blkw 1
err_a:					.blkb 1
err_b:					.blkb 1
err_u:					.blkw 1
err_x:					.blkw 1
err_y:					.blkw 1
err_s:					.blkw 1

.area sysrom


macro(led_toggle)
	lda	WPC_LEDS
	eora	#0x80
	sta	WPC_LEDS
endmacro


#ifdef DEBUG
macro(checkpoint)
	pshs	a
	lda	#$1
	sta	checkpoint_value
	puls	a
endmacro
#else
#define checkpoint(x)
#endif


	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;
	; Function:		sys_reset
	;
	; Description:
	;
	; Inputs:
	;
	; Outputs:
	;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
proc(sys_reset)
	;;; Initialize CPU
	clra									; Initialize zero page to 0000h
	tfr	a, dp

	;;; Initialize RAM
	led_toggle
	clra
	ldx	#USER_RAM_SIZE
ram_loop:
	sta	,x
	leax	-1,x
	bne	ram_loop

	/* Initialize the RAM protection circuit */
	lda	#RAM_UNLOCKED
	sta	WPC_RAM_LOCK
	lda	#RAM_LOCK_512
	sta	WPC_RAM_LOCKSIZE
	sta	WPC_RAM_LOCK

	;;; Set up the stack
	lds	#STACK_BASE
	checkpoint(1)

	;;; Test the RAM
	led_toggle
	checkpoint(2)

	;;; TODO : Checksum the ROM

	;;; Initialize I/O devices
	led_toggle
	clr	sys_init_complete
	checkpoint(3)
	jsr	sol_init
	checkpoint(4)
	jsr	dmd_init
	checkpoint(5)
	jsr	switch_init
	jsr	sound_init

	;;; Initialize subsystems
	led_toggle
	jsr	trace_init
	jsr	irq_init
	jsr	heap_init
	jsr	task_init
	jsr	deff_init

	lda	#0x06
	sta	WPC_ZEROCROSS_IRQ_CLEAR

	;;; Mark initialization complete
	inc	sys_init_complete

	;;; Enable interrupts
	led_toggle
	andcc	#~(CC_IRQ+CC_FIRQ)

	jsr	test_init
	jsr	c_task_create(lamp_demo)

	ldd	#0x1234
	ldu	#0x5678
	ldx	#0x9ABC
	ldy	#0xDEF0
	loop
		inc	0x0043

		cmpd	#0x1234
		ifne
			jsr	c_sys_error(ERR_TASK_REGISTER_SAVE)
		endif

		cmpu	#0x5678
		ifne
			jsr	c_sys_error(ERR_TASK_REGISTER_SAVE)
		endif

		cmpx	#0x9ABC
		ifne
			jsr	c_sys_error(ERR_TASK_REGISTER_SAVE)
		endif

		cmpy	#0xDEF0
		ifne
			jsr	c_sys_error(ERR_TASK_REGISTER_SAVE)
		endif
	
		jsr	task_yield
	endloop
endp


proc(irq_init)
	ldb	#1
	stb	irq_shift_count
	clr	irq_count
endp


interrupt proc(sys_irq)
	lda	#0x96
	sta	WPC_ZEROCROSS_IRQ_CLEAR

	;;;;;;;;;; Execute tasks every 1ms ;;;;;;;;;;;;;;
	led_toggle
	jsr	switch_rtt
	jsr	lamp_rtt
	jsr	sol_rtt

	inc	irq_count

	ldb	irq_shift_count
	lslb
	tstb
	ifz
		;;;;;;;;;;; Execute tasks every 8ms ;;;;;;;;;;;;;;;
		inc	tick_count

		incb
	endif
	stb	irq_shift_count
endp


interrupt proc(sys_firq)
	tst	WPC_PERIPHERAL_TIMER_FIRQ_CLEAR
	bmi	timer_int

dmd_int:
	jsr	dmd_rtt
	bra	end_firq

timer_int:

end_firq:
	clr	WPC_PERIPHERAL_TIMER_FIRQ_CLEAR
endp


interrupt proc(sys_nmi)
	jsr	c_sys_error(ERR_NMI)
endp

interrupt proc(sys_swi)
	jsr	c_sys_error(ERR_SWI)
endp

interrupt proc(sys_swi2)
	jsr	c_sys_error(ERR_SWI2)
endp

interrupt proc(sys_swi3)
	jsr	c_sys_error(ERR_SWI3)
endp


sys_error_const::
	orcc	#CC_IRQ
	pshs	a,x
	ldx	3,s
	lda	,x+
	sta	errno
	stx	3,s
	stx	err_pc
	puls	a,x
	sta	err_a
	stb	err_b
	stu	err_u
	stx	err_x
	sty	err_y
	leau	2,s
	stu	err_s
	loop
	endloop

