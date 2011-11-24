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

CALLSET_ENTRY (trough, dev_trough_enter)
{
	/* Audit center drains (outlanes are audited in their respective
	 * switches) */
	if (!event_did_follow (any_outlane, center_drain))
	{
		audit_increment (&system_audits.center_drains);
	}

	/* Note that there is one less ball in play now */
	device_remove_live ();
}


CALLSET_ENTRY (trough, dev_trough_kick_success)
{
	/* Note that there is one more ball in play now */
	device_add_live ();
}

