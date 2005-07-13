
#include <freewpc.h>

uint8_t triac_bits;


void triac_rtt (void)
{
}


void triac_on (triacbits_t bits)
{
	triac_bits |= bits;
}

void triac_off (triacbits_t bits)
{
	triac_bits &= ~bits;
}


void triac_init (void)
{
	triac_bits = 0;
}

