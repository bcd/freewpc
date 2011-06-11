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

/* CALLSET_SECTION (wheelie, __machine4__) */
#include <freewpc.h>
#include "wheelie.h"

#define JUMP_HEIGHT 6
#define BIKE_YPOS 25 - wheelie1_low_height

U8 wheelie_ypos;
U8 wheelie_xpos;
U8 wheelie_speed;
U8 wheelie_speed_calc;
U8 wheelie_buttons_pressed;
const U8 wheelie_max_level = 88;

enum wheelie_states {
	WHEELIE_FLAT,
	WHEELIE_FLAT2,
	WHEELIE_UP1,
	WHEELIE_UP2,
	WHEELIE_UP3,
	WHEELIE_JUMP,
	WHEELIE_DOWN3,
	WHEELIE_DOWN2,
	WHEELIE_DOWN1,
} wheelie_state;

static void draw_wheelie1 (U8 x, U8 y)
{
	bitmap_blit (&wheelie1_low_bits, x, y);
	dmd_flip_low_high ();
	bitmap_blit (&wheelie1_high_bits, x, y);
	dmd_flip_low_high ();
}

static void draw_wheelie2 (U8 x, U8 y)
{
	bitmap_blit (&wheelie2_low_bits, x, y);
	dmd_flip_low_high ();
	bitmap_blit (&wheelie2_high_bits, x, y);
	dmd_flip_low_high ();
}
static void draw_wheelie3 (U8 x, U8 y)
{
	bitmap_blit (&wheelie3_low_bits, x, y);
	dmd_flip_low_high ();
	bitmap_blit (&wheelie3_high_bits, x, y);
	dmd_flip_low_high ();
}
static void draw_wheelie4 (U8 x, U8 y)
{
	bitmap_blit (&wheelie4_low_bits, x, y);
	dmd_flip_low_high ();
	bitmap_blit (&wheelie4_high_bits, x, y);
	dmd_flip_low_high ();
}
static void draw_wheelie5 (U8 x, U8 y)
{
	bitmap_blit (&wheelie5_low_bits, x, y);
	dmd_flip_low_high ();
	bitmap_blit (&wheelie5_high_bits, x, y);
	dmd_flip_low_high ();
}
static void draw_wheelie6 (U8 x, U8 y)
{
	bitmap_blit (&wheelie6_low_bits, x, y);
	dmd_flip_low_high ();
	bitmap_blit (&wheelie6_high_bits, x, y);
	dmd_flip_low_high ();
}


struct progress_bar_ops wheelie_progress_bar = {
	.x = 39,
	.y = 1,
	.fill_level = &wheelie_speed_calc,
	.max_level = &wheelie_max_level,
	.bar_width = 88,
};

static bool check_for_marty (void)
{
	if (wheelie_speed_calc == 88)
		return TRUE;
	else
		return FALSE;
}

static bool wheelie_going_up (void)
{
	if (wheelie_state >= WHEELIE_UP1 && wheelie_state <= WHEELIE_JUMP)
		return TRUE;
	else
		return FALSE;
}

static void calculate_jump (void)
{
	if (wheelie_state > WHEELIE_UP1 && wheelie_state < WHEELIE_DOWN3)
		wheelie_xpos += 2;

	if (wheelie_going_up ())
	{	
		if (wheelie_ypos < JUMP_HEIGHT)
			wheelie_ypos++;
		else if (wheelie_ypos == JUMP_HEIGHT)
			wheelie_state = WHEELIE_DOWN3;
	}
	else if (wheelie_ypos > 0)
		wheelie_ypos--;

}

static void start_jump (void)
{
	if (wheelie_ypos <= 1)
	{
		sound_send (SND_ROCKET_KICK_REVVING);
		wheelie_state = WHEELIE_UP1;
	}
}

static void draw_background (void)
{
}

static void draw_wheelie (U8 xpos)
{
	switch (wheelie_state)
	{
		case WHEELIE_FLAT:
			draw_wheelie1 (xpos, BIKE_YPOS - wheelie_ypos);
			wheelie_state = WHEELIE_FLAT2;
			break;
		case WHEELIE_FLAT2:
			draw_wheelie2 (xpos, BIKE_YPOS - wheelie_ypos);
			wheelie_state = WHEELIE_FLAT;
			break;
		case WHEELIE_UP1:
			draw_wheelie3 (xpos, BIKE_YPOS - wheelie_ypos);
			wheelie_state = WHEELIE_UP2;
			break;
		case WHEELIE_UP2:
			draw_wheelie4 (xpos, BIKE_YPOS - wheelie_ypos);
			wheelie_state = WHEELIE_UP3;
			break;
		case WHEELIE_UP3:
			draw_wheelie5 (xpos, BIKE_YPOS - wheelie_ypos);
			wheelie_state = WHEELIE_JUMP;
			break;
		case WHEELIE_JUMP:
			draw_wheelie6 (xpos, BIKE_YPOS - wheelie_ypos);
			break;
		case WHEELIE_DOWN3:
			draw_wheelie5 (xpos, BIKE_YPOS - wheelie_ypos);
			wheelie_state = WHEELIE_DOWN2;
			break;
		case WHEELIE_DOWN2:
			draw_wheelie4 (xpos, BIKE_YPOS - wheelie_ypos);
			wheelie_state = WHEELIE_DOWN1;
			break;
		case WHEELIE_DOWN1:
			draw_wheelie3 (xpos, BIKE_YPOS - wheelie_ypos);
			wheelie_state = WHEELIE_FLAT;
			break;
	}
}

static void wheelie_sleep (void)
{
	switch (wheelie_speed)
	{
		default:
		case 1:
			task_sleep (TIME_300MS);
			break;
		case 2:
			task_sleep (TIME_200MS);
			break;
		case 3:
			task_sleep (TIME_133MS);
			break;
		case 4:
			task_sleep (TIME_100MS);
			break;
		case 5:
			task_sleep (TIME_66MS);
			break;
		case 6:
			task_sleep (TIME_33MS);
			break;
		case 7:
		case 8:
			task_sleep (TIME_16MS);
			break;
	}
}

static void wheelie_draw_speed (void)
{	if (check_for_marty ())
		sprintf ("%d MPH!!", wheelie_speed_calc);
	else
		sprintf ("%d MPH", wheelie_speed_calc);
	font_render_string_center (&font_quadrit, 64, 16, sprintf_buffer); 
}

static void wheelie_button_press (void)
{
	if (wheelie_speed < 8)
		bounded_increment (wheelie_buttons_pressed, 10);
	else
		bounded_increment (wheelie_buttons_pressed, 8);

	if (wheelie_buttons_pressed >= 10 && wheelie_speed < 8)
	{
		wheelie_buttons_pressed = 0;
		sound_send (SND_EXPLOSION_3);
		bounded_increment (wheelie_speed, 8);
	}
	wheelie_speed_calc = (wheelie_speed * 10) + wheelie_buttons_pressed;
	if (check_for_marty ())
	{
		//88 MPH!
	}

}

static void wheelie_start (void)
{
	wheelie_xpos = 2;
	wheelie_ypos = 0;
	wheelie_speed = 0;
	wheelie_speed_calc = 1;
	wheelie_buttons_pressed = 1;
}

void wheelie_deff (void)
{
	U8 i;
	wheelie_start ();
	//for (wheelie_xpos = 2; wheelie_xpos < 128 - wheelie1_low_width; wheelie_xpos += wheelie_speed)
	for (;;)
	{
		//calculate_jump ();
		dmd_alloc_pair_clean ();
		draw_background ();
		wheelie_draw_speed ();
		draw_wheelie (wheelie_xpos);
		if (!check_for_marty ())
			draw_progress_bar (&wheelie_progress_bar);
		sound_send (0x54);
		dmd_show2 ();
		wheelie_sleep ();
	}
	deff_exit ();
}

CALLSET_ENTRY (wheelie, sw_right_button, sw_left_button)
{
	if (deff_get_active () == DEFF_WHEELIE)
		wheelie_button_press ();
}
