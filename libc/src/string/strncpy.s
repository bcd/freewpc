	.module strncpy.s
	.area sysrom
	.globl _strncpy	; char *strncpy(char *s1, const char *s2, size_t n)
_strncpy:
	pshs	u,y,x
	ldx	8,s	; s1 -> X
	ldy	10,s	; s2 -> Y
	ldu	12,s	; n -> U
loop_copy:
	cmpu	#0
	beq	end
	leau	-1,u
	lda	,y+
	sta	,x+	; terminating NULL is copied
	bne	loop_copy
loop_fill:
	cmpu	#0
	beq	end
	leau	-1,u
	clr	,x+
	bra	loop_fill
end:
	ldd	8,s
	puls	x,y,u,pc
