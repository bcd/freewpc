	.module strpbrk.s
	.area sysrom
	.globl _strpbrk	; char *strpbrk(const char *s1, const char *s2)
_strpbrk:
	pshs	y,x
	ldx	6,s	; s1 -> X
loop_x:
	lda	,x+
	beq	no_occur
	ldy	8,s	; s2 -> Y
loop_y:
	tst	,y
	beq	loop_x	; if end of s2
	cmpa	,y+
	bne	loop_y	; if no match
	leax	-1,x	; correct off-by-one
	tfr	x,d
	puls	x,y,pc
no_occur:
	clra
	clrb
	puls	x,y,pc
