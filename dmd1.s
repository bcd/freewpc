

#include "wpc.h"


.area sysrom


;;;;;   ; X = pointer to display page
;;;;;proc(dmd_draw_border)
;;;;;   uses(a,b,u,x,y)
;;;;;   ldu   #0xffff
;;;;;   leay  480,x

;;;;;   repeat(a,16)
;;;;;   	stu   ,x++
;;;;;	   stu   ,y++
;;;;;	endrep
;;;;;
;;;;;   ldd   #0x03c0
;;;;;   repeat(u,28)
;;;;;   	sta   ,x
;;;;;	   stb   15,x
;;;;;		leax  16,x
;;;;;	endrep
;;;;;endp
;;;;;
;;;;;
;;;;;proc(dmd_draw_border_low)
;;;;;	uses(x)
;;;;;	ldx	#DMD_LOW_BASE
;;;;;	bsr	dmd_draw_border
;;;;;endp
;;;;;

;;;;; proc(dmd_shift_up)	; X = pointer to display page
;;;;; 	uses(d,u,x,y)
;;;;;	leay	16,x			; Y = pointer one line below current
;;;;;	ldd	#(31*16)/2	; D = number of words to be transferred
;;;;;	loop
;;;;;		ldu	,y++
;;;;;		stu	,x++
;;;;;		subd	#1
;;;;;		cmpd	#0
;;;;;	while(nz)
;;;;;	ldu	#0
;;;;;	stu	,x++
;;;;;	stu	,x++
;;;;;	stu	,x++
;;;;;	stu	,x++
;;;;;	stu	,x++
;;;;;	stu	,x++
;;;;;	stu	,x++
;;;;;	stu	,x++
;;;;;endp
;;;;;

