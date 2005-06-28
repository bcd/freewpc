	.module strxfrm.s
	.area sysrom
	.globl _strxfrm	; size_t strxfrm(char *s1, const char *s2, size_t n)
_strxfrm:
	pshs	u,y,x
	ldx	8,s	; s1 -> X
	ldy	10,s	; s2 -> Y
	ldu	12,s	; n -> U
	beq	loop_count
loop_copy:
	lda	,y+
	sta	,x+
	beq	end
	leau	-1,u
	cmpu	#0
	bne	loop_copy
loop_count:
	tst	,y+
	bne	loop_count
end:
	leay	-1,y	; correct off-by-one
	tfr	y,d
	subd	10,s
	puls	x,y,u,pc
