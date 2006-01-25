
#include <freewpc.h>

void handle_outlane (void)
{
	if (ballsave_test_active ())
		ballsave_launch ();
}

void sw_left_outlane_handler (void)
{
	score_add_current_const (0x10000);
	handle_outlane ();
}

void sw_right_outlane_handler (void)
{
	score_add_current_const (0x10000);
	handle_outlane ();
}

void sw_left_inlane_1_handler (void)
{
	score_add_current_const (0x1000);
}

void sw_left_inlane_2_handler (void)
{
	score_add_current_const (0x1000);
}

void sw_right_inlane_handler (void)
{
	score_add_current_const (0x1000);
}



DECLARE_SWITCH_DRIVER (sw_left_outlane)
{
	.fn = sw_left_outlane_handler,
	.flags = SW_PLAYFIELD,
	.sound = SND_DRAIN,
};

DECLARE_SWITCH_DRIVER (sw_right_outlane)
{
	.fn = sw_right_outlane_handler,
	.flags = SW_PLAYFIELD,
	.sound = SND_DRAIN,
};

DECLARE_SWITCH_DRIVER (sw_left_inlane_1)
{
	.fn = sw_left_inlane_1_handler,
	.flags = SW_PLAYFIELD,
	.sound = SND_INSIDE_LEFT_INLANE,
	.lamp = LM_LEFT_INLANE1,
};

DECLARE_SWITCH_DRIVER (sw_left_inlane_2)
{
	.fn = sw_left_inlane_2_handler,
	.flags = SW_PLAYFIELD,
	.sound = SND_INSIDE_LEFT_INLANE,
	.lamp = LM_LEFT_INLANE2,
};

DECLARE_SWITCH_DRIVER (sw_right_inlane)
{
	.fn = sw_right_inlane_handler,
	.flags = SW_PLAYFIELD,
	.sound = SND_INSIDE_LEFT_INLANE,
	.lamp = LM_RIGHT_INLANE,
};

