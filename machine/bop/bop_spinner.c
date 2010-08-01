/*
 * Copyright 2010 by Ewan Meadows (sonny_jim@hotmail.com)
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

//sound_code_t spinner_sounds[] = {
//	SND_SPINNER1, SND_SPINNER2, SND_SPINNER3, SND_SPINNER3
//};

score_t spinner_total;

CALLSET_ENTRY (bop_spinner, sw_spinner_slow)
{
	score (SC_1K);
	score_add (spinner_total, score_table[SC_1K]);
	sound_send (SND_SPINNER1 + random_scaled (4));
}

CALLSET_ENTRY (bop_spinner, start_ball)
{
	score_zero (spinner_total);
}
