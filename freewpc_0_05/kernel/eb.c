
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
 * This module implements the standard state machine of a pinball game.
 */


/** Defined in kernel/game.c */
U8 extra_balls;


static void update_extra_ball_lamp (void)
{
#ifdef MACHINE_EXTRA_BALL_LAMP
#endif /* MACHINE_EXTRA_BALL_LAMP */
}


void increment_extra_balls (void)
{
	if (extra_balls < system_config.max_ebs_per_bip)
	{
		extra_balls++;
		update_extra_ball_lamp ();
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
		return (FALSE);
}


void clear_extra_balls (void)
{
	extra_balls = 0;
}

