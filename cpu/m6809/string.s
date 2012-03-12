/*
 * Copyright 2010-2011 Brian Dominy <brian@oddchange.com>
 *
 * This file is part of FreeWPC.
 *
 * FreeWPC is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * FreeWPC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FreeWPC; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

	.module asm_string

/*
 * This file contains optimized versions of memset() and memcpy()
 * for the 6809 processor.  Each starts in a "large" mode in which
 * stores are done in bulk many bytes at a time to eliminate loop
 * overhead.  Then it switches to "small" mode to store any remaining
 * bytes one at a time.  The amount of unrolling was chosen to deal
 * with common sizes and to avoid taking excessive space.
 *
 * Keep in mind that these functions are never used for very small
 * block sizes; GCC will always emit inline code in those cases.
 * So they do not have to be very efficient in those cases because
 * it will never happen.
 *
 * Note that size_t is always a 16-bit, even when -mint8 is used.
 */

	.area .text

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;
	; void memset (void *buf, int c, size_t len);
	;
	; On entry, register X contains the buf pointer.
	; c is in register B, and len is pushed onto the stack.
	; (Note: this assumes 8-byte integers -- look at the __int8__
	; code.  Outside of FreeWPC, integers are normally 16-bits
	; and the other case is used.)
	;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	.globl _memset
_memset:
	pshs	u

	; Load the character to be set everywhere in B, and the
	; length in U.
#ifdef __int8__
	ldu   4,s    ; number of bytes
#else
	ldb   5,s    ; character to set
	ldu   6,s    ; number of bytes
#endif

	; The copy is done in two phases: first, we copy 16 bytes at
	; a time as much as possible, then we copy any remaining bytes.

	cmpu  #16
	blt   __memset_small

	; When copying 16 bytes, we use a 16-bit store operation.
	; Copy the character into the upperhalf of D, so that each store
	; sets 2 bytes.
	tfr   b,a    ; ASCII-only

__memset_large:
   std   ,x++
   std   ,x++
   std   ,x++
   std   ,x++
   std   ,x++
   std   ,x++
   std   ,x++
   std   ,x++
	leau  -16,u
	cmpu  #16
	bge   __memset_large

	; Copy the remaining bytes one at a time.  Save the length into
	; *m0, so that decrement will implicitly set the condition codes.
	; It must be less than 256, so the decrementing can ignore the
	; upper 8-bits.

__memset_small:
	stu	*m0
	tst	*m1
	beq	__memset_done
1$:
	stb	,x+
	dec	*m1
	bne   1$
__memset_done:
	puls	u,pc


	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;
	; void memcpy (void *dst, const void *src, size_t len);
	;
	; On entry, dst is in register X, and src and len are on the
	; stack.
	;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	.globl _memcpy
_memcpy:
	pshs	u

	; Load src and len into registers.
	ldu   4,s    ; source pointer
	ldd   6,s    ; number of bytes

	; If less than 16 bytes, then skip the large block copy completely.
	cmpd	#16
	blt	__memcpy_small

	; Save Y register for block loop
	sty	*m2

	; Calculate the number of 16 byte blocks to copy.
	lsrb
	lsrb
	lsrb
	lsrb
	stb   *m0
	tsta
	beq	__memcpy_large
	lsra
	lsra
	lsra
	lsra
	adda	*m0
	sta	*m0

	; Copy the 16 byte blocks first.
	; Put this into m0 so decrement sets the condition codes.
	; Note: we cheat here a little!  m0 is only 8-bits, so we can only
	; copy 256*16=4096 bytes this way.  On FreeWPC this is acceptable as
	; we never copy more than that.
	; Also save/restore Y register which is needed in this loop.

__memcpy_large:
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
	dec	*m0
	bne   __memcpy_large
	ldy	*m2

	; Copy the remaining bytes one at a time.  Refetch the count mod 16
	; from the stack.

__memcpy_small:
	ldb	7,s
	andb	#15
	beq	__memcpy_done
1$:
	lda	,u+
	sta	,x+
	decb
	bne   1$
__memcpy_done:
	puls	u,pc


	.globl _strlen
_strlen:
	stx	*m0
__strlen_loop:
	tst	,x+
	bne	__strlen_loop
	tfr	x,d
	subd	*m0
	tfr	d,x
	rts

	.globl _strcpy
_strcpy:
	pshs	u
	ldu	4,s
	stx	*m0
1$:
	ldd	,u++
	sta	,x+
	tsta
	beq	2$
	stb	,x+
	tstb
	bne	1$
2$:
	ldx	*m0
	puls	u,pc
