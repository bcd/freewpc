	.module strlen.s
	.area sysrom
	.globl _strlen	; size_t strlen(const char *s)
_strlen:
	pshs	x
	ldx	4,s	; s -> X
loop:
	tst	,x
	beq	end
	leax	1,x
	bra	loop
end:
	tfr	x,d
	subd	4,s
	puls	x,pc
