
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
					dbprintf ("Checking SW %d\n", col * 8 + row);
					if (!switch_poll (col * 8 + row))
					{
						/* The switch is open (active); opto power must
						 * be good */
						dbprintf ("Switch is open\n");
						return;
					}
				}
			}
		}
	}

	/* All optos are closed (inactive).  Either opto power is bad,
	 * or there are truly balls at all opto positions, which is
	 * highly unlikely.
	 */
	dbprintf ("Bad 12V?\n");
}

