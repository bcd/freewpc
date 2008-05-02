/*
 * Copyright 2006, 2007 by Brian Dominy <brian@oddchange.com>
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


#define MAX_STARS 8

#define MAX_STATE 4

struct star_state
{
	U8 time;
	U8 state;
	U8 x;
	U8 y;
} star_states[MAX_STARS];

static const U8 star_bitmaps[] = {
	3, 3, 0, 0, 0,
	3, 3, 0, 2, 0,
	3, 3, 0, 2, 0,
	3, 3, 2, 5, 2,
	3, 3, 2, 7, 2,
	3, 3, 2, 7, 2,
};

void star_draw (void)
{
	U8 n;
	for (n=0; n < MAX_STARS; n++)
	{
		struct star_state *s = &star_states[n];
		if (s->time)
		{
			//bitmap_erase_asm (...);
			bitmap_blit2 (star_bitmaps + s->state * 5, s->x, s->y);

			s->time--;

			if (random () < 128)
				;
			if (s->state == MAX_STATE)
				s->state--;
			else if (s->state == 0)
				s->state++;
			else if (random () < 192)
				s->state++;
			else
				s->state--;
		}
		else
		{
			if (random () < 64)
			{
				s->time = 4 + random_scaled (8);
				s->x = 4 + random_scaled (120);
				s->y = 2 + random_scaled (28);
				s->state = 0;
			}
		}
	}
}


void star_reset (void)
{
	U8 n;
	for (n=0; n < 8; n++)
		star_states[n].time = 0;
}


void star_task (void)
{
	U8 *dmd = (U8 *)task_get_arg ();
	U8 i;

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
	dmd[-DMD_BYTE_WIDTH + DMD_PAGE_SIZE] = 0;
	dmd[-DMD_BYTE_WIDTH] = 0;
	dmd[0+DMD_PAGE_SIZE] = 0;
	dmd[0] = 0;
	dmd[+DMD_BYTE_WIDTH] = 0;
	dmd[+DMD_BYTE_WIDTH+DMD_PAGE_SIZE] = 0;
	task_exit ();
}


#define star_create(x,y) \
do \
{ \
	task_pid_t tp = task_create_peer (star_task); \
	task_set_arg (tp, (PTR_OR_U16)(dmd_low_buffer + ((U16)y << 4) + x)); \
} while (0)

void starfield_task (void)
{
	do
	{
		star_create (1, 3);
		task_sleep (TIME_100MS * 3);
		star_create (14, 28);
		task_sleep (TIME_100MS * 3);
		star_create (14, 3);
		task_sleep (TIME_100MS * 3);
		star_create (1, 28);
		task_sleep (TIME_1S);
	}
	while (deff_get_active () == DEFF_AMODE);
	task_exit ();
}

void starfield_start (void)
{
	task_create_gid1 (GID_STARFIELD, starfield_task);
}

void starfield_stop (void)
{
	task_kill_gid (GID_STARFIELD);
}


