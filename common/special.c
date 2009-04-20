/*
 * Copyright 2007, 2008, 2009 by Brian Dominy <brian@oddchange.com>
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
#include <knocker.h>
#include <coin.h>

/**
 * \file
 * \brief Common logic for dealing with specials.
 *
 * This module implements the standard logic for awarding specials.
 */


void special_award (void)
{
	callset_invoke (special_award);
	switch (system_config.special_award)
	{
		case FREE_AWARD_CREDIT:
			add_credit ();
			break;

		case FREE_AWARD_EB:
			increment_extra_balls ();
			break;

		case FREE_AWARD_OFF:
			break;
	}

#ifdef DEFF_SPECIAL
	deff_start (DEFF_SPECIAL);
#endif
#ifdef LEFF_SPECIAL
	leff_start (LEFF_SPECIAL);
#endif
	knocker_fire ();
	audit_increment (&system_audits.specials);
}

