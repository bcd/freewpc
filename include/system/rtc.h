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

#ifndef _RTC_H
#define _RTC_H

__common__ void rtc_factory_reset (void);
__common__ void rtc_render_date (void);
__common__ void rtc_render_time (void);
__common__ void rtc_show_date_time (void);
__common__ void rtc_begin_modify (void);
__common__ void rtc_end_modify (U8 cancel_flag);
__common__ void rtc_next_field (void);
__common__ void rtc_modify_field (U8 up_flag);

#endif /* _RTC_H */
