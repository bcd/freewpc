
#include <freewpc.h>

#define MUSIC_STACK_SIZE 8


__fastram__ music_code_t music_stack[MUSIC_STACK_SIZE];
__fastram__ music_code_t *music_head;



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
		*(volatile uint8_t *)WPCS_DATA = code_lo;
	}
}


