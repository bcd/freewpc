/*
 * Copyright 2008-2011 by Brian Dominy <brian@oddchange.com>
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

void ll_scores_draw_current (U8 single_player)
{
	seg_write_string (0, 0, sprintf_buffer);
}

void ll_score_redraw (void)
{
	seg_alloc ();
	seg_erase ();
	scores_draw_status_bar ();
	scores_draw_current (SCORE_DRAW_ALL);
	seg_copy_low_to_high ();
	scores_draw_current (player_up);
	seg_show ();
}

void ll_score_change_player (void)
{
}

void ll_score_draw_timed (U8 min, U8 sec)
{
	sprintf ("TIME %d:%02d", min, sec);
	seg_write_string (1, 8, sprintf_buffer);
}

void ll_score_draw_ball (void)
{
	sprintf ("BALL %1i", ball_up);
	seg_write_string (1, 10, sprintf_buffer);
}

void ll_score_strobe_novalid (void)
{
	seg_show_other ();
	score_deff_sleep (TIME_133MS);
}

void ll_score_strobe_valid (void)
{
	score_deff_sleep (TIME_133MS);
}

