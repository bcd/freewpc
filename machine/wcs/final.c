
#include <freewpc.h>

#define FINAL_TASK_MB       0x1
#define FINAL_TASK_MANIA    0x2
#define FINAL_TASK_SUPERDOG 0x4
#define FINAL_TASK_2X       0x8
#define FINAL_TASK_ALL      0xF

__local__ U8 final_tasks;

U8 final_match_timer;

score_t final_match_jackpot;


void final_match_start (void)
{
	final_match_timer = 45;
}

void final_match_reset (void)
{
	final_tasks = 0;
}

void final_task_completed (U8 task)
{
	final_tasks |= task;
	if (final_tasks == FINAL_TASK_ALL)
	{
		flag_on (FLAG_FINAL_MATCH_LIT);
	}
}


CALLSET_ENTRY (final_match, dev_lock_enter)
{
	if (flag_test (FLAG_FINAL_MATCH_LIT))
	{
		final_match_start ();
	}
}

CALLSET_ENTRY (final_match, left_ramp_shot, right_ramp_shot)
{
	if (final_match_timer)
	{
	}
}

CALLSET_ENTRY (final_match, goal_shot)
{
	if (final_match_timer)
	{
	}
}

CALLSET_ENTRY (final_match, init)
{
	final_match_reset ();
}

