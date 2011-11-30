;;;
;;; Copyright 2008, 2009, 2010 by Brian Dominy <brian@oddchange.com>
;;;
;;; This file is part of FreeWPC.
;;;
;;; FreeWPC is free software; you can redistribute it and/or modify
;;; it under the terms of the GNU General Public License as published by
;;; the Free Software Foundation; either version 2 of the License, or
;;; (at your option) any later version.
;;;
;;; FreeWPC is distributed in the hope that it will be useful,
;;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;;; GNU General Public License for more details.
;;;
;;; You should have received a copy of the GNU General Public License
;;; along with FreeWPC; if not, write to the Free Software
;;; Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
;;;

; This file implements a fast bitmap blit function, which can draw
; an uncompressed bitmap onto an arbitrary location on the DMD.
; It has several advantages over the old C version:
;
; - Bits are shifted at most once.  Before, for each byte, we did two shifts.
; The second shift was even worse because we negated the shift count too.
; Now, every input byte is shifted exactly one time.  If it is not needed
; right away, the bits are saved in an 'overflow' register that is used on
; the next iteration.
;
; - Optimal code is added for some common cases, where the byte width of the
; image is 1 or 2 bytes, and at most 16-bits are actually written per row.
; Here, we can use 16-bit read/write operations for efficiency.  These
; cases arise with smaller fonts, which makes rendering large strings much
; better.
;
; - Shift logic is refactored.  The shift count does not change throughout
; the procedure, so the shift function is a constant for a single run.
;
; - Less code overall.  It's more compact, so it runs faster.
;
; - For large bitmaps, interior bytes can be MOVed directly to memory;
; there is no need to OR into whatever is there.  ORing is only required
; on the edges when we are not byte-aligned.
;

	.module bitmap_6809.s

	.area		ram
	.globl	_bitmap_src
_bitmap_src:
	.blkw 2

#define _bitmap_width _font_width
#define _bitmap_height _font_height
#define _bitmap_byte_width _font_byte_width

	; Local variables used by these functions are kept
	; in these soft registers.
#define blit_overflow m0
#define bitmap_byte_width2 m1
	; 'shift' is a temporary for saving the number of
	; bits that the image will need to be shifted "to the right"
	; (which actually requires an arithmetic left shift).
#define bitmap_shift m2

#define erase_pattern *m2

	; The main blit function.
	;
	; On entry, X should point to the destination where the bitmap
	; should be drawn, and B should be the shift value (0-7).  Both
	; registers are call-clobbered.
	.area .text
	.globl _bitmap_blit_asm
_bitmap_blit_asm:
	pshs	u,y

#ifdef PARANOID
	; Ensure that the destination is a valid DMD page pointer.
	cmpx	#0x3800
	bge	ok
	ldb	#99
	jmp	_fatal
ok:
#endif

	; Load the source/destination pointers
	ldu	_bitmap_src

	; Calculate a pointer to the shift function in Y.
	; Note, if shift is zero, then this function is a no-op,
	; but there is still the overhead of calling/returning.
	stb	*bitmap_shift
	comb				; B = 7-B
	andb	#7
	aslb				; Scale by 2 instructions per shift
	ldy	#shiftword7
	leay	b,y

	; Examine the width and shift to determine the best way to
	; render this bitmap.  There are 3 methods implemented here:
	; one is generic and works in all cases, but it has high
	; overhead for small bitmaps, which are common with fonts.
	; The other 2 methods are optimized for small bitmaps.
	; These methods are called do_large, loop8, and loop16.
	lda	_bitmap_width
	tfr	a,b
	addb	*bitmap_shift
	cmpb	#16				; Does everything fit within 16-bits?
	bgt	do_large			; No, can't do this very fast
	cmpa	#8					; Is the source data 8-bits?
	bgt	loop16			; No, use the loop16 method
								; Yes, use the loop8 method

	; loop8 works when the source data is 8 bits wide or less per row,
	; for all shift values (the input is extended to and written
	; 16-bits at a time).  It is slightly inefficient if source
	; plus shift is also within 8-bits, but this is a rare
	; case (small font, really small shift value).
	; Unrolling this for bitmap_height makes no sense since the
	; height is also likely to be a small value.
loop8:
	lda	,u+				; Load 8 bits of bitmap data
	clrb						; Zero-extend to 16-bits for shifting (2 cycles)
	jsr	,y					; Shift into position (avg. 26 cycles)
	ora	,x					; Merge into DMD page (1st byte)
	orb	1,x				; Merge into DMD page (2nd byte)
	std	,x					; Save 16-bits to DMD memory in one shot
	leax	16,x
	dec	*_bitmap_height
	bne	loop8
	stu	_bitmap_src
	puls	u,y,pc

	; loop16 works when the source data is 16-bits wide, and
	; the width + shift count is still within 16-bits.
	; That is, shifting does not increase the number of
	; output bytes to be written.  This is the most optimal
	; case, similar to above but we can read input data twice
	; as fast.
loop16:
	ldd	,u++				; Load 16 bits of bitmap data
	jsr	,y					; Shift into position (avg. 26 cycles)
	ora	,x					; Merge into DMD page
	orb	1,x
	std	,x					; Save to DMD memory
	leax	16,x
	dec	*_bitmap_height
	bne	loop16
	stu	_bitmap_src
	puls	u,y,pc

	; do large handles larger bitmaps.  On entry, a is the
	; bitmap width.
	;
	; TODO: for a large bitmap, the "jsr"s throughout are the
	; most inefficient part of the algorithm.  There could be
	; dozens of these calls.
	; Consider creating a variant of do_large per possible shift
	; value, thereby eliminating that overhead at the expense
	; of code bloat.
do_large:
	; Figure out how many bytes of input data there are per row.
	; There must be at least 2 ... maybe more.
	adda	#7
	lsra
	lsra
	lsra

	; Essentially, there are three fragments to be written per row:
	; the first byte, which must be ORed to preserve bits on the
	; left edge of the bitmap; the middle bytes, which can be
	; SET directly since they totally override what was there;
	; and the last byte, which also must be ORed to preserve
	; on the right edge.
	; Ignore the byte on the left edge since it is handled
	; separately.  So bitmap_byte_width is at least 1.
	deca
	sta	*_bitmap_byte_width
	sta	*bitmap_byte_width2

large_row_loop:
	; First, deal with the left edge byte.  This looks a lot like
	; the loop8 case above.
	lda	,u+
	clrb
	jsr	,y
	ora	,x

	; Now, deal with the remaining input bytes (at least one).
	; On entry to the middle loop, register B holds the overflow
	; bits from the previous iteration.
	; Because byte width is at least 1, the condition can be
	; checked after the first iteration.
large_middle_loop:
	sta	,x+
	stb	*blit_overflow

	lda	,u+
	clrb
	jsr	,y
	ora	*blit_overflow

	dec	*_bitmap_byte_width
	bne	large_middle_loop

	; Finally, whatever is in overflow needs to be ORed to the
	; display.
	orb	1,x
	std	,x+

	; Move on to the next row.
	; First, restore the width counter to its maximum.
	ldb	*bitmap_byte_width2
	stb	*_bitmap_byte_width
	; Reset the destination pointer to the left edge of the image.
	; The following calculation does B = 15-B, which is the amount
	; to be added to get back to the left edge.
	comb
	andb	#15
	abx
	dec	*_bitmap_height
	bne	large_row_loop
	stu	_bitmap_src
	puls	u,y,pc


	; Worst case for shifting by 7 is 14 instructions x 2 = 28 cycles.
	; On average it will be 14 cycles exactly.  Plus the 7 cycles to
	; call and 5 to return... average of 26 cycles.
	; Note each section must be exactly 2 bytes long for the
	; code above that calculates the start address to work.
	; (Cycle counts: 12, 16, 20, 24, 28, 32, 36, 29)
shiftword7:
	bra	fastshiftword7  ; see below, this case is optimized
shiftword6:
	asla
	rolb
shiftword5:
	asla
	rolb
shiftword4:
	asla
	rolb
shiftword3:
	asla
	rolb
shiftword2:
	asla
	rolb
shiftword1:
	asla
	rolb
shiftword0:
	rts

	; This version of shift by 7 takes only 14 cycles, plus 3 cycles to
	; branch here, for a savings of 11 cycles.  For large bitmaps that
	; require this, this can be significant.  TODO: do similar for
	; shift by 6?
fastshiftword7:
	rorb
	rora
	rorb
	andb	#128
	exg	a,b
	rts


	; The erase function.
	;
	; On entry, X should point to the destination where the bitmap
	; should be drawn, and B should be the shift value (0-7).  Both
	; registers are call-clobbered.  bitmap_width and bitmap_height
	; should contain the dimensions of the region to be erased.
	; This routine is basically a copy of the 'large loop' blit
	; function above, but with source data assumed to be all zeroes.
	.area .text
	.globl	_bitmap_erase_asm
_bitmap_erase_asm:
	pshs	y

	; Calculate a pointer to the shift function in Y.
	; Note, if shift is zero, then this function is a no-op,
	; but there is still the overhead of calling/returning.
	comb				; B = 7-B
	andb	#7
	aslb				; Scale by 2 instructions per shift
	ldy	#shiftword7
	leay	b,y

	; Figure out how many bytes of zeroes there are per row.
	lda	_bitmap_width
	adda	#7
	lsra
	lsra
	lsra

	; Essentially, there are three fragments to be written per row:
	; the first byte, which must be ORed to preserve bits on the
	; left edge of the bitmap; the middle bytes, which can be
	; SET directly since they totally override what was there;
	; and the last byte, which also must be ORed to preserve
	; on the right edge.
	; Ignore the byte on the left edge since it is handled
	; separately.  So bitmap_byte_width is at least 1.
	deca
	sta	*_bitmap_byte_width
	sta	*bitmap_byte_width2

	; Compute the erase pattern
	ldd	#0xffff
	jsr	,y
	coma
	comb
	std	erase_pattern

erase_large_row_loop:
	ldd	erase_pattern
	; First, deal with the left edge byte.  This looks a lot like
	; the loop8 case above.
	anda	,x

	; Now, deal with the remaining input bytes (at least one).
	; On entry to the middle loop, register B holds the overflow
	; bits from the previous iteration.
	; Because byte width is at least 1, the condition can be
	; checked after the first iteration.
erase_large_middle_loop:
	sta	,x+
	stb	*blit_overflow
	ldd	erase_pattern
	anda	*blit_overflow
	dec	*_bitmap_byte_width
	bne	erase_large_middle_loop

	; Finally, whatever is in overflow needs to be ANDed as well.
	andb	1,x
	std	,x+

	; Move on to the next row.
	; First, restore the width counter to its maximum.
	ldb	*bitmap_byte_width2
	stb	*_bitmap_byte_width
	; Reset the destination pointer to the left edge of the image.
	; The following calculation does B = 15-B, which is the amount
	; to be added to get back to the left edge.
	comb
	andb	#15
	abx
	dec	*_bitmap_height
	bne	erase_large_row_loop
	puls	y,pc


