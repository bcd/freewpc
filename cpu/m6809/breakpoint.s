
	.module	breakpoint.s

RTS_OPCODE=0x39
JMP_OPCODE=0x7E
COMMON_PAGE=56

	.area direct
bpt_handler:: .blkb 3


	.area	ram
bpt_addr: 	.blkb 3
bpt_dreg:	.blkb 2
bpt_xreg:	.blkb 2
bpt_yreg:	.blkb 2
bpt_ureg:	.blkb 2
bpt_time:   .blkb 2

	.area .text


	; When the breakpoint is reset, the breakpoint handler gets an RTS
	; instruction installed there, so the overhead of the call is
	; minimal.  Each call is only 7+5=12 CPU cycles.
_bpt_init::
_bpt_reset::
	lda	#RTS_OPCODE
	sta	*bpt_handler
	rts

	; When a breakpoint is set, the bpt_check is installed in the later
	; via a JMP instruction.  The overhead of each check is 4 extra cycles
	; for the JMP, plus the time it takes bpt_check to see if it is the
	; desired break address.
_bpt_set::
	leax	2,x
	stx	bpt_addr
	stb	bpt_addr+2
	lda	#JMP_OPCODE
	sta	*bpt_handler
	ldx	#_bpt_check
	stx	*bpt_handler+1
	rts

	; Check if the caller's address matches the installed breakpoint.
	; Take care not to destroy any registers in the process.
_bpt_check::
	stx	bpt_xreg             ; Ensure X is preserved
	ldx	,s                   ; Get the return address
	cmpx	bpt_addr             ; Does this match the installed breakpoint?
	bne	1$                   ; Nope, return

	sty	bpt_yreg
	stu	bpt_ureg
	std	bpt_dreg

	ldx	_sys_time            ; Save system time
	stx	bpt_time

	jsr	__far_call_handler   ; Handle the breakpoint hit
		.dw	_bpt_hit
		.db	COMMON_PAGE

	ldx	bpt_time             ; Restore system time
	stx	_sys_time
1$:
	ldx	bpt_xreg             ; Restore X and return
	rts
