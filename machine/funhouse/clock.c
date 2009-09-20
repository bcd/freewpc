
#include <freewpc.h>

typedef U8 min5_t;

__local__ U8 clock_hour;

__local__ min5_t clock_minute;

__local__ U8 clock_base_hour;

#define TIME_0_MIN  0
#define TIME_15_MIN 3
#define TIME_30_MIN 6
#define TIME_45_MIN 9

__machine__ void light_lock (void);


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

void fh_clock_advance_step (void)
{
	if ((clock_hour == 11) && (clock_minute >= TIME_30_MIN))
		return;
	if (clock_hour == 12)
		return;

	clock_minute++;
	if (clock_minute == 12)
	{
		clock_minute = 0;
		clock_hour++;
	}
}

void fh_clock_advance (min5_t minutes)
{
	if (lock_lit_p () || flag_test (FLAG_MULTIBALL_LIT) ||
		multiball_mode_running_p ())
		return;
	/* BUG: saw this not work, time advanced during QuickMB */

	while (minutes > 0)
	{
		minutes--;
		fh_clock_advance_step ();
		if (clock_hour == 11 && clock_minute == TIME_30_MIN)
		{
			light_lock ();
			break;
		}
	}
	fh_clock_update ();
}

void fh_clock_advance_to_1145 (void)
{
	clock_hour = 11;
	clock_minute = TIME_45_MIN;
	fh_clock_update ();
}

void fh_clock_advance_to_1200 (void)
{
	clock_hour = 0;
	clock_minute = TIME_0_MIN;
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

