	.module abort.s
	.area sysrom
	.globl _abort	; void abort(void)
_abort:
	swi
	.byte	1	; SIGABRT
	rts
