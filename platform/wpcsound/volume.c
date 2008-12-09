/*
 * Copyright 2008 by Brian Dominy <brian@oddchange.com>
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

U8 current_hw_volume;


void increase_volume (void)
{
	if (current_hw_volume >= EPOT_MAX)
		return;
	writeb (WPCS_EPOT, EPOT_LOUDER | EPOT_CLOCK);
	noop ();
	noop ();
	noop ();
	noop ();
	writeb (WPCS_EPOT, EPOT_LOUDER);
	current_hw_volume++;
}


void decrease_volume (void)
{
	if (current_hw_volume == 0)
		return;
	writeb (WPCS_EPOT, EPOT_LOUDER | EPOT_CLOCK);
	noop ();
	noop ();
	noop ();
	noop ();
	writeb (WPCS_EPOT, EPOT_LOUDER);
	current_hw_volume--;
}


void volume_off (void)
{
	U8 n;

	current_hw_volume = 200;
	for (n = 0; n < 200; n++)
		decrease_volume ();
	current_hw_volume = 0;
}


/** Translates a CPU volume value into a value for the
 * hardware. */
U8 translate_volume (U8 cpu_volume)
{
	return cpu_volume;
}


/** Change the volume at the e-pot.  new_volume is
given in terms of CPU units. */
void volume_set (U8 new_volume)
{
	new_volume = translate_volume (new_volume);
	while (new_volume > current_hw_volume)
		increase_volume ();
	while (new_volume < current_hw_volume)
		decrease_volume ();
}


void volume_init (void)
{
	volume_off ();
	volume_set (100);
}

