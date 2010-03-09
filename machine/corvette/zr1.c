/*
 * Copyright 2010 by Dominic Clifton <me@dominicclifton.name>
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
#include <corvette/zr1.h>

U8 zr1_pos_center;
U8 zr1_pos_full_left_opto_on;
U8 zr1_pos_full_left_opto_off;
U8 zr1_pos_full_right_opto_on;
U8 zr1_pos_full_right_opto_off;

void zr1_reset( void ) {
	global_flag_off(GLOBAL_FLAG_ZR1_CALIBRATED);
	global_flag_off(GLOBAL_FLAG_ZR1_SOLENOIDS_POWERED);

	// provide some default values, just in case. // TODO remove if possible
	zr1_pos_center = ZR_1_ENGINE_CENTER;
	zr1_pos_full_left_opto_on = ZR_1_ENGINE_LEFT_MIN;
	zr1_pos_full_left_opto_off = ZR_1_ENGINE_LEFT_MIN;
	zr1_pos_full_right_opto_on = ZR_1_ENGINE_RIGHT_MAX;
	zr1_pos_full_right_opto_off = ZR_1_ENGINE_RIGHT_MAX;
}

CALLSET_ENTRY (zr1, init)
{
	zr1_reset();
}
