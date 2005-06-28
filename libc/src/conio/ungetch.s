	.module ungetch.s
	.area sysrom
	.globl _ungetch	; int ungetch(unsigned char c);
_ungetch:
	ldb	2,s	; c -> B
	stb	___ungetched_char
	clra
	rts

	.area _DATA
	.globl ___ungetched_char
___ungetched_char:
	.byte	0
