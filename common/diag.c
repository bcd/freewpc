/*
 * Copyright 2006, 2007, 2008 by Brian Dominy <brian@oddchange.com>
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

/**
 * \file
 * \brief Hardware diagnostic checks.
 * Failures detected by this module result in the "credit dot".
 */

#include <freewpc.h>
#include <diag.h>


U8 diag_error_count;

/** Nonzero if diagnostic errors should be announced
 * as they are found */
U8 diag_announce_flag;


/**
 * Return the number of diagnostic errors.
 */
U8
diag_get_error_count (void)
{
	return diag_error_count;
}


/**
 * Called by a diagnostic to report an error.
 * messenger is a callback function for rendering
 * the text of the error message.
 */
void
diag_post_error (void_function messenger)
{
	diag_error_count++;
	if (diag_announce_flag && messenger)
	{
		if (diag_error_count == 1)
		{
		}
		dmd_alloc_low_clean ();
		messenger ();
		dmd_show_low ();
		task_sleep_sec (3);
	}
}


/**
 * Rerun all of the diagnostic tests.
 */
void
diag_run (void)
{
	/* Reset the error count */
	diag_error_count = 0;

	/* Invoke each of the diagnostics */
	callset_invoke (diagnostic_check);
}


CALLSET_ENTRY (diag, init_complete)
{
	diag_announce_flag = 0;
	diag_run ();
	if (diag_error_count > 0)
	{
		U8 n;
		dmd_alloc_low_high ();
		dmd_clean_page_low ();
		dmd_clean_page_high ();
		font_render_string_center (&font_mono5, 64, 10, "PRESS ENTER");
		font_render_string_center (&font_mono5, 64, 21, "FOR TEST REPORT");
		for (n = 0; n < 5; n++)
		{
			task_sleep (TIME_200MS);
			dmd_show_high ();
			task_sleep (TIME_200MS);
			dmd_show_low ();
			sound_send (SND_TEST_ALERT);
		}
		task_sleep_sec (1);
	}
}


CALLSET_ENTRY (diag, test_start)
{
	diag_announce_flag = 1;
	diag_run ();
}


#if 0

void diag_test_error (void)
{
	sprintf ("SAMPLE ERROR");
	font_render_string_center (&font_mono5, 64, 16, sprintf_buffer);
}

CALLSETX_ENTRY (diag, diagnostic_check)
{
	diag_post_error (diag_test_error);
}

#endif

