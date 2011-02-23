
#include <freewpc.h>

/** Find the highest numbered bit set in a bitmask.
 * Returns -1 if no bits are set. */
int scanbit (U8 bits)
{
	if (bits & 0x80) return 7;
	else if (bits & 0x40) return 6;
	else if (bits & 0x20) return 5;
	else if (bits & 0x10) return 4;
	else if (bits & 0x08) return 3;
	else if (bits & 0x04) return 2;
	else if (bits & 0x02) return 1;
	else if (bits & 0x01) return 0;
	else return -1;
}

