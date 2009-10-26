
#include <freewpc.h>

#ifndef EBFLEX_DEFAULT_LEVEL
#define EBFLEX_DEFAULT_LEVEL 8
#endif

#ifndef EBFLEX_PERIOD
#define EBFLEX_PERIOD 32
#endif

__permanent__ U16 ebflex_shots1;
__permanent__ U8 ebflex_awards1;

__permanent__ U16 ebflex_shots2;
__permanent__ U8 ebflex_awards2;

__permanent__ U8 ebflex_plays;

__local__ U8 ebflex_player_shots;
__permanent__ U8 ebflex_level;


CALLSET_ENTRY (ebflex, factory_reset)
{
	ebflex_shots1 = ebflex_awards1 = 0;
	ebflex_shots2 = ebflex_awards2 = 0;
	ebflex_plays = 0;
	ebflex_level = EBFLEX_DEFAULT_LEVEL;
}


CALLSET_ENTRY (ebflex, start_player)
{
	ebflex_player_shots = 0;
}


U8 ebflex_getlevel (void)
{
	return ebflex_level;
}


void ebflex_add_shots (U8 count)
{
	ebflex_shots1 += count;
	ebflex_player_shots += count;
	if (ebflex_player_shots == ebflex_level)
	{
		ebflex_awards1++;
		callset_invoke (ebflex_level_reached);
	}
}


U8 ebflex_get_shots (void)
{
	return ebflex_player_shots;
}


static void ebflex_update (void)
{
	U16 total_shots = ebflex_shots1 + ebflex_shots2;
	U8 total_awards = ebflex_awards1 + ebflex_awards2;
}


CALLSET_ENTRY (ebflex, game_audited)
{
	ebflex_plays++;

	if (ebflex_plays == EBFLEX_PERIOD)
	{
		/* Flex the level here */
		ebflex_update ();

		/* Discard half of the audit data */
		ebflex_shots2 = ebflex_shots1;
		ebflex_awards2 = ebflex_awards1;

		ebflex_plays = 0;
	}
}

