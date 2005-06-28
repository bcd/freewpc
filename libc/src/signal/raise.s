#include "signal.h"

	.module raise.s
	.area sysrom
	.globl _raise	; int raise(int sig)
_raise:
	ldb	3,s	; sig -> B
	cmpb	#SIGABRT
	blt	error
	cmpb	#SIGTERM
	bgt	error
	stb	signum
	swi
signum:
	.byte	0	;
	clra
	clrb
	rts
error:
	ldb	#1
	rts
