
#include <wpc.h>
#include <sys/task.h>
#include <sys/sound.h>

static void do_coin (uint8_t slot)
{
	sound_send (SND_SCROLL);
	task_exit ();
}

void sw_left_coin (void) __taskentry__
{
	do_coin (0);
}

void sw_center_coin (void) __taskentry__
{
	do_coin (1);
}

void sw_right_coin (void) __taskentry__
{
	do_coin (2);
}

void sw_fourth_coin (void) __taskentry__
{
	do_coin (3);
}

