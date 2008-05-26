/*
 * Copyright 2006, 2007, 2008 by Brian Dominy <brian@oddchange.com>
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

/**
 * \file
 * \brief The driver for the WPC real-time clock (RTC).
 */


/** PinMAME expects address 0x1800 in nvram to contain the
 * following date structure, as apparently all real WPC
 * ROMs put this information there. */
#ifdef CONFIG_PINMAME
struct wpc_pinmame_clock_data
{
	U16 year;
	U8 month;
	U8 day;
	U8 day_of_week;
};
#endif


/* Year is stored as an offset from 2000 */
static __nvram__ U8 year;
static __nvram__ U8 month;
static __nvram__ U8 day;
static __nvram__ U8 hour;
static __nvram__ U8 day_of_week;

U8 rtc_edit_field;

static struct date_time_backup
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
#ifdef HAVE_PAGING
	.reset_page = PAGE,
#endif
};

static U8 minute;

/** Keeps track of when the minute changes, for auditing */
static U8 last_minute;


static U8 days_in_month_table[] = {
	31, 28, 31, 30, 31, 30,
	31, 31, 30, 31, 30, 31,
};

static const char *day_names[] = {
	"SUNDAY", "MONDAY", "TUESDAY", "WEDNESDAY",
	"THURSDAY", "FRIDAY", "SATURDAY"
};


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
	day_of_week	+= ((year % 4) ? 0 : 1);
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
	wpc_nvram_get ();

	while (hour >= 24)
	{
		hour -= 24;
		wpc_asic_write (WPC_CLK_HOURS_DAYS,
			wpc_asic_read (WPC_CLK_HOURS_DAYS) - 24);
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
	wpc_nvram_put ();
}


/** Re-read the current date/time from the hardware */
static void rtc_hw_read (void)
{
	wpc_nvram_get ();
	hour = wpc_asic_read (WPC_CLK_HOURS_DAYS);
	minute = wpc_asic_read (WPC_CLK_MINS);
	csum_area_update (&rtc_csum_info);
	wpc_nvram_put ();
}


static void rtc_hw_write (void)
{
	writeb (WPC_CLK_HOURS_DAYS, hour);
	writeb (WPC_CLK_MINS, minute);
}


/** Re-read pinmame's simulated time values.  This contains the
 * year, month, and day as determined from the simulator's
 * operating system.  On real hardware, this value would need
 * to be configured in the utilities menu. */
#ifdef CONFIG_PINMAME
static void rtc_pinmame_read (void)
{
	struct wpc_pinmame_clock_data *clock_data;

	clock_data = (struct wpc_pinmame_clock_data *)0x1800;
	if (clock_data->year >= 2000)
	{
		wpc_nvram_get ();
		year = clock_data->year - 2000;
		month = clock_data->month;
		day = clock_data->day;
		rtc_calc_day_of_week ();
		rtc_normalize ();
		csum_area_update (&rtc_csum_info);
		wpc_nvram_put ();
	}
}
#endif


void rtc_factory_reset (void)
{
	/* Reset the date to Jan. 1, 2006 */
	year = 6;
	month = 1;
	day = 1;
	hour = 0;
	minute = 0;
	last_minute = 0;
	rtc_calc_day_of_week ();
}


CALLSET_ENTRY (rtc, init)
{
#ifdef CONFIG_PINMAME
	/* Once, during initialization, read the values of year, month, and
	day from memory locations that PinMAME writes.  It gets these
	from the system on which it is running.  Afterwards, FreeWPC will
	increment these correctly.  (And therefore, clock changes on the
	host system are ignored.) */
	rtc_pinmame_read ();
#elif defined (CONFIG_NATIVE)
	/* TODO - should read this from the native system */
	wpc_nvram_get ();
	year = 2008;
	month = 4;
	day = 1;
	rtc_calc_day_of_week ();
	rtc_normalize ();
	csum_area_update (&rtc_csum_info);
	wpc_nvram_put ();
#endif
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

	switch (system_config.date_style)
	{
		case DATE_TIME_STYLE_US:
		default:
			sprintf ("%d:%02d %s", 
				rtc_us_hours[hour], minute, (hour >= 12)?"PM":"AM");
			break;

		case DATE_TIME_STYLE_EURO:
			sprintf ("%02d:%02d", hour, minute);
			break;
	}
}


const char *rtc_edit_field_name[] = {
	"MONTH", "DAY", "YEAR", "HOUR", "MINUTE"
};


/** Show the current date/time on the DMD */
void rtc_show_date_time (void)
{
	dmd_alloc_low_clean ();

	sprintf ("%s", day_names[day_of_week]);
	font_render_string_center (&font_mono5, 64, 7, sprintf_buffer);
	rtc_render_date ();
	font_render_string_center (&font_mono5, 64, 16, sprintf_buffer);
	rtc_render_time ();
	font_render_string_center (&font_mono5, 64, 25, sprintf_buffer);

	if (rtc_edit_field != 0xFF)
	{
		sprintf ("%s", rtc_edit_field_name[rtc_edit_field]);
		font_render_string_left (&font_var5, 1, 1, sprintf_buffer);
	}

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
	wpc_nvram_get ();

	switch (rtc_edit_field)
	{
		case 0: /* month */
			if (up_flag)
				month++;
			else if (month > 1)
				month--;
			break;
		case 1: /* day */
			if (up_flag)
				day++;
			else if (day > 1)
				day--;
			break;
		case 2: /* year */
			if (up_flag)
				year++;
			else if (year > 0)
				year--;
			break;
		case 3: /* hour */
			if (up_flag && hour < 23)
				hour++;
			else if (hour > 0)
				hour--;
			rtc_hw_write ();
			break;
		case 4: /* minute */
			if (up_flag && minute < 59)
				minute++;
			else if (minute > 0)
				minute--;
			rtc_hw_write ();
			break;
	}

	wpc_nvram_put ();
	rtc_normalize ();
}

