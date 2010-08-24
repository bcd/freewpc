
#include <freewpc.h>

/* An example of how to use the multiball mode module.
	In this simple 2-ball multiball, the center ramp
	scores a 10 point jackpot. */

U8 mb_jackpot_count;

static void bonus_mb_jackpot (void)
{
	mb_jackpot_count++;
	score (SC_10);
}

static void afm_bonus_mb_update (enum mb_mode_state state)
{
	switch (state)
	{
		case MB_INACTIVE:
			mb_jackpot_count = 0;
			/* fallthru for effects */

		case MB_IN_GRACE:
			lamp_tristate_off (LM_LOCK_JACKPOT);
			break;

		case MB_ACTIVE:
			lamp_tristate_on (LM_LOCK_JACKPOT);
			device_multiball_set (2);
			break;
	}
}

struct mb_mode_ops afm_bonus_mb_mode =
{
	.callback_page = MACHINE_PAGE,
	.update = afm_bonus_mb_update,
	.music = 0,
	.deff_starting = 0,
	.deff_running = 0,
	.deff_ending = 0,
	.prio = PRI_NULL,
	.gid_running = GID_BONUS_MB,
	.gid_in_grace = GID_BONUS_MB_GRACE,
	.grace_period = TIME_3S,
};

CALLSET_ENTRY (bonusmb, center_ramp_shot)
{
	if (mb_mode_running_p (&afm_bonus_mb_mode))
	{
		bonus_mb_jackpot ();
	}
}

CALLSET_ENTRY (bonusmb, single_ball_play)
{
	mb_mode_single_ball (&afm_bonus_mb_mode);
}

CALLSET_ENTRY (bonusmb, end_ball)
{
	mb_mode_end_ball (&afm_bonus_mb_mode);
}

