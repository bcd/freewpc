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

/* CALLSET_SECTION (birthday, __machine3__) */

/* Show messages during attract mode on certain dates */

#include <freewpc.h>
#include <amode.h>

#define NUM_BIRTHDAYS 8

const struct {
	const U8 day;
	const U8 month;
	const char *message_line1;
	const char *message_line2;
	const U8 background;
} birthday_msgs[] = {
	{ 3, 7, "HAPPY BIRTHDAY", "BCD", IMG_BIRTHDAY },
	{ 10, 10, "HAPPY BIRTHDAY", "FEK", IMG_BIRTHDAY },
	{ 25, 12, "HAPPY BIRTHDAY", "JESUS", IMG_BIRTHDAY },
	{ 5, 10, "HAPPY BIRTHDAY", "MALC", IMG_BIRTHDAY },
	{ 30, 10, "HAPPY", "HALLOWEEN", NULL },
	{ 4, 6, "HAPPY", "INDEPENDENCE DAY", NULL },
	{ 4, 11, "HAPPY BIRTHDAY", "JIM", IMG_BIRTHDAY },
	{ 1, 1, "HAPPY", "NEW YEAR", NULL },
};

extern U8 month;
extern U8 day;
extern bool amode_show_scores_long;

static void draw_birthday_msg (U8 birthday_msg)
{
	if (birthday_msg > NUM_BIRTHDAYS)
		birthday_msg = NUM_BIRTHDAYS;
	dmd_sched_transition (&trans_bitfade_fast);
	dmd_map_overlay ();
	dmd_clean_page_low ();
	
	sprintf ("%s", birthday_msgs[birthday_msg].message_line1);
	font_render_string_center (&font_fixed6, 64, 6, sprintf_buffer);
	sprintf ("%s", birthday_msgs[birthday_msg].message_line2);
	font_render_string_center (&font_fixed6, 64, 22, sprintf_buffer);
	
	dmd_text_outline ();
	dmd_alloc_pair ();
	if (birthday_msgs[birthday_msg].background != NULL)
	{
		frame_draw (birthday_msgs[birthday_msg].background);
		dmd_overlay_outline ();
		dmd_show2 ();
	}
	else
	{
		dmd_show_low ();
	}
		amode_sleep_sec (6);
}

void check_birthdays (void)
{
	/* Don't bother checking if the date hasn't been set */
	extern U8 year;
	if (year == 0)
		return;
	U8 i;
	for (i = 0; i < NUM_BIRTHDAYS; i++)
	{
		if (month == birthday_msgs[i].month && day == birthday_msgs[i].day)
		{
			draw_birthday_msg (i);
			/* Only show one message per date */
			return;
		}
	}
}

CALLSET_ENTRY (birthday, amode_page)
{
	if (amode_show_scores_long)
		return;
	check_birthdays ();
}
