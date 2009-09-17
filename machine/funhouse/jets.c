/*
 * Copyright 2009 by Brian Dominy <brian@oddchange.com>
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

void jets_max (void)
{
	lamplist_apply (LAMPLIST_JETS, lamp_flash_on);
	lamplist_apply (LAMPLIST_JETS, lamp_off);
}

void jets_advance (void)
{
}

void jet_award (U8 lamp)
{
	if (lamp_flash_test (lamp))
	{
	}
	else if (lamp_test (lamp))
	{
	}
	else
	{
	}
}

CALLSET_ENTRY (jets, any_jet)
{
	lamplist_rotate_next (LAMPLIST_JETS, lamp_matrix);
	lamplist_rotate_next (LAMPLIST_JETS, lamp_flash_matrix);
}

CALLSET_ENTRY (jets, start_ball)
{
	lamplist_apply (LAMPLIST_JETS, lamp_flash_off);
	lamplist_apply (LAMPLIST_JETS, lamp_off);
	lamp_on (LM_UPPER_LEFT_JET);
}

