	.module getch.s
	.area sysrom
	.globl _getch	; int getch(void);
_getch:
	ldb	___ungetched_char
	beq	read_key
	clra
	sta	___ungetched_char
	rts
read_key:
	jsr	0xE806
	bcc	read_key
	clra
	rts
