

#include <freewpc.h>

.area ram

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
;;;;;proc(sys_reset)
;;;;;	;;; Initialize CPU
;;;;;	clra									; Initialize zero page to 0000h
;;;;;	tfr	a, dp
;;;;;
;;;;;	;;; Initialize RAM
;;;;;	led_toggle
;;;;;	clra
;;;;;	ldx	#USER_RAM_SIZE
;;;;;ram_loop:
;;;;;	sta	,x
;;;;;	leax	-1,x
;;;;;	bne	ram_loop
;;;;;
;;;;;	/* Initialize the RAM protection circuit */
;;;;;	lda	#RAM_UNLOCKED
;;;;;	sta	WPC_RAM_LOCK
;;;;;	lda	#RAM_LOCK_512
;;;;;	sta	WPC_RAM_LOCKSIZE
;;;;;	sta	WPC_RAM_LOCK
;;;;;
;;;;;	;;; Set up the stack
;;;;;	lds	#STACK_BASE
;;;;;
;;;;;	;;; Jump to C initialization
;;;;;	jmp	_init
;;;;;
;;;;;	ldd	#0x1234
;;;;;	ldu	#0x5678
;;;;;	ldx	#0x9ABC
;;;;;	ldy	#0xDEF0
;;;;;	loop
;;;;;		inc	0x0043
;;;;;
;;;;;		cmpd	#0x1234
;;;;;		ifne
;;;;;			jsr	c_sys_error(ERR_TASK_REGISTER_SAVE)
;;;;;		endif
;;;;;
;;;;;		cmpu	#0x5678
;;;;;		ifne
;;;;;			jsr	c_sys_error(ERR_TASK_REGISTER_SAVE)
;;;;;		endif
;;;;;
;;;;;		cmpx	#0x9ABC
;;;;;		ifne
;;;;;			jsr	c_sys_error(ERR_TASK_REGISTER_SAVE)
;;;;;		endif
;;;;;
;;;;;		cmpy	#0xDEF0
;;;;;		ifne
;;;;;			jsr	c_sys_error(ERR_TASK_REGISTER_SAVE)
;;;;;		endif
;;;;;	
;;;;;		jsr	task_yield
;;;;;	endloop
;;;;;endp
;;;;;

sys_error_const::
	orcc	#CC_IRQ
	pshs	a,x
	ldx	3,s
	lda	,x+
	sta	_errcode
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

