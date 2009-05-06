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

U8 ramp_divertor_prio;

void ramp_divertor_start (void)
{
}

void ramp_divertor_stop (void)
{
}

void ramp_divertor_error (void)
{
}

void ramp_divertor_update (void)
{
	global_flag_off (GLOBAL_FLAG_RAMP_DIVERTING);
	if (in_live_game)
	{
		ramp_divertor_prio = 0;
		callset_invoke (ramp_divertor_update);
	}
	if (global_flag_test (GLOBAL_FLAG_RAMP_DIVERTING))
		ramp_divertor_start ();
	else
		ramp_divertor_stop ();
}

bool ramp_divertor_alloc (U8 prio)
{
	if (prio > ramp_divertor_prio)
	{
		ramp_divertor_prio = prio;
		return TRUE;
	}
	return FALSE;
}

CALLSET_ENTRY (rampdiv, idle_every_second)
{
	/* The ramp divertor update runs every 1 second, so the low-level
	driver only needs to keep the coil on for about that long.  That way
	if scheduling fails the coil will automatically shut off */
	ramp_divertor_update ();
}

CALLSET_ENTRY (rampdiv, sw_l_ramp_diverted)
{
	/* This switch should only close if the global flag is ON. */
	if (!global_flag_test (GLOBAL_FLAG_RAMP_DIVERTING))
	{
		ramp_divertor_error ();
	}
}

CALLSET_ENTRY (rampdiv, sw_l_ramp_exit)
{
	/* This switch should only close if the global flag is OFF. */
	if (global_flag_test (GLOBAL_FLAG_RAMP_DIVERTING))
	{
		ramp_divertor_error ();
	}
}

CALLSET_ENTRY (rampdiv, init_complete)
{
	global_flag_off (GLOBAL_FLAG_RAMP_DIVERTING);
}

