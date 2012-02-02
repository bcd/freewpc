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

/**
 * \file
 * \brief Implement the extra ball buy-in feature.
 */

#include <freewpc.h>
#include <coin.h>

#define BUYIN_TIMEOUT   7  /* No. of seconds until buyin times out */


/** Amount of time left to buy an extra ball during buyin */
U8 buyin_offer_timer;

/** FALSE if buyin was purchased during the offer */
bool buyin_passed;

/** The number of buyins each player has had */
__local__ U8 buyin_count;

/*	Start/restart the buyin feature */
static void buyin_init (void)
{
	buyin_offer_timer = BUYIN_TIMEOUT;
	deff_restart (DEFF_BUYIN_OFFER);
#ifdef MACHINE_BUYIN_LAMP
	lamp_tristate_flash (MACHINE_BUYIN_LAMP);
#endif
}

/*	End the buyin feature */
static void buyin_exit (void)
{
	buyin_offer_timer = 0;
	deff_stop (DEFF_BUYIN_OFFER);
#ifdef MACHINE_BUYIN_LAMP
	lamp_tristate_off (MACHINE_BUYIN_LAMP);
#endif
}

/* Test if the buyin feature is still running.
	Consider both the display effect and the timer, as the deff may be
	temporarily masked by the coins message. */
static bool buyin_running_p (void)
{
#ifdef DEFF_BUYIN_OFFER
	if (deff_get_active () == DEFF_BUYIN_OFFER)
		return TRUE;
#endif
	if (buyin_offer_timer > 0)
		return TRUE;
	return FALSE;
}


static void buyin_abort (void)
{
	callset_invoke (buyin_abort);
	buyin_exit ();
}


/*	Offer to buy an extra ball.
	This is called only when buyin is enabled.  It returns when the extra ball
	is purchased, aborted, or times out. */
static void buyin_task (void)
{
	/* TODO - validate that a buyin offer is OK; check that the
	buyin limit hasn't been reached */

	/* By default there is no extra ball.  This changes to FALSE
	if a ball is bought. */
	buyin_passed = TRUE;


	/* Run the buyin countdown timer and display effect. */
	buyin_init ();
	while (buyin_running_p ())
	{
		task_sleep_sec (1);
		if (buyin_running_p ())
			buyin_offer_timer--;
	}
	task_sleep_sec (1);
	buyin_exit ();
}


/*	Called by the core system when an extra ball buyin can be offered.
	Returns FALSE if a ball was purchased, and the player up should not
	be switched.  Returns TRUE if the player switch continues as usual. */
CALLSET_BOOL_ENTRY (buyin, buyin_offer)
{
	if (system_config.buy_extra_ball == YES)
	{
		buyin_task ();
		return buyin_passed;
	}
	return TRUE;
}

/* Handle the start button during buyin. */
CALLSET_ENTRY (buyin, start_button_handler)
{
	if (buyin_running_p ())
	{
#ifdef MACHINE_BUYIN_SWITCH
		/* Machine has a buyin button, so start will abort. */
		buyin_abort ();
#else
		/* Machine does not have a buyin button, so start will
		act as the buyin button. */
		VOIDCALL (buyin_sw_buyin_button);
#endif
	}
}

/* Handle the buyin button during buyin. */
CALLSET_ENTRY (buyin, sw_buyin_button)
{
	if (buyin_running_p () && has_credits_p ())
	{
		remove_credit ();
		buyin_passed = FALSE;
		buyin_exit ();
	}
}


/* Handle a coin insert during buyin */
void buyin_coin_insert (void)
{
	if (buyin_running_p ())
	{
		buyin_init ();
	}
}


/*	Handle the flipper abort during buyin */
CALLSET_ENTRY (buyin, flipper_abort)
{
	if (buyin_running_p ())
	{
		buyin_abort ();
	}
}


/* Initialize the buyin count per player */
CALLSET_ENTRY (buyin, start_player)
{
	buyin_count = 0;
}

