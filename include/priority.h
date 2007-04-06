/*
 * Copyright 2006, 2007 by Brian Dominy <brian@oddchange.com>
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

#ifndef _PRIORITY_H
#define _PRIORITY_H

/*
 * Standard priorities for effects (display & lamps).
 * Use the standard priority unless you have a good reason not to.
 * Some thought has been putting into the ordering here.
 */
typedef enum _priority {
	PRI_NULL,
	PRI_AMODE,
	PRI_COINS,
	PRI_EGG1,
	PRI_EGG4=PRI_EGG1+3,

	PRI_SCORES,
	PRI_CREDITS,

	PRI_GAME_LOW1,
	PRI_GAME_LOW16=PRI_GAME_LOW1+15,

	PRI_GAME_MODE1,
	PRI_GAME_MODE16=PRI_GAME_MODE1+15,

	PRI_GAME_QUICK1,
	PRI_GAME_QUICK16=PRI_GAME_QUICK1+15,

	PRI_EB,
	PRI_JACKPOT,
	PRI_SUPER_JACKPOT,
	
	PRI_LEFF1,
	PRI_LEFF2,
	PRI_LEFF3,
	PRI_LEFF4,
	PRI_LEFF5,

	PRI_STATUS,
	PRI_SCORES_IMPORTANT,
	PRI_REPLAY,
	PRI_SPECIAL,
	PRI_BALL_SEARCH,
	PRI_VOLUME_CHANGE,
	PRI_TILT_WARNING,

	/* Every priority greater than PRI_SOLO indicates
	an effect that not only preempts, but also *kills*
	anything with lower priority. */
	PRI_SOLO,
	PRI_BONUS,
	PRI_TILT,
	PRI_GAME_OVER,
	PRI_EB_BUYIN,	
	PRI_HSENTRY,
	PRI_MATCH,
	PRI_1COIN_BUYIN,
	PRI_SLAMTILT,
	PRI_DEBUGGER,
} priority_t;

#endif /* _PRIORITY_H */

