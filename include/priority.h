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

/**
 * \file
 * \brief Define standard priorities for effects (display & lamps).
 *
 * These priority values are used for display and lamp effects to control
 * which effect gets activated when several of them have been started.
 * Use the standard priority unless you have a good reason not to.
 * Some thought has been putting into the ordering here.
 */

#ifndef _PRIORITY_H
#define _PRIORITY_H

typedef enum _priority {
	PRI_NULL=0,
	PRI_VOLUME_CHANGE_MUSIC,
	PRI_AMODE,
	PRI_EGG1,
	PRI_EGG2,
	PRI_EGG3,
	PRI_EGG4,
	PRI_SCORES,
	PRI_CREDITS,
	PRI_GAME_LOW1,
	PRI_GAME_LOW2,
	PRI_GAME_LOW3,
	PRI_GAME_LOW4,
	PRI_GAME_LOW5,
	PRI_GAME_LOW6,
	PRI_GAME_LOW7,
	PRI_GAME_LOW8,
	PRI_GAME_MODE1,
	PRI_GAME_MODE2,
	PRI_GAME_MODE3,
	PRI_GAME_MODE4,
	PRI_GAME_MODE5,
	PRI_GAME_MODE6,
	PRI_GAME_MODE7,
	PRI_GAME_MODE8,
	PRI_COINS,
	PRI_GAME_QUICK1,
	PRI_GAME_QUICK2,
	PRI_GAME_QUICK3,
	PRI_GAME_QUICK4,
	PRI_GAME_QUICK5,
	PRI_GAME_QUICK6,
	PRI_GAME_QUICK7,
	PRI_GAME_QUICK8,
	PRI_BALLSAVE,
	PRI_EB,
	PRI_MULTIBALL,
	PRI_JACKPOT,
	PRI_SUPER_JACKPOT,
	PRI_LEFF1, PRI_LEFF2, PRI_LEFF3, PRI_LEFF4, 
	PRI_LEFF5, PRI_LEFF6, PRI_LEFF7, PRI_LEFF8,
	PRI_STATUS,
	PRI_SCORE_GOAL,
	PRI_SCORES_IMPORTANT,
	PRI_REPLAY,
	PRI_SPECIAL,
	PRI_BALL_SEARCH,
	PRI_VOLUME_CHANGE_DISPLAY,
	PRI_TILT_WARNING,
	PRI_BONUS,
	PRI_TILT,
	PRI_GAME_OVER,
	PRI_EB_BUYIN,	
	PRI_HSENTRY,
	PRI_MATCH,
	PRI_1COIN_BUYIN,
	PRI_SLAMTILT,
	PRI_DEBUGGER,
	PRI_RESET,
} priority_t;

#endif /* _PRIORITY_H */

