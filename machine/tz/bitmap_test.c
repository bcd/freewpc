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

/* CALLSET_SECTION (bitmap_test, __machine2__) */
#include <freewpc.h>

/* Total Size, including x,y padding */
#define TOTAL_BITMAP_SIZE 10
#define BITMAP_SIZE 8
#define MAX_STAR2S 8

/* Planes go in order of low, high */
const U8 test_bitmap_2plane[] = {
	4,4,0,6,6,0,
	4,4,15,9,9,15,
};

const U8 star_bitmap_2plane[] = {
	8,8,128,64,64,32,32,31,65,66, // Top left Low
	8,8,0,128,128,192,192,224,190,188, // Top left High
	
	8,8,0,1,1,2,2,124,65,33,
	8,8,0,0,0,1,1,3,62,30, //Top Right High
	
	8,8,68,8,4,4,130,98,25,7,
	8,8,184,240,248,248,124,28,6,0, //Bottom left High
	
	8,8,17,8,16,16,32,35,76,112,
	8,8,14,7,15,15,31,28,48,0, //Bottom left High
};

const U8 dollar_bitmap_2plane[] = {
	8,8,0,64,0,240,8,68,4,190,
	8,8,0,0,64,8,244,8,72,4,

	8,8,0,1,0,31,0,1,0,15,
	8,8,0,0,1,0,31,0,1,0,

	8,8,64,64,0,252,0,64,0,0,
	8,8,184,0,64,0,252,0,64,0,

	8,8,17,17,16,15,0,1,0,0,
	8,8,14,8,9,0,15,0,1,0,
};

struct star2_state 
{
	U8 x;
	U8 y;
	U8 x_speed;
	U8 y_speed;
	U8 ticks_till_alive;
	bool dir_left;
} star2_states[MAX_STAR2S];

static void respawn_star (U8 star_number)
{
	struct star2_state *s = &star2_states[star_number];
	s->y = 0;
	s->x = 20;
	s->x += random_scaled (88);
	s->dir_left = random_scaled (2);
	s->ticks_till_alive = random_scaled (MAX_STAR2S);
	s->y_speed = random_scaled (3) + 1;
	s->x_speed = random_scaled (3) + 1;
}

static void move_star (U8 star_number)
{
	struct star2_state *s = &star2_states[star_number];
	if (s->ticks_till_alive == 0)
	{
		s->y += s->y_speed;
		if (s->dir_left == TRUE && s->x > s->x_speed)
			s->x -= s->x_speed;
		else
			s->x += s->x_speed;
		
		if (s->y >= 16 - s->y_speed || s->x <= s->x_speed || s->x >= 112 - s->x_speed)
			respawn_star (star_number);
	}
	else 
		s->ticks_till_alive--;
}

static void draw_star2 (U8 star_number)
{
	struct star2_state *s = &star2_states[star_number];
	if (s->ticks_till_alive)
		return;
	/* Draw the low plane */
	bitmap_blit (dollar_bitmap_2plane + TOTAL_BITMAP_SIZE, s->x, s->y);
	bitmap_blit (dollar_bitmap_2plane + (TOTAL_BITMAP_SIZE * 3), s->x + BITMAP_SIZE, s->y);
	bitmap_blit (dollar_bitmap_2plane + (TOTAL_BITMAP_SIZE * 5), s->x, s->y + BITMAP_SIZE);
	bitmap_blit (dollar_bitmap_2plane + (TOTAL_BITMAP_SIZE * 7), s->x + BITMAP_SIZE, s->y + BITMAP_SIZE);
	dmd_flip_low_high ();

	/* Draw the high plane */
	bitmap_blit (dollar_bitmap_2plane, s->x, s->y);
	bitmap_blit (dollar_bitmap_2plane + (TOTAL_BITMAP_SIZE * 2), s->x + BITMAP_SIZE, s->y);
	bitmap_blit (dollar_bitmap_2plane + (TOTAL_BITMAP_SIZE * 4), s->x, s->y + BITMAP_SIZE);
	bitmap_blit (dollar_bitmap_2plane + (TOTAL_BITMAP_SIZE * 6), s->x + BITMAP_SIZE, s->y + BITMAP_SIZE);
	dmd_flip_low_high ();
	
}

CALLSET_ENTRY (bitmap_test, score_deff_start)
{
	U8 i;
	for (i = 0; i < MAX_STAR2S; i++)
	{
		respawn_star (i);	
	}

}

CALLSET_ENTRY (bitmap_test, score_overlay)
{
	U8 i;
	for (i = 0; i < MAX_STAR2S; i++)
	{
		draw_star2 (i);
		move_star (i);
	}

}

void bitmap_test_deff (void)
{
	U8 i;
	timer_restart_free (GID_BITMAP_TEST, TIME_30S);

	for (i = 0; i < MAX_STAR2S; i++)
	{
		respawn_star (i);	
	}

	while (task_find_gid (GID_BITMAP_TEST))
	{
		dmd_alloc_pair_clean ();
		for (i = 0; i < MAX_STAR2S; i++)
		{
			draw_star2 (i);
			move_star (i);
		}
		dmd_show2 ();
		task_sleep (TIME_100MS);
	}
	deff_exit ();
}
