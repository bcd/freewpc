	.module strcmp.s
	.area sysrom
	.globl _strcmp	; int strcmp(const char *s1, const char *s2)
_strcmp:
	pshs	y,x
	ldx	6,s	; s1 -> X
	ldy	8,s	; s2 -> Y
loop:
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
	puls	x,y,pc
