
#include <freewpc.h>

void rule_info_show (void)
{
	extern void shot_define (const char *);
	shot_define (sprintf_buffer);
}

#define rule_info(format, args...) \
	do { \
		sprintf (format, ## args);\
		rule_info_show ();\
	} while (0);


/* Miscellaneous */

U8 lamp_to_be_flickered;

void lamp_flicker (void)
{
}

/* Multiball conflict */

bool major_mb_running (void)
{
	/* Trick, Main, or Midnight */
}

bool any_mb_running (void)
{
	/* major or Quick 2-ball */
}

void mb_conflict_update (void)
{
}

CALLSET_ENTRY (mb_conflict, start_ball, idle_every_second)
{
}

/* Grand Finale */

bool finale_can_be_started (void)
{
	return flag_test (FLAG_FINALE_LIT);
}

bool finale_is_running (void)
{
}

void finale_light (void)
{
	flag_on (FLAG_FINALE_LIT);
}

void finale_start (void)
{
	if (finale_can_be_started ())
	{
		flag_off (FLAG_FINALE_LIT);
		lamp_tristate_flash (LM_GRAND_FINALE);
		lamplist_apply (LAMPLIST_MODES, lamp_flash_on);
	}
}

void finale_end (void)
{
	extern void badge_reset (void);
	lamp_tristate_off (LM_GRAND_FINALE);
	badge_reset ();
}

CALLSET_ENTRY (finale, lamp_update)
{
	lamp_flash_if (LM_START_FINALE, finale_can_be_started ());
}

/* Badges */

void badges_complete (void)
{
	flag_on (FLAG_FINALE_LIT);
}

void badge_award (U8 lamp)
{
	lamp_tristate_on (lamp);
	if (lamplist_test_all (LAMPLIST_MODES, lamp_test))
	{
		badges_complete ();
	}
}

void badge_reset (void)
{
	lamplist_apply (LAMPLIST_MODES, lamp_off);
	lamplist_apply (LAMPLIST_MODES, lamp_flash_off);
}

CALLSET_ENTRY (badge, start_player)
{
	badge_reset ();
}


/* Illusions */
#define NUM_TRICKS 8
#define TR_CAPTIVE_BALL 0
#define TR_CENTER_RAMP 1
#define TR_TRUNK_WALL 2
#define TR_RIGHT_LOOP 3
#define TR_LEFT_LOOP 4
#define TR_RIGHT_RAMP 5
#define TR_RIGHT_ORBIT 6
#define TR_TRUNK_HOLE 7
#define TR_BIT(t) (1 << t)

const struct trick_info
{
	U8 mode_lamp;
	U8 shot_lamp;
	U8 goal_mult;
} trick_info_table[NUM_TRICKS] = {
	{ LM_TIGER_SAW, LM_TIGER_SAW_AWARD, 1 },
	{ LM_LEVITATE_WOMAN, LM_LEVITATE_AWARD, 1 },
	{ LM_TRUNK_ESCAPE, LM_TRUNK_LAMP, 1 },
	{ LM_SPIRIT_CARDS, LM_SPIRIT_AWARD, 10 },
	{ LM_SAFE_ESCAPE, LM_SAFE_AWARD, 1 },
	{ LM_METAMORPHOSIS, LM_METAMORPHOSIS_AWARD, 1 },
	{ LM_STRAIT_JACKET, LM_JACKET_AWARD, 1 },
	{ LM_HAT_MAGIC, LM_HAT_TRICK_AWARD, 1 },
};

__local__ struct trick
{
	U8 goal;
	U8 count;
	/* TBD - score level */
} trick_table[NUM_TRICKS];

__local__ U8 trick_level;
__local__ U8 tricks_finished;
__local__ U8 tricks_finished_this_ball;
__local__ U8 trick_shots_this_ball;

void trick_update_lamp (U8 n)
{
	struct trick *tr = trick_table + n;
	struct trick_info *trinfo = trick_info_table + n;
	U8 bit = TR_BIT(n);

	if (trick_shots_this_ball & bit)
		lamp_tristate_on (trinfo->shot_lamp);
	else
		lamp_tristate_flash (trinfo->shot_lamp);

	if (tricks_finished_this_ball & bit)
		lamp_tristate_flash (trinfo->mode_lamp);
	else if (tricks_finished & bit)
		lamp_tristate_on (trinfo->mode_lamp);
	else
		lamp_tristate_off (trinfo->mode_lamp);
}

void trick_update_all_lamps (void)
{
	U8 n;
	for (n=0; n < NUM_TRICKS; n++) {
		trick_update_lamp (n);
	}
}

void trick_init_level (void)
{
	U8 n;
	for (n=0; n < NUM_TRICKS; n++) {
		trick_table[n].count = 0;
		trick_table[n].goal = trick_level * trick_info_table[n].goal_mult;
	}
	trick_update_all_lamps ();
}

void trick_advance_level (void)
{
	if (trick_level < 5)
	{
		trick_level++;
		trick_init_level ();
	}
}

void trick_award_shot (U8 n)
{
	struct trick *tr = trick_table + n;
	struct trick_info *trinfo = trick_info_table + n;
	U8 bit = TR_BIT(n);
	trick_shots_this_ball |= bit;
	if (tr->count < tr->goal)
	{
		tr->count++;
		if (tr->count == tr->goal)
		{
			tricks_finished |= bit;
			tricks_finished_this_ball |= bit;
		}
	}
	trick_update_lamp (n);
}

CALLSET_ENTRY (trick, start_player)
{
	trick_level = 3;
	tricks_finished = 0;
	trick_init_level ();
}

CALLSET_ENTRY (trick, start_ball)
{
	tricks_finished_this_ball = 0;
	trick_shots_this_ball = 0;
	trick_update_all_lamps ();
}

CALLSET_ENTRY (trick, captive_ball_shot)
{ trick_award_shot (TR_CAPTIVE_BALL); }
CALLSET_ENTRY (trick, left_loop_shot)
{ trick_award_shot (TR_LEFT_LOOP); }
CALLSET_ENTRY (trick, trunk_wall_shot)
{ trick_award_shot (TR_TRUNK_WALL); }
CALLSET_ENTRY (trick, trunk_hole_shot)
{ trick_award_shot (TR_TRUNK_HOLE); }
CALLSET_ENTRY (trick, center_ramp_shot)
{ trick_award_shot (TR_CENTER_RAMP); }
CALLSET_ENTRY (trick, sw_spinner)
{ trick_award_shot (TR_RIGHT_LOOP); }
CALLSET_ENTRY (trick, right_ramp_shot)
{ trick_award_shot (TR_RIGHT_RAMP); }
CALLSET_ENTRY (trick, right_orbit_shot)
{ trick_award_shot (TR_RIGHT_ORBIT); }

/* Trick Multiball */

struct mb_mode_ops trickmb_mode = {
};

bool trickmb_can_be_started (void)
{
	return flag_test (FLAG_TRICK_MB_LIT);
}

bool trickmb_is_running (void)
{
}

CALLSET_ENTRY (trickmb, lamp_update)
{
}

CALLSET_ENTRY (trickmb, start_player)
{
	flag_off (FLAG_TRICK_MB_LIT);
}

CALLSET_ENTRY (trickmb, start_ball)
{
}

CALLSET_ENTRY (trickmb, single_ball_play)
{
}

CALLSET_ENTRY (trickmb, trunk_hole_shot)
{
}

/* Rollovers - Bonus X */

U8 bonusx;

void bonusx_deff (void)
{
	dmd_alloc_low_clean ();
	sprintf ("%1dX", bonusx);
	font_render_string_center (&font_fixed10, 64, 16, sprintf_buffer);
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}

void bonusx_leff (void)
{
	leff_exit ();
}

void bonusx_lane (U8 lamp)
{
	if (lamp_test (lamp))
	{
		score (SC_10K);
	}
	else
	{
		score (SC_50K);
		lamp_on (lamp);

		if (lamp_test (LM_ROLLOVER_1) && lamp_test (LM_ROLLOVER_2))
		{
			++bonusx;
			lamplist_apply (LAMPLIST_ROLLOVERS, lamp_off);
			sound_send (SND_TADA);
			//deff_start (DEFF_BONUSX);
			//leff_start (LEFF_BONUSX);
		}
	}
}

CALLSET_ENTRY (bonusx, sw_top_lane_1)
{
	bonusx_lane (LM_ROLLOVER_1);
}

CALLSET_ENTRY (bonusx, sw_top_lane_2)
{
	bonusx_lane (LM_ROLLOVER_2);
}

CALLSET_ENTRY (bonusx, sw_left_button, sw_right_button)
{
	lamplist_rotate_next (LAMPLIST_ROLLOVERS, matrix_lookup (LMX_DEFAULT));
}

CALLSET_ENTRY (bonusx, start_ball)
{
	bonusx = 1;
	lamp_off (LM_ROLLOVER_1);
	lamp_off (LM_ROLLOVER_2);
}

/* Theatre Letters */

#define THEATRE_LEN 7

__local__ U8 theatre_spell_count;

void theatre_spell_reset (void)
{
	theatre_spell_count = 0;
	lamplist_apply (LAMPLIST_THEATRE_SPELL, lamp_off);
}

void theatre_spell_complete (void)
{
	sound_send (SND_KABOOM);
	lamp_flash_on (LM_HURRY_UP);
}

CALLSET_ENTRY (theatre_spell, lamp_update)
{
	lamp_on (LM_SPELL_THEATRE);
}

CALLSET_ENTRY (theatre_spell, left_orbit_shot)
{
	if (theatre_spell_count < THEATRE_LEN)
	{
		theatre_spell_count++;
		lamplist_build_increment (LAMPLIST_THEATRE_SPELL, matrix_lookup (LMX_DEFAULT));
		if (theatre_spell_count == THEATRE_LEN)
		{
			theatre_spell_complete ();
		}
	}
}

CALLSET_ENTRY (theatre_spell, start_player)
{
	theatre_spell_reset ();
}

CALLSET_ENTRY (theatre_spell, start_ball)
{
	lamp_off (LM_HURRY_UP);
}

/* Magic Letters */

__local__ U8 magic_spell_count;
__local__ U8 magic_relight_ramp_count;
__local__ U8 magic_relight_ramp_init;

bool magic_can_be_spelled (void)
{
	return magic_spell_count < 5;
}

void magic_spell_reset (void)
{
	magic_spell_count = 0;
	lamplist_apply (LAMPLIST_MAGIC_SPELL, lamp_off);
}

void magic_spell_complete (void)
{
	extern void ball_lock_enable (void);
	sound_send (SND_MAGIC_SPELLED);
	ball_lock_enable ();
}

void magic_spell_advance (void)
{
	if (magic_can_be_spelled ())
	{
		magic_spell_count++;
		lamplist_build_increment (LAMPLIST_MAGIC_SPELL,
			matrix_lookup (LMX_DEFAULT));
		if (magic_spell_count == 5)
		{
			magic_spell_complete ();
		}
	}
}

CALLSET_ENTRY (magic_spell, lamp_update)
{
}

CALLSET_ENTRY (magic_spell, start_player)
{
	magic_spell_reset ();
	magic_relight_ramp_init = 0;
	magic_relight_ramp_count = 0;
	lamp_tristate_on (LM_CENTER_MAGIC);
	lamp_tristate_on (LM_RIGHT_MAGIC);
}

CALLSET_ENTRY (magic_spell, center_ramp_shot)
{
	if (lamp_test (LM_CENTER_MAGIC))
	{
		magic_spell_advance ();
	}
}

CALLSET_ENTRY (magic_spell, right_ramp_shot)
{
	if (lamp_test (LM_RIGHT_MAGIC))
	{
		magic_spell_advance ();
	}
}

CALLSET_ENTRY (magic_spell, sw_ramp_targets)
{
	if (magic_relight_ramp_count > 0)
	{
		if (--magic_relight_ramp_count == 0)
		{
			if (!lamp_test (LM_CENTER_MAGIC))
				lamp_on (LM_CENTER_MAGIC);
			else if (!lamp_test (LM_RIGHT_MAGIC))
				lamp_on (LM_RIGHT_MAGIC);
		}
	}
}

CALLSET_ENTRY (magic_spell, start_ball)
{
	magic_relight_ramp_count = magic_relight_ramp_init;
}

/* Main Multiball Ball Lock */

__local__ U8 locks_lit;
__local__ U8 balls_locked;
__local__ U8 locks_lit_per_spell;

bool ball_lock_can_be_collected (void)
{
	return locks_lit;
}

void ball_lock_enable (void)
{
	locks_lit += locks_lit_per_spell;
}

void ball_lock_award (void)
{
	balls_locked++;
}

CALLSET_ENTRY (ball_lock, lamp_update)
{
	lamp_flash_if (LM_LOCK_BALL, ball_lock_can_be_collected ());
}

CALLSET_ENTRY (ball_lock, start_ball)
{
	locks_lit = 0;
	locks_lit_per_spell = 3;
	balls_locked = 0;
}

CALLSET_ENTRY (ball_lock, trunk_back_shot)
{
	if (ball_lock_can_be_collected () && balls_locked < 2)
	{
		ball_lock_award ();
	}
}

CALLSET_ENTRY (ball_lock, trunk_wall_shot)
{
	extern void main_mb_start (void);
	if (ball_lock_can_be_collected () && balls_locked >= 2)
	{
		main_mb_start ();
	}
}

/* Main Multiball */

struct mb_mode_ops main_mb_mode = {
};

bool main_mb_can_be_started (void)
{
	return FALSE;
}

bool main_mb_is_running (void)
{
	return FALSE;
}

void main_mb_start (void)
{
}

CALLSET_ENTRY (main_mb, start_ball)
{
}

CALLSET_ENTRY (main_mb, single_ball_play)
{
}

/* Midnight Multiball */

bool midnight_mb_can_be_started (void)
{
	return flag_test (FLAG_MIDNIGHT_MB_LIT);
}

void midnight_mb_light (void)
{
	flag_on (FLAG_MIDNIGHT_MB_LIT);
}

void midnight_mb_start (void)
{
}

CALLSET_ENTRY (midnight_mb, trunk_hole_shot)
{
}

CALLSET_ENTRY (midnight_mb, single_ball_play)
{
}

/* Clock advance to Midnight */

__local__ U8 midnight_level;
__local__ U8 clock_time;
__local__ U8 clock_step_jet;

bool clock_can_be_awarded (void)
{
	return (clock_time < 72);
}

void clock_add_time (U8 steps)
{
	if (clock_can_be_awarded ())
	{
		clock_time += steps;
		if (clock_time >= 12 * 6)
		{
			midnight_mb_light ();
		}
	}
}

CALLSET_ENTRY (clock, lamp_update)
{
	lamp_on_if (LM_ADVANCE_CLOCK, clock_can_be_awarded ());
}

CALLSET_ENTRY (clock, sw_top_jet, sw_bottom_jet, sw_middle_jet)
{
	clock_add_time (clock_step_jet);
}

CALLSET_ENTRY (clock, right_orbit_shot)
{
	if (clock_can_be_awarded ())
	{
		clock_add_time (6); /* 1 hour */
	}
}

CALLSET_ENTRY (clock, start_player)
{
	midnight_level = 0;
	clock_time = 0;
}

CALLSET_ENTRY (clock, start_ball)
{
	clock_step_jet = 1;
}

/* Newton Ball countup rules */

__local__ U8 newton_count;
__local__ U8 newton_next_level;

void newton_hit_deff (void)
{
}

bool newton_can_be_awarded (void)
{
	return (newton_count < 250);
}

void newton_bonus_award (void)
{
	extern void quickmb_light (void);
	newton_next_level += 10;
	quickmb_light ();
}

CALLSET_ENTRY (newton, captive_ball_shot)
{
	if (newton_can_be_awarded ())
	{
		newton_count++;
		sound_send (SND_TIGER);
		if (newton_count == newton_next_level)
		{
			newton_bonus_award ();
		}
	}
}

CALLSET_ENTRY (newton, start_player)
{
	newton_count = 0;
	newton_next_level = 5;
}

/* Quick Multiball rules - use timed mode */

struct mb_mode_ops quick_mb_mode = {
};

bool quickmb_can_be_started (void)
{
	return flag_test (FLAG_QUICK_MB_LIT);
}

bool quickmb_is_qualified (void)
{
}

bool quickmb_is_running (void)
{
}

void quickmb_light (void)
{
	flag_on (FLAG_QUICK_MB_LIT);
}

void quickmb_start (void)
{
	if (quickmb_can_be_started ())
	{
		flag_off (FLAG_QUICK_MB_LIT);
		/* TBD */
	}
}

CALLSET_ENTRY (quickmb, start_ball)
{
}

CALLSET_ENTRY (quickmb, single_ball_play)
{
}

/* Double Scoring rules - use timed mode */

U8 double_score_timer;

void double_score_init (void)
{
}

struct timed_mode_ops double_score_mode = {
	DEFAULT_MODE,
	.init = double_score_init,
	.gid = GID_DOUBLE_SCORE_RUNNING,
	.init_timer = 15,
	.timer = &double_score_timer,
	.grace_timer = 2,
	.pause = system_timer_pause,
};

bool double_score_is_running (void)
{
	return timed_mode_running_p (&double_score_mode);
}

void double_score_start (void)
{
	timed_mode_begin (&double_score_mode);
}

CALLSET_ENTRY (double_score, display_update)
{
	timed_mode_display_update (&double_score_mode);
}

CALLSET_ENTRY (double_score, music_refresh)
{
	timed_mode_music_refresh (&double_score_mode);
}

CALLSET_ENTRY (double_score, end_ball)
{
	timed_mode_end (&double_score_mode);
}


/* Spirit Ring */

CALLSET_ENTRY (spirit_ring, start_player)
{
}

/* Hocus Pocus */

__local__ U8 hocus_pocus_count;
__local__ U8 hocus_pocus_goal;


bool hocus_pocus_can_be_started (void)
{
	return flag_test (FLAG_HOCUS_POCUS_LIT);
}

CALLSET_ENTRY (hocus_pocus, left_bank)
{
	if (hocus_pocus_can_be_started ())
		return;
	if (++hocus_pocus_count == hocus_pocus_goal)
	{
		flag_on (FLAG_HOCUS_POCUS_LIT);
		hocus_pocus_goal++;
	}
}

CALLSET_ENTRY (hocus_pocus, sw_left_drain_eddy, sw_right_drain_eddy)
{
	if (hocus_pocus_can_be_started ())
	{
		flag_off (FLAG_HOCUS_POCUS_LIT);
	}
}

CALLSET_ENTRY (hocus_pocus, start_ball)
{
	flag_off (FLAG_HOCUS_POCUS_LIT);
}

CALLSET_ENTRY (hocus_pocus, start_player)
{
	hocus_pocus_goal = 2;
	hocus_pocus_count = 0;
}


/* Basement - Mystery Award */

CALLSET_ENTRY (basement, start_player)
{
}

