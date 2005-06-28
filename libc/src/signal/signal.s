#include "signal.h"

	.module signal.s
	.area sysrom
	.globl _signal		; __sighandler_t signal(int sig, __sighandler_t func)
_signal:
	pshs	y,x
	ldb	7,s		; sig -> B
	cmpb	#SIGABRT
	blt	error
	cmpb	#SIGTERM
	bgt	error
	ldx	#___sig_vector
	aslb			; array of pointers (2 bytes)
	ldy	b,x		; read old signal handler
	pshs	y
	ldy	10,s		; func -> Y
	sty	b,x		; set new signal handler
	puls	a,b,x,y,pc
error:
	ldd	#__SIG_ERR
	puls	x,y,pc
