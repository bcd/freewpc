
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
 * \brief Handle the service button switches inside the coin door.
 */

extern __test__ void test_escape_button (void);
extern __test__ void test_down_button (void);
extern __test__ void test_up_button (void);
extern __test__ void test_enter_button (void);


void sw_escape_button_handler (void)
{
	if (!in_test)
	{
		add_credit ();
		audit_increment (&system_audits.service_credits);
	}
	else
		test_escape_button ();
}

void sw_down_button_handler (void)
{
	test_down_button ();

	if (!in_test)
		volume_down ();
}

void sw_up_button_handler (void)
{
	test_up_button ();
	
	if (!in_test)
		volume_up ();
}

void sw_enter_button_handler (void)
{
	test_enter_button ();
}


void sw_coin_door_closed_handler (void)
{
	dbprintf ("Coin door closed");
}


/* Declare switch drivers for the coin door & its buttons */

DECLARE_SWITCH_DRIVER (sw_coin_door_closed)
{
	.fn = sw_coin_door_closed_handler,
};

DECLARE_SWITCH_DRIVER (sw_escape_button)
{
	.fn = sw_escape_button_handler,
	.flags = SW_IN_TEST,
};

DECLARE_SWITCH_DRIVER (sw_down_button)
{
	.fn = sw_down_button_handler,
	.flags = SW_IN_TEST,
};

DECLARE_SWITCH_DRIVER (sw_up_button)
{
	.fn = sw_up_button_handler,
	.flags = SW_IN_TEST,
};

DECLARE_SWITCH_DRIVER (sw_enter_button)
{
	.fn = sw_enter_button_handler,
	.flags = SW_IN_TEST,
};

