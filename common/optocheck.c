
#include <freewpc.h>


/** Verifies that optos are working correctly at init time.
 * If no power is available, it will appear that all optos are
 * closed.  This is an error condition that should disable
 * playfield switch scanning.  This function does not return
 * until the condition is resolved.
 */
void opto_check (void)
{
	U8 col;
	U8 row;
	U8 bits;

	for (col = 0; col < SWITCH_BITS_SIZE; col++)
	{
		if ((bits = mach_opto_mask[col]) != 0)
		{
			for (row = 0; bits && row < 8; row++, bits >>= 1)
			{
				if (bits & 1)
				{
					if (switch_poll (col * 8 + row))
					{
						/* The switch is closed (inactive); opto power must
						 * be good */
						return;
					}
				}
			}
		}
	}

	/* All optos are open (active).  Either opto power is bad,
	 * or there are truly balls at all opto positions.
	 */
}

