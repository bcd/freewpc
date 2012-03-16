/*
 * Copyright 2007-2012 by Brian Dominy <brian@oddchange.com>
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
#include <highscore.h>
#include <replay.h>
#include <coin.h>
#include <diag.h>

extern U8 last_nonfatal_error_code;
extern task_gid_t last_nonfatal_error_gid;
extern __nvram__ U8 current_volume;


void ball_save_deff (void)
{
	dmd_alloc_pair ();
	dmd_clean_page_low ();
	sprintf ("PLAYER %d", player_up);
	font_render_string_center (&font_fixed6, 64, 8, sprintf_buffer);
	dmd_copy_low_to_high ();
	font_render_string_center (&font_fixed6, 64, 22, "BALL SAVED");
	dmd_show_low ();
	deff_swap_low_high (24, TIME_100MS);
	deff_exit ();
}


/** Display effect when locating missing balls prior to game start */
void locating_balls_deff (void)
{
	/* For Funhouse/Road Show, if a ball is sitting in the left plunger
	lane, announce that, as ball search isn't going to fix it. */
#ifdef MACHINE_LEFT_SHOOTER_SWITCH
	if (switch_poll_logical (MACHINE_LEFT_SHOOTER_SWITCH))
		generic_deff ("CLEAR BALL FROM", "LEFT SHOOTER");
	else
#else
	generic_deff ("LOCATING BALLS", "PLEASE WAIT...");
#endif
}


/** The display effect that appears immediately at the end of a game,
before returning to attract mode */
void game_over_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed6, 64, 16, "GAME OVER");
	dmd_show_low ();
	task_sleep_sec (2);

	/* In tournament mode, need to show the scores briefly here */
	if (tournament_mode_enabled)
	{
		dmd_alloc_low_clean ();
		scores_draw ();
		dmd_show_low ();
		task_sleep_sec (60);
		deff_exit ();
	}
	deff_exit ();
}


void draw_volume_bar (U8 n)
{
#if (MACHINE_DMD == 1)
	U8 *base = dmd_low_buffer + 22 * DMD_BYTE_WIDTH + 4;
	U8 val = 0x55;
	U8 h;
	static const U8 volume_bar_data[] = { 0x0, 0x1, 0x5, 0x15, 0x55 };
	while (n >= 4)
	{
		for (h=0; h < 8; h++)
			base[h * DMD_BYTE_WIDTH] = val;
		base++;
		n -= 4;
	}
	val = volume_bar_data[n];
	for (h=0; h < 8; h++)
		base[h * DMD_BYTE_WIDTH] = val;
#endif
}


#if (MACHINE_DMD == 1)
/** Draw a thin border 2 pixels wide around the given display page. */
void dmd_draw_border (U8 *dbuf)
{
	const dmd_buffer_t dbuf_bot = (dmd_buffer_t)((char *)dbuf + 480);
	register U16 *dbuf16 = (U16 *)dbuf;
	register U16 *dbuf16_bot = (U16 *)dbuf_bot;
	U8 i;

	for (i=0; i < 16; i++)
		*dbuf16_bot++ = *dbuf16++ = 0xFFFFUL;
	dbuf += 32;
	for (i=0; i < 28; i++)
	{
		dbuf[0] = 0x03;
		dbuf[15] = 0xC0;
		dbuf += 16;
	}
}


void dmd_draw_thin_border (U8 *dbuf)
{
	const dmd_buffer_t dbuf_bot = (dmd_buffer_t)((char *)dbuf + 496);
	register U16 *dbuf16 = (U16 *)dbuf;
	register U16 *dbuf16_bot = (U16 *)dbuf_bot;
	U8 i;

	for (i=0; i < 8; i++)
		*dbuf16_bot++ = *dbuf16++ = 0xFFFFUL;
	dbuf += 16;
	for (i=0; i < 30; i++)
	{
		dbuf[0] = 0x01;
		dbuf[15] = 0x80;
		dbuf += 16;
	}
}


/**
 * Draw a horizontal line at the specified y-coordinate.
 */
void dmd_draw_horiz_line (U16 *dbuf, U8 y)
{
	dbuf += y * (16 / 2);

	*dbuf++ = 0xffffUL;
	*dbuf++ = 0xffffUL;
	*dbuf++ = 0xffffUL;
	*dbuf++ = 0xffffUL;
	*dbuf++ = 0xffffUL;
	*dbuf++ = 0xffffUL;
	*dbuf++ = 0xffffUL;
	*dbuf++ = 0xffffUL;
}
#endif /* MACHINE_DMD */


/** The display effect for a volume change */
void volume_change_deff (void)
{
	dmd_alloc_low_clean ();
	sprintf ("VOLUME %d", current_volume);
	font_render_string_center (&font_fixed6, 64, 9, sprintf_buffer);
	draw_volume_bar (current_volume);
	dmd_show_low ();
	if (in_live_game)
		task_sleep_sec (3);
	else
		task_sleep_sec (5);
	deff_exit ();
}


/** The display effect for a non-fatal error */
void nonfatal_error_deff (void)
{
#ifdef DEBUGGER
	dmd_alloc_low_clean ();
	sprintf ("NONFATAL %ld", system_audits.non_fatal_errors);
	font_render_string_center (&font_mono5, 64, 10, sprintf_buffer);
	sprintf ("ERRNO %i GID %i", last_nonfatal_error_code, last_nonfatal_error_gid);
	font_render_string_center (&font_mono5, 64, 20, sprintf_buffer);
	dmd_show_low ();
	sound_send (SND_TEST_ALERT);
	task_sleep (TIME_200MS);
	sound_send (SND_TEST_ALERT);
	task_sleep (TIME_200MS);
	sound_send (SND_TEST_ALERT);
	task_sleep_sec (4);
#endif
	deff_exit ();
}


/** The display effect for the final ball goal */
void score_goal_deff (void)
{
	/* Show the replay if it is enabled and hasn't been awarded yet. */
	if (replay_can_be_awarded ())
	{
		replay_draw ();
	}
	else
	{
		grand_champion_draw ();
	}
	task_sleep_sec (2);
	deff_exit ();
}


void plunge_ball_deff (void)
{
	dmd_alloc_pair ();
	dmd_clean_page_low ();

	sprintf ("PLAYER %d", player_up);
	font_render_string_center (&font_fixed6, 64, 9, sprintf_buffer);
	dmd_draw_border (dmd_low_buffer);
	dmd_copy_low_to_high ();
#ifdef MACHINE_LAUNCH_SWITCH
	font_render_string_center (&font_fixed6, 64, 22, "PRESS LAUNCH");
#else
	font_render_string_center (&font_fixed6, 64, 22, "PLUNGE THE BALL");
#endif
	deff_swap_low_high (13, TIME_300MS);
	deff_exit ();
}


void coin_door_buttons_deff (void)
{
	dmd_alloc_low_clean ();
#if (MACHINE_DMD == 1)
	font_render_string_center (&font_var5, 64, 3, "COIN DOOR IS CLOSED");
#endif
	font_render_string_center (&font_var5, 64, 10, "OPEN COIN DOOR");
	font_render_string_center (&font_var5, 64, 17, "TO USE BUTTONS");
	dmd_show_low ();
	task_sleep_sec (3);
	deff_exit ();
}


void coin_door_power_deff (void)
{
	U8 n;
	for (n=0; n < 5; n++)
	{
		dmd_alloc_low_clean ();
		dmd_show_low ();
		task_sleep (TIME_200MS);

		dmd_alloc_low_clean ();
#if (MACHINE_DMD == 1)
		font_render_string_center (&font_fixed6, 64, 6, "COIN DOOR IS OPEN");
		font_render_string_center (&font_fixed6, 64, 16, "HIGH POWER");
		font_render_string_center (&font_fixed6, 64, 26, "IS DISABLED");
#else
		font_render_string_center (&font_fixed6, 64, 10, "HIGH POWER");
		font_render_string_center (&font_fixed6, 64, 21, "IS DISABLED");
#endif
		dmd_show_low ();
		sound_send (SND_TEST_ALERT);
		task_sleep (TIME_300MS);
	}
	task_sleep_sec (3);
	deff_exit ();
}


/** The tilt display effect runs until explicitly cancelled. */
void tilt_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_cu17, 64, 13, "TILT");
	dmd_show_low ();
	for (;;)
		task_sleep_sec (10);
}


void tilt_warning_deff (void)
{
	extern U8 tilt_warnings;
	dmd_alloc_pair_clean ();
	if (tilt_warnings % 2)
	{
		font_render_string_center (&font_fixed10, 64, 16, "DANGER");
	}
	else
	{
		font_render_string_center (&font_fixed10, 64, 7, "DANGER");
		font_render_string_center (&font_fixed10, 64, 23, "DANGER");
	}
	deff_swap_low_high (24, TIME_66MS);
	deff_exit ();
}


void slam_tilt_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed10, 64, 13, "SLAM TILT");
	dmd_show_low ();
	for (;;)
		task_sleep_sec (30);
}


/** Reduce a credit fraction to simplest terms. */
static inline void reduce_unit_fraction (U8 *units, U8 *units_per_credit)
{
	switch (*units_per_credit)
	{
		case 4:
			if (*units == 2)
			{
				*units = 1;
				*units_per_credit = 2;
			}
			break;

		case 6:
			switch (*units)
			{
				case 2:
					*units = 1;
					*units_per_credit = 3;
					break;
				case 4:
					*units = 2;
					*units_per_credit = 3;
					break;
			}
			break;
	}
}

/** Render the number of credits */
void credits_render (void)
{
#ifdef FREE_ONLY
	sprintf ("FREE ONLY");
#else
	if (price_config.free_play)
		sprintf ("FREE PLAY");
	else
	{
		if (get_units () != 0)
		{
			U8 units = get_units ();
			U8 units_per_credit = price_config.units_per_credit;

			/* There are fractional credits.  Reduce to the
			 * lowest common denominator before printing. */

			reduce_unit_fraction (&units, &units_per_credit);

			if (get_credits () == 0)
				sprintf ("%d/%d CREDIT", units, units_per_credit);
			else
				sprintf ("%d %d/%d CREDITS",
					get_credits (), units, units_per_credit);
		}
		else
		{
			if (get_credits () == 1)
				sprintf ("%d CREDIT", get_credits ());
			else
				sprintf ("%d CREDITS", get_credits ());
		}
	}
#endif
	if (diag_get_error_count ())
	{
		sprintf ("%E.");
	}
}


/** Draw the current credits full screen */
void credits_draw (void)
{
	dmd_alloc_pair ();
	dmd_clean_page_low ();

	credits_render ();
	font_render_string_center (&font_fixed6, 64, 9, sprintf_buffer);
	dmd_copy_low_to_high ();

	if (!has_credits_p ())
	{
		if (price_config.payment_method == PAY_COIN)
			sprintf ("INSERT COINS");
		else if (price_config.payment_method == PAY_TOKEN)
			sprintf ("INSERT TOKENS");
		else if (price_config.payment_method == PAY_CARD)
			sprintf ("SWIPE CARD");
		else if (price_config.payment_method == PAY_BILL)
			sprintf ("INSERT BILLS");
	}
	else
	{
		sprintf ("PRESS START");
	}
	font_render_string_center (&font_fixed6, 64, 22, sprintf_buffer);
}


void credits_deff (void)
{
	credits_draw ();
	deff_swap_low_high (in_live_game ? 12 : 20, 2 * TIME_100MS);
	deff_delay_and_exit (TIME_1S);
}


/** Display effect that runs during the extra ball buyin */
void buyin_offer_deff (void)
{
	U8 prev_timer;
	extern U8 buyin_offer_timer;

#if (MACHINE_DMD == 1)
	dmd_sched_transition (&trans_bitfade_slow);
#else
	seg_sched_transition (&seg_trans_fast_center_out);
#endif
	while (buyin_offer_timer > 0)
	{
		prev_timer = buyin_offer_timer;
		dmd_alloc_low_clean ();
		dmd_draw_border (dmd_low_buffer);
		font_render_string_center (&font_term6, 64, 5, "CONTINUE GAME");
#if (MACHINE_DMD == 1)
		sprintf ("%d", buyin_offer_timer);
		font_render_string_left (&font_mono5, 4, 3, sprintf_buffer);
		font_render_string_right (&font_mono5, 123, 3, sprintf_buffer);
		if (buyin_offer_timer % 2)
		{
			font_render_string_center (&font_bitmap8, 64, 16, "INSERT COINS");
			font_render_string_center (&font_bitmap8, 64, 26, "FOR EXTRA BALL");
		}
		else
		{
			font_render_string_center (&font_bitmap8, 64, 16, "THEN PRESS");
			font_render_string_center (&font_bitmap8, 64, 26, "BUY EXTRA BALL");
		}
#else
		sprintf ("%d SECS", buyin_offer_timer);
		seg_write_row_center (1, sprintf_buffer);
#endif
		dmd_show_low ();
		while (prev_timer == buyin_offer_timer)
			task_sleep (TIME_133MS);
	}
	task_sleep (TIME_1500MS);
	deff_exit ();
}

void player_tournament_ready_deff (void)
{
	U8 timer;

	dmd_alloc_low_clean ();
#if (MACHINE_DMD == 1)
	font_render_string_center (&font_fixed6, 64, 5, "TOURNAMENT");
	font_render_string_center (&font_fixed6, 64, 16, "MODE ENABLED");
#else
	font_render_string_center (&font_var5, 64, 5, "TOURNAMENT MODE");
#endif
	font_render_string_center (&font_var5, 64, 28, "PRESS START NOW");
	dmd_show_low ();

	timer = 7;
	do {
		task_sleep_sec (1);
	} while (--timer != 0);

	tournament_mode_enabled = OFF;
	deff_exit ();
}



