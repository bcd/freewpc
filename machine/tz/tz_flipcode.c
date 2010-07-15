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

/* CALLSET_SECTION ( tz_flipcode, __machine3__ ) */
#include <freewpc.h>
#include <eb.h>

extern char initials_data[3];

U8 tz_flipcode_number;

#define NUM_TZ_FLIPCODES 5
const char *tz_flipcodes[] = {
	"BCD",
	"FEK",
	"PUK",
	"MET",
	"SAM",
};

const char *tz_flipcode_text[] = {
	"THE WIZARD",
	"WELCOME BACK",
	"EXTRA BALL LIT",
	"GET BACK TO IRC",
	"MAX POWER",
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
	dmd_alloc_pair ();
	frame_draw (IMG_COW);
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
		if (strcmp((tz_flipcodes[i]), initials_data) == 0)
		{
			switch (i)
			{
				case 0:
					sound_send (SND_HEY_ITS_ONLY_PINBALL);
					break;
				case 1:
					callset_invoke (door_start_clock_chaos);
					sound_send (SND_TIME_IS_A_ONEWAY_STREET);
					break;
				case 2:
					light_easy_extra_ball ();
					sound_send (SND_YOU_UNLOCK_THIS_DOOR);
					break;
				case 3:
					sound_send (SND_WELCOME_RACE_FANS);
					break;
				case 4:
					callset_invoke (door_start_clock_chaos);
					sound_send (SND_WELCOME_RACE_FANS);
					break;
			}
			/* Store for deff */
			tz_flipcode_number = i;
			deff_start (DEFF_TZ_FLIPCODE_ENTERED);
		}
	}
}

CALLSET_ENTRY (tz_flipcode, tz_flipcode_entry)
{
	deff_start_sync (DEFF_TZ_FLIPCODE_ENTRY);
	SECTION_VOIDCALL (__common__, initials_enter);
	callset_invoke (check_tz_flipcode);
}

CALLSET_ENTRY (tz_flipcode, tz_flipcode_entry_stop)
{
	SECTION_VOIDCALL (__common__, initials_stop);
}
