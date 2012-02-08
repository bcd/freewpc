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

#ifndef _RTC_H
#define _RTC_H

#define HOURS_PER_DAY 24
#define DAYS_PER_WEEK 7
#define MAX_DAYS_PER_MONTH 31
#define MONTHS_PER_YEAR 12
#define MIN_YEAR 2000

enum day_of_week
{
	SUNDAY,
	MONDAY,
	TUESDAY,
	WEDNESDAY,
	THURSDAY,
	FRIDAY,
	SATURDAY,
};

struct date
{
	U8 year;
	U8 month;
	U8 day;
	U8 hour;
	U8 minute;
};

extern struct date current_date;
extern struct date edit_date;

__common__ bool date_verify (struct date *);
__common__ void rtc_reset (void);
__common__ void rtc_render_date (struct date *);
__common__ void rtc_render_time (struct date *);
__common__ void rtc_show_date_time (struct date *);
__common__ void rtc_begin_modify (void);
__common__ void rtc_end_modify (U8 cancel_flag);
__common__ void rtc_next_field (void);
__common__ void rtc_modify_field (U8 up_flag);
#ifndef timestamp_update
__common__ void timestamp_update (struct date *timestamp);
#endif

#endif /* _RTC_H */
