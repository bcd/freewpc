	.module exit.s
	.area sysrom
	.globl _exit	; void exit(int status)
_exit:
	leas	2,s	; eat saved PC
	jmp	___exit
