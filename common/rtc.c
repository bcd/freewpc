/*
 * Copyright 2006-2011 by Brian Dominy <brian@oddchange.com>
 *
 * This file is part of FreeWPC.
 *
 * FreeWPC is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * FreeWPC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FreeWPC; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <freewpc.h>
#include <diag.h>

/**
 * \file
 * \brief The driver for the WPC real-time clock (RTC).
 */


/** PinMAME expects address 0x1800 in nvram to contain the
 * following date structure, as apparently all real WPC
 * ROMs put this information there.  FreeWPC avoids placing
 * any data at this address. */
struct wpc_pinmame_clock_data
{
	U16 year;
	U8 month;
	U8 day;
	U8 day_of_week;
};


/** The editable fields of the date/time. */
enum rtc_field
{
	RTC_FIELD_MONTH,
	RTC_FIELD_DAY,
	RTC_FIELD_YEAR,
	RTC_FIELD_HOUR,
	RTC_FIELD_MINUTE,
	NUM_RTC_FIELDS,
	RTC_FIELD_INVALID = 0xFF
};

/** The current date/time */
__nvram__ struct date current_date;
/* year, month, day, hour, minute, day_of_week */

/** Keeps track of when the minute changes, for auditing */
static U8 last_minute;

/** The date/time being edited */
struct date edit_date;

/** Which field is being edited */
enum rtc_field rtc_edit_field;

/** System timestamps */
__nvram__ std_timestamps_t system_timestamps;

/** Checksum descriptor for the RTC info */
const struct area_csum rtc_csum_info = {
	.type = FT_DATE,
	.version = 1,
	.area = (U8 *)&current_date,
	.length = sizeof (struct date),
	.reset = rtc_reset,
};


static U8 days_in_month_table[] = {
	31, 28, 31, 30, 31, 30,
	31, 31, 30, 31, 30, 31,
};

#if (MACHINE_DMD == 1)
	/* At present there's no room on the display for a
	day name */
static const char *day_names[] = {
	"SUNDAY", "MONDAY", "TUESDAY", "WEDNESDAY",
	"THURSDAY", "FRIDAY", "SATURDAY"
};
#endif


bool date_verify (struct date *d)
{
	if (d->month == 0 || d->day == 0)
		return FALSE;
	if (d->year > 2050 - MIN_YEAR)
		return FALSE;
	if (d->day > 31)
		return FALSE;
	if (d->month > 12)
		return FALSE;
	if (d->hour >= 24)
		return FALSE;
	if (d->minute >= 60)
		return FALSE;
	return TRUE;
}


/** Returns the number of days in the current month.
 * This handles leap years in February correctly until 2100,
 * which is not be a leap year but will be detected as such. */
static U8 rtc_days_in_current_month (struct date *d)
{
	U8 days = days_in_month_table[d->month-1];
	/* Handle leap years */
	if ((d->month == 2) && ((d->year % 4) == 0))
		days++;
	return (days);
}


/** Calculate the day of the week (0=Sunday, 6=Saturday)
 * from the current values of year, month, and day.
 * This should only be called when needing to display a
 * date; the date structure does not maintain this.
 */
enum day_of_week rtc_calc_day_of_week (struct date *d)
{
	static U8 day_of_week_month_code[] = {
		0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5
	};
	U8 day_of_week;

	/* Compute (6 + year + (year/4) + month code + day - N) mod 7.
	N is 1 if it is a leap year and the month is January or February,
	else it is zero. */
	day_of_week = d->year;
	day_of_week += (d->year / 4);
	day_of_week += day_of_week_month_code[d->month-1];
	day_of_week += d->day;
	if (!(d->year % 4) && d->month <= 2)
		day_of_week--;
	day_of_week --;
	day_of_week %= DAYS_PER_WEEK;
	return day_of_week;
}


/** Normalizes the current date and time. */
static void rtc_normalize (struct date *d)
{
	pinio_nvram_unlock ();

	while (d->hour >= HOURS_PER_DAY)
	{
		d->hour -= HOURS_PER_DAY;
		writeb (WPC_CLK_HOURS_DAYS, readb (WPC_CLK_HOURS_DAYS) - HOURS_PER_DAY);
		d->day++;
	}

	if (d->day > rtc_days_in_current_month (d))
	{
		d->day = 1;
		d->month++;
	}

	if (d->month > MONTHS_PER_YEAR)
	{
		d->month = 1;
		d->year++;
		/* FreeWPC stores the year in nvram as the offset from
		the year 2000; therefore, this won't overflow until
		the year 2256. */
	}

	/* Update checksums and save */
	csum_area_update (&rtc_csum_info);
	pinio_nvram_lock ();
}


/** Re-read the current date/time from the hardware */
static void rtc_hw_read (void)
{
	pinio_nvram_unlock ();
	current_date.hour = readb (WPC_CLK_HOURS_DAYS);
	current_date.minute = readb (WPC_CLK_MINS);
	csum_area_update (&rtc_csum_info);
	pinio_nvram_lock ();
}


static void rtc_hw_write (void)
{
	writeb (WPC_CLK_HOURS_DAYS, current_date.hour);
	writeb (WPC_CLK_MINS, current_date.minute);
}


/** 
 * Probe PinMAME date information and apply if present.
 */
static void rtc_pinmame_read (void)
{
	/* Do not try this in native mode. */
#ifdef CONFIG_NATIVE
	/* TBD - read from system */
	pinio_nvram_unlock ();
	current_date.year = 2011 - MIN_YEAR;
	current_date.month = 6;
	current_date.day = 1;
	pinio_nvram_lock ();
	rtc_normalize (&current_date);
#else
	struct wpc_pinmame_clock_data *clock_data;

	clock_data = (struct wpc_pinmame_clock_data *)0x1800;

	/* A valid date requires that the year be 2010 or greater. */
	if (clock_data->year >= 2010)
	{
		/* Copy the PinMAME data into FreeWPC's date structure,
		then clear the PinMAME area so we don't do this again. */
		pinio_nvram_unlock ();
		current_date.year = clock_data->year - MIN_YEAR;
		current_date.month = clock_data->month;
		current_date.day = clock_data->day;
		clock_data->year = 0;
		pinio_nvram_lock ();
		rtc_normalize (&current_date);
	}
#endif
}


void rtc_reset (void)
{
	/* Reset the date to the time at which the software
	 * was built.
	 * TODO : this should trigger a CLOCK NOT SET message */
	current_date.year = 0;
	current_date.month = 1;
	current_date.day = 1;
	current_date.hour = 0;
	current_date.minute = 0;
	last_minute = 0;
}


CALLSET_ENTRY (rtc, init)
{
	/* Once, during initialization, probe the memory that PinMAME
	puts the host system's date information.  If detected, FreeWPC
	will apply those values to its saved date information. */
	rtc_pinmame_read ();

	rtc_edit_field = RTC_FIELD_INVALID;
}


CALLSET_ENTRY (rtc, idle_every_ten_seconds)
{
	/* Re-read the timer hardware registers and normalize the values. */
	rtc_hw_read ();
	rtc_normalize (&current_date);

	/* Did the minute value change? */
	if (current_date.minute != last_minute)
	{
		/* Note: the assumption here is that the idle task will
		 * always get called at least once per minute. */
		callset_invoke (minute_elapsed);
	}
	last_minute = current_date.minute;
}


CALLSET_ENTRY (rtc, diagnostic_check)
{
	if (current_date.year == 0)
		diag_post_error ("TIME AND DATE\nNOT SET\n", COMMON_PAGE);
}


/** Render the current date to the printf buffer */
void rtc_render_date (struct date *d)
{
	extern __common__ void locale_render_date (U8, U8, U16);
	locale_render_date (d->month, d->day, MIN_YEAR+d->year);
}


/** Render the current time to the printf buffer */
void rtc_render_time (struct date *d)
{
	static const U8 rtc_us_hours[] = {
		12, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
		12, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
	};

	switch (system_config.clock_style)
	{
		case CLOCK_STYLE_AMPM:
		default:
			sprintf ("%d:%02d %s",
				rtc_us_hours[d->hour], d->minute, (d->hour >= 12)?"PM":"AM");
			break;

		case CLOCK_STYLE_24HOUR:
			sprintf ("%02d:%02d", d->hour, d->minute);
			break;
	}
}


const char *rtc_edit_field_name[] = {
#if (MACHINE_DMD == 1)
	"MONTH", "DAY", "YEAR", "HOUR", "MINUTE"
#else
	"M", "D", "Y", "h", "m"
#endif
};

#ifdef CONFIG_DMD_OR_ALPHA

void rtc_render (struct date *d)
{
#if (MACHINE_DMD == 1)
	enum day_of_week day = rtc_calc_day_of_week (d);
	sprintf ("%s", day_names[day]);
	font_render_string_center (&font_mono5, 64, 7, sprintf_buffer);
	rtc_render_date (d);
	font_render_string_center (&font_mono5, 64, 16, sprintf_buffer);
	rtc_render_time (d);
	font_render_string_center (&font_mono5, 64, 25, sprintf_buffer);
#else
	rtc_render_date (d);
	font_render_string_center (&font_mono5, 64, 10, sprintf_buffer);
	rtc_render_time (d);
	font_render_string_center (&font_mono5, 64, 20, sprintf_buffer);
#endif
	if (rtc_edit_field != RTC_FIELD_INVALID)
	{
		sprintf ("%s", rtc_edit_field_name[rtc_edit_field]);
		font_render_string_left (&font_var5, 1, 1, sprintf_buffer);
	}
}


/** Show the current date/time on the DMD */
void rtc_show_date_time (struct date *d)
{
	dmd_alloc_low_clean ();
	rtc_render (d);
	dmd_show_low ();
}

#endif


void rtc_begin_modify (void)
{
	memcpy (&edit_date, &current_date, sizeof (struct date));
	rtc_edit_field = 0;
}

void rtc_end_modify (U8 cancel_flag)
{
	rtc_edit_field = RTC_FIELD_INVALID;
	if (!cancel_flag)
	{
		pinio_nvram_unlock ();
		current_date = edit_date;
		pinio_nvram_lock ();
		rtc_hw_write ();
		timestamp_update (&system_timestamps.clock_last_set);
	}
}

void rtc_next_field (void)
{
	rtc_edit_field++;
	if (rtc_edit_field >= NUM_RTC_FIELDS)
		rtc_edit_field = 0;
}


void rtc_modify_field (U8 up_flag)
{
	struct date *d = &edit_date;
	switch (rtc_edit_field)
	{
		case RTC_FIELD_MONTH:
			if (up_flag)
				d->month++;
			else if (d->month > 1)
				d->month--;
			break;
		case RTC_FIELD_DAY:
			if (up_flag)
				d->day++;
			else if (d->day > 1)
				d->day--;
			break;
		case RTC_FIELD_YEAR:
			if (up_flag && d->year < 99)
				d->year++;
			else if (d->year > 8)
				d->year--;
			break;
		case RTC_FIELD_HOUR:
			if (up_flag && d->hour < 23)
				d->hour++;
			else if (d->hour > 0)
				d->hour--;
			rtc_hw_write ();
			break;
		case RTC_FIELD_MINUTE:
			if (up_flag && d->minute < 59)
				d->minute++;
			else if (d->minute > 0)
				d->minute--;
			rtc_hw_write ();
			break;
		default:
			break;
	}
	rtc_normalize (d);
}


CALLSET_ENTRY (rtc, file_register)
{
	file_register (&rtc_csum_info);
}


void timestamp_update (struct date *timestamp)
{
	pinio_nvram_unlock ();
	memcpy (timestamp, &current_date, sizeof (struct date));
	pinio_nvram_lock ();
}

CALLSET_ENTRY (rtc, init_complete)
{
	struct date *d = (struct date *)&system_timestamps;
	while (d < (struct date *)&system_timestamps + sizeof (system_timestamps) / sizeof (struct date))
	{
		if (!date_verify (d))
		{
			pinio_nvram_unlock ();
			memset (d, 0, sizeof (struct date));
			pinio_nvram_lock ();
		}
		d++;
	}
}

