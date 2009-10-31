
#include <freewpc.h>

U8 speech_games_started;

enum speech_mood {
	MOOD_NORMAL,
};

static enum speech_mood mood;

void say (U16 code)
{
	speech_start (code, SL_3S);
}


CALLSET_ENTRY (speech, rudy_shot)
{
	say (SPCH_MILD_OWW);
}

CALLSET_ENTRY (speech, start_game)
{
	mood = MOOD_NORMAL;
	if (speech_games_started & 1)
		say (SPCH_FUNHOUSE);
	else
		say (SPCH_COME_ON_IN);
	speech_games_started++;
}

CALLSET_ENTRY (speech, dev_lock_kick_request)
{
	say (SPCH_HEADS_UP);
}

CALLSET_ENTRY (speech, add_player)
{
	if (num_players == 2)
		say (SPCH_LETS_PLAY_TWO);
}

CALLSET_ENTRY (speech, start_player)
{
}

CALLSET_ENTRY (speech, end_game)
{
	say (SPCH_GOOD_NIGHT);
}

CALLSET_ENTRY (speech, extra_ball_lit)
{
	say (SPCH_THERES_EXTRA_BALL);
}

CALLSET_ENTRY (speech, tilt)
{
	sample_start (SND_TILT, SL_4S);
}

