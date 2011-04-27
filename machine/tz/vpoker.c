/*
 * Copyright 2006-2010 by Brian Dominy <brian@oddchange.com>
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

/* CALLSET_SECTION (vpoker, __machine3__) */

#include <freewpc.h>

/**
 * \file
 * \brief Vpoker
 */

#define NUM_CARDS 13
#define NUM_SUITS 4

static const char vpoker_cards[] = "234567890JQKA";

/* string holding player cards */
char vpoker_hand[4];

bool vpoker_active;

U8 vpoker_index;
typedef enum {
	VPOKER_FIRST_DEAL,
	VPOKER_SECOND_DEAL,
	VPOKER_SHOW_RESULT,
	VPOKER_EXIT,
} vpoker_state_t;

static vpoker_state_t current_state;

void vpoker_draw_deff (void)
{
	while (in_test || task_find_gid (GID_VPOKER))
	{
		if (score_update_required ())
		{
			dmd_alloc_low_clean ();
			sprintf("%s", vpoker_hand);
			font_render_string_left (&font_fixed10, 0, 9, sprintf_buffer);
			
			switch (current_state)
			{
				case VPOKER_FIRST_DEAL:
					break;
				case VPOKER_SECOND_DEAL:
					break;
				case VPOKER_SHOW_RESULT:
					break;
				case VPOKER_EXIT:
					break;
			}
			switch (vpoker_index)
			{
				/* Draw where the marker is */
				case 0:
					font_render_string_left (&font_fixed10, 0, 5, "+");
					break;
				case 1:
					font_render_string_left (&font_fixed10, 10, 5, "+");
					break;
				case 2:
					font_render_string_left (&font_fixed10, 20, 5, "+");
					break;
				case 3:
					font_render_string_left (&font_fixed10, 30, 5, "+");
					break;
				case 4:
					font_render_string_left (&font_fixed10, 40, 5, "+");
					break;
				case 5:
					font_render_string_left (&font_fixed10, 50, 5, "+");
					break;
			}
			dmd_show_low ();
			task_sleep (TIME_66MS);
		}
	}
	task_sleep (TIME_500MS);
	deff_exit ();
}

void vpoker_stop (void)
{
	task_sleep_sec (1);
	task_kill_gid (GID_VPOKER);
}

static void init_player_hand (void)
{
	/* Pick 5 random cards */
	/* TODO Check for 5 Aces etc */
	U8 i;
	for (i = 0; i < 5; i++)
	{
		vpoker_hand[i] = vpoker_cards[random_scaled(NUM_CARDS)];
	}
}

void vpoker_running (void)
{
	vpoker_active = TRUE;
	vpoker_index = 0;
	current_state = VPOKER_FIRST_DEAL;
	init_player_hand ();

	task_sleep_sec (1);
		
	deff_start (DEFF_VPOKER_DRAW);

	while (task_find_gid (GID_VPOKER))
	{
		task_sleep (TIME_1S + TIME_66MS);
		score_update_request ();
	}
	current_state = VPOKER_EXIT;
	vpoker_active = FALSE;
	task_exit ();
}


void vpoker_enter (void)
{
	task_create_gid1 (GID_VPOKER, vpoker_running);
	while (task_find_gid (GID_VPOKER))
		task_sleep (TIME_133MS);
}

CALLSET_ENTRY (vpoker, sw_left_button)
{
	if (vpoker_active == TRUE)
	{
		bounded_decrement (vpoker_index, 0);
		score_update_request ();
	}
}

CALLSET_ENTRY (vpoker, sw_right_button)
{
	if (vpoker_active == TRUE)
	{
		bounded_increment (vpoker_index, 5);
		score_update_request ();
	}
}

CALLSET_ENTRY (vpoker, sw_buyin)
{
	if (vpoker_active == TRUE)
	{
		//vpoker_buyin_handler ();
		vpoker_active = FALSE;
		score_update_request ();
	}
}
