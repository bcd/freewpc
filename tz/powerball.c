
#include <freewpc.h>

/* Proximity switches will trigger whenever a steel ball passes over
 * them.  The powerball is detected by the lack of such closures.
 */


void sw_trough_prox_handler (void)
{
	/* The next ball to be served from the trough is
	 * definitely a steel ball. */
}


void sw_slot_prox_handler (void)
{
	/* If the ball came from the piano or camera, it must
	 * be steel.  Stop the detector task.
	 */
}


DECLARE_SWITCH_DRIVER (sw_trough_prox)
{
	.fn = sw_trough_prox_handler,
	.flags = SW_IN_GAME,
};


DECLARE_SWITCH_DRIVER (sw_slot_prox)
{
	.fn = sw_slot_prox_handler,
	.flags = SW_IN_GAME,
};

