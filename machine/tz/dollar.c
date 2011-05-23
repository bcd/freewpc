/*
 * Copyright 2011 by Ewan Meadows <sonny_jim@hotmail.com>
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

/* CALLSET_SECTION (dollar, __machine4__) */
#include <freewpc.h>
#define MAX_DOLLARS 8
#define DOLLAR_HEIGHT 14
#define DOLLAR_WIDTH 11
#define BITMAP_LENGTH 16

const U8 dollar_small_bitmap_2plane[] = {
	6,14,16,0,60,2,17,1,62,16,16,0,63,0,16,0, //Left high
	6,14,0,16,2,61,2,18,1,0,0,16,0,63,0,16, //Left low

	5,14,1,0,31,0,1,0,15,17,17,16,15,0,1,0, //Right high
	5,14,0,1,0,31,0,1,0,14,8,9,0,15,0,1, //Right low
};

struct dollar_state {
	U8 x;
	U8 y;
	U8 x_speed;
	U8 y_speed;
	bool alive;
	bool left;
} dollar_states[MAX_DOLLARS];

static void init_dollar (U8 dollar_number)
{
	struct dollar_state *d = &dollar_states[dollar_number];
	d->x = 64 - (DOLLAR_WIDTH / 2);
	d->y = 31 - DOLLAR_HEIGHT;
	d->x_speed = random_scaled (3) + 2;
	d->y_speed = random_scaled (2) + 1;
	d->alive = FALSE;
	d->left = random_scaled (2);
}

void init_all_dollars (void)
{
	U8 i;
	for (i = 0; i < MAX_DOLLARS;i++)
	{
		init_dollar (i);
	}
}

CALLSET_ENTRY (dollar, respawn_dollar)
{
	U8 i;
	for (i = 0; i < MAX_DOLLARS;i++)
	{
		struct dollar_state *d = &dollar_states[i];
		if (d->alive == FALSE)
		{
			init_dollar (i);
			d->alive = TRUE;
			return;
		}
	}
}

static void move_dollar (U8 dollar_number)
{
	struct dollar_state *d = &dollar_states[dollar_number];
	if (d->y > d->y_speed)
		d->y -= d->y_speed;
	else
		d->alive = FALSE;

	if (d->x > d->x_speed && d->left)
		d->x -= d->x_speed;
	else if (d->x < 128 - d->x_speed - DOLLAR_WIDTH && d->left == FALSE)
		d->x += d->x_speed;
	else
		d->alive = FALSE;
	//bounded_decrement (d->x_speed, 1);
}

static void draw_dollar (U8 dollar_number)
{
	struct dollar_state *d = &dollar_states[dollar_number];
	/* Check to make sure we don't draw off the screem */
	if (d->x > 128 - DOLLAR_WIDTH || d->y > 32 - DOLLAR_HEIGHT)
	{
		d->alive = FALSE;
		return;
	}
	
	/* Draw the low page */
	bitmap_blit (dollar_small_bitmap_2plane + BITMAP_LENGTH, d->x, d->y);
	bitmap_blit (dollar_small_bitmap_2plane + (BITMAP_LENGTH * 3), d->x + 5, d->y);
	/* Draw the high page */
	dmd_flip_low_high ();
	bitmap_blit (dollar_small_bitmap_2plane, d->x, d->y);
	bitmap_blit (dollar_small_bitmap_2plane + (BITMAP_LENGTH * 2), d->x + 5, d->y);
	dmd_flip_low_high ();
}

void dollar_overlay (void)
{
	U8 i;
	for (i = 0; i < MAX_DOLLARS;i++)
	{
		struct dollar_state *d = &dollar_states[i];
		if (d->alive)
		{
			draw_dollar (i);
			move_dollar (i);
		}
	}
}
