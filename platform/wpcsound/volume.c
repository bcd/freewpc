
#include <freewpc.h>

U8 current_hw_volume;


void increase_volume (void)
{
	if (current_hw_volume >= EPOT_MAX)
		return;
	writeb (WPCS_EPOT, EPOT_LOUDER | EPOT_CLOCK);
	noop ();
	noop ();
	noop ();
	noop ();
	writeb (WPCS_EPOT, EPOT_LOUDER);
	current_hw_volume++;
}


void decrease_volume (void)
{
	if (current_hw_volume == 0)
		return;
	writeb (WPCS_EPOT, EPOT_LOUDER | EPOT_CLOCK);
	noop ();
	noop ();
	noop ();
	noop ();
	writeb (WPCS_EPOT, EPOT_LOUDER);
	current_hw_volume--;
}


void volume_off (void)
{
	U8 n;

	current_hw_volume = 200;
	for (n = 0; n < 200; n++)
		decrease_volume ();
	current_hw_volume = 0;
}


/** Translates a CPU volume value into a value for the
 * hardware. */
U8 translate_volume (U8 cpu_volume)
{
}


/** Change the volume at the e-pot.  new_volume is
given in terms of CPU units. */
void volume_set (U8 new_volume)
{
}


void volume_init (void)
{
	volume_off ();
	volume_set (100);
}

