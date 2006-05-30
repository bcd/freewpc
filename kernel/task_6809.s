   .area sysrom
	.globl _task_save
_task_save:
	stu	*_task_save_U	;movhi: R:u -> *_task_save_U
	stx	*_task_save_X	;movhi: R:x -> *_task_save_X
; Begin inline assembler code
	puls	u
; End of inline assembler code
	ldx	*_task_current	;movhi: *_task_current -> R:x
	stu	2,x	;movhi: R:u -> 2,x
	ldu	*_task_save_U	;movhi: *_task_save_U -> R:u
	stu	12,x	;movhi: R:u -> 12,x
	ldu	*_task_save_X	;movhi: *_task_save_X -> R:u
	stu	6,x	;movhi: R:u -> 6,x
; Begin inline assembler code
	sty	8,x
	leau	,s
; End of inline assembler code
	leay	21,x	;addhi: R:y = R:x + #21
; Begin inline assembler code
	leay	,y
; End of inline assembler code
	clrb		;movqi: ZERO -> R:b
	bra	L33	;length = 2
L34:
; Begin inline assembler code
	lda	,u+
	sta	,y+
; End of inline assembler code
	incb		;addqi: R:b ++
L33:
	cmpu	#6135	;cmphi:
	bls	L34	;length = 2
	stb	10,x	;movqi: R:b -> 10,x
	cmpb	_task_largest_stack	;cmpqi:
	bls	L29	;length = 2
	stb	_task_largest_stack	;movqi: R:b -> _task_largest_stack
	ldd	2,x	;movhi: 2,x -> R:d
	std	_task_largest_stack_pc	;movhi: R:d -> _task_largest_stack_pc
L29:
	ldb	16380	;movqi: 16380 -> R:b
	stb	17,x	;movqi: R:b -> 17,x
; Begin inline assembler code
	jmp _task_dispatcher
; End of inline assembler code
	.globl _task_restore
_task_restore:
	stx	*_task_current	;movhi: R:x -> *_task_current
; Begin inline assembler code
	orcc	#0x10
	orcc	#0x40
	ldu	#6136
; End of inline assembler code
	ldb	10,x	;zero_extendqihi: 10,x -> R:d
	clra
	leay	d,x	;addhi: R:y = R:x + R:d
	leay	21,y	;addhi: R:y = R:y + #21
; Begin inline assembler code
	leay	,y
; End of inline assembler code
	ldb	10,x	;movqi: 10,x -> R:b
	bra	L42	;length = 2
L43:
; Begin inline assembler code
	lda	,-y
	sta	,-u
; End of inline assembler code
	decb		;addqi: R:b --
L42:
	tstb		;tstqi: R:b
	bne	L43	;length = 2
; Begin inline assembler code
	leas	,u
	andcc	#~0x40
	andcc	#~0x10
; End of inline assembler code
	ldb	17,x	;movqi: 17,x -> R:b
	stb	16380	;movqi: R:b -> 16380
; Begin inline assembler code
	ldu	2,x
	pshs	u
	ldy	8,x
	ldu	12,x
; End of inline assembler code
	clr	14,x	;movqi: ZERO -> R:14,x
; Begin inline assembler code
	ldx	6,x
	rts
; End of inline assembler code
	.globl _task_create
_task_create:
; Begin inline assembler code
	pshs	d,u,y
	pshs	d,u
	tfr	x,u
; End of inline assembler code
	jsr	_task_allocate	;CALL: R:x = _task_allocate (0 bytes)
; Begin inline assembler code
	stu	2,x
	puls	d,u
	sty	8,x
	stu	12,x
; End of inline assembler code
	clr	,x	;movqi: ZERO -> R:,x
	ldd	#0	;movhi: #0 -> R:d
	std	19,x	;movhi: R:d -> 19,x
	ldb	16380	;movqi: 16380 -> R:b
	stb	17,x	;movqi: R:b -> 17,x
	clr	10,x	;movqi: ZERO -> R:10,x
; Begin inline assembler code
	puls	d,u,y,pc
; End of inline assembler code

