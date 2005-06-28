	.module memcmp.s
	.area sysrom
	.globl _memcmp	; int memcmp(const void *s1, const void *s2, size_t n)
_memcmp:
	pshs	u,y,x
	ldx	8,s	; s1 -> X
	ldy	10,s	; s2 -> Y
	ldu	12,s	; n -> U
loop:
	cmpu	#0	; leau does not modify cc
	beq	equal
	leau	-1,u
	ldb	,y+
	cmpb	,x+
	beq	loop
	clra		; we need to extend the two bytes
	pshs	b,a	; to do an unsigned 8-bit difference
	ldb	-1,x	; correct off-by-one
	subd	,s++
	puls	x,y,u,pc
equal:
	clra
	clrb
	puls	x,y,u,pc
