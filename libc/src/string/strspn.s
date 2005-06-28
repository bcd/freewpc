	.module strspn.s
	.area sysrom
	.globl _strspn	; size_t strspn(const char *s1, const char *s2)
_strspn:
	pshs	y,x
	ldx	6,s	; s1 -> X
loop_x:
	lda	,x+
	beq	end	; end of s1 test
	ldy	8,s	; s2 -> Y
loop_y:
	tst	,y
	beq	end	; if end of s2
	cmpa	,y+
	beq	loop_x
	bra	loop_y
end:
	leax	-1,x	; correct off-by-one
	tfr	x,d
	subd	6,s
	puls	x,y,pc
