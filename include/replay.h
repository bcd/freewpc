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

#ifndef _REPLAY_H
#define _REPLAY_H

#define NUM_REPLAY_LEVELS 4

/* The number of games after which the auto replay score is recalculated */
#define AUTO_REPLAY_ADJUST_RATE 50

__common__ void replay_draw (void);
__common__ void replay_award (void);
__common__ void replay_check_current (void);
__common__ void replay_reset (void);
__common__ bool replay_can_be_awarded (void);

#ifdef MACHINE_REPLAY_CODE_TO_SCORE
__machine__ void MACHINE_REPLAY_CODE_TO_SCORE (score_t, U8);
#endif

#endif /* _REPLAY_H */
