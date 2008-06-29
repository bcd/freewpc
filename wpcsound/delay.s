
	.area .ctors
__ctors::
	.area .dtors
__dtors::

; MDELAY_COUNT = 290   ; works for 1s delays
MDELAY_COUNT = 315   ; works for 1s delays

	.area		.text
	.globl	_mdelay1
_mdelay1:
	ldx	#MDELAY_COUNT
1$:
	leax	-1,x
	bne	1$
	rts

