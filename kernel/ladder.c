/*
 * Copyright 2009 by Brian Dominy <brian@oddchange.com>
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

/**
 * \file
 * \brief Score ladder logic
 */

#include <freewpc.h>


void generic_ladder_reset (const struct generic_ladder *ladder)
{
	*(ladder->level) = 0;
}


void generic_ladder_advance (const struct generic_ladder *ladder)
{
	if (*(ladder->level) < ladder->size)
		(*(ladder->level))++;
}


void generic_ladder_score (const struct generic_ladder *ladder)
{
	score (ladder->scores[*(ladder->level)]);
}


void generic_ladder_score_and_advance (const struct generic_ladder *ladder)
{
	if (*(ladder->level) < ladder->size)
	{
		generic_ladder_score (ladder);
		generic_ladder_advance (ladder);
	}
}


void fixed_ladder_reset (const struct fixed_ladder *ladder)
{
	score_copy (ladder->current, ladder->base);
}

void fixed_ladder_advance (const struct fixed_ladder *ladder)
{
	if (score_compare (ladder->current, ladder->max) == -1)
		score_add (ladder->current, ladder->increment);
}

void fixed_ladder_scorex (const struct fixed_ladder *ladder, U8 multiplier)
{
	score_long_multiple (ladder->current, multiplier);
}

void fixed_ladder_score (const struct fixed_ladder *ladder)
{
	fixed_ladder_scorex (ladder, 1);
}

void fixed_ladder_score_and_advance (const struct fixed_ladder *ladder)
{
	fixed_ladder_score (ladder);
	fixed_ladder_advance (ladder);
}

void fixed_ladder_scorex_and_advance (const struct fixed_ladder *ladder, U8 multiplier)
{
	fixed_ladder_scorex (ladder, multiplier);
	fixed_ladder_advance (ladder);
}

