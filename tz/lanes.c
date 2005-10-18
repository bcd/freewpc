
#include <freewpc.h>


void sw_left_outlane_handler (void)
{
	static U8 score[] = { 0x00, 0x01, 0x00, 0x00 };
	score_add_current (score);
	sound_send (SND_DRAIN);
}

void sw_right_outlane_handler (void)
{
	static U8 score[] = { 0x00, 0x01, 0x00, 0x00 };
	score_add_current (score);
	sound_send (SND_DRAIN);
}

void sw_left_inlane_1_handler (void)
{
	static U8 score[] = { 0x00, 0x00, 0x10, 0x00 };
	score_add_current (score);
}

void sw_left_inlane_2_handler (void)
{
	static U8 score[] = { 0x00, 0x00, 0x10, 0x00 };
	score_add_current (score);
	sound_send (SND_INSIDE_LEFT_INLANE);
}

void sw_right_inlane_handler (void)
{
	static U8 score[] = { 0x00, 0x00, 0x10, 0x00 };
	score_add_current (score);
}



DECLARE_SWITCH_DRIVER (sw_left_outlane)
{
	.fn = sw_left_outlane_handler,
	.flags = SW_PLAYFIELD,
};

DECLARE_SWITCH_DRIVER (sw_right_outlane)
{
	.fn = sw_right_outlane_handler,
	.flags = SW_PLAYFIELD,
};

DECLARE_SWITCH_DRIVER (sw_left_inlane_1)
{
	.fn = sw_left_inlane_1_handler,
	.flags = SW_PLAYFIELD,
};

DECLARE_SWITCH_DRIVER (sw_left_inlane_2)
{
	.fn = sw_left_inlane_2_handler,
	.flags = SW_PLAYFIELD,
};

DECLARE_SWITCH_DRIVER (sw_right_inlane)
{
	.fn = sw_right_inlane_handler,
	.flags = SW_PLAYFIELD,
};

