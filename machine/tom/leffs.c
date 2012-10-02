
#include <freewpc.h>


void strobe_trunk_leff (void)
{
	lamplist_set_apply_delay (TIME_33MS);
	for (;;)
		lamplist_apply (LAMPLIST_TRUNK_VERTICAL, leff_toggle);
}

void bonusx_leff (void)
{
	U8 n;
	lamplist_set_apply_delay (TIME_50MS);
	for (n = 0; n < 8; n++)
		lamplist_apply (LAMPLIST_ROLLOVERS, leff_toggle);
	leff_exit ();
}

void theatre_alt_leff (void)
{
	lamplist_apply_leff_alternating (LAMPLIST_THEATRE_SPELL, 0);
	for (;;task_sleep (TIME_66MS))
		lamplist_apply (LAMPLIST_THEATRE_SPELL, leff_toggle);
}

void magic_alt_leff (void)
{
	lamplist_apply_leff_alternating (LAMPLIST_MAGIC_SPELL, 0);
	lamplist_set_apply_delay (TIME_66MS);
	for (;;task_sleep (TIME_66MS))
		lamplist_apply (LAMPLIST_MAGIC_SPELL, leff_toggle);
}

void random_flasher_leff (void)
{
	for (;;)
	{
		if (random () < 0x40) {
		flasher_pulse (FLASH_RETURN_LANE);
		task_sleep (TIME_166MS);
		}

		if (random () < 0x60) {
		flasher_pulse (FLASH_TRAP_DOOR);
		task_sleep (TIME_166MS);
		}

		if (random () < 0x80) {
		flasher_pulse (FLASH_SPIRIT_RING);
		task_sleep (TIME_166MS);
		}

		if (random () < 0x60) {
		flasher_pulse (FLASH_SAW);
		task_sleep (TIME_166MS);
		}

		if (random () < 0x40) {
		flasher_pulse (FLASH_JET);
		task_sleep (TIME_166MS);
		}

		if (random () < 0x80) {
		flasher_pulse (FLASH_TRUNK);
		task_sleep (TIME_166MS);
		}
	}
}
