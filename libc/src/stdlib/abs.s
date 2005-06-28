#include "errno.h"

	.module abs.s
	.area sysrom
	.globl _abs	; int abs(int i)
_abs:
	ldd	2,s	; i -> D
	bpl	end
	cmpd	#0x8000	; largest negative number
	bne	negate
	ldd	#ERANGE
	std	_errno
	rts
negate:			; two-complement
	nega
	negb
	sbca	#0
end:
	rts
