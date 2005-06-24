
#include <wpc.h>

/*
 * Division by 10 table.
 *
 * To calculate N/10, find the Nth entry in this array.  All 256
 * bytes are covered.
 *
 * approximate the quotient as q = i*0.00011001101 (binary):
 *    q = ((i>>2) + i) >> 1; times 0.101
 *    q = ((q   ) + i) >> 1; times 0.1101
 *    q = ((q>>2) + i) >> 1; times 0.1001101
 *    q = ((q   ) + i) >> 4; times 0.00011001101
 * 
 * compute the reimainder as r = i - 10*q 
 *    r = ((q<<2) + q) << 1; times 1010.
 *    r = i - r;
 */


uint16_t div10 (uint8_t v)
{
	register uint16_t res asm ("d");
	uint8_t i;

	__asm__ volatile (
	   "sta	%0"
		"lsra"
		"lsra		; a = i>>2"
		"adda	%0	; a = (i>>2 + i)"
		"lsra		; q (first row)"
		"adda	%0					"
		"lsra		; q (second row)"
		"lsra"
		"lsra"
		"adda	%0"
		"lsra"
		"adda	%0"
		"lsra"
		"lsra"
		"lsra"
		"lsra		; q = quotient (fourth row)"
		"tfr	a,b	; copy to b"
		"pshs	b"
		"aslb"
		"aslb"
		"addb	,s+"
		"aslb"
		"negb"
		"addb	%0" 
	:
	: "m" (i)
	: "d"
	);
	return (res);
}

