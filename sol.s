
#include "wpc.h"
#include "mach_coil.h"

.area fastram

high_power_coils::		.blkb 1
low_power_coils::			.blkb 1
flash1_coils::				.blkb 1
flash2_coils::				.blkb 1
aux_coils1::				.blkb 1
aux_coils2::				.blkb 1
coils_cksum::				.blkb 1


.area ram

pulse_queue_head::		.blkw 1
pulse_queue_tail::		.blkw 1

pulse_queue::				.blkw 8


.area sysrom


proc(sol_init)
	uses(x,y)
	ldx	#high_power_coils
	ldy	#6
	jsr	bzerol
	bsr	sol_update_cksum
endp


proc(sol_calc_cksum)
	uses(x,y)
	returns(a)
	ldx	#high_power_coils
	ldy	#6
	jsr	cksum
endp

proc(sol_update_cksum)
	uses(a)
	bsr	sol_calc_cksum
	sta	coils_cksum
endp


proc(sol_verify_cksum)
	uses(a)
	bsr	sol_calc_cksum
	cmpa	coils_cksum
	ifeq
		true
	else
		false
	endif
endp


proc(sol_rtt)
	;bsr	sol_verify_cksum
	;iftrue
		lda	low_power_coils
		sta	WPC_SOL_LOWPOWER_OUTPUT
		lda	high_power_coils
		sta	WPC_SOL_HIGHPOWER_OUTPUT
		lda	flash1_coils
		sta	WPC_SOL_FLASH1_OUTPUT
		lda	flash2_coils
		sta	WPC_SOL_FLASH2_OUTPUT

		lda	aux_coils2
		sta	WPC_EXTBOARD1
	;endif
endp


proc(sol_lookup)
	requires(a)		; A = solenoid number
	returns(x,b)
	ldx	#high_power_coils
	stx	WPC_SHIFTADDR
	sta	WPC_SHIFTBIT
	ldx	WPC_SHIFTADDR
	ldb	WPC_SHIFTBIT
endp


proc(sol_on)
	uses(b,x)
	bsr	sol_lookup
	orb	,x
	stb	,x
	jsr	sol_update_cksum
endp


proc(sol_off)
	uses(b,x)
	bsr	sol_lookup
	comb
	andb	,x
	stb	,x
	jsr	sol_update_cksum
endp


proc(sol_serve)
	uses(a)
	lda	#SOL_BALL_SERVE
	bsr	sol_on
	jsr	c_task_sleep(TIME_33MS)
	bsr	sol_off
endp


