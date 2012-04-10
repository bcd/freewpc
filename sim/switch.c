/*
 * Copyright 2008-2011 by Brian Dominy <brian@oddchange.com>
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
#include <simulation.h>

int show_switch_levels = 0;

int sim_no_switch_power = 0;
int sim_no_opto_power = 0;

/** The simulated switch matrix inputs.  This acts as a buffer between the
 * simulation code and the actual product code, but it serves the same
 * purpose.  Only one matrix is needed, however.
 */
U8 sim_switch_matrix[SWITCH_BITS_SIZE+1];



U8 *sim_switch_matrix_get (void)
{
	return sim_switch_matrix;
}


static void sim_switch_update (int sw)
{
	U8 level = sim_switch_matrix[sw/8] & (1 << (sw%8));

	/* Redraw the switch */
	if (show_switch_levels)
		ui_write_switch (sw, level);
	else
		ui_write_switch (sw, level ^ switch_is_opto (sw));

	/* Update the signal tracker */
	signal_update (SIGNO_SWITCH + sw, !!level);
}


void sim_switch_toggle (int sw)
{
	if (sim_no_switch_power)
		return;
	if (sim_no_opto_power && switch_is_opto (sw))
		return;

	sim_switch_matrix[sw / 8] ^= (1 << (sw % 8));
	sim_switch_update (sw);
}

void sim_switch_set (int sw, int on)
{
	if (sim_no_switch_power)
		return;
	if (sim_no_opto_power && switch_is_opto (sw))
		return;

	if (switch_is_opto (sw))
		on = !on;
	if (on)
		sim_switch_matrix[sw / 8] |= (1 << (sw % 8));
	else
		sim_switch_matrix[sw / 8] &= ~(1 << (sw % 8));
	sim_switch_update (sw);
}

unsigned int sim_switch_timer;

int sim_switch_read (int sw)
{
	return sim_switch_matrix[sw/8] & (1 << (sw%8));
}

void sim_switch_finish (int sw)
{
	if (--sim_switch_timer == 0)
	{
		sim_switch_toggle (sw);
	}
	else
		sim_time_register (16, FALSE, (time_handler_t)sim_switch_finish, sw);
}

void sim_switch_depress (int sw)
{
	if (sim_switch_timer == 0)
	{
		sim_switch_toggle (sw);
		sim_switch_timer = 10;
		sim_time_register (16, FALSE, (time_handler_t)sim_switch_finish, sw);
	}
}


void flipper_button_depress (int sw)
{
	sim_switch_toggle (sw);
	task_sleep (TIME_33MS);
	sim_switch_toggle (sw+4);
	task_sleep (TIME_66MS);

	sim_switch_toggle (sw);
	task_sleep (TIME_33MS);
	sim_switch_toggle (sw+4);
	task_sleep (TIME_66MS);
}

void sim_switch_init (void)
{
	switchnum_t sw;

	/* Initialize switch levels to zero by default */
	memset (sim_switch_matrix, 0, SWITCH_BITS_SIZE);
	sim_switch_matrix[9] = 0xFF;

	conf_add ("sw.no_power", &sim_no_switch_power);
	conf_add ("sw.no_opto_power", &sim_no_opto_power);

	/* For any switches declared as an opto, set initial
	switch level to 1 */
	for (sw = 0; sw < NUM_SWITCHES; sw++)
		if (switch_is_opto (sw))
		{
			sim_switch_toggle (sw);
		}
		else
			sim_switch_update (sw);
	sim_switch_timer = 0;
}

