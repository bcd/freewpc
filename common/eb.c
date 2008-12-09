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


/** Update the extra ball lamp to reflect the number of
collected extra balls by the current player up. */
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


/** Returns whether or not an extra ball can be awarded.  Game logic
can use this function to avoid lighting an extra ball if it will
be impossible to collect it. */
bool can_award_extra_ball (void)
{
	return ((extra_balls_earned < system_config.max_ebs)
		&& (extra_balls_earned_this_bip < system_config.max_ebs_per_bip));
}


/** Award an extra ball to the current player.  If an extra ball
cannot be awarded, then it is skipped. */
void increment_extra_balls (void)
{
	if (can_award_extra_ball ())
	{
		extra_balls++;
		update_extra_ball_lamp ();

		audit_increment (&system_audits.extra_balls_awarded);

		extra_balls_earned++;
		extra_balls_earned_this_bip++;
#ifdef DEFF_EXTRA_BALL
		deff_start (DEFF_EXTRA_BALL);
#endif
#ifdef LEFF_EXTRA_BALL
		leff_start (LEFF_EXTRA_BALL);
#endif
	}
}


/** Decreases the current player's extra ball at the end of
ball, if necessary.  Returns TRUE if the current player should
be kept, or FALSE if should advance to the next player/ball
number. */
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


CALLSET_ENTRY (extra_ball, start_player)
{
	extra_balls_earned = 0;
}


CALLSET_ENTRY (extra_ball, start_game)
{
	extra_balls = 0;
}

