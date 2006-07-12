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


void match_award (void)
{
	audit_increment (&system_audits.match_credits);
}

void match_deff (void)
{
}

void do_match (volatile bool will_match)
{
}

void match_start (void) 
{
	U8 match_flag;

	/* Nothing to do if match has been disabled */
	if (system_config.match_feature == 0)
		return;

	/* Randomly decide whether or not to award a match. */
	match_flag = random () >> 3; /* returns 0-31 */
	match_flag *= 3; /* now between 0-96, pretty close */
	if (match_flag <= system_config.match_feature)
	{
		/* Yep, will match */
		do_match (TRUE);
	}
	else
	{
		do_match (FALSE);
	}
}

