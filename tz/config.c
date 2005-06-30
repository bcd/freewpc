
#include <freewpc.h>


/** An array in which each 1 bit represents an opto switch
 * and 0 represents a normal switch.  This is used for
 * determining the logical level of a switch (open/closed)
 * rather than its actual level.
 */
const uint8_t mach_opto_mask[] = {
	0x00, 0x70, 0x28, 0x0, 0x0, 0x10, 0x0, 0x3E, 0x5D,
};


