/*
 * Copyright 2007, 2008 by Brian Dominy <brian@oddchange.com>
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

#ifndef __FORMAT_H
#define __FORMAT_H

__test2__ void decimal_render (U8 val);
__test2__ void on_off_render (U8 val);
__test2__ void yes_no_render (U8 val);
__test2__ void clock_style_render (U8 val);
__test2__ void date_style_render (U8 val);
__test2__ void lang_render (U8 val);
__test2__ void replay_system_render (U8 val);
__test2__ void hs_reset_render (U8 val);
__test2__ void free_award_render (U8 val);
__test2__ void game_restart_render (U8 val);
__test2__ void percent_render (U8 val);
__test2__ void replay_score_render (U8 val);
__test2__ void minutes_render (U8 val);
__test2__ void brightness_render (U8 val);
__test2__ void collection_text_render (U8 val);
__test2__ void printer_type_render (U8 val);
__test2__ void baud_rate_render (U8 val);
__test2__ void time_interval_render (U8 val);

__test2__ void integer_audit (audit_t val) ;
__test2__ void percentage_of_games_audit (audit_t val);
__test2__ void secs_audit (audit_t val);
__test2__ void us_dollar_audit (audit_t val);
__test2__ void currency_audit (audit_t val);
__test2__ void total_earnings_audit (audit_t val __attribute__((unused)));
__test2__ void average_per_game_audit (audit_t val);
__test2__ void average_per_ball_audit (audit_t val);

__test2__ void render_audit (audit_t val, audit_format_type_t);


#endif /* __FORMAT_H */
