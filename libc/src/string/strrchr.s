	.module strrchr.s
	.area sysrom
	.globl _strrchr	; char *strrchr(const char *s, int c);
_strrchr:
	pshs	x
	ldx	4,s	; s -> X
	lda	7,s	; LO(c) -> A
loop_pos:
	tst	,x+
	bne	loop_pos
loop_scan:
	cmpa	,-x
	beq	found
	cmpx	4,s
	bne	loop_scan
	clra
	clrb
	puls	x,pc
found:
	tfr	x,d
	puls	x,pc
