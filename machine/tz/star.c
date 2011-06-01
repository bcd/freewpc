/*
 * Copyright 2006-2011 by Brian Dominy <brian@oddchange.com>
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

/* CALLSET_SECTION (star, __machine2__) */
#include <freewpc.h>

#define MAX_STARS 12

#define MAX_STATE 4

bool draw_bouncing_overlay;

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
				s->y = 2 + random_scaled (24);
				s->state = 0;
			}
		}
	}
}

CALLSET_ENTRY (star, idle_every_ten_seconds)
{
	check_bitmap_overlay ();
}

CALLSET_ENTRY (star, score_deff_start)
{
	U8 n;
	for (n=0; n < MAX_STARS; n++)
		star_states[n].time = 0;
	check_bitmap_overlay ();
}

CALLSET_ENTRY (star, score_overlay)
{
	/* Don't draw any stars if paused */
	if (task_find_gid (GID_MUTE_AND_PAUSE))
		return;
	star_draw ();
	if (draw_bouncing_overlay)
		stardrop_overlay_draw ();
}
