
#include <freewpc.h>


U8 autofire_request_count;


void sw_autofire1_handler (void)
{
	score_add_current_const (0x100);
}


void sw_autofire2_handler (void)
{
	score_add_current_const (0x200);
	if (autofire_request_count == 0)
	{
	}
}


DECLARE_SWITCH_DRIVER(sw_autofire1)
{
	.flags = SW_PLAYFIELD | SW_IN_GAME,
	.fn = sw_autofire1_handler,
};


DECLARE_SWITCH_DRIVER(sw_autofire2)
{
	.flags = SW_PLAYFIELD | SW_IN_GAME,
	.fn = sw_autofire2_handler,
};


__taskentry__ void autofire_handler (void)
{
	while (autofire_request_count > 0)
	{
		/* Open autofire diverter */
		sol_on (SOL_SHOOTER_DIV);

		/* Request kick from trough */
		device_request_kick (device_entry (DEV_TROUGH));

		/* Wait for autofire switch to go off once */
		/* TODO : add timeout here */
		task_sleep_sec (2);
		while (!switch_poll_logical (SW_AUTOFIRE2))
			task_sleep (TIME_66MS);

		/* Close autofire diverter */
		sol_off (SOL_SHOOTER_DIV);

		/* Wait for the ball to settle */
		task_sleep_sec (1);

retry:
		/* Disable right orbit shots */

		/* Open diverter again and kick ball */
		sol_on (SOL_SHOOTER_DIV);
		task_sleep (TIME_100MS * 5);

		sol_pulse (SOL_AUTOFIRE);
		task_sleep (TIME_100MS * 5);

		/* Close diverter */
		sol_off (SOL_SHOOTER_DIV);

		/* Check that ball actually launched OK.
		 * If it didn't, then retry the kick. */
		task_sleep_sec (2);
		if (switch_poll_logical (SW_AUTOFIRE2))
			goto retry;

		/* Decrement request count */
		autofire_request_count--;
	}
	task_exit ();
}

void autofire_add_ball (void)
{
	autofire_request_count++;
	task_create_gid1 (GID_AUTOFIRE_HANDLER, autofire_handler);
}

CALLSET_ENTRY (autofire, init)
{
	autofire_request_count = 0;
}

