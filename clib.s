
#include "wpc.h"

.area sysrom

	; memcmp (X, Y, A)
proc(memcmp)
	uses(a,b,x,y)
	loop
		ldb	,x+
		cmpb	,y+
		ifnz
			false
			return
		else
			deca
			ifz
				true
				return
			endif
		endif
	endloop
endp


	; memcpy (X, Y, A)
proc(memcpy)
	uses(a,b,x,y)
	loop
		ldb	,y+
		stb	,x+
		deca
	while(nz)
endp


	;;; memset (X, B, A)
proc(memset)	; X = pointer, B = character to write, A = number of chars
	uses(a,x)
	loop
		stb	,x+
		deca
	while(ne)
endp


proc(memsetl) ; X = pointer, B = character, Y = count
	uses(y,x)
	loop
		stb	,x+
		leay	-1,y
	while(ne)
endp

	;;; bzero (X, A)
proc(bzero)
	uses(a,x)
	loop
		clr	,x+
		deca
	while(ne)
endp


proc(bzerol)	; X = pointer, Y = length
	uses(y,x)
	loop
		clr	,x+
		leay	-1,y
		cmpy	#0000
	while(ne)
endp


	; X = pointer, Y = count
	; Returns checksum in A
proc(cksum)
	requires(x,y)
	uses(x,y)
	clra
	loop
		adda	,x+
		leay	-1,y
		cmpy	#0000
	while(nz)
	coma
endp


