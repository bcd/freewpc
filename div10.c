
#include <freewpc.h>

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
	uint8_t i = v;

	__asm__ volatile (
	   "\ttfr b,a\n"
	   "\tsta	%0\n"
		"\tlsra\n"
		"\tlsra		; a = i>>2\n"
		"\tadda	%0	; a = (i>>2 + i)\n"
		"\tlsra		; q (first row)\n"
		"\tadda	%0					\n"
		"\tlsra		; q (second row)\n"
		"\tlsra\n"
		"\tlsra\n"
		"\tadda	%0\n"
		"\tlsra\n"
		"\tadda	%0\n"
		"\tlsra\n"
		"\tlsra\n"
		"\tlsra\n"
		"\tlsra		; q = quotient (fourth row)\n"
		"\ttfr	a,b	; copy to b\n"
		"\tpshs	b\n"
		"\taslb\n"
		"\taslb\n"
		"\taddb	,s+\n"
		"\taslb\n"
		"\tnegb\n"
		"\taddb	%0\n"
	:
	: "m" (i)
	: "d"
	);
	return (res);
}

