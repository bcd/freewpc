
#include <freewpc.h>

#if (MACHINE_DCS == 0)

#define MUSIC_STACK_SIZE 8


__fastram__ music_code_t music_stack[MUSIC_STACK_SIZE];
__fastram__ music_code_t *music_head;
uint8_t current_volume;


void music_off (void)
{
	music_stack[0] = *(uint8_t *)WPCS_DATA = 0;
	music_head = music_stack;
}

void music_set (music_code_t code)
{
	*music_head = *(uint8_t *)WPCS_DATA = code;
}

void music_change (music_code_t code)
{
	if (code != *music_head)
		music_set (code);
}

void sound_init (void)
{
	*(uint8_t *)WPCS_CONTROL_STATUS = 0;
	current_volume = DEFAULT_VOLUME;
	volume_update ();
	music_off ();
}

void sound_send (sound_code_t code)
{
	uint8_t code_lo;
	uint8_t code_hi;

	asm ("ldd %0" :: "m" (code));
	asm ("sta %0" :: "m" (code_hi));
	asm ("stb %0" :: "m" (code_lo));

	if (code_hi == 0)
	{
		*(volatile uint8_t *)WPCS_DATA = code_lo;
	}
	else
	{
		*(volatile uint8_t *)WPCS_DATA = SND_START_EXTENDED;
		task_sleep (TIME_66MS);
		*(volatile uint8_t *)WPCS_DATA = code_lo;
	}
}


void volume_update (void)
{
}


void volume_deff (void) __taskentry__
{
	dmd_alloc_low_clean ();
	sprintf ("VOLUME %d", current_volume);
	font_render_string_center (&font_5x5, 64, 13, sprintf_buffer);
	volume_update ();
	music_change (2);
	dmd_show_low ();
	task_sleep_sec (4);
	music_off ();
	deff_exit ();
}


void volume_down (void)
{
	if (current_volume > MIN_VOLUME)
	{
		current_volume--;
	}
	deff_restart (DEFF_VOLUME_CHANGE);
}


void volume_up (void)
{
	if (current_volume < MAX_VOLUME)
	{
		current_volume++;
	}
	deff_restart (DEFF_VOLUME_CHANGE);
}

#endif /* !MACHINE_DCS */

