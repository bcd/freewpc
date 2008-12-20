;;; Copyright 2007, 2008 by Brian Dominy <brian@oddchange.com>
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

DMD_PAGE_WIDTH=512

	; Keep loop_count in a fast direct memory slot.
#define loop_count *m0

	.area .text

	;--------------------------------------------------------
	;
	; void dmd_clean_page (void *dst);
	;
	; X = pointer to display page
	;--------------------------------------------------------
	.globl _dmd_clean_page
_dmd_clean_page:
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

	lda	#5
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
	; TODO - when doing zero operation, we can push
	; DP=0 to get one extra byte moved per instruction.
	; If interrupts are disabled, we can set S=0 before
	; this loop and get 2 more bytes out of it.  There
	; would be a penalty for the setup/restore though.
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

   ; Get the source pointer off the stack
	ldu	6,s

	; In each iteration, we will copy 16 bytes.
	lda	#(DMD_PAGE_WIDTH / 16)
	sta	loop_count

1$:
	; Read 4 bytes at a time using a fast pull instruction.
	; Then write them out using 2 word stores.
	; Do all of this 4 times to transfer a total of 16 bytes.
	; U moves automatically throughout; X is adjusted
	; forward once at the end of each block.
	; TODO - verify order of stores is correct
	pulu	d,y
	std	,x
	sty	2,x
	pulu	d,y
	std	4,x
	sty	6,x
	pulu	d,y
	std	8,x
	sty	10,x
	pulu	d,y
	std	12,x
	sty	14,x
	leax	16,x
	dec	loop_count
	bne	1$
	puls	u,y,pc

