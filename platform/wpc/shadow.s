
DMD_LOW_BUFFER = 0x3800
DMD_HIGH_BUFFER = 0x3A00

#define loop_count *m0
#define row_count *m1
#define words_per_row *m2
#define temp *m2

	.area		.text
	.globl	_dmd_shadow
_dmd_shadow:
	pshs	u

	; Shadow the top row of the display, by a direct copy.
	ldx	#DMD_LOW_BUFFER
	ldu	#DMD_HIGH_BUFFER
	lda	#8
	sta	words_per_row
shadow_copy_top_row:
	ldd	,x++
	stu	,u++
	dec	words_per_row
	bne	shadow_copy_top_row

	; Shadow the middle 30 rows of the display.  For each of
	; these rows, we output the same 16-bit into place, just
	; above it, and just below it.  Since we go from bottom
	; to top, the "just below" case can be set directly, but
	; the other two need to take into account previous iterations,
	; and must use OR instructions.
	; It is relatively fast because we only use 16-bit loads/stores.
	; This gets the shadowing right in the vertical dimension.
	; X and U are already correct set from above.
	lda	#30
	sta	row_count
shadow_copy_rows:
	lda	#8
	sta	words_per_row
shadow_copy_row:
	ldd	,x
	std	16,u
	ora	-16,u
	orb	-15,u
	std	-16,u
	ldd	,x++
	ora	,u
	orb	,u
	std	,u++
	dec	words_per_row
	bne	shadow_copy_row
	dec	row_count
	bne	shadow_copy_rows

	; Shadow copy the bottom row.
	lda	#8
	sta	words_per_row
shadow_copy_bottom_row:
	ldd	,x++
	stu	,u++
	dec	words_per_row
	bne	shadow_copy_bottom_row

	; Shadow copy the columns.
	; In the first iteration, we get it right except for the
	; boundary between adjacent 16-bit values.
	; Note also that the source image is not used at all here;
	; we are completely operating on the final image.  We can
	; use X as it already advanced from low to high from the
	; previous instructions.
	; There are 256 words to be manipulated.  loop_count rolls
	; around completely.
	clr	loop_count
shadow_copy_cols1:
	ldd	,x
	lslb
	rola
	ora	,x
	orb	1,x
	std	temp
	ldd	,x
	lsra
	rorb
	ora	temp
	orb	temp+1
	std	,x++
	dec	loop_count
	bne	shadow_copy_cols1

	; A second pass is needed to fix up the boundary cases.
	; There are only 15 per row, and we don't wraparound.
	ldu	#DMD_HIGH_BUFFER + 1
	lda	#32
	sta	row_count
shadow_copy_cols2_rows:
	lda	#15
	sta	words_per_row
shadow_copy_cols2_row:
	ldd	,u
	; All that need happen is that if either the LSB of A or the
	; MSB of B is a 1, then both need to be a 1.
	bita	#0x01
	bne	modify
	bitb	#0x80
	beq	no_modify

modify:
	ora	#0x01
	orb	#0x80
	std	,u++
	dec	words_per_row
	bne	shadow_copy_cols2_row
	bra	next_row

no_modify:
	leau	2,u
	dec	words_per_row
	bne	shadow_copy_cols2_row

next_row:
	leau	2,u
	dec	row_count
	bne	shadow_copy_cols2_rows

	puls	u,pc
