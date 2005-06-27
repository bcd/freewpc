
#include <freewpc.h>

.area fastram

;;;;;/* The raw current state of the switches */
;;;;;switch_raw_bits::			.BLKB SWITCH_BITS_SIZE
;;;;;
;;;;;/* 0 if different from last reading, 1 if the same */
;;;;;switch_changed_bits::	.BLKB SWITCH_BITS_SIZE
;;;;;
;;;;;/* Latched version of above, 1 if switch still needs service */
;;;;;switch_pending_bits::	.BLKB	SWITCH_BITS_SIZE
;;;;;
;;;;;/* 1 if switch is scheduled for service but hasn't run yet
;;;;;	These bits are set when the idle task scans the pending bits
;;;;;	and finds work to be done */
;;;;;switch_queued_bits::		.BLKB	SWITCH_BITS_SIZE
;;;;;

#define switch_raw_bits			_switch_bits
#define switch_changed_bits	_switch_bits + (1 * SWITCH_BITS_SIZE)
#define switch_pending_bits	_switch_bits + (2 * SWITCH_BITS_SIZE)
#define switch_queued_bits		_switch_bits + (3 * SWITCH_BITS_SIZE)


.area sysrom

switch_handlers::
	/* Column 0 */
	.dw	_sw_left_coin 
	.dw	_sw_center_coin
	.dw	_sw_right_coin
	.dw	_sw_fourth_coin
	.dw	_sw_escape_button
	.dw	_sw_down_button
	.dw	_sw_up_button
	.dw	_sw_enter_button

	/* Column 1 */
	.dw	0, 0, _sw_start_button, 0, 0, 0, 0, 0

	/* Column 2 */
	.dw	0, 0, 0, 0, 0, 0, 0, 0

	/* Column 3 */
	.dw	0, 0, 0, 0, 0, 0, 0, 0

	/* Column 4 */
	.dw	0, 0, 0, 0, 0, 0, 0, 0

	/* Column 5 */
	.dw	0, 0, 0, 0, 0, 0, 0, 0

	/* Column 6 */
	.dw	0, 0, 0, 0, 0, 0, 0, 0

	/* Column 7 */
	.dw	0, 0, 0, 0, 0, 0, 0, 0



;;;;;proc(switch_init)
;;;;;	uses(x,y)
;;;;;	ldx	#switch_raw_bits
;;;;;	ldy	#_mach_opto_mask
;;;;;	lda	#SWITCH_BITS_SIZE
;;;;;	jsr	memcpy
;;;;;
;;;;;	ldx	#switch_changed_bits
;;;;;	ldy	#SWITCH_BITS_SIZE * 3
;;;;;	jsr	bzerol
;;;;;endp
;;;;;

proc(switch_rtt)
	ldb	#0x1
	stb	WPC_SW_COL_STROBE

define(rowpoll, [
	ldb	#pow2($1)
	lda	WPC_SW_ROW_INPUT
	stb	WPC_SW_COL_STROBE
	ldb	switch_raw_bits + $1
	sta	switch_raw_bits + $1
	eorb	switch_raw_bits + $1
	stb	switch_changed_bits + $1
	orb	switch_pending_bits + $1
	stb	switch_pending_bits + $1
])

	lda	WPC_SW_CABINET_INPUT		/* Get the current reading */
	ldb	switch_raw_bits			/* Get previous reading value */
	sta	switch_raw_bits			/* Store present reading */
	eorb	switch_raw_bits			/* Compare with last reading */
											/* B=0 : switch is same as before */
											/* B=1 : switch is different */
	stb	switch_changed_bits		/* Store change information */
	orb	switch_pending_bits		/* Add any changed switches into pending set */
	stb	switch_pending_bits		/* Save pending set */

	rowpoll(1)
	rowpoll(2)
	rowpoll(3)
	rowpoll(4)
	rowpoll(5)
	rowpoll(6)
	rowpoll(7)
	rowpoll(8)
endp


proc(switch_idle_task)
	uses(a,b,x)
	local(byte,rawbits)
	endlocal

	; Checking the pending bits on each switch column
	ldx	#0000

	; A holds the loop counter over the switch columns (0-7)
	clra
	loop
	
		orcc	#CC_IRQ								; Disable interrupts
		ldb	switch_raw_bits,x					; Grab the raw bits
		stb	rawbits
		ldb	switch_pending_bits,x			; Grab the pending bits
		andb	rawbits								; Only service 'active' bits
		clr	switch_pending_bits,x			; Clear the pending bits
		andcc	#~CC_IRQ								; Enable interrupts
		tstb											; Anything to be done here?
		ifnz
			; Yes, some switches here need to be serviced

			; Save the current column number and convert to
			; a switch number (0-63)
			pshs	a
			asla
			asla
			asla

			; Scan each bit to determine if the Nth switch needs service
			; or not.
			loop
				bitb	#0x01
				ifnz
					; Schedule the switch (A=switch number)
					pshs	x
					ldx	#switch_sched
					jsr	task_create
					puls	x
				endif
				inca
				lsrb
				lsr	rawbits
				tstb
			while(ne)

			; Restore the column number
			puls	a
		endif

		; Advance to next column
		leax	1,x
		inca
		cmpa	#SWITCH_BITS_SIZE
	while(ne)
endp

	; A = switch number
proc(switch_sched)
	tfr	a,b

	pshs	b
	ldx	#switch_handlers
	aslb
	ldx	b,x
	cmpx	#0
	ifnz
		jsr	task_create
	endif
	jsr	c_task_sleep(TIME_100MS * 4)
	puls	b

	jsr	c_task_sleep(TIME_100MS)

	/* Clear the queued bits for this switch, so that it can
	 * be scheduled again.
	 */
	tfr	b,a
	ldx	#switch_queued_bits
	bitshift
	comb
	andb	,x
	stb	,x

	jsr	task_exit
endp


