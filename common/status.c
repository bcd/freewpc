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

#include <freewpc.h>
#include <coin.h>
#include <replay.h>

/**
 * \file
 * \brief Implements the status report feature.
 * To activate status report, hold both flippers in while a game is in
 * progress.
 */


U8 status_report_cancel_delay;


#ifdef CONFIG_DMD_OR_ALPHA
void status_page_init (void)
{
	dmd_alloc_low_clean ();
	dmd_draw_border (dmd_low_buffer);
}


void status_page_complete (void)
{
	task_ticks_t timeout = TIME_3S / TIME_66MS;
	dmd_show_low ();
	status_report_cancel_delay = FALSE;
	while (!status_report_cancel_delay && (timeout > 0))
	{
		task_sleep (TIME_66MS);
		timeout--;
	}
}


void status_report_deff (void)
{
	status_page_init ();
	font_render_string_center (&font_fixed6, 64, 16, "STATUS REPORT");
	status_page_complete ();

	status_page_init ();
	sprintf ("BALL %d", ball_up);
	font_render_string_center (&font_mono5, 64, 11, sprintf_buffer);
	credits_render ();
	font_render_string_center (&font_mono5, 64, 21, sprintf_buffer);
	status_page_complete ();

	status_page_init ();
	replay_draw ();
	status_page_complete ();

#ifdef CONFIG_RTC
	rtc_show_date_time (&current_date);
	dmd_draw_border (dmd_low_buffer);
	status_page_complete ();
#endif

	/* Allow other modules to report status as well.
	 * The order is unspecified. */
	callset_invoke (status_report);

	deff_exit ();
}

#endif

/** Task that is restarted anytime a flipper button is initially
 * pressed.  It polls the flipper switches continously to
 * see if it is being held down for the amount of time necessary
 * to start the status report.
 */
void status_report_monitor (void)
{
	/* A count of 40 equates to a 4s hold down period. */
	U8 count = 40;

	/* Wait until the player has kept the flipper button(s) down
	 * for an extended period of time.  Abort as soon as the
	 * flippers are released. */
	while (--count > 0)
	{
		if (!switch_poll_logical (SW_LEFT_BUTTON)
			&& !switch_poll_logical (SW_RIGHT_BUTTON))
			goto done;
		task_sleep (TIME_100MS);
	}

	/* OK, start the status report deff */
start:
	deff_start (DEFF_STATUS_REPORT);

	/* Wait until the flippers are released, or the status report
	 * deff terminates.  If the flippers are released, then
	 * exit the task.  If the status report finishes, go back to
	 * the beginning -- after another 5sec, it will get started
	 * again. */
	for (;;)
	{
		if (!switch_poll_logical (SW_LEFT_BUTTON)
			&& !switch_poll_logical (SW_RIGHT_BUTTON))
			break;

		if (deff_get_active () != DEFF_STATUS_REPORT)
		{
			task_sleep_sec (5);
			goto start;
		}

		task_sleep (TIME_100MS);
	}

	/* Stop the status report */
	deff_stop (DEFF_STATUS_REPORT);

done:
	task_exit ();
}


static void status_report_check (void)
{
	if (task_find_gid (GID_STATUS_REPORT_MONITOR))
	{
		status_report_cancel_delay = TRUE;
	}
	else
		task_create_gid (GID_STATUS_REPORT_MONITOR, status_report_monitor);
}


CALLSET_ENTRY (status_report, sw_left_button, sw_right_button)
{
	if (in_live_game)
		status_report_check ();
}

