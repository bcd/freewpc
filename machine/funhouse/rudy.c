
#include <freewpc.h>

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


void rudy_eye_update (void)
{
	switch (EYE_DIRECTION (rudy_eyes))
	{
		case EYES_STRAIGHT:
			eye_direction_stop ();
			break;
		case EYES_LEFT:
			eye_direction_start_reverse ();
			break;
		case EYES_RIGHT:
			eye_direction_start_forward ();
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

CALLSET_ENTRY (rudy, init)
{
	rudy_eyes = EYES_STRAIGHT + EYELID_OPEN;
}

CALLSET_ENTRY (rudy, init_complete)
{
	rudy_eye_update ();
}

