	.module rand.s	; algorithm from Pascal Base
	.area sysrom
	.globl _rand	; int rand(void)
_rand:
	lda	___random_seed+1
	tfr	a,b
	anda	#1
	lsrb
	lsrb
	lsrb
	lsrb
	adca	#0
	ldb	___random_seed
	lslb
	adca	#0
	lslb
	lslb
	adca	#0
	lsra
	ldd	___random_seed
	rolb
	rola
	std	___random_seed
	anda	#0x7F	; required by ANSI C
	rts

	.area _DATA
	.globl	___random_seed
___random_seed:
	.word	1	; required by ANSI C
