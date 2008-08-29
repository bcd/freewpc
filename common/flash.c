
#include <freewpc.h>

struct flasher_action
{
	task_ticks_t pulse_time;
	U8 duty;
	task_ticks_t delay_time;
};

#define END_FLASHER_ACTION { 0, 0, 0 }

#define END_FLASHER_SET 0xff

#define ONTIME(x)  (4 * (x))
#define OFFTIME(x) (x)

struct flasher_action flasher_rapid_action[] =
{
	{ ONTIME(TIME_66MS), SOL_DUTY_25, OFFTIME(TIME_166MS) },
	END_FLASHER_ACTION
};


struct flasher_action flasher_pulsating_action[] =
{
	{ ONTIME(TIME_200MS), SOL_DUTY_12, OFFTIME(TIME_100MS) },
	{ ONTIME(TIME_200MS), SOL_DUTY_25, OFFTIME(TIME_100MS) },
	{ ONTIME(TIME_200MS), SOL_DUTY_50, OFFTIME(TIME_100MS) },
	{ ONTIME(TIME_200MS), SOL_DUTY_25, OFFTIME(TIME_100MS) },
	{ ONTIME(TIME_200MS), SOL_DUTY_12, OFFTIME(TIME_100MS) },
	END_FLASHER_ACTION
};


void flasher_act (U8 id, struct flasher_action *action)
{
	while (action->pulse_time != 0)
	{
		sol_start_real (id, action->duty, action->pulse_time);
		task_sleep (action->delay_time);
		action++;
	}
}


void flasher_set_act (U8 *idlist, struct flasher_action *action)
{
	while (action->pulse_time != 0)
	{
		U8 *ids = idlist;
		do {
			sol_start_real (*ids++, action->duty, action->pulse_time);
		} while (*ids != END_FLASHER_SET);
		task_sleep (action->delay_time);
		action++;
	}
}


void flasher_repeat_act (U8 id, struct flasher_action *action, U16 repeat_count)
{
	do {
		flasher_act (id, action);
	} while (--repeat_count > 0);
}


void flash_test_task (void)
{
	flasher_act (FLASH_GOAL, flasher_pulsating_action);
	task_exit ();
}

CALLSET_ENTRY (flash_test, sw_escape)
{
	task_recreate_gid (GID_FLASHER_TEST, flash_test_task);
}

