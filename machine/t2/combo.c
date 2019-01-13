/*
 * Copyright 2017 by Noah Luskey <LuskeyNoah@gmail.com>
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

/* Combos - Handles lighting the shots that can be done as a combo and
   awarding points. 
   TODO: Light effects for Combos
   TODO: Display effects for making Combos
   TODO: Scoring
   TODO: Generate events on made Combos?
   TODO: figure out more todos
*/

#include <freewpc.h>

typedef enum {
	SHOT_DATABASE=0,    /* furthest left little hole thing */
	SHOT_ESCAPE_ROUTE,  /* left loop */
	SHOT_AI_RAMP,       /* left ramp */
	SHOT_SKULL,         /* skull, obviously */
	SHOT_COMMAND_RAMP,  /* right ramp */
	SHOT_CHASE_LOOP,    /* right loop */
} previous_shot_t;

#define COMBO_TIME_LIMIT TIME_4S


CALLSET_ENTRY (combo, start_ball)
{
}


