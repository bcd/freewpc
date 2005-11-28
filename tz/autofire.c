
#include <freewpc.h>


U8 autofire_request_count;


__taskentry__ void autofire_handler (void)
{
	while (autofire_request_count > 0)
	{
		/* Open autofire diverter */
		sol_on (SOL_SHOOTER_DIV);

		/* Request kick from trough */
		device_request_kick (device_entry (DEV_TROUGH));

		/* Wait for autofire switch to go off once */
		task_sleep_sec (2);

		/* Close autofire diverter */
		sol_off (SOL_SHOOTER_DIV);

		/* Wait for the ball to settle */
		task_sleep_sec (1);

		/* Disable right orbit shots */

		/* Open diverter again and kick ball */
		sol_on (SOL_SHOOTER_DIV);
		task_sleep (TIME_100MS * 5);

		sol_on (SOL_AUTOFIRE);
		task_sleep (TIME_100MS);
		sol_off (SOL_AUTOFIRE);
		task_sleep (TIME_100MS * 5);

		/* Close diverter */
		sol_off (SOL_SHOOTER_DIV);

		/* Check that ball actually launched OK */
		task_sleep_sec (1);

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

