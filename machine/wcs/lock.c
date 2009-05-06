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

void lock_magnet_enable (void)
{
	flag_on (FLAG_LOCK_MAGNET_ENABLE);
}

void lock_magnet_disable (void)
{
	flag_off (FLAG_LOCK_MAGNET_ENABLE);
}

CALLSET_ENTRY (lock_lane, sw_r_ramp_exit)
{
	if (in_live_game && flag_test (FLAG_LOCK_MAGNET_ENABLE))
	{
		/* start the magnet catch for X ms */
		sol_req_state (SOL_LOCK_MAGNET);

#ifdef BCD_WCS
		/* If the tail switch -- the next one that would
		 * become permanently closed should a ball enter
		 * the lock -- is not working, then consider this
		 * entry criteria for a virtual lock. */
		device_t *dev = device_entry (DEVNO_LOCK);
		if (device_empty_p (dev)) /* assume last switch broken */
		{
			device_add_virtual (dev);
		}
#endif
	}
}

CALLSET_ENTRY (lock_lane, dev_lock_enter)
{
	lock_magnet_disable ();
}


CALLSET_ENTRY (lock_lane, dev_lock_kick_attempt)
{
	free_timer_start (TIM_LOCK_KICKING, TIME_3S);
}


CALLSET_ENTRY (lock_lane, sw_left_flipper_lane)
{
#ifdef BCD_WCS
	/* If the head switch -- the one that would open
	 * when a ball leaves the lock -- is not working
	 * and a ball was just ejected, then consider this
	 * a successful attempt.
	 */
	device_t *dev = device_entry (DEVNO_LOCK);
	if (free_timer_test (TIM_LOCK_KICKING) && (dev->actual_count == 1))
		/* assume last switch broken */
	{
		free_timer_stop (TIM_LOCK_KICKING);
		device_remove_virtual (dev);
	}
#endif
}


CALLSET_ENTRY (lock_lane, start_game)
{
	lock_magnet_disable ();
}

CALLSET_ENTRY (lock_lane, end_game)
{
	lock_magnet_disable ();
}
