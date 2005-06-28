	.module kbhit.s
	.area sysrom
	.globl _kbhit	; int kbhit(void);
_kbhit:
	jsr	0xE809
	bcc	no_key
	ldb	#1
	rts
no_key:
	clra
	clrb
	rts
