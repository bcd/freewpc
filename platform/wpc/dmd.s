
DMD_PAGE_WIDTH=512

LOOP_COUNT=5

	; Keep loop_count in a fast direct memory slot.
#define loop_count *m0

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
	; (102 bytes in 17x12=204 cycles, means this takes
	; about 0.5ms to execute.)
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

	.globl _dmd_copy_asm
_dmd_copy_asm:
	; X = destination
	; stack ptr = source
	pshs	u,y

	; Convert so that U points to the end of the
	; destination buffer, and X is the end of the
	; source buffer.
	leau	DMD_PAGE_WIDTH,x
	ldx	6,s
	leax	DMD_PAGE_WIDTH,x

	; In each iteration, we will copy 16 bytes.
	lda	#(DMD_PAGE_WIDTH / 16)
	sta	loop_count

1$:
	; Read 4 bytes into registers, then write them
	; out at once using a push instruction.
	; Do this 4 times to transfer a total of 16 bytes.
	; Notice the copy is done from end to start of
	; buffer since pshu decrements its address.
	; TBD: is it better to use offsets here, or should
	; we use autodecrement mode with no offset?
	ldy	,x
	ldd	-2,x
	pshu	d,y ; TODO - check order

	ldy	-4,x
	ldd	-6,x
	pshu	d,y ; TODO - check order

	ldy	-8,x
	ldd	-10,x
	pshu	d,y ; TODO - check order

	ldy	-12,x
	ldd	-14,x
	pshu	d,y ; TODO - check order

	leax	-16,x
	dec	loop_count
	bne	1$

	puls	u,y,pc

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
