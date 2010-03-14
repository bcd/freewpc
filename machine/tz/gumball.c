/*
 * Copyright 2006-2009 by Brian Dominy <brian@oddchange.com>
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
#include <gumball_div.h>
#include <diag.h>
#include <search.h>

U8 gumball_pending_releases;
__local__ U8 gumball_enable_count;

volatile U8 gumballs_released;

/** The number of balls known to be in the gumball machine */
__permanent__ U8 gumball_count;

/** The maximum number of balls to keep in the gumball machine */
__permanent__ U8 max_gumball_count;

/** The position of the Powerball in the gumball machine.
0 = Powerball next to be ejected, 1 = behind that, etc.
0xFF means the Powerball is not in the gumball. */
__permanent__ U8 gumball_pb_position;

__permanent__ U8 gumball_flags;

U8 gumball_dispense_error_count;


#define GUMBALL_COUNTED 0x1
#define GUMBALL_MISSING 0x2
#define GUMBALL_LOADING 0x4
#define GUMBALL_GENEVA  0x8
#define GUMBALL_BALL_SEARCH 0x10

#define GUMBALL_MAX 5

void gumball_deff (void)
{
	/*
	dmd_alloc_low_clean ();
	psprintf("1 GUMBALL", "%d GUMBALLS", gumball_collected_count);
	font_render_string_center (&font_fixed6, 64, 7, sprintf_buffer);
	sprintf("%d MILLION", gumball_score);
	font_render_string_center (&font_fixed6, 64, 18, sprintf_buffer);
	dmd_show_low ();
	task_sleep_sec (1);
	deff_exit ();
	*/
	sound_send (SND_GUMBALL_LOAD_START);
	U16 fno;
	for (fno = IMG_GUMBALL_START; fno <= IMG_GUMBALL_END; fno += 2)
	{
		dmd_alloc_pair ();
		frame_draw (fno);
		dmd_show2 ();
		task_sleep (TIME_100MS);
	}

	sound_send (SND_GUMBALL_LOAD_END);
	/* Show last two frames two times */
	for (fno = 0 ; fno > 3; fno++)
	{
		dmd_alloc_pair ();
		frame_draw (IMG_GUMBALL_END - 2);
		dmd_show2 ();
		task_sleep (TIME_100MS);
		dmd_alloc_pair ();
		frame_draw (IMG_GUMBALL_END);
		dmd_show2 ();
		task_sleep (TIME_100MS);
	}
	task_sleep_sec (1);
	deff_exit ();
}

CALLSET_ENTRY (gumball, lamp_update)
{
	//if (gumball_load_is_enabled ())
	if (gumball_flags & GUMBALL_LOADING)
		lamp_tristate_flash (LM_GUMBALL_LANE);
	else
		lamp_tristate_off (LM_GUMBALL_LANE);
}


/*
 * The gumball count increases when the 'gumball enter' opto
 * closes following a popper event.  We require both of these
 * events for redundancy.
 */

CALLSET_ENTRY (gumball, sw_gumball_enter)
{
	if (gumball_count < max_gumball_count)
	{
		gumball_count++;
		dbprintf ("Gumball entered\n");
		callset_invoke (dev_gumball_enter);
	}
}


CALLSET_ENTRY (gumball, dev_popper_kick_attempt)
{
}


/*
 * The gumball count decreases when the 'gumball exit' switch
 * closes during a release cycle.
 */

CALLSET_ENTRY (gumball, sw_gumball_exit)
{
	if (timer_kill_gid (GID_GUMBALL_RELEASE))
	{
		if (gumball_count > 0 && !(gumball_flags & GUMBALL_BALL_SEARCH))
		{
			gumball_count--;
			dbprintf ("Gumball exited\n");
			callset_invoke (dev_gumball_kick_success);
		}
		gumballs_released++;
	}
}


CALLSET_ENTRY (gumball, sw_gumball_geneva)
{
	gumball_flags |= GUMBALL_GENEVA;
}


/*************************************************************/
/* Gumball APIs                                              */
/*************************************************************/

U8 gumball_get_count (void)
{
	return gumball_count;
}

/*
 * Request a trough load of the gumball machine.
 * There must be no balls on the playfield while this occurs.
 */
void gumball_load_from_trough (void)
{
	extern void autofire_add_ball (void);

	dbprintf ("Gumball will load on magnet\n");
	gumball_flags |= GUMBALL_LOADING;

	/* Serve a ball from the trough to the autoplunger. */
	/* TODO - what if this fails? */
	autofire_add_ball ();
}


/*
 * Enable a load of the gumball machine (from the playfield).
 */
void gumball_load_enable (void)
{
	gumball_flags |= GUMBALL_LOADING;
}


/*
 * Handle the release of one or more gumballs in a background task.
 */
void gumball_release_task (void)
{
	U8 timeout;

	gumball_dispense_error_count = 0;
	while (gumball_pending_releases > 0)
	{
		/* Begin the release of a single ball */
		gumball_flags &= ~GUMBALL_GENEVA;
		gumballs_released = 0;

		/* Start the gumball motor.  When the Geneva switch closes, it indicates
		one ball should have been dispensed, then we stop the motor.
		We timeout after about 2 seconds in case this switch is not working. */
		dbprintf ("Releasing 1 gumball.\n");
		sol_enable (SOL_GUMBALL_RELEASE);
		task_sleep (TIME_200MS);
		timeout = 60;
		while (!(gumball_flags & GUMBALL_GENEVA) && (--timeout > 0))
			task_sleep (TIME_33MS);
		sol_disable (SOL_GUMBALL_RELEASE);
		dbprintf ("Release cycle complete.\n");

		/* Verify that a ball was dispensed.  Wait briefly for the
		'exit' event to be generated.  gumballs_released will tell us
		how many balls actually came out (hopefully 1).  Wait up to 2.5
		seconds for the feedback */
		timeout = 25;
		while (gumballs_released == 0 && --timeout > 0)
			task_sleep (TIME_100MS);
		dbprintf ("Detected %d balls released\n", gumballs_released);

		/* However many balls were released, decrement the pending count */
		if (gumballs_released)
		{
			gumball_pending_releases -= gumballs_released;
		}
		else
		{
			/* Just give up after awhile... */
			if (++gumball_dispense_error_count == 3)
			{
				dbprintf ("Release aborted\n");
				gumball_pending_releases = 0;
				break;
			}
		}
	}
	gumball_flags &= ~GUMBALL_BALL_SEARCH;
	task_exit ();
}


/*
 * Request to release one ball from the gumball machine.
 */
void gumball_release (void)
{
	if (feature_config.disable_gumball == YES)
		return;
	gumball_pending_releases++;
	task_create_gid1 (GID_GUMBALL_RELEASE, gumball_release_task);
}


/*
 * Request to empty all balls in the gumball machine.
 */
void gumball_empty (void)
{
	U8 count;
	for (count = gumball_count; count > 0; --count)
		gumball_release ();
}


/*****************************
 * Gumball load approach
 *****************************/

CALLSET_ENTRY (gumball, right_loop_entered)
{
	/* TODO : Gumball load should not be allowed during a
	multiball if the Powerball is on the field */
	if (feature_config.disable_gumball == YES)
		return;

	/* If gumball load is enabled, open the divertor now. */
	if (gumball_flags & GUMBALL_LOADING)
	{
		gumball_div_start ();
		/* TODO : If this times out without the ball reaching
		the lane/popper, this bit flag should be reset. */
	}
}

CALLSET_ENTRY (gumball, sw_gumball_lane, dev_popper_enter)
{
	/* Stop the gumball diverter as soon as the lane switch
	is seen, or the popper switch closes.  Use either of
	these for redundancy. */
	if (gumball_flags & GUMBALL_LOADING)
	{
		bounded_decrement (gumball_enable_count, 0);
		gumball_div_stop ();
		/* Gumball load must be re-enabled for multiple balls. */
		gumball_flags &= ~GUMBALL_LOADING;
	}
}

CALLSET_ENTRY (gumball, device_update)
{
	/* Recalculate the value of the GUMBALL_LOADING flag, which
	says whether or not a load cycle begins on a right magnet event.
	Gumball load may be disabled in some circumstances. */
}


/*************************************
 * Gumball autoload from full trough
 *************************************/

void sw_far_left_trough_monitor (void)
{
	U8 timeout = TIME_3S / TIME_200MS;
	device_t *dev = device_entry (DEVNO_TROUGH);

	/* Wait while the device subsystem is recounting all balls.
	We can't poll the trough count during this time. */
 	while (task_find_gid (GID_DEVICE_PROBE))
		task_sleep (TIME_100MS);

	/* Poll the switch for up to 3 seconds.  If it ever opens,
	then abort.  It must stay closed and the trough must
	remain full in order for us to continue. */
	dbprintf ("Polling trough overflow\n");
	while (timeout > 0)
	{
		task_sleep (TIME_200MS);
		if ((!switch_poll_logical (SW_FAR_LEFT_TROUGH))
			|| (dev->actual_count != dev->size))
		{
			dbprintf ("Trough overflow abort\n");
			task_exit ();
		}
		timeout--;
	}

	/* At this point, we know there is an extra ball in the
	trough that needs to be loaded.  But can we safely load it
	now?
		Certainly, if another gumball load is already in
	progress, we should wait for that to finish.
		If there are balls on the playfield, then generally the
	answer is 'no'.  TODO : There may be cases that are safe.
		Outside of a game, live_balls is never zero. */
	dbprintf ("Gumball load pending.\n");
	while ((gumball_flags & GUMBALL_LOADING) || (live_balls > 0))
	{
		task_sleep (TIME_100MS);
	}

	/* Start the load */
	dbprintf ("Gumball load started.\n");
	gumball_load_from_trough ();
	task_exit ();
}


static void gumball_poll_trough (void)
{
	if (feature_config.disable_gumball == YES)
		return;
	if (in_test)
		return;
	task_recreate_gid (GID_FAR_LEFT_TROUGH_MONITOR, sw_far_left_trough_monitor);
}


CALLSET_ENTRY (gumball, sw_far_left_trough, start_ball, amode_start)
{
	gumball_poll_trough ();
}


CALLSET_ENTRY (gumball, empty_balls_test)
{
	gumball_empty ();
}


/* Miscellaneous event handling */

CALLSET_ENTRY (gumball, ball_search)
{
	if (ball_search_count == 3 && feature_config.disable_gumball == NO)
	{
		dbprintf ("Gumball ball searching.\n");
		gumball_flags |= GUMBALL_BALL_SEARCH;
		gumball_release ();
	}
}


CALLSET_ENTRY (gumball, factory_reset)
{
	/* At factory reset, assume that the gumball contains all of the
	balls that it should.  If this is not the case, we should soon
	discover it...
		If there are more balls inside than configured, then the
	trough count will be low and it will appear as a ball missing.
	Ball search will be tried to find it; the gumball ball search
	handler should do a release cycle to recover from this.
		If there are fewer balls inside, far left trough should
	see an extra ball and invoke reload. */
	max_gumball_count = feature_config.installed_balls - 3;
	gumball_count = max_gumball_count;
	gumball_flags = 0;
}


CALLSET_ENTRY (gumball, diagnostic_check)
{
	if (feature_config.disable_gumball == YES)
		diag_post_error ("GUMBALL DISABLED\nBY ADJUSTMENT\n", PAGE);
}


CALLSET_ENTRY (gumball, init_complete)
{
	gumball_flags &= ~(GUMBALL_LOADING | GUMBALL_GENEVA);
	if (gumball_count > GUMBALL_MAX || gumball_count > max_gumball_count)
		gumball_factory_reset ();
	gumball_poll_trough ();
}


