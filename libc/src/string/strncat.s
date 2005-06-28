	.module strncat.s
	.area sysrom
	.globl _strncat	; char *strncat(char *s1, const char *s2, size_t n)
_strncat:
	pshs	u,y,x
	ldx	8,s	; s1 -> X
	ldy	10,s	; s2 -> Y
	ldu	12,s	; n -> U
loop_search:
	tst	,x
	beq	loop_copy
	leax	1,x
	bra	loop_search
loop_copy:
	cmpu	#0
	beq	end
	leau	-1,u
	lda	,y+
	beq	end
	sta	,x+
	bra	loop_copy
end:
	clr	,x	; terminating NULL is copied
	ldd	8,s
	puls	x,y,u,pc
