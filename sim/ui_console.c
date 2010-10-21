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

/* \file ui_console.c
 * \brief A dumb console-based UI for the built-in WPC simulator.
 */

void ui_print_command (const char *cmdline)
{
}

void ui_console_render_string (const char *buffer)
{
	printf ("[STR] %s\n", buffer);
}

void ui_write_debug (enum sim_log_class c, const char *buffer)
{
	if (c == SLC_DEBUG_PORT)
		printf ("[PROG] %s", buffer);
	else
		printf ("[SIM]  %s", buffer);
	printf ("\n");
}

void ui_write_solenoid (int solno, int on_flag)
{
	printf ("[SIM] Sol %d is %s\n", solno, on_flag ? "on" : "off");
}

void ui_write_lamp (int lampno, int on_flag)
{
}

void ui_write_triac (int triacno, int on_flag)
{
	printf ("[SIM] Triac %d is %s\n", triacno, on_flag ? "on" : "off");
}

void ui_write_switch (int switchno, int on_flag)
{
	printf ("[SIM] Switch %d is %s\n", switchno, on_flag ? "active" : "inactive");
}

void ui_write_sound_command (unsigned int x)
{
	printf ("[SIM] Sound board command 0x%X\n", x);
}

void ui_write_sound_reset (void)
{
	printf ("[SIM] Sound board reset.\n");
}

void ui_write_task (int taskno, int gid)
{
}

#if (MACHINE_DMD == 1)
void ui_refresh_asciidmd (unsigned char *data)
{
}
#else
void ui_refresh_display (unsigned int x, unsigned int y, char c)
{
}
#endif

void ui_update_ball_tracker (unsigned int ballno, const char *location)
{
	printf ("[SIM] Ball %d is at %s\n", ballno, location);
}

void ui_init (void)
{
}

void ui_exit (void)
{
}

