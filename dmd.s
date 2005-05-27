

#include "wpc.h"


.area ram

free_page:			.blkb 1
low_page:			.blkb 1
high_page:			.blkb 1
visible_page:		.blkb 1

.area sysrom


proc(dmd_init)
	lda	#0xFF
	sta	WPC_DMD_FIRQ_ROW_VALUE

	clra
	sta	low_page
	sta	WPC_DMD_LOW_PAGE
	sta	high_page
	sta	WPC_DMD_HIGH_PAGE
	sta	visible_page
	sta	WPC_DMD_ACTIVE_PAGE
	inca
	sta	free_page
endp


proc(dmd_rtt)
endp


proc(dmd_alloc)
	returns(a)
	lda	free_page
	pshs	a
	inca
	anda	#(DMD_PAGE_COUNT - 1)
	sta	free_page
	puls	a
endp


proc(dmd_alloc_low)
	uses(a)
	bsr	dmd_alloc
	sta	low_page
	sta	WPC_DMD_LOW_PAGE
endp


proc(dmd_alloc_high)
	uses(a)
	bsr	dmd_alloc
	sta	high_page
	sta	WPC_DMD_HIGH_PAGE
endp


proc(dmd_alloc_low_high)
	bsr	dmd_alloc_low
	bsr	dmd_alloc_high
endp


proc(dmd_show_low)
	uses(a)
	lda	low_page
	sta	visible_page
	sta	WPC_DMD_ACTIVE_PAGE
endp


proc(dmd_show_high)
	uses(a)
	lda	high_page
	sta	visible_page
	sta	WPC_DMD_ACTIVE_PAGE
endp


proc(dmd_clean_page)	; X = page pointer
	uses(a,u,x)
	ldu	#0
	repeat(a, 64)
		stu	,x++
		stu	,x++
		stu	,x++
		stu	,x++
	endrep
endp


proc(dmd_clean_low)
	uses(x)
	ldx	#DMD_LOW_BASE
	bsr	dmd_clean_page
endp


proc(dmd_clean_high)
	uses(x)
	ldx	#DMD_HIGH_BASE
	bsr	dmd_clean_page
endp


proc(dmd_invert_page)
	uses(d,u,x)
	ldu	#64
	loop
		ldd	,x
		coma
		comb
		std	,x++
		ldd	,x
		coma
		comb
		std	,x++
		ldd	,x
		coma
		comb
		std	,x++
		ldd	,x
		coma
		comb
		std	,x++
		leau	-1,u
		cmpu	#0000
	lwhile(ne)
endp


proc(dmd_copy_page) ; Y = source, X = destination
	uses(a,u,x,y)
	repeat(a, 64)
		ldu	,y++
		stu	,x++
		ldu	,y++
		stu	,x++
		ldu	,y++
		stu	,x++
		ldu	,y++
		stu	,x++
	endrep
endp

proc(dmd_copy_low_to_high)
	uses(x,y)
	ldx	#DMD_HIGH_BASE
	ldy	#DMD_LOW_BASE
	bsr	dmd_copy_page
endp


proc(dmd_alloc_low_clean)
	uses(x)
	jsr	dmd_alloc_low
	ldx	#DMD_LOW_BASE
	jsr	dmd_clean_page
endp


proc(dmd_alloc_high_clean)
	uses(x)
	jsr	dmd_alloc_high
	ldx	#DMD_HIGH_BASE
	jsr	dmd_clean_page
endp


   ; X = pointer to display page
proc(dmd_draw_border)
   uses(a,b,u,x,y)
   ldu   #0xffff
   leay  480,x

   repeat(a,16)
   	stu   ,x++
	   stu   ,y++
	endrep

   ldd   #0x03c0
   repeat(u,28)
   	sta   ,x
	   stb   15,x
		leax  16,x
	endrep
endp


proc(dmd_draw_border_low)
	uses(x)
	ldx	#DMD_LOW_BASE
	bsr	dmd_draw_border
endp


proc(dmd_shift_up)	; X = pointer to display page
endp




