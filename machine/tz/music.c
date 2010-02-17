/*
 * Copyright 2006, 2007, 2008 by Brian Dominy <brian@oddchange.com>
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
/* TODO Use EB button to change music */
#include <freewpc.h>
/* Plunger is always -1 */
struct music_selections {
	MUS_TZ_IN_PLAY
	MUS_POWERBALL_PLUNGER, MUS_POWERBALL_IN_PLAY
	MUS_MULTIBALL_LIT_PLUNGER, MUS_MULTIBALL_LIT

void select_music (void)
{
	while (!multi_ball_play ()&& switch_poll_logical (SW_SHOOTER))
	{
		deff_start (DEFF_SELECT_MUSIC);
		//start task to look at buttons
		//chnage 
	}
}
