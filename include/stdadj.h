/*
 * Copyright 2006, 2007, 2008, 2009 by Brian Dominy <brian@oddchange.com>
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

#define MAX_REPLAY_LEVELS 4
#define MAX_COIN_SLOTS 4

/* Yes/No value defines */
#define YES 1
#define NO 0

/* Off/On value defines */
#define OFF 0
#define ON 1

/* Date/Time styles */
#define CLOCK_STYLE_AMPM 0
#define CLOCK_STYLE_24HOUR 1

#define DATE_STYLE_US 0
#define DATE_STYLE_EURO 1

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

/* Replay systems */
#define REPLAY_FIXED 0
#define REPLAY_AUTO 1

/* Currency types */
#define CUR_DOLLAR 0
#define CUR_FRANC 1
#define CUR_LIRA 2
#define CUR_PESETA 3
#define CUR_YEN 4
#define CUR_DM 5
#define CUR_GBP 6
#define CUR_EURO 7
#define CUR_TOKEN 8
#define NUM_CURRENCY_TYPES 9

/* Pricing modes */
#define PRICE_CUSTOM 0
#define PRICE_USA_25CENT 1
#define PRICE_USA_50CENT 2
#define PRICE_USA_50_75_100 3
#define PRICE_USA_3_FOR_1 4
#define PRICE_USA_5_FOR_2 5
#define PRICE_USA_75CENT 6
#define PRICE_USA_DOLLAR 7
#define PRICE_UK 8
#define PRICE_EURO 9
#define NUM_PRICING_MODES 10

/* Coin door types */
#define COIN_DOOR_CUSTOM 0
#define COIN_DOOR_25_25_25 1
#define COIN_DOOR_25_100_25 2
#define COIN_DOOR_UK 3
#define COIN_DOOR_EURO 4
#define NUM_COIN_DOOR_TYPES 5

/* Languages */
#define LANG_US_ENGLISH 0
#define LANG_GERMAN 1
#define LANG_FRENCH 2
#define LANG_SPANISH 3
#define LANG_UK_ENGLISH 4
#define NUM_LANGUAGES 5

/* Payment Method */
#define PAY_COIN 0
#define PAY_TOKEN 1
#define PAY_CARD 2
#define PAY_BILL 3
#define PAY_CUSTOM 4
#define MAX_PAY_METHODS 5

typedef struct
{
	adjval_t balls_per_game; /* done */
	adjval_t max_players; /* done */
	adjval_t tilt_warnings; /* done */
	adjval_t max_ebs; /* done */
	adjval_t max_ebs_per_bip; /* done */
	adjval_t replay_system;
	adjval_t replay_percent;
	adjval_t replay_start;
	adjval_t replay_levels;
	adjval_t replay_level[MAX_REPLAY_LEVELS];
	adjval_t replay_boost;
	adjval_t replay_award; /* done */
	adjval_t special_award; /* done */
	adjval_t match_award;
	adjval_t extra_ball_ticket;
	adjval_t max_tickets_per_player; /* done */
	adjval_t match_feature; /* done */
	adjval_t custom_message;
	adjval_t language;
	adjval_t clock_style; /* done */
	adjval_t date_style; /* done */
	adjval_t show_date_and_time; /* done */
	adjval_t allow_dim_illum;
	adjval_t tournament_mode; /* done */
	adjval_t euro_digit_sep; /* done */
	adjval_t min_volume_control; /* done */
	adjval_t gi_power_saver;
	adjval_t power_saver_level;
	/* The ticket_board adjustment says if a ticket dispenser
	is connected, or if a coin meter is attached. */
	adjval_t ticket_board;
	adjval_t no_bonus_flips; /* done */
	adjval_t game_restart; /* done */
	adjval_t allow_chase_ball;

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

/**
 * Use CONFIG_TIMED_GAME for a compile-time conditional
 * for things that need to be included in a timed game.
 *
 * Use config_timed_game for a runtime check to see if
 * a timed game is in progress.  The difference is, it is
 * possible to build a program with no timed game support
 * at all; it is also possible to turn it off even if
 * compiled in via adjustment.
 */
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
	adjval_t pricing_mode; /* done */
	adjval_t coin_units[MAX_COIN_SLOTS]; /* done */
	adjval_t units_per_credit; /* done */
	adjval_t units_per_bonus;
	adjval_t bonus_credits;
	adjval_t min_units;
	adjval_t coin_door_type; /* done */
	adjval_t collection_text; /* done */
	adjval_t slot_values[MAX_COIN_SLOTS];
	adjval_t max_credits; /* done */
	adjval_t free_play; /* done */
	adjval_t hide_coin_audits;
	adjval_t payment_method;
	adjval_t base_coin_size;
	adjval_t coin_meter_units;
	adjval_t fast_bill_slot;
	adjval_t min_coin_msec;
	adjval_t slamtilt_penalty; /* done */
	adjval_t allow_hundredths;
	adjval_t credit_fraction;
} pricing_adj_t;

extern __nvram__ pricing_adj_t price_config;


typedef struct
{
	adjval_t highest_scores;
	adjval_t hstd_award;
	adjval_t champion_hstd;
	adjval_t champion_credits; /* done */
	adjval_t hstd_credits[4]; /* done */
	adjval_t hs_reset_every; /* done */
} hstd_adj_t;

extern __nvram__ hstd_adj_t hstd_config;

typedef struct
{
	adjval_t column_width; /* done */
	adjval_t lines_per_page; /* done */
	adjval_t pause_every_page; /* done */
	adjval_t printer_type;
	adjval_t serial_baud_rate;
	adjval_t serial_dtr;
	adjval_t nsm_stub_only;
	adjval_t auto_printout;
	adjval_t auto_line_fill;
} printer_adj_t;

extern __nvram__ printer_adj_t printer_config;

#ifdef MACHINE_FEATURE_ADJUSTMENTS
extern __nvram__ feature_adj_t feature_config;
#endif

extern adjval_t *last_adjustment_changed;

void adj_modified (adjval_t *adjp);
void adj_init (void);

#endif /* _STDADJ_H */
