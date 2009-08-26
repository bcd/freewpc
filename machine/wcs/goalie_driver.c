/*
 * Copyright 2008, 2009 by Brian Dominy <brian@oddchange.com>
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
#include <diag.h>
#include <goalie.h>

typedef enum
{
	GOALIE_LOST,
	GOALIE_LEFT_TO_RIGHT,
	GOALIE_RIGHT_TO_LEFT,
} goalie_position_t;

goalie_position_t goalie_position;

#define GOALIE_NOT_MOVING    0x0
#define GOALIE_TARGET_MASK   0x0F
#define GOALIE_MOVING        0x10
#define GOALIE_TARGETING     0x20
#define GOALIE_CALIBRATING   0x40
#define GOALIE_EVIL          0x80

U8 goalie_status;

U8 goalie_distance;

U8 goal_width;

#define GOAL_EDGE_LEFT_SEEN  0x1
#define GOAL_EDGE_RIGHT_SEEN 0x2

U8 goal_edges_seen;

U8 goalie_calibration_timer;

#define GOALIE_ERROR_LEFT_MARK   0x1
#define GOALIE_ERROR_RIGHT_MARK  0x2

U8 goalie_errors;

U8 goalie_alloc_prio;


void goalie_command (U8 command)
{
	goalie_status = command;
	if ((goalie_status & GOALIE_MOVING) &&
		 ((feature_config.disable_goalie == NO) || in_test))
	{
		goalie_start ();
	}
	else
	{
		goalie_stop ();
	}
}

void goalie_alloc (U8 prio)
{
}


void goalie_free (U8 prio)
{
}


void goalie_test_toggle (void)
{
	if (goalie_status & GOALIE_CALIBRATING)
		return;
	else if (goalie_status & GOALIE_MOVING)
		goalie_command (GOALIE_NOT_MOVING);
	else
		goalie_command (GOALIE_MOVING);
}

CALLSET_ENTRY (goalie_driver, sw_goalie_left)
{
	goal_edges_seen |= GOAL_EDGE_LEFT_SEEN;
	goalie_errors &= ~GOAL_EDGE_LEFT_SEEN;
	if (goalie_position == GOALIE_LEFT_TO_RIGHT)
	{
		/* TODO - right opto not seen.
		If this persists, we can take goalie_distance/2
		as the expected value to simulate it. */
	}
	goalie_position = GOALIE_LEFT_TO_RIGHT;
	goalie_distance = 0;
}

CALLSET_ENTRY (goalie_driver, sw_goalie_right)
{
	goal_edges_seen |= GOAL_EDGE_RIGHT_SEEN;
	goalie_errors &= ~GOAL_EDGE_RIGHT_SEEN;
	if (goalie_position == GOALIE_RIGHT_TO_LEFT)
	{
		/* TODO - left opto not seen. */
	}
	else
	{
		if (goalie_distance > goal_width)
			goal_width = goalie_distance;
	}
	goalie_position = GOALIE_RIGHT_TO_LEFT;
}


/**
 * The main driver for the goalie runs at task level every 100ms.
 * This is frequently enough to service it OK; interrupt level is
 * overkill.
 */
CALLSET_ENTRY (goalie_driver, idle_every_100ms)
{
	if (goalie_status & GOALIE_MOVING)
	{
		if (goalie_position & GOALIE_LEFT_TO_RIGHT)
		{
			if (goalie_distance < 250)
				goalie_distance++;
		}
		else if (goalie_distance > 0)
			goalie_distance--;

		if (unlikely (goalie_status & GOALIE_TARGETING))
		{
			if ((goalie_status & GOALIE_TARGET_MASK) == goalie_distance)
			{
				goalie_command (GOALIE_NOT_MOVING);
			}
		}
		else if (unlikely (goalie_status & GOALIE_CALIBRATING))
		{
			if (goal_edges_seen == GOAL_EDGE_LEFT_SEEN+GOAL_EDGE_RIGHT_SEEN)
			{
				goalie_command (GOALIE_NOT_MOVING);
			}
		}
	}
}

CALLSET_ENTRY (goalie_driver, idle_every_second)
{
	if (unlikely (goalie_status & GOALIE_CALIBRATING))
	{
		if (goal_edges_seen == GOAL_EDGE_LEFT_SEEN+GOAL_EDGE_RIGHT_SEEN)
		{
			/* Stop calibration successfully */
			goalie_command (GOALIE_NOT_MOVING);
		}
		else if (--goalie_calibration_timer == 0)
		{
			/* Stop calibration with failure */
			goalie_errors |= (~goal_edges_seen & 0x3);
			goalie_command (GOALIE_NOT_MOVING);
		}
	}
}

void goalie_recalibrate (void)
{
	goal_width = 7;
	goal_edges_seen = 0;
	goalie_errors = 0;
	goalie_position = GOALIE_LOST;
	goalie_calibration_timer = 5;
	goalie_alloc_prio = 0;
	goalie_command (GOALIE_MOVING | GOALIE_CALIBRATING);
}


void goalie_status_display (void)
{
	const char *s;

	font_render_string_center (&font_mono5, 64, 2, "GOALIE TEST");

	if (goalie_status & GOALIE_CALIBRATING)
		s = "CALIBRATING";
	else if (goalie_status & GOALIE_MOVING)
		s = "RUNNING";
	else
		s = "STOPPED";
	font_render_string_center (&font_var5, 64, 9, s);

	if (goalie_status & GOALIE_CALIBRATING)
	{
		sprintf ("%d SECONDS LEFT", goalie_calibration_timer);
		s = sprintf_buffer;
	}
	else if (goalie_errors == 0)
	{
		s = "OK";
	}
	else
	{
		sprintf ("ERROR %02X", goalie_errors);
		s = sprintf_buffer;
	}
	font_render_string_center (&font_var5, 64, 16, s);

	sprintf ("POS %d/%d", goalie_distance, goal_width);
	font_render_string_center (&font_mono5, 64, 24, sprintf_buffer);

	dmd_show_low ();
}


CALLSET_ENTRY (goalie_driver, init)
{
	goalie_status = GOALIE_NOT_MOVING;
}

CALLSET_ENTRY (goalie_driver, diagnostic_check)
{
	/* this should timeout */
	while (goalie_status & GOALIE_CALIBRATING)
		task_sleep (TIME_100MS);
	if (goalie_errors)
		diag_post_error ("GOALIE IS\nNOT WORKING\n", MACHINE_PAGE);
}


CALLSET_ENTRY (goalie_driver, init_complete)
{
	/* TODO - this should also be done after exiting test mode! */
	goalie_recalibrate ();
}

CALLSET_ENTRY (goalie_driver, start_ball)
{
	goalie_command (GOALIE_NOT_MOVING);
}

CALLSET_ENTRY (goalie_driver, end_ball)
{
	goalie_command (GOALIE_NOT_MOVING);
}

CALLSET_ENTRY (goalie_driver, end_game)
{
	goalie_command (GOALIE_NOT_MOVING);
}

CALLSET_ENTRY (goalie_driver, ball_search)
{
	if (!(goalie_status & GOALIE_MOVING))
	{
	}
}

CALLSET_ENTRY (goalie_logic, idle_every_second)
{
	if (in_live_game)
	{
		if (flag_test (FLAG_GOAL_LIT) || flag_test (FLAG_MULTIBALL_RUNNING))
		{
			goalie_command (GOALIE_MOVING);
			return;
		}
		else
		{
			goalie_command (GOALIE_NOT_MOVING);
		}
	}
}


