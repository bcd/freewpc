
#include <freewpc.h>
#include <eye_direction.h>

#define EYE_DIRECTION(x)   (x & 3)
#define EYES_STRAIGHT 0
#define EYES_LEFT 1
#define EYES_RIGHT 2

#define EYE_LID(x)         (x & 0xF0)
#define EYELID_NORMAL 0
#define EYELID_OPEN 0x10
#define EYELID_CLOSED 0x20
#define EYELID_FLUTTER 0x30


U8 rudy_eyes;


static void rudy_eyedir_timeout (void)
{
	task_sleep_sec (4);
	eye_direction_stop ();
	rudy_eyes &= (EYES_LEFT | EYES_RIGHT);
	task_exit ();
}


void rudy_eye_update (void)
{
	switch (EYE_DIRECTION (rudy_eyes))
	{
		case EYES_STRAIGHT:
			eye_direction_stop ();
			break;
		case EYES_LEFT:
			eye_direction_start_reverse ();
			task_recreate_gid_while (GID_RUDY_EYE_TIMEOUT, rudy_eyedir_timeout,
				TASK_DURATION_INF);
			break;
		case EYES_RIGHT:
			eye_direction_start_forward ();
			task_recreate_gid_while (GID_RUDY_EYE_TIMEOUT, rudy_eyedir_timeout,
				TASK_DURATION_INF);
			break;
	}

	switch (EYE_LID(rudy_eyes))
	{
		case EYELID_NORMAL:
		case EYELID_OPEN:
			sol_request (SOL_EYELIDS_OPEN);
			break;

		case EYELID_CLOSED:
			sol_request (SOL_EYELIDS_CLOSED);
			break;
	}
}

void rudy_eye_change (U8 flags)
{
	if (flags != rudy_eyes)
	{
		rudy_eyes = flags;
		rudy_eye_update ();
	}
}

void rudy_look_left (void)
{
	rudy_eye_change (EYELID_OPEN+EYES_LEFT);
}

void rudy_look_straight (void)
{
	rudy_eye_change (EYELID_OPEN+EYES_STRAIGHT);
}

void rudy_look_right (void)
{
	rudy_eye_change (EYELID_OPEN+EYES_RIGHT);
}

void rudy_sleep (void)
{
}

void rudy_wakeup (void)
{
}

void rudy_wide_eyes (void)
{
}

static void rudy_blink_task (void)
{
	rudy_eye_change (EYELID_CLOSED);
	task_sleep_sec (1);
	rudy_eye_change (EYELID_OPEN);
	task_exit ();
}

void rudy_blink (void)
{
	task_recreate_gid_while (GID_RUDY_BLINK, rudy_blink_task,
		TASK_DURATION_INF);
}

CALLSET_ENTRY (rudy, init)
{
	rudy_eyes = EYES_STRAIGHT + EYELID_OPEN;
}

CALLSET_ENTRY (rudy, init_complete)
{
	rudy_eye_update ();
}

