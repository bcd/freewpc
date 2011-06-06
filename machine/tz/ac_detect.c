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

#include <freewpc.h>
#include <test.h>
#include <window.h>
#include <system/ac.h>

extern U8 ac_zerocross_errors;

const char *ac_type_text[] = {
	"US",
	"EXPORT",
};

bool ac_export;

void ac_detect_init (void)
{
	ac_export = TRUE;
}

void ac_detect_draw (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_mono5, 64, 2, "AC TEST");
	
	/* The zc_timer gets trigged twice every cycle, so it'll hover between
	 * 8-9ms for export and 7-8ms for US */
	if (zc_timer_stored == 9 )
		ac_export = TRUE;
	else if (zc_timer_stored == 7)
		ac_export = FALSE;

	font_render_string_center (&font_fixed10, 64, 14, ac_type_text[ac_export]);
	sprintf ("FREQ %dMS", zc_timer_stored);
	font_render_string_right (&font_mono5, 54, 22, sprintf_buffer);
	sprintf ("%d ERRS", ac_zerocross_errors);
	font_render_string_left (&font_mono5, 84, 22, sprintf_buffer);

	dmd_show_low ();
}

void ac_detect_thread (void)
{
	for (;;)
	{
		ac_detect_draw ();
		task_sleep (TIME_100MS);
	}
}

struct window_ops ac_detect_window = {
	DEFAULT_WINDOW,
	.draw = ac_detect_draw,
	.init = ac_detect_init,
	.thread = ac_detect_thread,
};

struct menu ac_detect_test_item = {
	.name = "AC TEST",
	.flags = M_ITEM,
	.var = { .subwindow = { &ac_detect_window, NULL } },
};
