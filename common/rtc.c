/*
 * Copyright 2006-2010 by Brian Dominy <brian@oddchange.com>
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


/** The memory state of the realtime clock.  These variables
hold the 'base time' that is not counted in the ASIC, plus
a snapshot of the last value that was read from the ASIC. */
struct rtc_state
{
	U8 year;
	U8 month;
	U8 day;
	U8 hour;
	U8 min;
	U8 day_of_week;
};


/** The editable fields of the date/time. */
#define RTC_FIELD_MONTH 0
#define RTC_FIELD_DAY 1
#define RTC_FIELD_YEAR 2
#define RTC_FIELD_HOUR 3
#define RTC_FIELD_MINUTE 4
#define NUM_RTC_FIELDS 5


/* Year is stored as an offset from 2000 */
__nvram__ U8 year;
__nvram__ U8 month;
__nvram__ U8 day;
__nvram__ U8 hour;
static __nvram__ U8 day_of_week;

U8 rtc_edit_field;

struct date_time_backup
{
	U8 year;
	U8 month;
	U8 day;
	U8 hour;
	U8 min;
} backup;


/** Checksum descriptor for the RTC info */
static __nvram__ U8 rtc_csum;
const struct area_csum rtc_csum_info = {
	.area = &year,
	.length = 5,
	.csum = &rtc_csum,
	.reset = rtc_factory_reset,
	.reset_page = PAGE,
};

U8 minute;

/** Keeps track of when the minute changes, for auditing */
static U8 last_minute;


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


/** Returns the number of days in the current month.
 * This handles leap years in February correctly until 2100,
 * which is not be a leap year but will be detected as such. */
static U8 rtc_days_in_current_month (void)
{
	U8 days = days_in_month_table[month-1];
	/* Handle leap years */
	if ((month == 2) && ((year % 4) == 0))
		days++;
	return (days);
}


/** Calculate the day of the week (0=Sunday, 6=Saturday)
from the current values of year, month, and day. */
static void rtc_calc_day_of_week (void)
{
	static U8 day_of_week_month_code[] = {
		0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5
	};

	/* Compute (6 + year + (year/4) + month code + day - N) mod 7.
	N is 1 if it is a leap year and the month is January or February,
	else it is zero. */
	day_of_week = year;
	day_of_week += (year / 4);
	day_of_week += day_of_week_month_code[month-1];
	day_of_week += day;
	if (!(year % 4) && month <= 2)
		day_of_week--;
	day_of_week --;

	/* The mod 7 is the hard part to do on the 6809.
	 * The technique used here is to do repeated subtraction.
	 * For values larger than 64, subtract 63 at a time to speed up
	 * the computation. */
#ifndef __m6809__
	day_of_week %= 7;
#else
	while (day_of_week >= 7)
	{
		if (day_of_week > 64)
			day_of_week -= (64 - 1);
		else
			day_of_week -= 7;
	}
#endif
}


/** Normalizes the current date and time. */
static void rtc_normalize (void)
{
	pinio_nvram_unlock ();

	while (hour >= 24)
	{
		hour -= 24;
		writeb (WPC_CLK_HOURS_DAYS, readb (WPC_CLK_HOURS_DAYS) - 24);
		day++;
	}

	if (day > rtc_days_in_current_month ())
	{
		day = 1;
		month++;
	}

	if (month > 12)
	{
		month = 1;
		year++;
		/* FreeWPC stores the year in nvram as the offset from
		the year 2000; therefore, this won't overflow until
		the year 2256. */
	}

	rtc_calc_day_of_week ();

	/* Perform sanity checks, just in case. */
	if ((month < 1) || (month > 12))
		month = 1;
	if ((day < 1) || (day > 31))
		day = 1;

	/* Update checksums and save */
	csum_area_update (&rtc_csum_info);
	pinio_nvram_lock ();
}


/** Re-read the current date/time from the hardware */
static void rtc_hw_read (void)
{
	pinio_nvram_unlock ();
	hour = readb (WPC_CLK_HOURS_DAYS);
	minute = readb (WPC_CLK_MINS);
	csum_area_update (&rtc_csum_info);
	pinio_nvram_lock ();
}


static void rtc_hw_write (void)
{
	writeb (WPC_CLK_HOURS_DAYS, hour);
	writeb (WPC_CLK_MINS, minute);
}


/** 
 * Probe PinMAME date information and apply if present.
 */
static void rtc_pinmame_read (void)
{
	/* Do not try this in native mode. */
#ifndef CONFIG_NATIVE
	struct wpc_pinmame_clock_data *clock_data;

	clock_data = (struct wpc_pinmame_clock_data *)0x1800;

	/* A valid date requires that the year be 2010 or greater. */
	if (clock_data->year >= 2010)
	{
		/* Copy the PinMAME data into FreeWPC's date structure,
		then clear the PinMAME area so we don't do this again. */
		pinio_nvram_unlock ();
		year = clock_data->year - 2000;
		month = clock_data->month;
		day = clock_data->day;
		clock_data->year = 0;
		pinio_nvram_lock ();
		rtc_normalize ();
	}
#endif
}


CALLSET_ENTRY (rtc, factory_reset)
{
	/* Reset the date to the time at which the software
	 * was built.
	 * TODO : this should trigger a CLOCK NOT SET message */
	pinio_nvram_unlock ();
	year = 0;
	month = 1;
	day = 1;
	hour = 0;
	minute = 0;
	last_minute = 0;
	rtc_calc_day_of_week ();
	pinio_nvram_lock ();
}


CALLSET_ENTRY (rtc, init)
{
	/* Once, during initialization, probe the memory that PinMAME
	puts the host system's date information.  If detected, FreeWPC
	will apply those values to its saved date information. */
	rtc_pinmame_read ();

	rtc_edit_field = 0xFF;
}


CALLSET_ENTRY (rtc, idle_every_ten_seconds)
{
	/* Re-read the timer hardware registers and normalize the values. */
	rtc_hw_read ();
	rtc_normalize ();

	/* Did the minute value change? */
	if (minute != last_minute)
	{
		/* Note: the assumption here is that the idle task will
		 * always get called at least once per minute. */
		callset_invoke (minute_elapsed);
	}
	last_minute = minute;
}


CALLSET_ENTRY (rtc, diagnostic_check)
{
	if (year == 0)
		diag_post_error ("TIME AND DATE\nNOT SET\n", COMMON_PAGE);
}


/** Render the current date to the printf buffer */
void rtc_render_date (void)
{
	extern __common__ void locale_render_date (U8, U8, U16);
	locale_render_date (month, day, 2000+year);
}


/** Render the current time to the printf buffer */
void rtc_render_time (void)
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
				rtc_us_hours[hour], minute, (hour >= 12)?"PM":"AM");
			break;

		case CLOCK_STYLE_24HOUR:
			sprintf ("%02d:%02d", hour, minute);
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


void rtc_render (void)
{
#if (MACHINE_DMD == 1)
	sprintf ("%s", day_names[day_of_week]);
	font_render_string_center (&font_mono5, 64, 7, sprintf_buffer);
	rtc_render_date ();
	font_render_string_center (&font_mono5, 64, 16, sprintf_buffer);
	rtc_render_time ();
	font_render_string_center (&font_mono5, 64, 25, sprintf_buffer);
#else
	rtc_render_date ();
	font_render_string_center (&font_mono5, 64, 10, sprintf_buffer);
	rtc_render_time ();
	font_render_string_center (&font_mono5, 64, 20, sprintf_buffer);
#endif
	if (rtc_edit_field != 0xFF)
	{
		sprintf ("%s", rtc_edit_field_name[rtc_edit_field]);
		font_render_string_left (&font_var5, 1, 1, sprintf_buffer);
	}
}


/** Show the current date/time on the DMD */
void rtc_show_date_time (void)
{
	dmd_alloc_low_clean ();
	rtc_render ();
	dmd_show_low ();
}


void rtc_begin_modify (void)
{
	rtc_edit_field = 0;
}

void rtc_end_modify (U8 cancel_flag)
{
	rtc_edit_field = 0xFF;
}

void rtc_next_field (void)
{
	rtc_edit_field++;
	if (rtc_edit_field >= 5)
		rtc_edit_field = 0;
}


void rtc_modify_field (U8 up_flag)
{
	pinio_nvram_unlock ();

	switch (rtc_edit_field)
	{
		case RTC_FIELD_MONTH:
			if (up_flag)
				month++;
			else if (month > 1)
				month--;
			break;
		case RTC_FIELD_DAY:
			if (up_flag)
				day++;
			else if (day > 1)
				day--;
			break;
		case RTC_FIELD_YEAR:
			if (up_flag && year < 99)
				year++;
			else if (year > 8)
				year--;
			break;
		case RTC_FIELD_HOUR:
			if (up_flag && hour < 23)
				hour++;
			else if (hour > 0)
				hour--;
			rtc_hw_write ();
			break;
		case RTC_FIELD_MINUTE:
			if (up_flag && minute < 59)
				minute++;
			else if (minute > 0)
				minute--;
			rtc_hw_write ();
			break;
	}

	pinio_nvram_lock ();
	rtc_normalize ();
}

