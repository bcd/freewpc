	.module strstr.s
	.area sysrom
	.globl _strstr	; char *strstr(const char *s1, const char *s2)
_strstr:
	pshs	y,x
	ldx	6,s	; s1 -> X
	ldy	8,s	; s2 -> Y
	lda	,y	; s2[0] -> A
	beq	no_occur
loop_x:
	tst	,x
	beq	no_occur
	cmpa	,x+
	bne	loop_x
	pshs	x	; save s1 pos
	leay	1,y	; sync Y with X
loop_both:
	ldb,	y+
	beq	found
	cmpb	,x+
	beq	loop_both
	puls	x	; restore s1 pos
	ldy	8,s	; restore s2 -> Y
	bra	loop_x
found:
	puls	a,b	; saved s1 pos -> D
	subd	#1	; correct off-by-one
	puls	x,y,pc
no_occur:
	clra
	clrb
	puls	x,y,pc
