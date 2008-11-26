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


CALLSET_ENTRY (pity, bonus)
{
	sound_send (MUS_BONUS_BONG);
	task_sleep (TIME_500MS);
	sound_send (MUS_BONUS_BONG);
	task_sleep (TIME_300MS);
	sound_send (MUS_BONUS_BONG);
	task_sleep (TIME_300MS);
}

CALLSET_ENTRY (pity, any_pf_switch)
{
	score (SC_730);
}

CALLSET_ENTRY (pity, sw_spinner_slow)
{
	score (SC_10K);
}

void pity_skill_lane (void)
{
	sound_send (SND_WAIT_BELL);
}

CALLSET_ENTRY (pity, sw_rollover_1)
{
	sound_send (SND_DING1);
	callset_invoke (any_rollover);
}

CALLSET_ENTRY (pity, sw_rollover_2)
{
	sound_send (SND_DING2);
	callset_invoke (any_rollover);
}

CALLSET_ENTRY (pity, sw_rollover_3)
{
	sound_send (SND_DING3);
	callset_invoke (any_rollover);
}

CALLSET_ENTRY (pity, sw_rollover_4)
{
	sound_send (SND_DING4);
	callset_invoke (any_rollover);
}

CALLSET_ENTRY (pity, sw_shooter)
{
	/* Sound only on the closed->open transition */
	if (!switch_poll_logical (SW_SHOOTER))
	{
		sound_send (SND_PLUNGE);
	}
}

