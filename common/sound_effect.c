
#include <freewpc.h>

/* CALLSET_SECTION (sound_effect, __effect__) */

U8 speech_prio;


/**
 * Invoked periodically to update the background music.
 */
CALLSET_ENTRY (sound_effect, music_update)
{
}

void speech_running (void)
{
	task_exit ();
}

/**
 * Invoke a sound board command for speech.
 * DURATION says how long the speech will take to complete.
 * PRIORITY controls whether or not the call will be made,
 * if another speech call is in progress.
 */
void speak (sound_code_t code,
				task_ticks_t duration,
				U8 priority)
{
}


void sound_play_with_refresh (sound_code_t code,
										task_ticks_t duration)
{
	sound_send (code);
	task_sleep (duration);
	sound_effect_music_update ();
}


CALLSET_ENTRY (sound_effect, init)
{
	speech_prio = 0;
}
