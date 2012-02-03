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

/* Based on initials.c */

#include <freewpc.h>

/**
 * \file
 * \brief Entry of player's Personal Identification Number
 * This can be used to 'secure' things like flipcode entry
 * and tournament games
 */


#define PIN_LENGTH 4

/** The array of characters that can be entered.
 * Keep the length of this as a power of 2 (16) so that
 * the circular buffer implementation is simple.
 */
static const unsigned char pin_chars[] = "0123456789ABCDEF";

#if (MACHINE_DMD == 1)
#define MAX_LETTERS_SHOWN 12
#else
#define MAX_LETTERS_SHOWN 1
#endif
#define MAX_INITIAL_INITIAL (sizeof (pin_chars) - MAX_LETTERS_SHOWN)

#define SELECT_OFFSET 2
#define FONT_WIDTH 8
#define ALPHABET_LEN 16

/* The amount of time left to enter the PIN */
U8 pin_enter_timer;

/* The index of the next number to be entered */
U8 pin_index;

/* The index of the currently selected letter/symbol */
U8 pin_selection;

/* The array of pin */
char pin_data[PIN_LENGTH+1];

/* The callback function to invoke when pin is entered OK */
void (*pin_enter_complete) (void);


/**
 * The display effect for the enter PIN entry screen.
 *
 * It continuously redraws itself whenever the timer, the selected
 * character, or the entered characters change.
 */
void enter_pin_deff (void)
{
	while (in_test || task_find_gid (GID_ENTER_PIN))
	{
		if (score_update_required ())
		{
#if (MACHINE_DMD == 1)
			U8 n;
			dmd_alloc_low_clean ();
			font_render_string_left (&font_var5, 0, 1, "ENTER PIN");
			font_render_string_left (&font_fixed10, 0, 9, pin_data);

			if (pin_selection < MAX_INITIAL_INITIAL+1)
			{
				sprintf ("%12s", pin_chars + pin_selection);
				font_render_string_left (&font_bitmap8, 0, 23, sprintf_buffer);
			}
			else
			{
				U8 x;

				x = ALPHABET_LEN - pin_selection;
				sprintf ("%*s", x, pin_chars + pin_selection);
				font_render_string_left (&font_bitmap8, 0, 23, sprintf_buffer);

				x = MAX_LETTERS_SHOWN - x;
				sprintf ("%*s", x, pin_chars);
				x = MAX_LETTERS_SHOWN - x;
				font_render_string_left (&font_bitmap8, x * FONT_WIDTH, 23, sprintf_buffer);
			}

			for (n = 22; n <= 30; n++)
			{
				dmd_low_buffer[16UL * n + SELECT_OFFSET - 1] ^= 0x80;
				dmd_low_buffer[16UL * n + SELECT_OFFSET] ^= 0x7F;
			}

			sprintf ("%d", pin_enter_timer);
			font_render_string_right (&font_fixed6, 126, 3, sprintf_buffer);
			dmd_show_low ();
#elif (MACHINE_ALPHANUMERIC == 1)
			seg_alloc_clean ();
			seg_write_string (0, 0, "ENTER PIN");
			sprintf ("%c", pin_chars[pin_selection]);
			seg_write_string (0, 15, sprintf_buffer);
			seg_write_string (1, 0, pin_data);
			sprintf ("%d", pin_enter_timer);
			seg_write_string (1, 14, sprintf_buffer);
			seg_show ();
#endif
		}
		task_sleep (TIME_66MS);
	}
	task_sleep (TIME_500MS);
	deff_exit ();
}

void pin_stop (void)
{
	task_sleep_sec (1);
	task_kill_gid (GID_ENTER_PIN);
	pin_enter_timer = 0;
}


void pin_running (void)
{
	task_sleep_sec (1);
	pin_enter_timer = 30;
	memset (pin_data, 0, sizeof (pin_data));
	pin_index = 0;
	pin_selection = 0;
#ifdef DEFF_ENTER_PIN
	deff_start (DEFF_ENTER_PIN);
#endif

#if 1
	pin_enter_complete = null_function;
#endif

	while (pin_enter_timer > 0)
	{
		task_sleep (TIME_1S + TIME_66MS);
		pin_enter_timer--;
		score_update_request ();
	}
	task_exit ();
}


void pin_enter (void)
{
	task_create_gid1 (GID_ENTER_PIN, pin_running);
	while (task_find_gid (GID_ENTER_PIN))
		task_sleep (TIME_133MS);
}


CALLSET_ENTRY (pin, sw_left_button)
{
	if (pin_enter_timer)
	{
		--pin_selection;
		pin_selection %= ALPHABET_LEN;
		score_update_request ();
	}
}


CALLSET_ENTRY (pin, sw_right_button)
{
	if (pin_enter_timer)
	{
		++pin_selection;
		pin_selection %= ALPHABET_LEN;
		score_update_request ();
	}
}

CALLSET_ENTRY (pin, start_button_handler)
{
	if (pin_enter_timer && pin_index != PIN_LENGTH)
	{
		pin_data[pin_index] =
			pin_chars[(pin_selection + SELECT_OFFSET) % ALPHABET_LEN];
		score_update_request ();
		if (++pin_index == PIN_LENGTH)
		{
			(*pin_enter_complete) ();
			pin_stop ();
		}
	}
}

