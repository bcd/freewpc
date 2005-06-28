	.module clock.s
	.area _CODE
	.globl _clock		; clock_t clock(void);
_clock:
	lda	first		; first call ?
	beq	end		; no!
	clra
	sta	first
	orcc	#0x50		; disable F and I IRQs
	clrb
	std	timer_tick	; zero timer_tick
	ldd	#timer_handler
	std	0x6027		; hook up timer
	lda	0x6019
	ora	#0x20		; validate timer IRQ
	sta	0x6019
	ldd	#clock_exit
	pshs	b,a
	jsr	_atexit		; register clock_exit
	leas	2,s
	andcc	#0xAF		; re-enable F and I IRQs
end:
	ldd	timer_tick
	rts
clock_exit:
	orcc	#0x50		; disable F and I IRQs
	lda	0x6019
	anda	#0xDF		; invalidate timer IRQ
	sta	0x6019
	andcc	#0xAF		; re-enable F and I IRQs
	inc	first
	rts
timer_handler:
	pshs	b,a
	ldd	timer_tick
	addd	#1
	std	timer_tick
	puls	a,b
	cmpx	0xE7C6		; ack. timer interrupt
	rti

	.area _DATA
first:
	.byte	1

	.area _BSS
timer_tick:
	.blkw	1
