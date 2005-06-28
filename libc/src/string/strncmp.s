	.module strncmp.s
	.area sysrom
	.globl _strncmp	; int strncmp(const char *s1, const char *s2, size_t n)
_strncmp:
	pshs	u,y,x
	ldx	8,s	; s1 -> X
	ldy	10,s	; s2 -> Y
	ldu	12,s	; n -> U
loop:
	cmpu	#0
	beq	equal
	leau	-1,u
	ldb	,y
	beq	end
	tst	,x
	beq	end
	cmpb	,x
	bne	end
	leax	1,x
	leay	1,y
	bra	loop
end:
	clra		; we need to extend the two bytes
	pshs	b,a	; to do an unsigned 8-bit difference
	ldb	,x
	subd	,s++
	puls	x,y,u,pc
equal:
	clra
	clrb
	puls	x,y,u,pc
