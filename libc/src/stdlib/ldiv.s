	.module ldiv.s
	.area sysrom
	.globl _ldiv	; ldiv_t ldiv(long numer, long denom);
_ldiv:
	jmp	_div
