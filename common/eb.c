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

/**
 * \file
 * \brief Common logic for dealing with extra balls
 *
 * This module implements the standard logic for awarding extra balls.
 */


/** Number of extra balls earned */
U8 extra_balls;

/** Number of extra balls collected by this player */
__local__ U8 extra_balls_earned;

/** Number of extra balls collected on this ball in play */
U8 extra_balls_earned_this_bip;


static void update_extra_ball_lamp (void)
{
#ifdef MACHINE_EXTRA_BALL_LAMP
	if (extra_balls > 0)
	{
		lamp_tristate_on (MACHINE_EXTRA_BALL_LAMP);
	}
	else
	{
		lamp_tristate_off (MACHINE_EXTRA_BALL_LAMP);
	}
#endif /* MACHINE_EXTRA_BALL_LAMP */
}


void increment_extra_balls (void)
{
	if ((extra_balls_earned < system_config.max_ebs)
		&& (extra_balls_earned_this_bip < system_config.max_ebs_per_bip))
	{
		extra_balls++;
		update_extra_ball_lamp ();

		audit_increment (&system_audits.extra_balls_awarded);

		extra_balls_earned++;
		extra_balls_earned_this_bip++;
	}
}


bool decrement_extra_balls (void)
{
	if (extra_balls > 0)
	{
		extra_balls--;
		update_extra_ball_lamp ();
		return (TRUE);
	}
	else
	{
		extra_balls_earned_this_bip = 0;
		return (FALSE);
	}
}


CALLSET_ENTRY(extra_ball, start_player)
{
	extra_balls_earned = 0;
}


void clear_extra_balls (void)
{
	extra_balls = 0;
}

