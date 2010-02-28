/*
 * Copyright 2006, 2007, 2008, 2009 by Brian Dominy <brian@oddchange.com>
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

/* CALLSET_SECTION (powerball, __machine2__) */
#include <freewpc.h>
//#define PB_DEBUG


/* Proximity switches will trigger whenever a steel ball passes over
 * them.  The powerball is detected by the lack of such closures.
 */



typedef enum {
	PF_STEEL_DETECTED = 1,
	PF_PB_DETECTED,
	TROUGH_STEEL_DETECTED,
	TROUGH_PB_DETECTED,
} pb_event_t;

/** The general location of the powerball */
U8 pb_location;

/** If in lock or trough, the number of kicks before the 
 * powerball is back in play. */
U8 pb_depth;

/** Nonzero if the powerball has been detected and needs to be announced. */
U8 pb_announce_needed;

/** For debugging, this stores the last PB event that was asserted */
U8 last_pb_event;

#define switch_poll_trough_metal() switch_poll_logical(SW_TROUGH_PROXIMITY)

void pb_detect_deff (void)
{
#ifdef PB_DEBUG
	dmd_alloc_low_clean ();
	font_render_string_center (&font_mono5, 64, 8, "POWERBALL STATUS");

	if (pb_location == 0)
		sprintf ("MISSING");
	else if (pb_location & PB_IN_LOCK)
		sprintf ("LOCK %d", pb_depth);
	else if (pb_location & PB_IN_TROUGH)
		sprintf ("TROUGH %d", pb_depth);
	else if (pb_location & PB_IN_GUMBALL)
		sprintf ("GUMBALL %d", pb_depth);
	else if (pb_location & PB_IN_PLAY)
		sprintf ("IN PLAY");
	font_render_string_center (&font_mono5, 64, 15, sprintf_buffer);

	sprintf ("LAST EVENT %d", last_pb_event);
	font_render_string_center (&font_mono5, 64, 22, sprintf_buffer);
	
	dmd_show_low ();
	task_sleep_sec (3);
#else
#if 0
	//dmd_alloc_low_high ();
	dmd_alloc_pair ();
	dmd_clean_page_low ();
	font_render_string_center (&font_fixed10, 64, 7, "POWERBALL");
	dmd_sched_transition (&trans_vstripe_right2left);
	sound_send (SND_POWERBALL_QUOTE);
	dmd_show_low ();
	task_sleep (TIME_500MS);
	wpc_dmd_set_high_page (wpc_dmd_get_low_page () + 1);
	dmd_copy_low_to_high ();
	dmd_invert_page (dmd_low_buffer);
	deff_swap_low_high (32, TIME_100MS);
#endif
	sound_send (SND_POWERBALL_QUOTE);
	U16 fno;
	U8 i;
	/* Loop anim 6 times */
	for (i = 0;i < 5;i++)
	{
		for (fno = IMG_POWERBALL_START; fno <= IMG_POWERBALL_END; fno += 2)
		{
			dmd_alloc_pair ();
			frame_draw (fno);
			//copy text to seperate page
			//OR current page 
	//		dmd_overlay_onto_color ();
			dmd_flip_low_high ();
			font_render_string_center (&font_var5, 64, 25, "POWERBALL");
			dmd_flip_low_high ();
			dmd_show2 ();
			task_sleep (TIME_100MS);
	//		dmd_map_overlay ();
		}
	}
	//task_sleep_sec (1);
#endif
	deff_exit ();
}


void pb_loop_deff (void)
{
	generic_deff ("POWERBALL LOOP", "10,000,000");
}


CALLSET_ENTRY (pb_detect, lamp_update)
{
	if ((pb_location & PB_IN_PLAY) && !multi_ball_play ())
	{
		lamp_tristate_flash (LM_LEFT_POWERBALL);
		lamp_tristate_flash (LM_RIGHT_POWERBALL);
	}
	else if (pb_location & PB_MAYBE_IN_PLAY)
	{
		lamp_tristate_on (LM_LEFT_POWERBALL);
		lamp_tristate_on (LM_RIGHT_POWERBALL);
	}
	else
	{
		lamp_tristate_off (LM_LEFT_POWERBALL);
		lamp_tristate_off (LM_RIGHT_POWERBALL);
	}
}


/** Called when the powerball is known to be in a particular location.
 * Because there is only one powerball installed in the machine, this
 * information is definitive.  The location says where the ball is;
 * for a container device, depth says how many kicks it would take to
 * eject the powerball from it.
 *
 * PB_IN_PLAY is used when it is guaranteed to be in play, even during
 * multiball.
 *
 * PB_MAYBE_IN_PLAY is used when it might be in play, but it is uncertain.
 * It *was* in play prior to some event during multiball.
 */
void pb_set_location (U8 location, U8 depth)
{
	if (pb_location != location)
	{
		pb_location = location;
		if (pb_location & PB_HELD)
		{
			pb_depth = depth;
			pb_announce_needed = 0;
		}
		else if (pb_location & PB_IN_PLAY)
		{
			flag_on (FLAG_POWERBALL_IN_PLAY);
			pb_announce_needed = 1;
			callset_invoke (powerball_present);
		}
		else if (pb_location & PB_MAYBE_IN_PLAY)
		{
			flag_off (FLAG_POWERBALL_IN_PLAY);
			pb_announce_needed = 0;
			callset_invoke (powerball_lost);
			/* TODO - in the 'maybe' state, pulse magnets to
			figure out the ball type */
		}
	}
}

/** Called when the powerball is known *NOT* to be in a particular 
 * location.  If that's where we thought the powerball was before,
 * then it's missing.  Otherwise, nothing is learned. */
void pb_clear_location (U8 location)
{
	if (pb_location == location)
	{
		pb_location = PB_MISSING;
		flag_off (FLAG_POWERBALL_IN_PLAY);
		pb_announce_needed = 0;
		callset_invoke (powerball_absent);
		/* TODO : music is not being stopped correctly if Powerball
		drains during multiball and game doesn't know where it is. */
		music_refresh ();
#ifdef PB_DEBUG
		deff_restart (DEFF_PB_DETECT);
#else
		deff_stop (DEFF_PB_DETECT);
#endif
	}
}


/** Asserts a powerball detection event.  The significance depends on
 * the current state.
 * Because proximity sensors trigger only when steel balls move over them
 * (assuming they are working correctly), we can trust an assertion of
 * steel ball a little more than one about the Powerball. */
void pb_detect_event (pb_event_t event)
{
	last_pb_event = event;
	switch (event)
	{
		/* Steel ball detected on playfield, via Slot Proximity */
		case PF_STEEL_DETECTED:
			if (single_ball_play ())
				pb_clear_location (PB_IN_PLAY);
				pb_clear_location (PB_MAYBE_IN_PLAY);
#ifdef PB_DEBUG
			else
				pb_clear_location (0);
#endif
			break;

		/* Powerball detected on playfield, because Slot Proximity
		 * did not trigger when a ball had to travel over it. */
		case PF_PB_DETECTED:
			pb_set_location (PB_IN_PLAY, 0);
			break;

		case TROUGH_STEEL_DETECTED:
			if (device_entry (DEVNO_TROUGH)->actual_count == 1)
				pb_clear_location (PB_IN_TROUGH);
#ifdef PB_DEBUG
			else
				pb_clear_location (0);
#endif
			break;

		case TROUGH_PB_DETECTED:
			pb_set_location (PB_IN_TROUGH, 1);
			break;
	}
}


/** Announce that the powerball is in play, if it is.  This is
only called at certain points when we want to announce this.
The powerball may have been detected sometime earlier. */
void pb_announce (void)
{
	if (pb_announce_needed)
	{
#ifdef PB_DEBUG
		deff_restart (DEFF_PB_DETECT);
#else
		deff_start (DEFF_PB_DETECT);
#endif
		music_refresh ();
		pb_announce_needed = 0;
	}
}


/** Poll the trough proximity switch to see if the next ball
to be served is a steel ball or the powerball.   This is
called anytime the trough changes in some way. */
void pb_poll_trough (void)
{
	if (switch_poll_logical (SW_RIGHT_TROUGH))
	{
		if (switch_poll_trough_metal ())
		{
			pb_detect_event (TROUGH_STEEL_DETECTED);
			dbprintf ("pb: trough steel\n");
		}
		else
		{
			pb_detect_event (TROUGH_PB_DETECTED);
			dbprintf ("pb: trough white ball\n");
		}
	}
	else
	{
		dbprintf ("pb: trough empty\n");
	}
}


/** Called when a ball enters the trough or the lock. */
void pb_container_enter (U8 location, U8 devno)
{
	device_t *dev = device_entry (devno);

	/* If the powerball is known to be in play, then the act
	of a ball entering a device is significant. */
	if (pb_location == PB_IN_PLAY)
	{
		if (single_ball_play ())
		{
			/* In single ball play, things are fairly deterministic.
			 * We know the powerball is no longer in play, and it is in
			 * the device it just entered at a specific location.
			 *
			 * It is also possible that we might kick out the 
			 * Powerball immediately from the same device, when it is
			 * the only ball in the device.  Changing
			 * the state here and then back again will cause the
			 * powerball to be reannounced.  So optimize this by not
			 * doing anything.
			 */
			if (dev->max_count != 1)
			{
				/* Powerball will be kept here */
				pb_clear_location (PB_IN_PLAY);
				pb_set_location (location, dev->actual_count);
			}
		}
		else
		{
			/* In multiball, container enter might mean the
			powerball entered it or not... we just don't
			know */
			pb_set_location (PB_MAYBE_IN_PLAY, 0);
		}
	}
	else
	{
		/* If the Powerball is not known to be in play, then a container enter
		event isn't important.  (Either it's in a device somewhere or
		we're in the "maybe" state.) */
	}
}


/** Called when a ball successfully exits the trough or the lock.
 * If the powerball was known to be in the device, then its depth
 * shifts down by 1, and it may be in play now. */
void pb_container_exit (U8 location)
{
	if (pb_location == location)
	{	
		if (--pb_depth == 0)
		{
			pb_set_location (PB_IN_PLAY, 0);
			pb_announce ();
		}
	}

}

CALLSET_ENTRY (pb_detect, music_refresh)
{
	if (pb_location == PB_IN_PLAY && !multi_ball_play ())
		music_request (MUS_POWERBALL_IN_PLAY, PRI_GAME_MODE3);
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
	event_did_follow (camera_or_piano, slot_prox);
	/* TODO : if this switch triggers and we did not expect
	 * a ball in the undertrough....??? */
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

CALLSET_ENTRY (pb_detect, dev_trough_enter)
{
	/* Note: during the call to enter, live_balls has not been updated
	and reflects the value prior to the ball entering the device. */
	dbprintf ("PB: trough entered\n");
	pb_container_enter (PB_IN_TROUGH, DEVNO_TROUGH);
	pb_poll_trough ();
}

CALLSET_ENTRY (pb_detect, dev_lock_enter)
{
	pb_container_enter (PB_IN_LOCK, DEVNO_LOCK);
}

CALLSET_ENTRY (pb_detect, dev_trough_kick_attempt)
{
	dbprintf ("PB: about to kick trough\n");
	pb_poll_trough ();
}

CALLSET_ENTRY (pb_detect, dev_trough_kick_success)
{
	dbprintf ("PB: trough kick OK\n");
	pb_container_exit (PB_IN_TROUGH);
	pb_poll_trough ();
}

CALLSET_ENTRY (pb_detect, dev_lock_kick_success)
{
	pb_container_exit (PB_IN_LOCK);
}


CALLSET_ENTRY (pb_detect, start_ball)
{
	pb_clear_location (PB_IN_PLAY);
}


CALLSET_ENTRY (pb_detect, init)
{
	pb_location = PB_MISSING;
	last_pb_event = 0;
	pb_poll_trough ();
}

