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

#ifndef _STDADJ_H
#define _STDADJ_H

typedef U8 adjval_t;

/* Yes/No value defines */
#define YES 1
#define NO 0

/* Off/On value defines */
#define OFF 0
#define ON 1

/* Date/Time styles */
#define DATE_TIME_STYLE_US 0
#define DATE_TIME_STYLE_EURO 1

/* Game restart setting */
#define GAME_RESTART_NEVER 0
#define GAME_RESTART_SLOW 1
#define GAME_RESTART_ALWAYS 2

/* Free awards */
#define FREE_AWARD_OFF 0
#define FREE_AWARD_CREDIT 1
#define FREE_AWARD_EB 2
#define FREE_AWARD_TICKET 3
#define FREE_AWARD_POINTS 4

/* Currency types */
#define CUR_US_DOLLAR 0

typedef struct
{
	adjval_t balls_per_game;
	adjval_t max_players;
	adjval_t tilt_warnings;
	adjval_t max_ebs;
	adjval_t max_ebs_per_bip;
	adjval_t replay_system;
	adjval_t replay_percent;
	adjval_t replay_start;
	adjval_t replay_levels;
	adjval_t replay_level[4];
	adjval_t replay_boost;
	adjval_t replay_award;
	adjval_t special_award;
	adjval_t match_award;
	adjval_t extra_ball_ticket;
	adjval_t max_tickets_per_player;
	adjval_t match_feature;
	adjval_t custom_message;
	adjval_t language;
	adjval_t clock_style;
	adjval_t date_style;
	adjval_t show_date_and_time;
	adjval_t allow_dim_illum;
	adjval_t tournament_mode;
	adjval_t euro_digit_sep;
	adjval_t min_volume_control;
	adjval_t gi_power_saver;
	adjval_t power_saver_level;
	adjval_t ticket_board;
	adjval_t no_bonus_flips;
	adjval_t game_restart;

	adjval_t family_mode;
#ifdef MACHINE_LAUNCH_SWITCH
	adjval_t flipper_plunger;
	adjval_t timed_plunger;
#endif
	adjval_t game_music;
#ifdef CONFIG_TIMED_GAME
	adjval_t timed_game;
#endif
	adjval_t buy_extra_ball;
} std_adj_t;

extern __nvram__ std_adj_t system_config;

#ifdef CONFIG_TIMED_GAME
#define config_timed_game (system_config.timed_game)
#else
#define config_timed_game OFF
#endif

#ifdef MACHINE_LAUNCH_SWITCH
#define config_flipper_plunger (system_config.flipper_plunger)
#define config_timed_plunger (system_config.timed_plunger)
#else
#define config_flipper_plunger OFF
#define config_timed_plunger OFF
#endif

typedef struct
{
	adjval_t pricing_mode;
	adjval_t coin_units[4];
	adjval_t units_per_credit;
	adjval_t units_per_bonus;
	adjval_t bonus_credits;
	adjval_t min_units;
	adjval_t coin_door_type;
	adjval_t collection_text;
	adjval_t slot_values[4];
	adjval_t max_credits;
	adjval_t free_play;
	adjval_t hide_coin_audits;
	adjval_t one_coin_buyin;
	adjval_t base_coin_size;
	adjval_t coin_meter_units;
	adjval_t dollar_bill_slot;
	adjval_t min_coin_msec;
	adjval_t slamtilt_penalty;
	adjval_t allow_hundredths;
	adjval_t credit_fraction;
} pricing_adj_t;

extern __nvram__ pricing_adj_t price_config;


typedef struct
{
	adjval_t highest_scores;
	adjval_t hstd_award;
	adjval_t champion_hstd;
	adjval_t champion_credits;
	adjval_t hstd_credits[4];
	adjval_t hs_reset_every;
} hstd_adj_t;

extern __nvram__ hstd_adj_t hstd_config;

#ifdef MACHINE_FEATURE_ADJUSTMENTS
extern __nvram__ feature_adj_t feature_config;
#endif

void adj_modified (void);
void adj_init (void);

#endif /* _STDADJ_H */
