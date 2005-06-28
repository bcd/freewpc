	.module memchr.s
	.area sysrom
	.globl _memchr	; void *memchr(const void *s, unsigned char c, size_t n)
_memchr:
	pshs	y,x
	ldx	6,s	; s -> X
	lda	8,s	; c -> A
	ldy	9,s	; n -> Y
loop:
	beq	no_occur
	cmpa	,x+
	beq	found
	leay	-1,y
	bra	loop
found:
	leax	-1,x	; correct off-by-one
	tfr	x,d
	puls	x,y,pc
no_occur:
	clra
	clrb
	puls	x,y,pc
