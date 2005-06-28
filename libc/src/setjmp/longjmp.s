; jmp_buf:
;   ar br xr yr ur sr pc dp cc
;   0  1  2  4  6  8  10 12 13
;
	.module longjmp.s
	.area sysrom
	.globl _longjmp	; void longjmp(jmp_buf env, int val)
_longjmp:
	ldx	2,s	; env -> X
	ldd	4,s	; val -> D
	std	,x	; store retval in env->dr
	ldu	6,x
	lds	8,x
	lda	12,x
	tfr	a,dp
	lda	13,x
	ldy	10,x
	pshs	y,a	; pshs pc,cc
	ldd	,x	; reload retval
	ldy	4,x
	ldx	2,x
	puls	cc,pc
