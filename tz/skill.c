
#include <freewpc.h>

bool skill_shot_enabled;
U8 skill_switch_reached;

void enable_skill_shot (void)
{
	skill_shot_enabled = TRUE;
	skill_switch_reached = 0;
}

void disable_skill_shot (void)
{
	skill_shot_enabled = FALSE;
}

void award_skill_shot (void)
{
	mark_ball_in_play ();
	disable_skill_shot ();
	sound_send (SND_SKILL_SHOT_CRASH_1);
	switch (skill_switch_reached)
	{
		case 1: score_add_current_const (0x25000); break;
		case 2: score_add_current_const (0x50000); break;
		case 3: score_add_current_const (0x100000); break;
	}
}

void skill_switch_monitor (void) __taskentry__
{
	if (skill_switch_reached < 3)
		task_sleep_sec (1);
	else
		task_sleep_sec (3);
	award_skill_shot ();
	task_exit ();
}


void award_skill_switch (U8 sw)
{
	if (skill_switch_reached < sw)
	{
		skill_switch_reached = sw;
		task_recreate_gid (GID_SKILL_SWITCH_TRIGGER, skill_switch_monitor);
		sound_send (skill_switch_reached + SND_SKILL_SHOT_RED);
	}
	else
	{
		task_kill_gid (GID_SKILL_SWITCH_TRIGGER);
		award_skill_shot ();
	}
}


void sw_lower_skill_handler (void) __taskentry__
{
	if (skill_shot_enabled)
	{
		award_skill_switch (1);
	}
	task_exit ();
}


void sw_center_skill_handler (void) __taskentry__
{
	if (skill_shot_enabled)
	{
		award_skill_switch (2);
	}
	task_exit ();
}


void sw_upper_skill_handler (void) __taskentry__
{
	if (skill_shot_enabled)
	{
		award_skill_switch (3);
	}
	task_exit ();
}


DECLARE_SWITCH_DRIVER (sw_lower_skill)
{
	.fn = sw_lower_skill_handler,
	.flags = SW_IN_GAME,
};


DECLARE_SWITCH_DRIVER (sw_center_skill)
{
	.fn = sw_center_skill_handler,
	.flags = SW_IN_GAME,
};


DECLARE_SWITCH_DRIVER (sw_upper_skill)
{
	.fn = sw_upper_skill_handler,
	.flags = SW_IN_GAME,
};

