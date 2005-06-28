	.module atexit.s
	.area sysrom
	.globl _atexit		; int atexit(void (*func)(void))
_atexit:
	pshs	y,x
	ldx	6,s		; func -> X
	lda	___exit_count
	cmpa	#16		; at most 16 registered functions
	beq	error
	ldy	#___exit_func
	asla			; array of pointers (2 bytes)
	stx	a,y
	inc	___exit_count
	clra
	clrb
	puls x,y,pc
error:
	ldb	#1
	puls x,y,pc
