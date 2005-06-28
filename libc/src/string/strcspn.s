	.module strcspn.s
	.area sysrom
	.globl _strcspn	; size_t strcspn(const char *s1, const char *s2)
_strcspn:
	pshs	y,x
	ldx	6,s	; s1 -> X
loop_x:
	lda	,x+
	ldy	8,s	; s2 -> Y
loop_y:
	cmpa	,y
	beq	end	; also used for end of s1 test
	tst	,y+
	beq	loop_x	; if end of s2
	bra	loop_y
end:
	leax	-1,x	; correct off-by-one
	tfr	x,d
	subd	6,s
	puls	x,y,pc
