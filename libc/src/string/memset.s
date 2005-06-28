	.module memset.s
	.area sysrom
	.globl _memset	; void *memset(void *s, int c, size_t n)
_memset:
	pshs	u,x
	ldx	6,s	; s -> X
	lda	9,s	; LO(c) -> A
	ldu	10,s	; n -> U
loop:
	cmpu	#0	; leau does not modify cc
	beq	end
	leau	-1,u
	sta	,x+
	bra	loop
end:
	ldd	6,s
	puls	x,u,pc
