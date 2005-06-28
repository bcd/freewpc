	.module memcpy.s
	.area sysrom
	.globl _memcpy	; void *memcpy(void *s1, const void *s2, size_t n)
_memcpy:
	pshs	u,y,x
	ldx	8,s	; s1 -> X
	ldy	10,s	; s2 -> Y
	ldu	12,s	; n -> U
	jsr	_movedata
	ldd	8,s
	puls	x,y,u,pc
