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

#define BUYIN_TIMEOUT   7  /* No. of seconds until buyin times out */


/** Amount of time left to buy an extra ball during buyin */
U8 buyin_offer_timer;


/** Display effect that runs during the extra ball buyin */
void buyin_offer_deff (void)
{
	dmd_sched_transition (&trans_bitfade_slow);
	for (;;)
	{
		dmd_alloc_low_clean ();
		dmd_draw_border (dmd_low_buffer);
		font_render_string_center (&font_term6, 64, 4, "CONTINUE GAME");
		/* TODO : draw timer */
		dmd_show_low ();
		task_sleep (TIME_500MS);
	}
	deff_exit ();
}


/** Offer to buy an extra ball.  This is called from the game state
 * machine when it detects that a player has played his last ball. */
void buyin_offer (void)
{
	buyin_offer_timer = BUYIN_TIMEOUT;
	deff_start (DEFF_BUYIN_OFFER);
	while (buyin_offer_timer > 0)
	{
		task_sleep_sec (1);
		buyin_offer_timer--;
	}
	task_sleep_sec (1);
	deff_stop (DEFF_BUYIN_OFFER);
}


