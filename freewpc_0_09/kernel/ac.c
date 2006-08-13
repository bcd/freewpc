
/*
 * Copyright 2006 by Brian Dominy <brian@oddchange.com>
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

/**
 * \file
 * \brief AC/zerocross circuit handling
 *
 * This module is not yet complete and is not used.
 */

U8 ac_last_zerocross_reading;
U8 ac_zerocross_same_count;
bool ac_zerocross_broken;

void ac_rtt (void)
{
#if 0
	U8 ac_current_zerocross_reading = wpc_read_ac_zerocross ();

	if (ac_current_zerocross_reading == ac_last_zerocross_reading)
	{
		ac_zerocross_same_count++;
		if (ac_zerocross_same_count >= 32)
		{
			ac_zerocross_broken = TRUE;
		}
	}
	else
	{
		ac_zerocross_broken = FALSE;
	}

	ac_last_zerocross_reading = ac_current_zerocross_reading;
#endif
}


void ac_idle_task (void)
{
	/** If zerocross is working, we are in a real machine.
	 * Pinmame doesn't support this yet. */
	if (!ac_zerocross_broken)
	{
		fatal (ERR_REAL_HARDWARE);
	}
}


void ac_init (void)
{
	ac_last_zerocross_reading = 0;
	ac_zerocross_same_count = 0;
	ac_zerocross_broken = TRUE;
}

