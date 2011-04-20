/*
 * Copyright 2011 by Brian Dominy <brian@oddchange.com>
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

#ifndef __DEFFDATA_H
#define __DEFFDATA_H

#define MAX_DEFF_DATA 10

extern U8 deff_data_pending[];
extern U8 deff_data_pending_count;
extern U8 deff_data_active[];
extern U8 deff_data_active_count;

/**
 * Renitialize display effect data.
 * Called prior to deff_start() for any effect where D_SCORE is set.
 */
#define deff_data_init() \
	deff_data_pending_count = 0;

/**
 * Push a data item to the effect.
 * Called multiple times, after deff_init(), to send data to the effect for
 * display.  These calls should *immediately* preceed deff_start() to ensure
 * that the right values get shown.
 */
#define deff_data_push(var) \
	do { \
		memcpy (deff_data_pending + deff_data_pending_count, &var, sizeof(var)); \
		deff_data_pending_count += sizeof (var); \
	} while (0)

/**
 * Load pushed data into a running effect.
 * This is called by the OS whenever a new effect actually starts.
 * deff_start() does not guarantee that the effect will run; if low priority,
 * the pushed items will simply be discarded, and not affect the current deff.
 */
#define deff_data_load() \
	do { \
		memcpy (deff_data_active, deff_data_pending, deff_data_pending_count); \
		deff_data_active_count = 0; \
	} while (0)

/**
 * Pull a data item for display.  This is called by the running display
 * effect.  Items should be pulled in the same order that they were pushed
 * (i.e. a first-in, first-out queue).
 */
#define deff_data_pull(var) \
	do { \
		memcpy (&var, deff_data_active + deff_data_active_count, sizeof(var)); \
		deff_data_active_count += sizeof (var); \
	} while (0)

/**
 * Like deff_data_pull(), but only returns a pointer to the item, and does not
 * copy it.
 */
#define deff_data_pull_pointer(ptr) \
	do { \
		ptr = deff_data_active + deff_data_active_count; \
		deff_data_active_count += sizeof (*ptr); \
	} while (0)

#endif /* __DEFFDATA_H */
