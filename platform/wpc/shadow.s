;;; Copyright 2008-2010 by Brian Dominy <brian@oddchange.com>
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

DMD_LOW_BUFFER = 0x3800
DMD_HIGH_BUFFER = 0x3A00

#define temp *m0
#define row_count *m2
#define words_per_row *m3

	.area		.text
	.globl	_dmd_shadow
_dmd_shadow:
	pshs	u

	; Clear the first, second, and last rows of the destination
	; buffer.  The top and bottom rows do not contain any source
	; data, so the text should not ride up against the edge of
	; the display.  The rows must be cleared since the page isn't
	; guaranteed to be clean.
	ldu	#DMD_HIGH_BUFFER
	lda	#8
	sta	words_per_row
	ldd	#0
shadow_copy_clear:
	std	0x10,u
	std	0x1F0,u
	std	,u++
	dec	words_per_row
	bne	shadow_copy_clear

	ldx	#DMD_LOW_BUFFER + 16

	; Shadow the middle 30 rows of the display.  For each of
	; these rows, we output the same 16-bit into place, just
	; above it, and just below it.  Since we go from top to
	; bottom, the "just below" case can be set directly, but
	; the other two need to take into account previous iterations,
	; and must use OR instructions.
	; It is relatively fast because we only use 16-bit loads/stores.
	; This gets the shadowing right in the vertical dimension.
	; X and U are already correct set from above.
	;
	; In the first iteration, we get columns right except for the
	; boundary between adjacent 16-bit values.
	; Note also that the source image is not used at all here;
	; we are completely operating on the final image.  We can
	; use X as it already advanced from low to high from the
	; previous instructions.

	lda	#30
	sta	row_count
shadow_copy_rows:
	lda	#8
	sta	words_per_row
shadow_copy_row:
	ldd	,x               ; Read 16 bits from source
	std	16,u             ; Copy to the row below
	ora	-16,u            ; OR them into the row above
	orb	-15,u            ; ""
	std	-16,u            ; ""
	ldd	,x               ; Reload source data
	lsla                   ; Shift all bits to the right
	rolb                   ; ""
	ora	,x               ; OR with the original source
	orb	1,x              ; ""
	std	temp             ; Save this
	ldd	,x++             ; Reload source data one more time
	lsrb                   ; Now shift all bits to the left
	rora                   ; ""
	ora	temp             ; Combine with original+right shifted
	orb	temp+1           ; ""
	ora	,u               ; OR this into the destination
	orb	1,u              ; ""
	std	,u++             ; ""
	dec	words_per_row
	bne	shadow_copy_row
	dec	row_count
	bne	shadow_copy_rows

	inc	_task_dispatching_ok

	; A second pass is needed to fix up the boundary cases.
	; There are only 7 per row, and we don't wraparound.
	; TODO - this is not quite right.  If we match on a pixel
	; that was added above by shadowing, then we get a double
	; sized shadow.  This code really should be integrated into
	; the above loop.
	ldx	#DMD_LOW_BUFFER + 17
	lda	#30
	sta	row_count
shadow_copy_cols2_rows:
	lda	#7
	sta	words_per_row
shadow_copy_cols2_row:
	ldd	,x++         ; Load 16-bits from the source
	; All that need happen is that if either the LSB of A or the
	; MSB of B is a 1, then both need to be a 1.
	bita	#0x80
	bne	modify
	bitb	#0x01
	beq	no_modify

modify:
	ldd	0x1FE,x
	ora	#0x80
	orb	#0x01
	std	0x1FE,x
	dec	words_per_row
	bne	shadow_copy_cols2_row
	bra	next_row

no_modify:
	dec	words_per_row
	bne	shadow_copy_cols2_row

next_row:
	leax	2,x
	dec	row_count
	bne	shadow_copy_cols2_rows

	puls	u,pc
