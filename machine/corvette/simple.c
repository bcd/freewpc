/*
 * Copyright 2010 by Dominic Clifton <me@dominicclifton.name>
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
#include <loop_gate.h>
#include <diverter.h>
#include <zr_1_up_rev_gate.h>

CALLSET_ENTRY (simple, device_update) {
	if (flag_test (FLAG_LOOP_GATE_OPENED)) {
		loop_gate_start ();
	}
	else
	{
		loop_gate_stop ();
	}

	if (flag_test (FLAG_ZR_1_UP_REV_GATE_OPENED)) {
		zr_1_up_rev_gate_start ();
	}
	else
	{
		zr_1_up_rev_gate_stop ();
	}

}

CALLSET_ENTRY (simple, end_ball, tilt) {
	flag_off (FLAG_ZR_1_UP_REV_GATE_OPENED);
	flag_off (FLAG_LOOP_GATE_OPENED);
}

CALLSET_ENTRY (simple, start_ball) {
	flag_on (FLAG_ZR_1_UP_REV_GATE_OPENED);
}


/***
 * XXX diverter testing
 */
CALLSET_ENTRY (diverter_test, start_ball) {
	dbprintf ("diverter_test: start_ball\n");
	diverter_start();
}

CALLSET_ENTRY (diverter_test, end_ball) {
	dbprintf ("diverter_test: end_ball\n");
	diverter_stop();
}

