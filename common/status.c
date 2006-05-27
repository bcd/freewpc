/*
 * Copyright 2006 by Brian Dominy <brian@oddchange.com>
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


void status_report_deff (void)
{
	dmd_alloc_low_clean ();
	dmd_draw_border (dmd_low_buffer);
	font_render_string_center (&font_fixed6, 64, 16, "STATUS REPORT");
	dmd_show_low ();
	task_sleep_sec (2);

	dmd_alloc_low_clean ();
	dmd_draw_border (dmd_low_buffer);
	sprintf ("BALL %d", ball_up);
	font_render_string_center (&font_mono5, 64, 11, sprintf_buffer);
	credits_render ();
	font_render_string_center (&font_mono5, 64, 21, sprintf_buffer);
	dmd_show_low ();
	task_sleep_sec (3);

	rtc_show_date_time ();
	dmd_draw_border (dmd_low_buffer);
	task_sleep_sec (3);

	dmd_alloc_low_clean ();
	dmd_draw_border (dmd_low_buffer);
	font_render_string_center (&font_mono5, 64, 11, "REPLAY AT");
	font_render_string_center (&font_mono5, 64, 21, "TBD");
	dmd_show_low ();
	task_sleep_sec (3);

#ifdef MACHINE_STATUS_REPORT
	MACHINE_STATUS_REPORT
#endif

	deff_exit ();
}


/** Task that is restarted anytime a flipper button is initially
 * pressed.  It polls the flipper switches continously to
 * see if it is being held down for the amount of time necessary
 * to start the status report.
 */
void status_report_monitor (void)
{
	/* A count of 50 equates to a 5s hold down period. */
	int count = 50;

	/* Wait until the player has kept the flipper button(s) down
	 * for an extended period of time.  Abort as soon as the
	 * flippers are released. */
	while (--count > 0)
	{
		if (!switch_poll_logical (SW_LL_FLIP_SW) 
			&& !switch_poll_logical (SW_LR_FLIP_SW))
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
		if (!switch_poll_logical (SW_LL_FLIP_SW) 
			&& !switch_poll_logical (SW_LR_FLIP_SW))
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

void status_report_check (void)
{
	task_create_gid1 (GID_STATUS_REPORT_MONITOR, status_report_monitor);
}
