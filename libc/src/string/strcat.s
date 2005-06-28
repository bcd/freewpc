	.module strcat.s
	.area sysrom
	.globl _strcat	; char *strcat(char *s1, const char *s2)
_strcat:
	pshs	y,x
	ldx	6,s	; s1 -> X
	ldy	8,s	; s2 -> Y
loop_search:
	tst	,x
	beq	loop_copy
	leax	1,x
	bra	loop_search
loop_copy:
	lda	,y+
	sta	,x+	; terminating NULL is copied
	bne	loop_copy
	ldd	6,s
	puls	x,y,pc
