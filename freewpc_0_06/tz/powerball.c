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

/* Proximity switches will trigger whenever a steel ball passes over
 * them.  The powerball is detected by the lack of such closures.
 */


void sw_trough_prox_handler (void)
{
	/* The next ball to be served from the trough is
	 * definitely a steel ball. */
}


void sw_slot_prox_handler (void)
{
	/* If the ball came from the piano or camera, it must
	 * be steel.  Stop the detector task.
	 */
}


DECLARE_SWITCH_DRIVER (sw_trough_prox)
{
	.fn = sw_trough_prox_handler,
	.flags = SW_IN_GAME,
};


DECLARE_SWITCH_DRIVER (sw_slot_prox)
{
	.fn = sw_slot_prox_handler,
	.flags = SW_IN_GAME,
};

