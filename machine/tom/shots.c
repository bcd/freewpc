
#include <freewpc.h>

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


CALLSET_ENTRY (shot, init)
{
	shot_name = NULL;
}

CALLSET_ENTRY (shot, left_lane_enter)
{
	shot_define ("LEFT ORBIT");
	callset_invoke (left_orbit_shot);
}

CALLSET_ENTRY (shot, sw_center_ramp_enter)
{
}

CALLSET_ENTRY (shot, sw_center_ramp_exit)
{
	shot_define ("CENTER RAMP");
	callset_invoke (left_ramp_shot);
}

CALLSET_ENTRY (shot, sw_right_ramp_exit_1, sw_right_ramp_exit_2)
{
	shot_define ("RIGHT RAMP");
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
	callset_invoke (right_orbit_shot);
}

CALLSET_ENTRY (shot, sw_captive_ball_top)
{
	shot_define ("CAPTIVE BALL");
	callset_invoke (captive_ball_shot);
}

CALLSET_ENTRY (shot, sw_loop_left)
{
	if (!task_kill_gid (GID_RIGHT_LOOP_DEBOUNCE))
	{
		shot_define ("LEFT LOOP"); /* not working */
		callset_invoke (left_loop_shot);
		timer_restart_free (GID_LEFT_LOOP_DEBOUNCE, TIME_3S);
	}
}

CALLSET_ENTRY (shot, sw_spinner)
{
	if (!task_kill_gid (GID_LEFT_LOOP_DEBOUNCE) ||
			!task_find_gid (GID_RIGHT_LOOP_DEBOUNCE))
	{
		shot_define ("RIGHT LOOP");
		callset_invoke (right_loop_shot);
		timer_restart_free (GID_RIGHT_LOOP_DEBOUNCE, TIME_3S);
	}
}

CALLSET_ENTRY (shot, sw_trunk_hit)
{
	if (!task_kill_gid (GID_TRUNK_DEBOUNCE))
	{
		shot_define ("TRUNK WALL");
		callset_invoke (trunk_wall_shot);
		timer_restart_free (GID_TRUNK_DEBOUNCE, TIME_2S);
	}
}

CALLSET_ENTRY (shot, sw_subway_micro)
{
	if (task_find_gid (GID_TRUNK_DEBOUNCE))
	{
		shot_define ("TRUNK HOLE");
		callset_invoke (trunk_hole_shot);
	}
}


void tbd (void)
{
/* Detect center loops ... ignore the second switch */
callset_invoke (loop_shot);

/* Detect a direct spinner shot when it occurs before either
	of the center loop switches */

/* Detect front trunk hole by trunk eddy then hole */

/* Detect rear trunk hole by center loop switch then hole */

/* Detect trunk hit = trunk eddy debounced when trunk is in
	closed position */

/* Trunk hole shot disabled Basement kickout score.  Also
	whenever Basement is thought to be closed */
callset_invoke (basement_shot);
}
