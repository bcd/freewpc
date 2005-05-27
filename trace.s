

.area ram

trace_buffer_ptr::	.blkw	1

trace_buffer::			.blkb	64
trace_buffer_end = .

.area sysrom


trace_init::
	pshs	x
	ldx	#trace_buffer
	stx	trace_buffer_ptr
	puls	x,pc


trace_byte::
	pshs	x
	ldx	trace_buffer_ptr
	sta	,x+
	cmpx	#trace_buffer_end
	blt	1$
	ldx	#trace_buffer
1$:
	stx	trace_buffer_ptr
	puls	x,pc


trace_word::
	pshs	a
	jsr	trace_byte
	tfr	b,a
	jsr	trace_byte
	puls	a,pc

