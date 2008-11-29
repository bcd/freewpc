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

#include <freewpc.h>

#ifndef MACHINE_BALL_IN_PLAY_MUSIC
#define MACHINE_BALL_IN_PLAY_MUSIC 2
#endif

#ifndef MACHINE_START_BALL_MUSIC
#define MACHINE_START_BALL_MUSIC 3
#endif


CALLSET_ENTRY (audio, music_refresh)
{
	if (!in_live_game || in_bonus)
		return;
	else if (valid_playfield)
		music_request (MACHINE_BALL_IN_PLAY_MUSIC, PRI_SCORES);
	else
		music_request (MACHINE_START_BALL_MUSIC, PRI_SCORES);
}


CALLSET_ENTRY (audio, bonus)
{
	music_refresh ();
}


CALLSET_ENTRY (audio, end_ball)
{
	music_refresh ();
}


CALLSET_ENTRY (audio, end_game)
{
	music_refresh ();
	if (!in_test)
	{
		/* TODO - start timed with fade out */
#ifdef MACHINE_END_GAME_MUSIC
		music_set (MACHINE_END_GAME_MUSIC);
#endif
	}
}


CALLSET_ENTRY(audio, start_game)
{
#ifdef MACHINE_START_GAME_SOUND
	sound_send (MACHINE_START_GAME_SOUND);
#endif
}

