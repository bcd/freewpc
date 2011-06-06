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

#include <freewpc.h>
#include <status.h>

//#define PB_DEBUG


/* Proximity switches will trigger whenever a steel ball passes over
 * them.  The powerball is detected by the lack of such closures.
 */

/* Each of these represents a possible state for the powerball
 * detector.  They are mutually exclusive, but expressed as bitmasks
 * so that ranges can be tested more easily. */

#define PB_MISSING       0x0
#define PB_IN_LOCK       0x1
#define PB_IN_TROUGH     0x2
#define PB_IN_GUMBALL    0x4
#define PB_IN_PLAY       0x8
#define PB_MAYBE_IN_PLAY 0x10
#define PB_HELD         (PB_IN_LOCK | PB_IN_TROUGH | PB_IN_GUMBALL)
#define PB_KNOWN			(PB_HELD | PB_IN_PLAY)



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

CALLSET_ENTRY (pb_detect, status_report)
{
	status_page_init ();
	font_render_string_center (&font_mono5, 64, 8, "POWERBALL LOCATION");

	if (pb_location == 0)
		sprintf ("UNKNOWN");
	else if (pb_location & PB_IN_LOCK)
		sprintf ("LOCK %d DEEP", pb_depth);
	else if (pb_location & PB_IN_TROUGH)
		sprintf ("TROUGH %d DEEP", pb_depth);
	else if (pb_location & PB_IN_GUMBALL)
		sprintf ("GUMBALL %d DEEP", 2 - pb_depth);
	else if (pb_location & PB_IN_PLAY)
		sprintf ("IN PLAY");
	font_render_string_center (&font_mono5, 64, 15, sprintf_buffer);
	status_page_complete ();
}

void pb_detect_deff (void)
{
	sound_send (SND_POWERBALL_QUOTE);
	if (!multi_ball_play ())
		leff_start (LEFF_POWERBALL_ANNOUNCE);
	U16 fno;
	U8 i;
	/* Loop anim 6 times */
	for (i = 0;i < 5;i++)
	{
		dmd_alloc_pair_clean ();
		bool on = TRUE;
		for (fno = IMG_POWERBALL_START; fno <= IMG_POWERBALL_END; fno += 2)
		{
			dmd_map_overlay ();
			dmd_clean_page_low ();
			if (on)
			{
				//font_render_string_center (&font_var5, 64, 26, "POWERBALL");
				font_render_string_center (&font_var5, 64, 16, "POWERBALL");
				on = FALSE;
			}
			else
			{
				font_render_string_center (&font_fixed6, 64, 16, "POWERBALL");
				on = TRUE;
			}
			dmd_text_outline ();
			dmd_alloc_pair ();
			frame_draw (fno);
			dmd_overlay_outline ();
			dmd_show2 ();
			task_sleep (TIME_66MS);
		}
	}
	deff_exit ();
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
			global_flag_off (GLOBAL_FLAG_POWERBALL_IN_PLAY);
			pb_depth = depth;
			pb_announce_needed = 0;
			magnet_disable_catch (MAG_LEFT);
			magnet_disable_catch (MAG_RIGHT);
		}
		else if (pb_location & PB_IN_PLAY)
		{
			global_flag_on (GLOBAL_FLAG_POWERBALL_IN_PLAY);
			pb_announce_needed = 1;
			callset_invoke (powerball_present);
			/* Turn the magnets on to help with detection */
			if (single_ball_play ())
			{
				magnet_enable_catch (MAG_LEFT);
				magnet_enable_catch (MAG_RIGHT);
			}
		}
		else if (pb_location & PB_MAYBE_IN_PLAY)
		{
			global_flag_off (GLOBAL_FLAG_POWERBALL_IN_PLAY);
			pb_announce_needed = 0;
			callset_invoke (powerball_lost);
			/* in the 'maybe' state, try to grab ball with
			 * magnets to figure out the ball type
			 * This is done by maghelper.c */
		}
	}
}

/* Used by maghelper.c to turn on the magnets or not */
bool pb_maybe_in_play (void)
{
	if (single_ball_play () 
		&& pb_location & PB_MAYBE_IN_PLAY)
		return TRUE;
	else
		return FALSE;
}

/* Used by multiball.c to see if we should lock the pb */
bool pb_in_lock (void)
{
	if (pb_location & PB_IN_LOCK)
		return TRUE;
	else
		return FALSE;
}

/** Called when the powerball is known *NOT* to be in a particular 
 * location.  If that's where we thought the powerball was before,
 * then it's missing.  Otherwise, nothing is learned. */
void pb_clear_location (U8 location)
{
	if (pb_location == location)
	{
		pb_location = PB_MISSING;
		global_flag_off (GLOBAL_FLAG_POWERBALL_IN_PLAY);
		pb_announce_needed = 0;
		callset_invoke (powerball_absent);
		/* TODO : music is not being stopped correctly if Powerball
		drains during multiball and game doesn't know where it is. */
		music_update ();
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
			/* Disable the magnet catches used for PB detection */
			magnet_disable_catch (MAG_LEFT);
			magnet_disable_catch (MAG_RIGHT);
			if (single_ball_play ())
			{
				pb_clear_location (PB_IN_PLAY);
				pb_clear_location (PB_MAYBE_IN_PLAY);
			}
			else if (pb_location & PB_IN_GUMBALL)
			{
				pb_clear_location (PB_IN_PLAY);
				pb_clear_location (PB_MAYBE_IN_PLAY);
			}
			break;

		/* Powerball detected on playfield, because Slot Proximity
		 * did not trigger when a ball had to travel over it. */
		case PF_PB_DETECTED:
			pb_set_location (PB_IN_PLAY, 0);
			pb_clear_location (PB_MAYBE_IN_PLAY);
			if (timer_kill_gid (GID_MB_JACKPOT_COLLECTED))
			{
				callset_invoke (powerball_jackpot);
			}
			break;

		case TROUGH_STEEL_DETECTED:
			if (live_balls == 0)
			{
				pb_clear_location (PB_MAYBE_IN_PLAY);
			}
			if (device_entry (DEVNO_TROUGH)->actual_count == 1)
			{	
				pb_clear_location (0);
			}
#ifdef PB_DEBUG
			else
				pb_clear_location (0);
#endif
			break;

		case TROUGH_PB_DETECTED:
			pb_set_location (PB_IN_TROUGH, 1);
			pb_clear_location (PB_MAYBE_IN_PLAY);
			break;
		case GUMBALL_PB_DETECTED:
			pb_set_location (PB_IN_GUMBALL, 1);
			pb_clear_location (PB_MAYBE_IN_PLAY);
			break;
	}
}

/** Announce that the powerball is in play, if it is.  This is
only called at certain points when we want to announce this.
The powerball may have been detected sometime earlier. */
void pb_announce (void)
{
	task_kill_gid (GID_GUMBALL_MUSIC_BUG);
	if (pb_announce_needed)
	{
#ifdef PB_DEBUG
		deff_restart (DEFF_PB_DETECT);
#else
		deff_start (DEFF_PB_DETECT);
#endif
		music_update ();
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
		/* Allow time for the trough to settle */
		task_sleep (TIME_200MS);
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


/* Called when a ball enters the trough or lock. */
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
				pb_clear_location (PB_MAYBE_IN_PLAY);
				pb_set_location (location, dev->actual_count);
			}
		}
		else
		{
			/* In multiball, container enter might mean the
			powerball entered it or not... we just don't
			know */
			if (pb_location == PB_IN_PLAY)
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

/* Used by maghelper.c to aid with powerball detection */
CALLSET_ENTRY (pb_detect, left_ball_grabbed, right_ball_grabbed)
{
	if (single_ball_play ())
	{
		task_kill_gid (GID_POWERBALL_MAG_DETECT);
		pb_clear_location (PB_IN_PLAY);
		pb_clear_location (PB_MAYBE_IN_PLAY);
	}
}

/* Starts when a ball is detected on the magnet but is then killed by
 * a successful grab */
void powerball_magnet_detect_task (void)
{
	/* Wait a little while for the ball to be grabbed */
	task_sleep (TIME_600MS);
//	pb_set_location (PB_IN_PLAY, 0);
//	pb_announce ();
	pb_detect_event (PF_PB_DETECTED);
	task_exit ();
}

CALLSET_ENTRY (pb_detect, music_refresh)
{
	if (pb_location == PB_IN_PLAY && !multi_ball_play ())
		music_request (MUS_POWERBALL_IN_PLAY, PRI_GAME_MODE3);
}

/* Powerball slot proximity */
CALLSET_ENTRY (pb_detect, sw_slot_proximity)
{

	/* TODO If I could find out which GID was triggered more recently, I
	 * could kill one and not the other, strengthing detection during
	 * multiball.
	 */
	task_kill_gid (GID_CAMERA_SLOT_PROX_DETECT);
	task_kill_gid (GID_PIANO_SLOT_PROX_DETECT);
	pb_detect_event (PF_STEEL_DETECTED);
	//event_did_follow (camera_or_piano, slot_prox);
	/* TODO : if this switch triggers and we did not expect
	 * a ball in the undertrough....??? */
}

CALLSET_ENTRY (pb_detect, powerball_in_gumball)
{
	pb_detect_event (GUMBALL_PB_DETECTED);
}

CALLSET_ENTRY (pb_detect, check_magnet_grab)
{
	task_recreate_gid (GID_POWERBALL_MAG_DETECT, powerball_magnet_detect_task);
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

CALLSET_ENTRY (pb_detect, dev_gumball_enter)
{
	//pb_container_enter (PB_IN_GUMBALL, DEVNO_GUMBALL);

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

CALLSET_ENTRY (pb_detect, init)
{
	pb_location = PB_MISSING;
	last_pb_event = 0;
	pb_poll_trough ();
}

