
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
#include <rtsol.h>

#define MAX_ACTIVE_FLASHERS 4


typedef struct {
	solnum_t flasher;
	U8 duration;
} flasher_pulse_t;


__fastram__ flasher_pulse_t flashers_active[MAX_ACTIVE_FLASHERS];


void flasher_pulse (solnum_t n)
{
	U8 i;
	for (i=0; i < MAX_ACTIVE_FLASHERS; i++)
		if (flashers_active[i].flasher == 0)
		{
			flashers_active[i].flasher = n;
			flashers_active[i].duration = TIME_66MS;
			return;
		}
}


void flasher_rtt (void)
{
	U8 i;
	for (i=0; i < MAX_ACTIVE_FLASHERS; i++)
	{
		if (flashers_active[i].flasher != 0)
		{
			rt_sol_enable (flashers_active[i].flasher);
			if (--flashers_active[i].duration == 0)
			{
				rt_sol_disable (flashers_active[i].flasher);
				flashers_active[i].flasher = 0;
			}
		}
	}
}


void flasher_init (void)
{
	U8 i;
	for (i=0; i < MAX_ACTIVE_FLASHERS; i++)
	{
		flashers_active[i].flasher = 0;
	}
}


