/*
 * Copyright 2010 by Brian Dominy <brian@oddchange.com>
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
#include <hwsim/sound-ext.h>

static void sound_ext_reset (void)
{
	ui_write_sound_reset ();
}

void sound_ext_command (U16 cmd)
{
	ui_write_sound_command (cmd);
}

static void sound_ext_write_data (U8 val)
{
	static int write_count = 0;
	static U16 cmd = 0;

	cmd <<= 8;
	cmd |= val;
	write_count++;

	if (write_count == 2)
	{
		sound_ext_command (cmd);
		cmd = 0;
		write_count = 0;
	}
}


U8 sound_ext_read (void *board, unsigned int regno)
{
	return 0;
}


void sound_ext_write (void *board, unsigned int regno, U8 val)
{
	switch (regno)
	{
		case SOUND_ADDR_DATA:
			sound_ext_write_data (val);
			break;
		case SOUND_ADDR_RESET_STATUS:
			sound_ext_reset ();
			break;
	}
}

