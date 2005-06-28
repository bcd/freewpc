	.module strcpy.s
	.area sysrom
	.globl _strcpy	; char *strcpy(char *s1, const char *s2)
_strcpy:
	pshs	y,x
	ldx	6,s	; s1 -> X
	ldy	8,s	; s2 -> Y
loop:
	lda	,y+
	sta	,x+	; terminating NULL is copied
	bne	loop
	ldd	6,s
	puls	x,y,pc
