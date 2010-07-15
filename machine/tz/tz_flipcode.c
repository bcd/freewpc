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

extern void intials_stop (void);
extern void pin_stop (void);

U8 tz_flipcode_number;

#define NUM_TZ_FLIPCODES 6
const char *tz_flipcodes[] = {
	"BCD",
	"FEK",
	"PUK",
	"MET",
	"SAM",
	"SUN",
};

/* No particular reason why the PIN's are like this
 * I just think it looks pleasing */
const char *tz_flipcode_pins[] = {
	"1234",
	"2345",
	"3456",
	"4567",
	"5678",
	"6789",
};

const char *tz_flipcode_text[] = {
	"THE WIZARD",
	"WELCOME BACK",
	"EXTRA BALL LIT",
	"GET BACK TO IRC",
	"MAX POWER",
	"LIKE THE WEATHER",
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
	dmd_alloc_pair ();
	frame_draw (IMG_COW);
	sprintf ("HI %s", initials_data);
	font_render_string_center (&font_times10, 40, 11, sprintf_buffer);
	font_render_string_center (&font_var5, 40, 24, tz_flipcode_text[tz_flipcode_number]);
	
	dmd_show2 ();
	task_sleep_sec (3);
	deff_exit ();
}

void tz_flipcode_entry_deff (void)
{
	dmd_alloc_pair_clean ();
	font_render_string_center (&font_var5, 40, 11, "THE POWER");
	font_render_string_center (&font_var5, 40, 22, "SAYS ...");
	dmd_show2 ();
	sound_send (SND_THUNDER1);
	task_sleep_sec (2);
	deff_exit ();
}

CALLSET_ENTRY (tz_flipcode, check_tz_flipcode)
{
	U8 i;
	for (i = 0; i < NUM_TZ_FLIPCODES; i++)
	{
		if (strcmp(tz_flipcodes[i], initials_data) == 0
			&& strcmp(tz_flipcode_pins[i], pin_data) == 0)
		{
			switch (i)
			{
				/* BCD */
				case 0:
					sound_send (SND_HEY_ITS_ONLY_PINBALL);
					break;
				/* FEK */
				case 1:
					callset_invoke (door_start_clock_chaos);
					sound_send (SND_TIME_IS_A_ONEWAY_STREET);
					break;
				/* PUK */
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
			}
			/* Store for deff */
			tz_flipcode_number = i;
			deff_start (DEFF_TZ_FLIPCODE_ENTERED);
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
	music_set (MUS_TZ_IN_PLAY);
	task_exit ();
}

/* Abort flipcode entry if the ball leaves the plunger */
CALLSET_ENTRY (tz_flipcode, any_pf_switch)
{
	if (task_kill_gid (GID_TZ_FLIPCODE_RUNNING))
	{
		callset_invoke (tz_flipcode_entry_stop);	
	}
}

CALLSET_ENTRY (tz_flipcode, tz_flipcode_entry)
{
	task_create_gid (GID_TZ_FLIPCODE_RUNNING, tz_flipcode_running);
}

CALLSET_ENTRY (tz_flipcode, tz_flipcode_entry_stop)
{
	SECTION_VOIDCALL (__common__, pin_stop);
	SECTION_VOIDCALL (__common__, initials_stop);
}
