/*
 * Copyright 2006, 2007 by Brian Dominy <brian@oddchange.com>
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
#include <test.h>

/**
 * \file
 * \brief Handle the service button switches inside the coin door.
 */

CALLSET_ENTRY (service, sw_escape)
{
	if (!in_test)
	{
		add_credit ();
		audit_increment (&system_audits.service_credits);
	}
}

CALLSET_ENTRY (service, sw_down)
{
	test_down_button ();

	if (!in_test)
		callset_invoke_held (SW_DOWN, TIME_500MS, TIME_100MS, volume_down);
}

CALLSET_ENTRY (service, sw_up)
{
	test_up_button ();
	
	if (!in_test)
		callset_invoke_held (SW_UP, TIME_500MS, TIME_100MS, volume_up);
}


CALLSET_ENTRY (service, sw_coin_door_closed)
{
	if (switch_poll_logical (SW_COIN_DOOR_CLOSED))
	{
		/* Coin door is closed */
	}
	else
	{
		/* Coin door is opened */
	}
}

