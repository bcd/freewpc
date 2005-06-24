
#include "wpc.h"

#define TMR_OFF_ID		0
#define TMR_OFF_STATE	1
#define TMR_SIZE			2

#define TMR_TABLE_SIZE	(TMR_SIZE * MAX_TIMERS)


/*
 * Format of the timer state:
 *
 * Bit 7 is a flag indicating whether or not the timer is running.
 * Bits 0-6 stores the current value of the timer.
 * These timers have a different granularity than the tasks do;
 * each timer tick represents 33ms, or 4 normal ticks.
 * Thus, every 33ms, the timer update procedure runs through all
 * of its running timers and decrements their counts by 1.
 * The maximum timeout is 33ms * 255 = 255/30 seconds = ~8 sec.
 */

.area ram

timer_table::	.blkb TMR_TABLE_SIZE


.area sysrom

proc(timer_init)
	uses(x,y)
	ldx	#timer_table
	ldy	#TMR_TABLE_SIZE
	jsr	bzero
endp


proc(timer_update_proc)
	loop

		ldx	#timer_table
		loop
			tst	TMR_OFF_ID,x
			ifnz
				lda	TMR_OFF_STATE,x
				bita	#0x80
				ifz
					deca
					sta	TMR_OFF_STATE,x

					tsta
					ifz
						jsr	timer_free_slot
					endif
				endif
			endif

			leax	TMR_SIZE,x
			cmpx	#timer_table + TMR_TABLE_SIZE
		while(lt)

		jsr	c_task_sleep(TIME_33MS)
	endloop
endp


proc(timer_find_slot)
	requires(a)				/* A = timer ID, or zero to allocate */
	returns(x)				/* X = slot pointer */

	ldx	#timer_table
	loop
		cmpa	TMR_OFF_ID,x
		ifeq
			true
			return
		endif

		leax	TMR_SIZE,x
		cmpx	#timer_table + TMR_TABLE_SIZE
	while(lt)
	false
endp


proc(timer_allocate_slot)
	uses(a)
	returns(x)

	clra
	bsr	timer_find_slot
	iffalse
		jsr	c_sys_error(ERR_NO_FREE_TIMERS)
	endif
endp


proc(timer_free_slot)
	requires(x)				/* X = slot pointer */
	clr	TMR_OFF_ID,x
	clr	TMR_OFF_STATE,x
endp



	; A = timer ID
	; B = initial timer value
proc(timer_start)
	uses(a,x)
	bsr	timer_allocate_slot
	lsra									/* Convert to timer ticks from task ticks */
	lsra
	sta	TMR_OFF_ID,x
	stb	TMR_OFF_STATE,x
endp


	; A = timer ID
proc(timer_test)
	bsr	timer_find_slot
endp


proc(timer_restart)
endp

	; A = timer ID
proc(timer_suspend)
	uses(x)
	bsr	timer_find_slot
	lda	#0x80
	ora	TMR_OFF_STATE,x
	sta	TMR_OFF_STATE,x
endp


	; A = timer ID
proc(timer_resume)
	uses(x)
	bsr	timer_find_slot
	lda	#0x7F
	anda	TMR_OFF_STATE,x
	sta	TMR_OFF_STATE,x
endp


	; A = timer ID
proc(timer_stop)
	uses(x)
	bsr	timer_find_slot
	bsr	timer_free_slot
endp



