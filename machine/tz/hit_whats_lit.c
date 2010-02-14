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

#include <freewpc.h>
struct {
	const char *shot_name;
	U8 switch_info_t;
	U8 lamp_num;
	bool shot_lit;
} hwl_shots[] = {
	{ "LEFT RAMP", SW_LEFT_RAMP_EXIT, LM_BONUS_X, FALSE },
	{ "RIGHT RAMP", SW_RIGHT_RAMP, LM_BATTLE_POWER, FALSE },
	{ "LOCK", SW_LOCK_LOWER, LM_LOCK_ARROW, FALSE },
};

