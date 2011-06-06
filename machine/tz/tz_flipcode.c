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

/* Basic flipcode and PIN
 * To start input:
 * hold down both flippers and the buyin button whilst the ball is in the plunger lane
 * See common/pause.c for start conditions
 */

/* CALLSET_SECTION ( tz_flipcode, __machine3__ ) */
#include <freewpc.h>
#include <eb.h>

extern char initials_data[3];
extern char pin_data[4];
extern U8 initials_enter_timer;
extern U8 pin_enter_timer;
extern bool juggle_ball;

extern void intials_stop (void);
extern void pin_stop (void);

/* Used to invalidate high score, 
 * currently invalidates all scores of a multiplayer game */
bool flipcode_used;

U8 tz_flipcode_number;

extern score_t temp_score;

#define NUM_TZ_FLIPCODES 8
struct {
	const char *initials;
	char *pin;
	char *text;
} tz_flipcodes[] = {
	{ "BCD", "1234", "THE WIZARD"},
	{ "FEK", "2345", "WELCOME BACK"},
	{ "MAC", "3456", "EXTRA BALL LIT"},
	{ "MET", "4567", "GET BACK TO IRC"},
	{ "SAM", "5678", "MAX POWER"},
	{ "SUN", "6789", "LIKE THE WEATHER"},
	{ "JND", "789A", "MEGA POINTS"},
	{ "EDI", "89AB", "FROM THE TELLY"},
};

#ifndef CONFIG_NATIVE
U8 strcmp (char *s1, char *s2)
{
	while (*s1 == *s2++)
		if (*s1++ == 0)
			return (0);
	return (*s1 - *--s2);
}
#endif

void tz_flipcode_entered_deff (void)
{
	dmd_map_overlay ();
	dmd_clean_page_low ();

	sprintf ("HI %s", initials_data);
	font_render_string_center (&font_times10, 40, 11, sprintf_buffer);
	font_render_string_center (&font_var5, 40, 24, tz_flipcodes[tz_flipcode_number].text);

	dmd_text_outline ();
	dmd_alloc_pair ();
//	frame_draw (IMG_COW);
	dmd_overlay_outline ();
	dmd_show2 ();
	task_sleep_sec (3);
	deff_exit ();
}

void tz_flipcode_entry_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_var5, 40, 11, "THE POWER");
	font_render_string_center (&font_var5, 40, 22, "SAYS ...");
	dmd_show_low ();
	sound_send (SND_THUNDER1);
	task_sleep_sec (2);
	deff_exit ();
}

CALLSET_ENTRY (tz_flipcode, check_tz_flipcode)
{
	U8 i;
	for (i = 0; i < NUM_TZ_FLIPCODES; i++)
	{
		if (strcmp(tz_flipcodes[i].initials, initials_data) == 0
			&& strcmp(tz_flipcodes[i].pin, pin_data) == 0)
		{
			switch (i)
			{
				/* BCD */
				case 0:
					sound_send (SND_HEY_ITS_ONLY_PINBALL);
					break;
				/* FEK */
				case 1:
				//	juggle_ball = TRUE;
				//	flag_on (FLAG_SNAKE_READY);
				//	deff_start_sync (DEFF_SNAKE_READY);
					//callset_invoke (door_start_clock_millions);
					callset_invoke (mball_start);
					callset_invoke (mball_start_3_ball);
					//tz_clock_request_time (2, 30);
					break;
				/* MAC */
				case 2:
					light_easy_extra_ball ();
					sound_send (SND_YOU_UNLOCK_THIS_DOOR);
					break;
				/* MET */
				case 3:
					sound_send (SND_WELCOME_RACE_FANS);
					break;
				/* SAM */
				case 4:
					callset_invoke (door_start_clock_chaos);
					sound_send (SND_WELCOME_RACE_FANS);
					break;
				/* SUN */
				case 5:
					callset_invoke (mball_start_3_ball);
					sound_send (SND_ITS_SUNNY_DRIVE_TIME);
					break;
				/* JND */
				case 6:
					score_mul (current_score, 2);
					sound_send (SND_NO_CREDITS);
					break;
				/* EDI */
				case 7:
					sound_send (SND_HEY_ITS_ONLY_PINBALL);
					break;
			}
			/* Store for deff */
			tz_flipcode_number = i;
			flipcode_used = TRUE;
			deff_start_sync (DEFF_TZ_FLIPCODE_ENTERED);
			return;
		}
	}
	sound_send (SND_BUYIN_CANCELLED);
}

void tz_flipcode_running (void)
{
	music_off ();
	deff_start_sync (DEFF_TZ_FLIPCODE_ENTRY);
	SECTION_VOIDCALL (__common__, initials_enter);
	SECTION_VOIDCALL (__common__, pin_enter);
	callset_invoke (check_tz_flipcode);
	task_exit ();
}

void tz_flipcode_entry_stop (void)
{
	SECTION_VOIDCALL (__common__, pin_stop);
	SECTION_VOIDCALL (__common__, initials_stop);
}


/* Abort flipcode entry if the ball leaves the plunger */
CALLSET_ENTRY (tz_flipcode, any_pf_switch)
{
	if (task_kill_gid (GID_TZ_FLIPCODE_RUNNING))
	{
		tz_flipcode_entry_stop ();	
	}
}

CALLSET_ENTRY (tz_flipcode, start_game)
{
	flipcode_used = FALSE;
}

CALLSET_ENTRY (tz_flipcode, machine_paused)
{
	/* Start TZ Flipcode entry if enabled 
	 * and all the conditions are met */
	if (!switch_poll (SW_LEFT_BUTTON) && !switch_poll (SW_RIGHT_BUTTON)
		&& feature_config.tz_flipcodes == YES
		&& system_config.tournament_mode != YES
		&& !valid_playfield)
	{	
		task_create_gid (GID_TZ_FLIPCODE_RUNNING, tz_flipcode_running);
	}
}

CALLSET_ENTRY (tz_flipcode, machine_unpaused)
{
	if (task_kill_gid (GID_TZ_FLIPCODE_RUNNING))
	{
		tz_flipcode_entry_stop ();
	}
}
