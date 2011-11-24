/*
 * Copyright 2009, 2010, 2011 Brian Dominy <brian@oddchange.com>
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

	.module	bcd_string

	.area .text

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;
	; void bcd_string_add (bcd_t *dst, const bcd_t *src, U8 len);
	;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	.globl _bcd_string_add
_bcd_string_add:
	pshs	u
	ldu	4,s
	decb
	andcc	#~0x01
1$:
	lda	b,u
	adca	b,x
	daa
	sta	b,x
	decb
	bge	1$
	puls	u,pc


	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;
	; void bcd_string_increment (bcd_t *s, U8 len);
	;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	.globl _bcd_string_increment
_bcd_string_increment:
	decb
	andcc	#~0x01
1$:
	lda	b,x
	inca
	daa
	sta	b,x
	bcc	2$
	decb
	bge	1$
2$:
	rts


	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;
	; void bcd_string_sub (bcd_t *dst, const bcd_t *src, U8 len);
	;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	.globl _bcd_string_sub
_bcd_string_sub:
	pshs	u
	ldu	4,s
	decb
	stb	*m0

	; Step 1 - compute the ten's complement of the subtrahend.
	; Take the nine's complement, and then add one.
	; This becomes U, replacing the second argument.
1$:
	lda	#0x99
	suba	b,u
	sta	,-s
	decb
	bge	1$

	; Step 2 - add the inverse, just like bcd_string_add
	ldb	*m0
	inc	b,x
	andcc	#~0x01
2$:
	lda	b,s
	adca	b,x
	daa
	sta	b,x
	decb
	bge	2$

	ldb	*m0
	incb
	leas	b,s
	puls	u,pc


/* The multiplication functions below are not used now */
#if 0

bcd_temp1:
	.blkb	6
bcd_temp2:
	.blkb	6


	; void bcd_string_mul (bcd_t *dst, U8 factor, U8 len);
	.globl	_bcd_string_mul
_bcd_string_mul:
#define arg_factor 2,s
#define arg_len 3,s
	ldu	#bcd_temp1

	; Copy the value into temp
	ldb	arg_len
1$:
	lda	,x
	sta	,u
	decb
	bne	1$

	dec	arg_len

	; FACTOR-1 times, add the temp value back to the result.
2$:
	dec	arg_factor
	bne	3$
	; x = x + u
	ldb	arg_len
	andcc	#~0x01

3$:
	lda	b,x
	adca	b,u
	daa
	sta	b,x
	decb
	bge	3$
	bra	2$

4$:
	rts


	.globl _bcd_string_mul10
_bcd_string_mul10:
#define lower_half *m0
#define loop_counter *m1

	; SAVE_REG(x, y, d)
	pshs	y
	decb
	leax	b,x
	leay	b,y
	clr	lower_half
	stb	loop_counter

1$:
	clra
	ldb	,x

	; Shift left 4 times.  B now contains the correct bits in the
	; upper half.  A contains the lower half for the next byte.
	aslb
	rola
	aslb
	rola
	aslb
	rola
	aslb
	rola
	orb	lower_half
	sta	lower_half

	stb	,x

	; TODO - dec destroys the overflow flag!
	dec	loop_counter
	bne	1$

	; RESTORE_REG(x, y, d, pc)
	puls	y,pc

#endif
