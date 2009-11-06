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

#ifndef _PROTOS_H
#define _PROTOS_H

__machine__ void fh_clock_advance (U8);
__machine__ void rudy_look_left (void);
__machine__ void rudy_look_straight (void);
__machine__ void rudy_look_right (void);
__machine__ void rudy_blink (void);

__machine__ void light_lock (void);
__machine__ bool lock_lit_p (void);
__machine__ bool frenzy_lit_p (void);
__machine__ bool frenzy_running_p (void);
__machine__ void frenzy_start (void);
__machine__ void frenzy_collect (void);
__machine__ void super_frenzy_start (void);
__machine__ bool multiball_mode_running_p (void);

__machine__ void say (U16 code);

typedef U8 min5_t;

#define CLK_5_MIN  1
#define CLK_10_MIN 2
#define CLK_15_MIN 3
#define CLK_20_MIN 4
#define CLK_25_MIN 5
#define CLK_30_MIN 6

__machine__ void fh_clock_set (U8 hour, min5_t minute);
__machine__ void fh_clock_advance_step (void);

#define TIME_0_MIN  0
#define TIME_15_MIN 3
#define TIME_30_MIN 6
#define TIME_45_MIN 9

/* Effect priorities */

#define PRI_SUPERDOG          PRI_GAME_QUICK2
#define PRI_MB                PRI_GAME_QUICK4
#define PRI_QUICKMB           PRI_GAME_QUICK4
#define PRI_MIRROR            PRI_GAME_QUICK6

#define PRI_SUPERDOG_MODE     PRI_GAME_MODE2
#define PRI_FRENZY_MODE       PRI_GAME_MODE5

#define PRI_AMODE_LEFF        PRI_LEFF1
#define PRI_RUDY_LEFF         PRI_LEFF2
#define PRI_SHOOTER_LEFF      PRI_LEFF2
#define PRI_SUPERDOG_LEFF     PRI_LEFF3
#define PRI_KICKOUT_LEFF      PRI_LEFF3
#define PRI_MB_LEFF           PRI_LEFF4
#define PRI_JACKPOT_LEFF      PRI_LEFF8

#endif
