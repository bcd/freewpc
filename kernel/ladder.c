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

#include <freewpc.h>


/**
 * The generic ladder tracks an award whose values increase in
 * an irregular way.
 */
struct generic_ladder
{
	U8 *scores;
	U8 size;
	U8 *level;
};


/**
 * The fixed ladder tracks an award whose values increase by
 * fixed size steps.
 */
struct fixed_ladder
{
	score_t base;
	score_t increment;
	score_t max;
	score_t *current;
};



void generic_ladder_reset (struct generic_ladder *ladder)
{
	*(ladder->level) = 0;
}


void generic_ladder_score (struct generic_ladder *ladder)
{
	score (ladder->scores[*(ladder->level)]);
	(*(ladder->level))++;
}



void fixed_ladder_reset (struct fixed_ladder *ladder)
{
	score_copy (ladder->current, ladder->base);
}

void fixed_ladder_score_multiple (struct fixed_ladder *ladder, U8 multiplier)
{
	score_long_multiple (ladder->current, multiplier);
	if (score_compare (ladder->current, ladder->max) == -1)
		score_add (ladder->current, ladder->increment);
}

void fixed_ladder_score (struct fixed_ladder *ladder)
{
	fixed_ladder_score_multiple (ladder, 1);
}

