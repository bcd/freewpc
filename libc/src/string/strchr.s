	.module strchr.s
	.area sysrom
	.globl _strchr	; char *strchr(const char *s, int c);
_strchr:
	pshs	x
	ldx	4,s	; s -> X
	lda	7,s	; LO(c) -> A
loop:
	cmpa	,x
	beq	found
	tst	,x+
	bne	loop
	clra
	clrb
	puls	x,pc
found:
	tfr	x,d
	puls	x,pc
