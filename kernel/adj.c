
#include <freewpc.h>

std_adj_t system_config;
pricing_adj_t price_config;

static const std_adj_t std_adj_defaults = {
	.balls_per_game = 3,
	.tilt_warnings = 2,
	.max_ebs = 10,
	.max_ebs_per_bip = 4,
	.replay_system = 0,
	.replay_percent = 0,
	.replay_start = 0,
	.replay_levels = 0,
	.replay_level = { 0, 0, 0, 0 },
	.replay_boost = 0,
	.replay_award = 0,
	.special_award = 0,
	.match_award = 0,
	.extra_ball_ticket = 0,
	.max_tickets_per_player = 0,
	.match_feature = 0,
	.custom_message = 0,
	.language = 0,
	.clock_style = 0,
	.date_style = 0,
	.show_date_and_time = 0,
	.allow_dim_illum = 0,
	.tournament_mode = 0,
	.euro_digit_sep = 0,
	.min_volume_control = 0,
	.gi_power_saver = 0,
	.power_saver_level = 0,
	.ticket_board = 0,
	.no_bonus_flips = 1,
	.game_restart = 2,
};


void adj_init (void)
{
	memcpy (&system_config, &std_adj_defaults, sizeof (std_adj_t));
}

