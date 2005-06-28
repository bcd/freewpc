	.module getche.s
	.area sysrom
	.globl _getche	; int getche(void);
_getche:
	ldb	___ungetched_char
	beq	read_key
	clra
	sta	___ungetched_char
	rts
read_key:
	jsr	0xE806
	bcc	read_key
	pshs	b
	jsr	_putchar
	puls	b
	clra
	rts
