/*
 * Copyright 2006 by Brian Dominy <brian@oddchange.com>
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


/** PinMAME expects address 0x1800 in nvram to contain the
 * following date structure, as apparently all real WPC
 * ROMs put this information there. */
struct wpc_pinmame_clock_data
{
	U16 year;
	U8 month;
	U8 day;
	U8 day_of_week;
};


/* Year is stored as an offset from 2000 */
static __nvram__ U8 year;
static __nvram__ U8 month;
static __nvram__ U8 day;
static __nvram__ U8 hour;
static __nvram__ U8 checksum;
static U8 minute;

/** Keeps track of when the minute changes, for auditing */
static U8 last_minute;


static U8 days_in_month_table[] = {
	31, 28, 31, 30, 31, 30,
	31, 31, 30, 31, 30, 31,
};

static const char *month_names[] = {
	"JAN.", "FEB.", "MAR.", "APR.", "MAY", "JUN.",
	"JUL.", "AUG.", "SEP.", "OCT.", "NOV.", "DEC.",
};


static void rtc_checksum_update (void)
{
	/* TODO */
	checksum = 0;
}

void rtc_factory_reset (void)
{

	/* Reset the date to Jan. 1, 2006 */
	wpc_nvram_get ();
	year = 6;
	month = 1;
	day = 1;
	hour = 0;
	minute = 0;
	wpc_nvram_put ();
	rtc_checksum_update ();
	last_minute = 0;
}


static U8 rtc_days_in_current_month (void)
{
	U8 days = days_in_month_table[month-1];
	/* Handle leap years */
	if ((month == 2) && ((year % 4) == 0))
		days++;
	return (days);
}


static void rtc_normalize (void)
{
	wpc_nvram_get ();
	while (hour > 24)
	{
		hour -= 24;
		(*(volatile U8 *)WPC_CLK_HOURS_DAYS) -= 24;

		day++;
		if (day > rtc_days_in_current_month ())
		{
			day = 1;
			month++;
			if (month > 12)
			{
				month = 1;
				year++;
			}
		}
	}

	if ((month < 1) || (month > 12))
		month = 1;
	if ((day < 1) || (day > 31))
		day = 1;
	wpc_nvram_put ();
}

static void rtc_hw_read (void)
{
	wpc_nvram_get ();
	hour = *(volatile U8 *)WPC_CLK_HOURS_DAYS;
	minute = *(volatile U8 *)WPC_CLK_MINS;
	rtc_checksum_update ();
	wpc_nvram_put ();
}


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
		wpc_nvram_put ();
	}
}


void rtc_idle_task (void)
{
	rtc_hw_read ();
	rtc_normalize ();

	if (minute != last_minute)
	{
		audit_increment (&system_audits.minutes_on);
		rtc_pinmame_read ();
	}
	last_minute = minute;
}


void rtc_render_date (void)
{
	switch (system_config.clock_style)
	{
		case DATE_TIME_STYLE_US:
		default:
			sprintf ("%s %d, 20%02d", month_names[month-1], day, year);
			break;

		case DATE_TIME_STYLE_EURO:
			sprintf ("%d %s 20%02d", day, month_names[month-1], year);
			break;
	}
}


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
				rtc_us_hours[hour], minute, (hour > 12)?"PM":"AM");
			break;

		case DATE_TIME_STYLE_EURO:
			sprintf ("%02d:%02d", hour, minute);
			break;
	}
}


void rtc_show_date_time (void)
{
	dmd_alloc_low_clean ();

	rtc_render_date ();
	font_render_string_center (&font_mono5, 64, 11, sprintf_buffer);
	rtc_render_time ();
	font_render_string_center (&font_mono5, 64, 21, sprintf_buffer);

	dmd_show_low ();
}


void rtc_init (void)
{
}

