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

/**
 * \file
 * \brief Hardware diagnostic checks.
 * Failures detected by this module result in the "credit dot".
 */

#include <freewpc.h>
#include <diag.h>


/** The number of diagnostic errors that have been detected */
U8 diag_error_count;

/** Nonzero if diagnostic errors should be announced
 * as they are found */
U8 diag_announce_flag;

#ifdef CONFIG_DMD_OR_ALPHA
static void
diag_message_start (void)
{
	dmd_alloc_pair ();
	dmd_clean_page_low ();
}


static void
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


static void
diag_message_scroll (void)
{
	dmd_sched_transition (&trans_scroll_left);
	dmd_show_low ();
	task_sleep_sec (5);
	barrier ();
}
#endif


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
 * message is a string to be reported when announcements
 * are enabled.
 */
void
diag_post_error (char *message, U8 page)
{
	/* Increment total error count */
	diag_error_count++;

#ifdef CONFIG_DMD_OR_ALPHA
	/* If announcements are on, then write a message.
	If it is the first such error detected, then print the
	initial "TEST REPORT" message too. */
	if (diag_announce_flag)
	{
		if (diag_error_count == 1)
		{
			diag_message_start ();
			font_render_string_center (&font_mono5, 64, 6, "TEST REPORT...");
			diag_message_flash ();
		}

		message_write (message, page);
		diag_message_scroll ();
	}
#endif
}


/**
 * Run each of the diagnostic checks, tallying up the number
 * of errors that are found.
 */
void
diag_run_task (void)
{
	task_sleep (TIME_100MS);
	dbprintf ("Running diags...");
	diag_error_count = 0;
	callset_invoke (diagnostic_check);
	dbprintf ("%d errors.\n", diag_error_count);
	task_exit ();
}


/**
 * Announce if there are diagnostic errors.  This is called at the
 * end of the system reset message, just before entering
 * attract mode.
 */
void
diag_announce_if_errors (void)
{
	if (diag_error_count > 0)
	{
#ifdef CONFIG_DMD_OR_ALPHA
		diag_message_start ();
		font_render_string_center (&font_mono5, 64, 10, "PRESS ENTER");
		font_render_string_center (&font_mono5, 64, 21, "FOR TEST REPORT");
		diag_message_flash ();
#endif
	}
}


/**
 * Rerun all of the diagnostic tests.
 */
static void
diag_run (void)
{
	/* Create in a separate task context, to avoid
	 * stack overflow problems. */
	task_recreate_gid (GID_DIAG_RUNNING, diag_run_task);
	while (task_find_gid (GID_DIAG_RUNNING))
		task_sleep (TIME_100MS);
	barrier ();
}


/**
 * At the end of initialization, and whenever attract mode is restarted
 * due to game end or test mode exit, run the diagnostics and
 * determine if there are errors, but don't announce
 * anything.
 */
CALLSET_ENTRY (diag, amode_start)
{
	diag_announce_flag = 0;
	diag_run ();
}


/**
 * On entry to test mode, run the diagnostics and
 * announce each error one-by-one as it is detected.
 */
CALLSET_ENTRY (diag, test_start)
{
	diag_announce_flag = 1;
	diag_run ();
}

