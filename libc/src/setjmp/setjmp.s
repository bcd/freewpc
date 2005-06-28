; jmp_buf:
;   ar br xr yr ur sr pc dp cc
;   0  1  2  4  6  8  10 12 13
;
	.module setjmp.s
	.area sysrom
	.globl _setjmp	; int setjmp(jmp_buf env)
_setjmp:
	pshs	y,x
	ldx	6,s	; env -> X
	sta	,x
	stb	1,x
	ldy	,s	; saved X -> Y
	sty	2,x
	ldy	2,s	; saved Y -> Y
	sty	4,x
	stu	6,x
	leay	6,s	; pre-jsr state -> Y
	sty	8,x
	ldy	4,s	; saved PC -> Y
	sty	10,x
	tfr	dp,a
	sta	12,x
	tfr	cc,a
	sta	13,x
	clra
	clrb
	puls	x,y,pc
