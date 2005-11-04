
#include <freewpc.h>

void loop_timer (void) __taskentry__
{
	task_sleep_sec (3);
	task_exit ();
}

void enter_loop (void)
{
	score_add_current_const (0x1000);
	sound_send (SND_LOOP_ENTER);
}


void award_loop (void)
{
	score_add_current_const (0x25000);
	sound_send (SND_SPIRAL_AWARDED);
}


void abort_loop (void)
{
	score_add_current_const (0x1000);
	sound_send (SND_SPIRAL_SAME_SIDE_EXIT);
}


void award_left_loop (void)
{
	award_loop ();
}


void award_right_loop (void)
{
	award_loop ();
}



void sw_left_loop_handler (void) __taskentry__
{
	if (task_kill_gid (GID_LEFT_LOOP_ENTERED))
	{
		/* Left loop aborted */
		abort_loop ();
	}
	else if (task_kill_gid (GID_RIGHT_LOOP_ENTERED))
	{
		/* Right loop completed */
		award_right_loop ();
	}
	else
	{
		/* Left loop started */
		task_create_gid (GID_LEFT_LOOP_ENTERED, loop_timer);
		enter_loop ();
	}
	task_exit ();
}


void sw_right_loop_top_handler (void) __taskentry__
{
	task_exit ();
}


void sw_right_loop_handler (void) __taskentry__
{
	if (task_kill_gid (GID_LEFT_LOOP_ENTERED))
	{
		/* Left loop completed */
		award_left_loop ();
	}
	else if (task_kill_gid (GID_RIGHT_LOOP_ENTERED))
	{
		/* Right loop aborted */
		abort_loop ();
	}
	else
	{
		/* Right loop started */
		task_create_gid (GID_RIGHT_LOOP_ENTERED, loop_timer);
		enter_loop ();
	}
	task_exit ();
}


DECLARE_SWITCH_DRIVER (sw_left_loop)
{
	.flags = SW_PLAYFIELD,
	.fn = sw_left_loop_handler,
};


DECLARE_SWITCH_DRIVER (sw_right_loop_top)
{
	.flags = SW_PLAYFIELD,
	.fn = sw_right_loop_top_handler,
};


DECLARE_SWITCH_DRIVER (sw_right_loop)
{
	.flags = SW_PLAYFIELD,
	.fn = sw_right_loop_handler,
};

