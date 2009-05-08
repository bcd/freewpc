
#include <freewpc.h>

/* Quick Multiball rules:
	- Serve a second ball to the plunger.  Kickout wherever ball was locked before
	for 2-ball play.
	- Other multiballs cannot be started/run concurrent to this one?
	- Alternate left/right ramps for jackpot, like Scared Stiff.
	- Goalie hits increases Jackpot value
 */

#define QMB_LEFT_JACKPOT 0x1
#define QMB_RIGHT_JACKPOT 0x2

score_t quickmb_jackpot_value;

score_t quickmb_total;

U8 quickmb_level;

U8 quickmb_jackpots_lit;

__local__ U8 quickmb_ramp_count;

__local__ U8 quickmb_ramps_to_start;

score_id_t quickmb_ladder_scores[] = {
};

struct generic_ladder quickmb_ladder =
{
	sizeof (quickmb_ladder_scores),
	quickmb_ladder_scores,
	&quickmb_level
};


bool quickmb_can_be_started (void)
{
	return FALSE;
}

void quickmb_start (void)
{
	if (!quickmb_can_be_started ())
		return;

	score_zero (quickmb_total);
	/* Initialize jackpot to 200K */
	score_zero (quickmb_jackpot_value);
	score_add_byte (quickmb_jackpot_value, 3, 0x20);
	quickmb_jackpots_lit = QMB_LEFT_JACKPOT | QMB_RIGHT_JACKPOT;
	/* start effects */
}

void quickmb_advance (void)
{
	quickmb_ramp_count++;
	if (quickmb_ramp_count == quickmb_ramps_to_start)
	{
		quickmb_start ();
		if (quickmb_ramps_to_start == 6)
			quickmb_ramps_to_start = 20;
		else
			quickmb_ramps_to_start += 20;
	}
}

void quickmb_jackpot_award (void)
{
}

void quickmb_jackpot_increase (void)
{
}

void quickmb_end (void)
{
	if (flag_test (FLAG_QUICKMB_RUNNING))
	{
		flag_off (FLAG_QUICKMB_RUNNING);
		/* start end effects */
	}
	quickmb_jackpots_lit = 0;
}


CALLSET_ENTRY (quickmb, sw_goalie_target)
{
	if (flag_test (FLAG_QUICKMB_RUNNING))
	{
		quickmb_jackpot_increase ();
	}
}

CALLSET_ENTRY (quickmb, left_ramp_shot)
{
	if (flag_test (FLAG_QUICKMB_RUNNING))
	{
		if (quickmb_jackpots_lit & QMB_LEFT_JACKPOT)
		{
			quickmb_jackpot_award ();
			quickmb_jackpots_lit &= ~QMB_LEFT_JACKPOT;
			quickmb_jackpots_lit |= QMB_RIGHT_JACKPOT;
		}
	}
	else
	{
		quickmb_advance ();
	}
}

CALLSET_ENTRY (quickmb, right_ramp_shot)
{
	if (flag_test (FLAG_QUICKMB_RUNNING))
	{
		if (quickmb_jackpots_lit & QMB_RIGHT_JACKPOT)
		{
			quickmb_jackpot_award ();
			quickmb_jackpots_lit &= ~QMB_RIGHT_JACKPOT;
			quickmb_jackpots_lit |= QMB_LEFT_JACKPOT;
		}
	}
	else
	{
		quickmb_advance ();
	}
}

CALLSET_ENTRY (quickmb, lamp_update)
{
}

CALLSET_ENTRY (quickmb, start_player)
{
	quickmb_ramp_count = 0;
	quickmb_ramps_to_start = 6;
}

CALLSET_ENTRY (quickmb, start_ball, single_ball_play)
{
	quickmb_end ();
}

