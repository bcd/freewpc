
#include <freewpc.h>

__local__ U8 matches_played;

__local__ U8 matches_won_bitset;

U8 matches_won;

U8 match_timer;

U8 player_goals;

U8 player_misses;

U8 miss_threshold;

U8 opponent_goals;

void newmatch_running_deff (void)
{
	when_score_changes
	{
		dmd_alloc_low_clean ();
		sprintf ("YOU: %d", player_goals);
		font_render_string_left (&font_var5, 1, 27, sprintf_buffer);
		sprintf ("THEM: %d", opponent_goals);
		font_render_string_right (&font_var5, 127, 27, sprintf_buffer);
		sprintf_current_score ();
		font_render_string_left (&font_num5x7, 1, 1, sprintf_buffer);
		dmd_show_low ();
	}
}

void newmatch_goal_deff (void)
{
}

void newmatch_win_deff (void)
{
}

void newmatch_win (void)
{
	matches_won_bitset |= 1;
	matches_won++;
}

void newmatch_loss (void)
{
}

void newmatch_tie (void)
{
}

void newmatch_init (void)
{
	lamp_off (LM_STRIKER);
	player_goals = opponent_goals = 0;
	player_misses = 0;
	matches_won_bitset <<= 1;

	if (matches_won < 3)
		miss_threshold = 3 - matches_won;
	else
		miss_threshold = 1;
}

void newmatch_timeout (void)
{
	if (player_goals > opponent_goals)
		newmatch_win ();
	else if (player_goals < opponent_goals)
		newmatch_loss ();
	else
		newmatch_tie ();
}

void newmatch_exit (void)
{
	lamp_on (LM_STRIKER);
}

void newmatch_score_goal (void)
{
	player_goals++;
	sample_start (MUS_GOAL_IS_LIT, SL_2S);
	speech_start (SPCH_GOALLL, SL_3S);
}

void newmatch_opponent_goal (void)
{
	opponent_goals++;
}

void newmatch_miss (void)
{
	player_misses++;
	if (player_misses == miss_threshold)
	{
		newmatch_opponent_goal ();
		player_misses = 0;
	}
}


struct timed_mode_ops match_mode =
{
	DEFAULT_MODE,
	.init = newmatch_init,
	.exit = newmatch_exit,
	.timeout = newmatch_timeout,
	.gid = GID_WCS_MATCH_RUNNING,
	.deff_running = DEFF_NEWMATCH_RUNNING,
	.music = MUS_RESTART_RUNNING,
	.prio = PRI_GAME_MODE3,
	.init_timer = 30,
	.timer = &match_timer,
};


CALLSET_ENTRY (newmatch, striker_shot)
{
	if (timed_mode_running_p (&match_mode))
		return;
	timed_mode_begin (&match_mode);
}

CALLSET_ENTRY (newmatch, goal_shot)
{
	if (!timed_mode_running_p (&match_mode))
		return;
	newmatch_score_goal ();
}

CALLSET_ENTRY (newmatch, sw_goalie_target)
{
	if (!timed_mode_running_p (&match_mode))
		return;
	sample_start (SND_POW, SL_1S);
	newmatch_miss ();
}

CALLSET_ENTRY (newmatch, sw_striker_1, sw_striker_2, sw_striker_3)
{
	if (!timed_mode_running_p (&match_mode))
		return;
	newmatch_miss ();
	sample_start (SND_DRIBBLE, SL_1S);
}

#if 0
CALLSET_XENTRY (newmatch, sw_striker_2)
{
	newmatch_sw_striker_1 ();
}

CALLSET_XENTRY (newmatch, sw_striker_3)
{
	newmatch_sw_striker_1 ();
}
#endif

CALLSET_ENTRY (newmatch, lamp_update)
{
}

CALLSET_ENTRY (newmatch, music_refresh)
{
	timed_mode_music_refresh (&match_mode);
}

CALLSET_ENTRY (newmatch, display_update)
{
	timed_mode_deff_update (&match_mode);
}

CALLSET_ENTRY (newmatch, start_player)
{
	matches_played = 0;
	matches_won = 0;
	matches_won_bitset = 0;
}

CALLSET_ENTRY (newmatch, start_ball)
{
	timed_mode_finish (&match_mode);
}

CALLSET_ENTRY (newmatch, valid_playfield)
{
	if (!matches_won_bitset)
		timed_mode_begin (&match_mode);
}

