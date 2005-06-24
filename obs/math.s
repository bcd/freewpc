

.area sysrom

#if 0
/*
 * Division by 10 table.
 *
 * To calculate N/10, find the Nth entry in this array.  All 256
 * bytes are covered.
 *
 * approximate the quotient as q = i*0.00011001101 (binary):
 *    q = ((i>>2) + i) >> 1; /* times 0.101 */
 *    q = ((q   ) + i) >> 1; /* times 0.1101 */
 *    q = ((q>>2) + i) >> 1; /* times 0.1001101 */
 *    q = ((q   ) + i) >> 4; /* times 0.00011001101 */
 * 
 * compute the reimainder as r = i - 10*q 
 *    r = ((q<<2) + q) << 1; /* times 1010. */
 *    r = i - r;
 */
#endif

proc(div10)
	requires(a)		// A = input 8-bit unsigned
	returns(a,b)	// A = quotient, B = remainder
	local(byte,i)
	endlocal

	sta	i
	lsra
	lsra				// a = i>>2
	adda	i			// a = (i>>2 + i)
	lsra				// q (first row)

	adda	i
	lsra				// q (second row)

	lsra
	lsra
	adda	i
	lsra				// q (third row)

	adda	i
	lsra
	lsra
	lsra
	lsra				// q = quotient (fourth row)

	tfr	a,b		// copy to b
	pshs	b	
	aslb
	aslb
	addb	,s+
	aslb
	negb
	addb	i
endp






