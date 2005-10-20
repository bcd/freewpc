
#include <freewpc.h>
#include <queue.h>

#if (MACHINE_DCS == 0)

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


void music_off (void)
{
	music_stack[0] = *(uint8_t *)WPCS_DATA = 0;
	music_head = music_stack;
}

void music_set (music_code_t code)
{
	*music_head = code;
	sound_queue_insert (code);
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


void sound_init (void)
{
	*(uint8_t *)WPCS_CONTROL_STATUS = 0;
	current_volume = DEFAULT_VOLUME;
	sound_queue_init ();
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
		sound_queue_insert (code_lo);
	}
	else
	{
		sound_queue_insert (SND_START_EXTENDED);
		sound_queue_insert (code_lo);
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

