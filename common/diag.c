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


void
diag_message_start (void)
{
	dmd_alloc_low_high ();
	dmd_clean_page_low ();
}


void
diag_message_flash (void)
{
	U8 n;
	dmd_clean_page_high ();
	for (n = 0; n < 5; n++)
	{
		task_sleep (TIME_200MS);
		dmd_show_high ();
		task_sleep (TIME_100MS);
		dmd_show_low ();
		sound_send (SND_TEST_ALERT);
	}
	task_sleep_sec (1);
	barrier ();
}


void
diag_message_scroll (void)
{
	dmd_sched_transition (&trans_scroll_left);
	dmd_show_low ();
	task_sleep_sec (3);
	barrier ();
}


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
diag_post_error (const char *message1, const char *message2)
{
	diag_error_count++;
	if (diag_announce_flag)
	{
		if (diag_error_count == 1)
		{
			diag_message_start ();
			font_render_string_center (&font_mono5, 64, 6, "TEST REPORT...");
			diag_message_flash ();
		}
		diag_message_start ();
		if (message2)
		{
			font_render_string_center (&font_mono5, 64, 10, message1);
			font_render_string_center (&font_mono5, 64, 20, message2);
		}
		else
		{
			font_render_string_center (&font_mono5, 64, 16, message1);
		}
		diag_message_scroll ();
	}
}


/**
 * Go through all of the diags.
 */
void
diag_run_task (void)
{
	dbprintf ("Running diags...");
	diag_error_count = 0;
	callset_invoke (diagnostic_check);
	dbprintf ("%d errors.\n", diag_error_count);

	if ((diag_announce_flag == 0) && (diag_error_count > 0))
	{
		diag_message_start ();
		font_render_string_center (&font_mono5, 64, 10, "PRESS ENTER");
		font_render_string_center (&font_mono5, 64, 21, "FOR TEST REPORT");
		diag_message_flash ();
	}

	task_exit ();
}


/**
 * Rerun all of the diagnostic tests.
 */
static void
diag_run (void)
{
	/* Create in a separate task context, to avoid
	 * stack overflow problems. */
	task_pid_t tp = task_recreate_gid (GID_DIAG_RUNNING, diag_run_task);
	while (task_find_gid (GID_DIAG_RUNNING))
		task_sleep (TIME_100MS);
	barrier ();
}


/**
 * At the end of initialization, run the diagnostics
 * and announce on one screen if there are any errors.
 */
CALLSET_ENTRY (diag, init_complete)
{
	diag_announce_flag = 0;
	diag_run ();
}


/**
 * On entry to test mode, run the diagnostics and
 * read the errors one-by-one.
 */
CALLSET_ENTRY (diag, test_start)
{
	diag_announce_flag = 1;
	diag_run ();
}

