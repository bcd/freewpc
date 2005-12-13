
#include <freewpc.h>
#include <queue.h>

#define SOUND_QUEUE_LEN 8

struct {
	U8 head;
	U8 tail;
	U8 elems[SOUND_QUEUE_LEN];
} sound_queue;

#define MUSIC_STACK_SIZE 8

__fastram__ music_code_t music_stack[MUSIC_STACK_SIZE];
__fastram__ music_code_t *music_head;
uint8_t current_volume;



static void sound_queue_init (void)
{
	queue_init ((queue_t *)&sound_queue);
}

static void sound_queue_insert (U8 val)
{
	queue_insert ((queue_t *)&sound_queue, SOUND_QUEUE_LEN, val);
}

static U8 sound_queue_remove (void)
{
	return queue_remove ((queue_t *)&sound_queue, SOUND_QUEUE_LEN);
}

extern inline bool sound_queue_empty (void)
{
	return queue_empty ((queue_t *)&sound_queue);
}


void music_set (music_code_t code)
{
	if ((current_volume > 0) || (code == MUS_OFF))
	{
		*music_head = code;
#if (MACHINE_DCS == 1)
		sound_queue_insert (0);
#endif
		sound_queue_insert (*music_head);
	}
}

void music_off (void)
{
	music_head = music_stack;
	music_set (MUS_OFF);
}

void music_change (music_code_t code)
{
	if (code != *music_head)
		music_set (code);
}


void sound_rtt (void)
{
	if (!sound_queue_empty ())
	{
		*(volatile U8 *)WPCS_DATA = sound_queue_remove ();
	}
}


void sound_reset (void)
{
	music_off ();	
}


void sound_init (void)
{
#if (MACHINE_DCS == 0)
	*(uint8_t *)WPCS_CONTROL_STATUS = 0;
	current_volume = DEFAULT_VOLUME;
	sound_queue_init ();
	volume_update ();
	music_off ();
#else
	static U8 dcs_init_string[] = {
		0x8C, 0xB2, 0x7B, 0x40, 0x49, 0xFB, 0xE5, 0xAF, 0x59, 0x7B,
		0xC4, 0xAA, 0x83, 0x37, 0x28, 0xC8, 0xE6, 0xE7, 0xD4, 0x85,
		0xD9, 0x16, 0x10, 0x64, 0x58, 0xC6, 0xCC, 0x93, 0x85, 0x0F,
		0x7C
	};
	int i, j;

	current_volume = DEFAULT_VOLUME;
	sound_queue_init ();

	for (i=0; i < sizeof (dcs_init_string); i++)
	{
		for (j=0; j < 4; j++)
		{
			*(uint8_t *)WPCS_CONTROL_STATUS = dcs_init_string[i];
			task_sleep (1); /* 8ms */
			*(uint8_t *)WPCS_CONTROL_STATUS = dcs_init_string[i];
			task_sleep (1); /* 8ms */
			*(uint8_t *)WPCS_CONTROL_STATUS = dcs_init_string[i];
			task_sleep (1); /* 8ms */
			*(uint8_t *)WPCS_CONTROL_STATUS = dcs_init_string[i];
			task_sleep (1); /* 8ms */
		}
	}

	task_exit ();
#endif
}

void sound_send (sound_code_t code)
{
	uint8_t code_lo;
	uint8_t code_hi;

	if (current_volume == 0)
		return;

	asm ("ldd %0" :: "m" (code));
	asm ("sta %0" :: "m" (code_hi));
	asm ("stb %0" :: "m" (code_lo));

#if (MACHINE_DCS == 0)
	if (code_hi == 0)
	{
		sound_queue_insert (code_lo);
	}
	else
#endif
	{
#if (MACHINE_DCS == 1)
		sound_queue_insert (code_hi);
#else
		sound_queue_insert (SND_START_EXTENDED);
#endif
		sound_queue_insert (code_lo);
	}
}


void volume_update (void)
{
	if (current_volume == 0)
		sound_queue_insert (MUS_OFF);
	else
		music_set (*music_head);
}


void volume_deff (void) __taskentry__
{
	dmd_alloc_low_clean ();
	sprintf ("VOLUME %d", current_volume);
	font_render_string_center (&font_5x5, 64, 13, sprintf_buffer);
	volume_update ();
	if (!in_game)
		music_change (2);
	dmd_show_low ();
	task_sleep_sec (4);
	if (!in_game)
		music_off ();
	deff_exit ();
}


void volume_down (void)
{
	if (current_volume > MIN_VOLUME)
	{
		current_volume--;
		volume_update ();
	}
	deff_restart (DEFF_VOLUME_CHANGE);
}


void volume_up (void)
{
	if (current_volume < MAX_VOLUME)
	{
		current_volume++;
		volume_update ();
	}
	deff_restart (DEFF_VOLUME_CHANGE);
}

