	.module memmove.s
	.area sysrom
	.globl _memmove	; void *memmove(void *s1, const void *s2, size_t n)
_memmove:
	pshs	u,y,x
	ldx	8,s	; s1 -> X
	ldy	10,s	; s2 -> Y
	ldu	12,s	; n -> U
	cmpx	10,s
	bhs	reverse
	jsr	_movedata
	bra	end
reverse:
	ldd	12,s
	jsr	_movedata_rev
end:
	ldd	8,s
	puls	x,y,u,pc

	.globl _movedata_rev
_movedata_rev:
	leax	d,x	; end of the string
	leay	d,y	; end of the string
loop:
	cmpu	#0	; leau does not modify cc
	beq	end_rev
	leau	-1,u
	lda	,-y
	sta	,-x
	bra	loop
end_rev:
	rts
