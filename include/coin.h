/*
 * Copyright 2006-2009 by Brian Dominy <brian@oddchange.com>
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

#ifndef _COIN_H
#define _COIN_H

struct coin_state
{
	/* The number of raw coin units paid */
	U8 units;

	/* The number of credits accumulated.  Units are converted
	to credits based on the "units per credit" adjustment. */
	U8 credits;

	/* The total number of units earned for "units per bonus". */
	U8 total_units;
};

__effect__ void credits_render (void);
__effect__ void credits_draw (void);
__common__ void lamp_start_update (void);
__common__ void add_credit (void);
__common__ bool has_credits_p (void);
__common__ void remove_credit (void);
__common__ void credits_clear (void);
__common__ void units_clear (void);

extern inline U8 get_credits (void)
{
	extern struct coin_state coin_state;
	return coin_state.credits;
}

extern inline U8 get_units (void)
{
	extern struct coin_state coin_state;
	return coin_state.units;
}


#endif /* _COIN_H */
