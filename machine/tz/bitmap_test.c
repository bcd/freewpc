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
#define TOTAL_BITMAP_SIZE 18
/* Width of each bitmap */
#define BITMAP_SIZE 8
/* How many are shown at once */
#define MAX_BITMAPS 1
/* How many different bitmaps that are defined */
#define NUM_BITMAPS 4

static bool bitmap_bounce;
extern bool draw_bouncing_overlay;

/* Planes go in order of low, high */
const U8 star_bitmap_2plane[] = {
	8,16,128,64,64,32,32,31,65,66,68,8,4,4,130,98,25,7, //Left low
	8,16,0,128,128,192,192,224,190,188,184,240,248,248,124,28,6,0, //Left high
	
	8,16,0,1,1,2,2,124,65,33,17,8,16,16,32,35,76,112, //Right low
	8,16,0,0,0,1,1,3,62,30,14,7,15,15,31,28,48,0, //Right High
};

const U8 dollar_bitmap_2plane[] = {
	8,16,0,64,0,240,8,68,4,248,64,64,0,252,0,64,0,0,
	8,16,0,0,64,8,244,8,72,4,184,0,64,0,252,0,64,0,

	8,16,0,1,0,31,0,1,0,15,17,17,16,15,0,1,0,0,
	8,16,0,0,1,0,31,0,1,0,14,8,9,0,15,0,1,0,
};

const U8 pound_bitmap_2plane[] = {
	8,16,128,192,224,224,224,224,248,248,248,224,224,252,252,252,0,0,
	8,16,64,32,16,16,16,24,4,4,4,24,28,0,2,2,252,0,

	8,16,31,63,63,57,1,1,31,31,31,1,1,63,63,63,0,0,
	8,16,32,64,64,70,62,30,32,32,32,30,62,64,64,64,63,0,
};

const U8 trophy_bitmap_2plane[] = {
	8,16,240,6,1,1,1,1,2,4,8,16,32,32,32,48,8,248,
	8,16,0,240,254,114,50,114,116,56,240,224,192,192,192,192,240,0,

	8,16,7,48,64,64,64,64,32,16,8,4,2,2,2,6,8,15,
	8,16,0,7,63,39,39,39,23,14,7,3,1,1,1,1,7,0,
};


struct bitmap_state 
{
	U8 x;
	U8 y;
	U8 x_speed;
	U8 y_speed;
	U8 ticks_till_alive;
	U8 type;
	bool dir_left;
	bool dir_up;
} bitmap_states[MAX_BITMAPS];

static void change_direction (U8 bitmap_number)
{
	struct bitmap_state *s = &bitmap_states[bitmap_number];

	if (s->y >= 16 - s->y_speed && s->dir_up == FALSE)
		s->dir_up = TRUE;
	else if (s->y_speed >= s->y && s->dir_up == TRUE)
		s->dir_up = FALSE;

	if (s->x >= 112 - s->x_speed && s->dir_left == FALSE)
		s->dir_left = TRUE;
	else if (s->x <= s->x_speed && s->dir_left == TRUE)
		s->dir_left = FALSE;
	
	s->y_speed = random_scaled (3) + 1;
	s->x_speed = random_scaled (3) + 1;
//	if (random_scaled(2))
//		s->type = random_scaled (NUM_BITMAPS);
}
//TODO Always start in the middle?
static void respawn_bitmap (U8 bitmap_number)
{
	struct bitmap_state *s = &bitmap_states[bitmap_number];
	s->y = 0;
	s->x = 20;
	s->x += random_scaled (88);
	s->dir_left = random_scaled (2);
	s->dir_up = FALSE;
	s->ticks_till_alive = random_scaled (MAX_BITMAPS);
	s->y_speed = random_scaled (3) + 1;
	s->x_speed = random_scaled (3) + 1;
//	s->type = random_scaled (NUM_BITMAPS);
}

static bool check_boundary (U8 bitmap_number)
{
	struct bitmap_state *s = &bitmap_states[bitmap_number];
	if ((s->y >= 16 - s->y_speed &&s->dir_up == FALSE)
			|| (s->y_speed >= s->y && s->dir_up == TRUE)
			|| (s->x <= s->x_speed && s->dir_left == TRUE) 
			|| (s->x >= 112 - s->x_speed && s->dir_left == FALSE))
		return TRUE;
	else
		return FALSE;
}

static void move_bitmap (U8 bitmap_number)
{
	struct bitmap_state *s = &bitmap_states[bitmap_number];
	if (s->ticks_till_alive == 0)
	{
		if (s->dir_up == TRUE && s->y >= s->y_speed)
			s->y -= s->y_speed;
		else if (s->y + s->y_speed <= 32)
			s->y += s->y_speed;

		if (s->dir_left == TRUE && s->x >= s->x_speed)
			s->x -= s->x_speed;
		else if (s->x + s->x_speed <= 112)
			s->x += s->x_speed;
		
		if (check_boundary (bitmap_number))
		{
			if (bitmap_bounce)
				change_direction (bitmap_number);
			else
				respawn_bitmap (bitmap_number);
		}
	}
	else 
		s->ticks_till_alive--;
}

static void draw_bitmap (U8 bitmap_number)
{
	struct bitmap_state *s = &bitmap_states[bitmap_number];
	U8 *src;
	if (s->ticks_till_alive || bitmap_number > MAX_BITMAPS)
		return;
	/* Don't draw if it's going to be off the screen */
	if (s->x > 112 || s->y > 16)
	{
		respawn_bitmap (bitmap_number);
		return;
	}
	

	if (s->type == 1)
		src = &pound_bitmap_2plane;
	else if (s->type == 2)
		src = &star_bitmap_2plane;
	else if (s->type == 3)
		src = &trophy_bitmap_2plane;
	else
		src = &dollar_bitmap_2plane;
	
	/* Draw the 2nd plane */
	bitmap_blit (src + TOTAL_BITMAP_SIZE, s->x, s->y);
	bitmap_blit (src + (TOTAL_BITMAP_SIZE * 3), s->x + BITMAP_SIZE, s->y);
	dmd_flip_low_high ();

	/* Draw the 1st plane */
	bitmap_blit (src, s->x, s->y);
	bitmap_blit (src + (TOTAL_BITMAP_SIZE * 2), s->x + BITMAP_SIZE, s->y);
	dmd_flip_low_high ();
	
}

static void bitmap_set_type (U8 type)
{
//	if (type > NUM_BITMAPS - 1)
//		type = NUM_BITMAPS - 1;
	U8 i;
	for (i = 0; i < MAX_BITMAPS; i++)
	{
		struct bitmap_state *s = &bitmap_states[i];
		s->type = type;
	}
}

CALLSET_ENTRY (bitmap_test, start_ball)
{
	bitmap_bounce = TRUE;
	U8 i;
	for (i = 0; i < MAX_BITMAPS; i++)
	{
		respawn_bitmap (i);	
	}

}

void stardrop_overlay_draw (void)
{
	U8 i;
	for (i = 0; i < MAX_BITMAPS; i++)
	{
		draw_bitmap (i);
		move_bitmap (i);
	}

}

void check_bitmap_overlay (void)
{
	draw_bouncing_overlay = FALSE;
	if (num_players != 1)
		return;

	if (score_compare (current_score, score_table[SC_100M]) == 1)
	{
		draw_bouncing_overlay = TRUE;
		bitmap_set_type (0);
	}

	if (score_compare (current_score, score_table[SC_500M]) == 1)
		bitmap_set_type (3);
	else if (score_compare (current_score, score_table[SC_400M]) == 1)
		bitmap_set_type (3);
	else if (score_compare (current_score, score_table[SC_300M]) == 1)
		bitmap_set_type (2);
	else if (score_compare (current_score, score_table[SC_200M]) == 1)
		bitmap_set_type (1);
}

void bitmap_test_deff (void)
{
	U8 i;

	for (i = 0; i < MAX_BITMAPS; i++)
	{
		respawn_bitmap (i);	
		bitmap_set_type (random_scaled (NUM_BITMAPS));
	}

	bitmap_bounce = TRUE;
	for (;;)
	{
		dmd_alloc_pair_clean ();
		stardrop_overlay_draw ();
		dmd_show2 ();
		task_sleep (TIME_16MS);
	}
	deff_exit ();
}
