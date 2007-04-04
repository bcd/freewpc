/*
 * Copyright 2006 by Brian Dominy <brian@oddchange.com>
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

__nvram__ U8 replay_score[HIGH_SCORE_WIDTH];
__nvram__ U8 replay_csum;


const struct area_csum replay_csum_info = {
	.area = replay_score,
	.length = HIGH_SCORE_WIDTH,
	.csum = &replay_csum,
	.reset = replay_reset,
#ifdef HAVE_PAGING
	.reset_page = PAGE,
#endif
};

void replay_award (void)
{
	switch (system_config.replay_award)
	{
		case FREE_AWARD_CREDIT:
			add_credit ();
			break;

		case FREE_AWARD_EB:
			increment_extra_balls ();
			break;

		case FREE_AWARD_OFF:
			break;
	}
#ifdef DEFF_REPLAY
	deff_start (DEFF_REPLAY);
#endif
	audit_increment (&system_audits.replays);
	knocker_fire ();
}


/** Check if the current score has exceeded the next replay level. */
void replay_check_current (void)
{
	if (0)
	{
		replay_award ();
	}
}

/** Reset the replay score to its default value. */
void replay_reset (void)
{
}


CALLSET_ENTRY (replay, end_game)
{
	/* After so many games, auto-adjust the replay score if
	configured to do so. */
}


CALLSET_ENTRY (replay, init)
{
	/* Initialize the replay subsystem. */
}

