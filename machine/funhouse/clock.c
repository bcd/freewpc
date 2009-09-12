
#include <freewpc.h>

typedef U8 min5_t;

__local__ U8 clock_hour;

__local__ min5_t clock_minute;

__local__ U8 clock_base_hour;


void fh_clock_update (void)
{
	lampnum_t lamp;

	lamplist_apply_nomacro (LAMPLIST_CLOCK_HOURS, lamp_off);
	lamp = lamplist_index (LAMPLIST_CLOCK_HOURS, clock_hour);
	lamp_on (lamp);

	lamplist_apply_nomacro (LAMPLIST_CLOCK_MINUTES, lamp_off);
	lamp = lamplist_index (LAMPLIST_CLOCK_MINUTES, clock_minute);
	lamp_on (lamp);
}


void fh_clock_set (U8 hour, min5_t minute)
{
	if (hour != clock_hour || minute != clock_minute)
	{
		clock_hour = hour;
		clock_minute = minute;
		fh_clock_update ();
	}
}

S8 fh_clock_compare (U8 hour, min5_t minute)
{
	if (hour < clock_hour)
		return -1;
	else if (hour > clock_hour)
		return 1;
	else if (minute < clock_minute)
		return -1;
	else if (minute > clock_minute)
		return 1;
	else
		return 0;
}

void fh_clock_advance (min5_t minutes)
{
	if (flag_test (FLAG_MULTIBALL_LIT))
		return;
	if (flag_test (FLAG_MULTIBALL_RUNNING))
		return;

	clock_minute += minutes;
	while (clock_minute >= 12)
	{
		clock_minute -= 12;
		clock_hour++;
		if (clock_hour >= 12)
			clock_hour -= 12;
	}

	fh_clock_update ();
}


void fh_clock_reset (void)
{
	clock_hour = clock_base_hour;
	if (clock_base_hour > 3)
		clock_base_hour -= 3;
	clock_minute = 0;
	fh_clock_update ();
}


CALLSET_ENTRY (fh_clock, start_player)
{
	clock_base_hour = 8;
	fh_clock_reset ();
}

