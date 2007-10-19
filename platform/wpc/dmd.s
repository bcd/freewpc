
DMD_PAGE_WIDTH=512

LOOP_COUNT=5

	.area ram
loop_count:
	.blkb 1

	.area .text

	;--------------------------------------------------------
	;
	; void dmd_zero (void *dst);
	;
	; X = pointer to display page
	;--------------------------------------------------------
	.globl _dmd_zero
_dmd_zero:
	clrb

	;--------------------------------------------------------
	;
	; void dmd_memset (void *dst, U8 fill);
	;
	; X = pointer to display page
	; B = character to fill
	;--------------------------------------------------------
	.globl _dmd_memset
_dmd_memset:
	pshs	y,u

	leau	DMD_PAGE_WIDTH,x

	lda	#LOOP_COUNT
	sta	loop_count

	tfr	b,a
	tfr	d,x
	tfr	x,y

	; The core loop consists of 17 pshu instructions,
	; each initializing 6 bytes for a total of 102
	; bytes per iteration.  Over 5 iterations, this
	; initialized 510 of the 512 DMD display bytes.
	; At the end one more pshu is needed to assign
	; the final 2 bytes.
1$:
	pshu	a,b,x,y
	pshu	a,b,x,y
	pshu	a,b,x,y
	pshu	a,b,x,y
	pshu	a,b,x,y
	pshu	a,b,x,y
	pshu	a,b,x,y
	pshu	a,b,x,y
	pshu	a,b,x,y
	pshu	a,b,x,y
	pshu	a,b,x,y
	pshu	a,b,x,y
	pshu	a,b,x,y
	pshu	a,b,x,y
	pshu	a,b,x,y
	pshu	a,b,x,y
	pshu	a,b,x,y
	dec	loop_count
	bne	1$
	pshu	x

	puls	y,u,pc


#if 0
	; Render a single row of pixels
	; X = destination buffer pointer
	; B = starting bit offset (0-7)
	; Y = source data pointer
	; A = length
	.globl blit_row
blit_row:

	ldu	#blit_row_lookup_table
	jmp	[b,u]

blit_row_lookup_table:
	.dw	blit_row_0
	.dw	blit_row_1
	.dw	blit_row_2
	.dw	blit_row_3
	.dw	blit_row_4
	.dw	blit_row_5
	.dw	blit_row_6
	.dw	blit_row_7

blit_row_0:
	ldu	,y++
	stu	,x++
	suba	#2
	bgt	blit_row_0
	bra	blit_row_done

blit_row_1:
	lda	,y
	rora
	rora
	anda	#0x80
	eora	,x

	ldb	,y
	lsrb
	eorb	1,y
	stb	*temp
	ldb	1,y
	rorb
	rorb
	andb	#0x80
	eorb	*temp
	std	,x

	ldb	1,y
	lsrb
	eorb	2,y
	stb	2,x

blit_row_2:
blit_row_3:
blit_row_4:
blit_row_5:
blit_row_6:
blit_row_7:


blit_row_done:
	rts
#endif
