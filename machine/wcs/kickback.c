
#include <freewpc.h>
#include "kickback_driver.h"

void kickback_enable (void)
{
	lamplist_apply (LAMPLIST_KICKBACKS, lamp_on);
	spsol_enables |= (1 << 5);
}

void kickback_disable (void)
{
	lamplist_apply (LAMPLIST_KICKBACKS, lamp_off);
	spsol_enables &= ~(1 << 5);
}

static inline bool kickback_enabled (void)
{
	return lamp_test (LM_KICKBACK_LOWER);
}


void kickback_finish (void)
{
	task_sleep_sec (3);
	kickback_disable ();
	task_exit ();
}


CALLSET_ENTRY (kickback, sw_kickback)
{
	if (kickback_enabled ())
	{
		sound_send (SND_WHISTLE);
		task_create_gid1 (GID_KICKBACK_FINISH, kickback_finish);
	}
	else
	{
	}
}

CALLSET_ENTRY (kickback, sw_kickback_upper_exit)
{
}

CALLSET_ENTRY (kickback, sw_kickback_upper_enter)
{
	sound_send (SPCH_STAY_COOL_DADDY_O);
	kickback_enable ();
}

CALLSET_ENTRY (kickback, start_player)
{
	kickback_enable ();
}

CALLSET_ENTRY (kickback, start_ball)
{
	if (kickback_enabled ())
	{
		kickback_enable ();
	}
	else
	{
		kickback_disable ();
	}
}

