
#include <freewpc.h>
#include <trap_door.h>

const char *shot_name;

void shot_deff (void)
{
	if (shot_name)
	{
		dmd_alloc_low_clean ();
		font_render_string_center (&font_fixed6, 64, 16, shot_name);
		dmd_show_low ();
		task_sleep_sec (2);
	}
	shot_name = NULL;
	deff_exit ();
}

void shot_define (const char *name)
{
	shot_name = name;
	deff_start (DEFF_SHOT);
}

U8 last_combo_shot;
U8 combo_count;

void combo_reset (void)
{
	combo_count = 0;
	last_combo_shot = 0xFF;
}

void combo_detect (U8 id)
{
	if (id == last_combo_shot)
		return;
	if (!free_timer_test (TIM_COMBO))
		combo_reset ();

	free_timer_restart (TIM_COMBO, TIME_4S);
	last_combo_shot = id;
	combo_count++;
	if (combo_count == 3)
	{
	}
}

CALLSET_ENTRY (shot, init, start_ball)
{
	combo_reset ();
	shot_name = NULL;
}

CALLSET_ENTRY (shot, sw_left_lane_enter)
{
	shot_define ("LEFT ORBIT");
	combo_detect (0);
	callset_invoke (left_orbit_shot);
}

CALLSET_ENTRY (shot, sw_left_lane_exit)
{
	/* this switch is on the orbit return ramp */
}

CALLSET_ENTRY (shot, sw_center_ramp_enter)
{
}

CALLSET_ENTRY (shot, sw_center_ramp_exit)
{
	shot_define ("CENTER RAMP");
	combo_detect (1);
	callset_invoke (center_ramp_shot);
}

CALLSET_ENTRY (shot, sw_right_ramp_enter)
{
	if (!free_timer_test (TIM_RIGHT_RAMP_ENTERED))
	{
		free_timer_start (TIM_RIGHT_RAMP_ENTERED, TIME_4S);
	}
	else
	{
		free_timer_stop (TIM_RIGHT_RAMP_ENTERED);
	}
}

CALLSET_ENTRY (shot, sw_right_ramp_exit_1, sw_right_ramp_exit_2)
{
	shot_define ("RIGHT RAMP");
	combo_detect (2);
	callset_invoke (right_ramp_shot);
}

CALLSET_ENTRY (shot, sw_top_lane_1)
{
	shot_define ("LEFT ROLLOVER");
}

CALLSET_ENTRY (shot, sw_top_lane_2)
{
	shot_define ("RIGHT ROLLOVER");
}

/* Right orbit shot not awarded on falloff from the bumpers */
CALLSET_ENTRY (shot, sw_right_lane_enter)
{
	shot_define ("RIGHT ORBIT");
	combo_detect (3);
	callset_invoke (right_orbit_shot);
}

CALLSET_ENTRY (shot, sw_captive_ball_top)
{
	shot_define ("CAPTIVE BALL");
	combo_detect (4);
	callset_invoke (captive_ball_shot);
}

CALLSET_ENTRY (shot, sw_captive_ball_rest)
{
}

CALLSET_ENTRY (shot, sw_loop_left)
{
	if (!task_kill_gid (GID_RIGHT_LOOP_DEBOUNCE))
	{
		shot_define ("LEFT LOOP"); /* not working */
		combo_detect (5);
		callset_invoke (left_loop_shot);
		timer_restart_free (GID_LEFT_LOOP_DEBOUNCE, TIME_3S);
		free_timer_restart (TIM_LOOP_TO_LOCK, TIME_2S);
	}
}

CALLSET_ENTRY (shot, sw_spinner)
{
	/* TBD - ignore spinner on right ramp drain out.
	Also require left loop switch to be seen before awarding
	the right loop - spinner by itself is not enough */
	if (!task_kill_gid (GID_LEFT_LOOP_DEBOUNCE) &&
			!task_find_gid (GID_RIGHT_LOOP_DEBOUNCE) &&
			!free_timer_test (TIM_RIGHT_RAMP_ENTERED))
	{
		shot_define ("RIGHT LOOP");
		combo_detect (6);
		sound_send (SND_RIFFLE);
		callset_invoke (right_loop_shot);
		timer_restart_free (GID_RIGHT_LOOP_DEBOUNCE, TIME_3S);
		free_timer_restart (TIM_LOOP_TO_LOCK, TIME_2S);
	}
}

CALLSET_ENTRY (shot, sw_trunk_hit)
{
	if (!task_kill_gid (GID_TRUNK_DEBOUNCE))
	{
		shot_define ("TRUNK WALL");
		combo_detect (7);
		sound_send (SND_TRUNK_HIT);
		callset_invoke (trunk_wall_shot);
		timer_restart_free (GID_TRUNK_DEBOUNCE, TIME_2S);
	}
}

CALLSET_ENTRY (shot, sw_subway_micro)
{
	if (free_timer_test (TIM_LOOP_TO_LOCK))
	{
		shot_define ("TRUNK HOLE");
		callset_invoke (trunk_lock_shot);
	}
	else if (task_find_gid (GID_TRUNK_DEBOUNCE))
	{
		shot_define ("TRUNK HOLE");
		combo_detect (7);
		callset_invoke (trunk_hole_shot);
	}
}

CALLSET_ENTRY (shot, dev_subway_enter)
{
	shot_define ("SUBWAY ENTER");
}

CALLSET_ENTRY (shot, dev_popper_enter)
{
	shot_define ("POPPER ENTER");
}

CALLSET_ENTRY (shot, dev_popper_kick_attempt)
{
	/* open the Trap Door first */
	trap_door_start ();
}

CALLSET_ENTRY (shot, dev_popper_kick_success, dev_popper_kick_failure)
{
	trap_door_stop ();
}

