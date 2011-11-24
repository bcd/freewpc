/*
 * Copyright 2008-2011 by Brian Dominy <brian@oddchange.com>
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

#include <freewpc.h>
#include <simulation.h>


U8 hwtimer_value;

void hwtimer_periodic (void *data __attribute__((unused)))
{
	if (hwtimer_value & 0x80)
	{
	}
	else if (hwtimer_value > 0)
	{
		--hwtimer_value;
		if (hwtimer_value == 0)
		{
			hwtimer_value = 0x80;
		}
		else
		{
			sim_time_register (1, FALSE, hwtimer_periodic, NULL);
		}
	}
}


U8 hwtimer_read (void)
{
	return hwtimer_value;
}

void hwtimer_write (U8 val)
{
	hwtimer_value = val;
	if (val != 0)
	{
		simlog (SLC_DEBUG, "Hardware timer started.");
		sim_time_register (1, FALSE, hwtimer_periodic, NULL);
	}
}

void hwtimer_init (void)
{
	hwtimer_write (0);
}

