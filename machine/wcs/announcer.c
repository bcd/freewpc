
#include <freewpc.h>

typedef struct
{
	U8 count;
	sound_code_t codes[0];
} sound_code_array_t;


void announcer_task (void)
{
	sound_code_t code = task_get_arg ();
	task_sleep_sec (2);
	speech_start (code, SL_4S);
	task_exit ();
}

/**
 * Request that an announcer say something.
 *
 * - If he is already talking, he won't say anything to overlay
 * the current speech.
 * - Speech should not start 'right away' when the software says so;
 * be more human-like and give some time for him to chime in.
 * - Randomly, just ignore some requests:
 * 	- If the announcer has been talking a lot lately, then he's more
 *		likely not to say something new, unless it's really high priority.
 */
void announce (sound_code_t code)
{
	task_pid_t tp;

	tp = task_find_gid (GID_ANNOUNCER_SPEAKING);
	if (tp)
		return;

	tp = task_create_gid (GID_ANNOUNCER_SPEAKING, announcer_task);
	task_set_arg (tp, code);
}


/**
 * Request that the announcer say one of several codes.
 */
void announce_random (sound_code_array_t *list)
{
}

