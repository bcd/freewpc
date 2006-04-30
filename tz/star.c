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


void new_star_task (void)
{
	U16 offset = ((U16)random () & 0x3F) * 4;
	U8 rand8;
	U8 mask; 

	do {
		rand8 = random () & 0x07;
		mask = 1 << rand8;
		rand8++;
		mask |= 1 << rand8;
	} while (FALSE); // ((dmd_low_buffer[offset] & mask) == 0);

	for (;;)
	{
		task_sleep_sec (random () & 0x03);
		dmd_low_buffer[offset] |= mask;
		dmd_low_buffer[offset+16] |= mask;

		task_sleep_sec (random () & 0x03);
		dmd_low_buffer[offset] &= ~mask;
		dmd_low_buffer[offset+16] &= ~mask;
	}
}


void new_starfield_start (void)
{
	int i;
	for (i=0; i < 16; i++)
		task_create_gid (GID_NEW_STAR, new_star_task);
}


void new_starfield_stop (void)
{
	task_kill_gid (GID_NEW_STAR);
}


void star_task (void)
{
	U8 *dmd = (U8 *)task_get_arg ();
	int i;

	for (i=3; i>0; i--)
	{
		if (deff_get_active () != DEFF_AMODE)
			break;

		dmd[-DMD_BYTE_WIDTH] = 0x10;
		dmd[-DMD_BYTE_WIDTH + DMD_PAGE_SIZE] = 0x10;
		task_sleep (TIME_33MS);

		dmd[0] = 0x3C;
		dmd[DMD_PAGE_SIZE] = 0x3C;
		task_sleep (TIME_66MS);

		dmd[+DMD_BYTE_WIDTH] = 0x8;
		dmd[+DMD_BYTE_WIDTH+DMD_PAGE_SIZE] = 0x8;
		task_sleep (TIME_33MS);

		dmd[-DMD_BYTE_WIDTH] = 0x8;
		dmd[-DMD_BYTE_WIDTH+DMD_PAGE_SIZE] = 0x8;
		task_sleep (TIME_66MS);

		dmd[0] = 0x18;
		dmd[0+DMD_PAGE_SIZE] = 0x18;
		task_sleep (TIME_33MS);

		dmd[+DMD_BYTE_WIDTH] = 0x10;
		dmd[+DMD_BYTE_WIDTH+DMD_PAGE_SIZE] = 0x10;
		task_sleep (TIME_100MS);
	}
	dmd[-DMD_BYTE_WIDTH] = 0;
	dmd[0] = 0;
	dmd[+DMD_BYTE_WIDTH] = 0;
	task_exit ();
}


#define star_create(x,y) \
do \
{ \
	task_t *tp = task_create_child (star_task); \
	task_set_arg (tp, (U16)(dmd_low_buffer + ((U16)y << 4) + x)); \
} while (0)

void starfield_task (void)
{
	while (deff_get_active () == DEFF_AMODE)
	{
		star_create (1, 3);
		task_sleep_sec (1);
		star_create (14, 28);
		task_sleep_sec (1);
		star_create (14, 3);
		task_sleep_sec (1);
		star_create (1, 28);
		task_sleep_sec (2);
	}
}

void starfield_start (void)
{
	task_create_gid1 (GID_STARFIELD, starfield_task);
}

void starfield_stop (void)
{
	task_kill_gid (GID_STARFIELD);
}


