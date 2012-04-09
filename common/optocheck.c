/*
 * Copyright 2008-2010 by Brian Dominy <brian@oddchange.com>
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

/**
 * \file
 * \brief Check that power to the optos is working correctly.
 */

#include <freewpc.h>


/** Verifies that optos are working correctly at init time.
 * If no power is available, it will appear that all optos are
 * closed.  This is an error condition that should disable
 * playfield switch scanning.  This function does not return
 * until the condition is resolved.
 */
void opto_check (void)
{
#ifdef CONFIG_PLATFORM_WPC
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
					if (rt_switch_poll (col * 8 + row))
					{
						/* The switch is level is high (inactive); opto power must
						 * be good */
						return;
					}
				}
			}
		}
	}

	/* All optos have low level (active).  Either opto power is bad,
	 * or there are truly balls at all opto positions, which is
	 * highly unlikely.
	 */
	dbprintf ("Bad opto power?\n");
	task_sleep (TIME_33MS);
#endif /* CONFIG_PLATFORM_WPC */
}

