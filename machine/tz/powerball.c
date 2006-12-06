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

/* Proximity switches will trigger whenever a steel ball passes over
 * them.  The powerball is detected by the lack of such closures.
 */

typedef enum {
	PB_MISSING,
	PB_IN_LOCK,
	PB_IN_TROUGH,
	PB_IN_GUMBALL,
	PB_IN_PLAY,
	PB_MAYBE_IN_PLAY,
} pb_location_t;

typedef enum {
	PF_STEEL_DETECTED,
	PF_PB_DETECTED,
	TROUGH_STEEL_DETECTED,
	TROUGH_PB_DETECTED,
} pb_event_t;

/** The general location of the powerball */
pb_location_t pb_location;

/** If in lock or trough, the number of kicks before the 
 * powerball is back in play. */
U8 pb_depth;

U8 pb_announce_needed;

#define switch_poll_trough_metal(sw)		switch_poll_logical(SW_TROUGH_PROXIMITY)
#define switch_poll_trough_powerball(sw)	(!switch_poll_logical(SW_TROUGH_PROXIMITY))

void pb_set_location (pb_location_t location, U8 depth)
{
}

void pb_detect_event (pb_event_t event)
{
	switch (event)
	{
		case PF_STEEL_DETECTED:
			if (live_balls == 1)
			{
				if ((pb_location == PB_IN_PLAY) || (pb_location == PB_MAYBE_IN_PLAY))
					pb_set_location (PB_MISSING, 0);
			}
			break;

		case PF_PB_DETECTED:
			pb_set_location (PB_IN_PLAY, 0);
			break;

		case TROUGH_STEEL_DETECTED:
			break;

		case TROUGH_PB_DETECTED:
			pb_set_location (PB_IN_TROUGH, 1);
			break;
	}
}

void pb_announce (void)
{
	if (pb_announce_needed)
	{
	}
}

void pb_poll_trough (void)
{
	if (switch_poll_trough_metal ())
	{
		pb_detect_event (TROUGH_STEEL_DETECTED);
	}
	else
	{
		pb_detect_event (TROUGH_PB_DETECTED);
	}
}

CALLSET_ENTRY (pb_detect, init)
{
	pb_location = PB_MISSING;
	pb_depth = 0;
}

CALLSET_ENTRY (pb_detect, sw_camera)
{
	event_can_follow (camera_or_piano, slot_prox, TIME_5S);
}

CALLSET_ENTRY (pb_detect, sw_piano)
{
	event_can_follow (camera_or_piano, slot_prox, TIME_5S);
}

CALLSET_ENTRY (pb_detect, sw_slot_proximity)
{
	/* Ball in tunnel is not the powerball */
	/* If balls in play == 1, then powerball is definitely not in play */
	event_did_follow (camera_or_piano, slot_prox);
	pb_detect_event (PF_STEEL_DETECTED);
}

CALLSET_ENTRY (pb_detect, dev_slot_enter)
{
	if (event_did_follow (camera_or_piano, slot_prox))
	{
		/* Proximity sensor did not trip ; must be the powerball */
		pb_detect_event (PF_PB_DETECTED);
	}
	pb_announce ();
}

CALLSET_ENTRY (pb_detect, start_game)
{
	pb_poll_trough ();
}

CALLSET_ENTRY (pb_detect, trough_kick_success)
{
	pb_announce ();
	pb_poll_trough ();
}


