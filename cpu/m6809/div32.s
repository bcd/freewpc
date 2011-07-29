/*
 * Copyright 2011 Brian Dominy <brian@oddchange.com>
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

/*
 * An implementation of 32-bit long division.  This is optimized for the
 * 6809.  From C code, use the following prototype:
 *
 * void divide32 (const U32 *dividend, U32 *divisor, U32 *quot_remainder);
 *
 * The quot_remainder arg is a pointer to two consecutive U32s; the first will
 * hold the quotient and the second the remainder.  These do not need to be
 * initialized prior to the call.
 *
 * This algorithm could be optimized in a few minor ways but it is kept
 * simple on purpose to minimize code space.  This is already far better than
 * what we were doing before.
 */

#define dividend 0,y
#define divisor 0,x
#define quotient 0,u
#define remainder 4,u
#define high(arg) arg
#define low(arg) 2+arg
#define shiftcount *m0
#define CARRY 1

	.module div32.s
	.area .text
	.globl _divide32
_divide32:
	pshs	u,y
	; Move parameters into the proper registers.
	; On entry, dividend is in X, and others are on the stack.
	tfr	x,y
	ldx	6,s
	ldu	8,s

	;--------------------------------------------------------------
	; Y = dividend (read-only, used only to initialize remainder)
	; X = divisor (read/write during call, but unchanged afterwards
	; U = quotient/remainder pair (write only)
	;--------------------------------------------------------------
divide_start:
	; Fail if divisor is zero.  Check low bits first, these are
	; mostly not to be zero.
	ldd	low(divisor)
	bne	notzero
	ldd	high(divisor)
	bne	notzero
divide_error:
	lbra	done
notzero:

	; Initialize the remainder to the dividend.
	; The dividend is no longer used.
	ldd	high(dividend)
	std	high(remainder)
	ldd	low(dividend)
	std	low(remainder)
#undef dividend

	; Prescale the divisor N.  Find the largest number Nx2^M that
	; fits in 32-bits.  (In other words, shift left until the most
	; significant bit is 1.)  Store M+1 as the maximum number of
	; subtractions.  Use a soft register to hold this.
	;
	; If during the loop we find that the divisor becomes larger than
	; the dividend/remainder, then we can stop, because no higher values
	; can be subtracted and the loop below will only end up shifting
	; useless zeroes into the result.  However, doing an exact comparison
	; adds extra cycles here.  Thus, we do an inexact comparison -- using
	; only the uppermost 8-bits -- which only adds two instructions.
	; This can optimize out up to 7 iterations of the main loop, which is
	; a definite win.
	;
	; Since register A is not used in the loop, we cache the MSB of the
	; divisor in A to save a few cycles on operations on that byte, and
	; write it out when we're done.
	ldb	#1
	lda	,x
	bmi	prescale_exit
prescale_loop:
	incb
	asl	3,x
	rol	2,x
	rol	1,x
	rola
	tsta
	bmi	prescale_exit
	cmpa	high(remainder)
	bls	prescale_loop
prescale_exit:
	sta	,x
	stb	shiftcount

	; Initialize the quotient to 0.
	ldd	#0
	std	high(quotient)
	std	low(quotient)

	; On each iteration, try to subtract the current divisor multiple from
	; the remainder. If we can, then shift a 1 into the result, else shift a 0.
	; To test, we need to know if divisor is less than or equal to remainder.
	; After all iterations are done, divisor will hold its original value
	; since we have shifted it back (undoing the above loop).
main_loop:
	ldd	high(divisor)
	cmpd	high(remainder)
	bhi	shift0            ; Divisor is too large, shift a 0.
	blo	shift1            ; Divisor is less, shift a 1.
	                        ; Upper 16-bits not conclusive, check lower 16.
	ldd	low(divisor)
	cmpd	low(remainder)
	bhi	shift0            ; Divisor is too large, shift a 0.
	                        ; Divisor is <= remainder, so subtract ok.
shift1:
	ldd	low(remainder)
	subd	low(divisor)
	std	low(remainder)
	ldd	high(remainder)
	sbcb	1,x
	sbca	,x
	std	high(remainder)
	orcc	#CARRY
	rol	3,u               ; Shift a 1 into the LSB
	bra	shift
shift0:
	asl	3,u               ; Shift a 0 into the LSB
shift:                     ; Shift remaining bits of quotient
	rol	2,u
	rol	1,u
	rol	,u
	dec	shiftcount
	beq	done
	; Note: take care that the loop must run "shiftcount" times, but we must
	; only shift our divisor one less than that.
	lsr	,x                ; Shift to the next lower divisor value
	ror	1,x
	ror	2,x
	ror	3,x
	bra	main_loop
done:
	puls	u,y,pc

