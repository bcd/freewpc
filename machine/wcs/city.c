
#include <freewpc.h>

#define NUM_CITIES 9

static const char *city_names[] = {
	"CHICAGO", "DALLAS", "BOSTON",
	"NEW YORK", "ORLANDO", "WASHINGTON D.C.",
	"SAN FRANCISCO", "DETROIT", "LOS ANGELES"
};

static const U8 city_spins_needed[] = {
	10, 10, 10, 15, 15, 15, 20, 25, 30
};

__local__ U8 cities_reached;

__local__ U8 city_spins_left;

score_t spinner_value;

score_t city_value;

#define city_lamp lamplist_index(LAMPLIST_CITIES, cities_reached)

void city_spinner_reset (void)
{
	city_spins_left = city_spins_needed[cities_reached];
	lamp_tristate_flash (city_lamp);
}


void city_value_increase (void)
{
	score_add (city_value, score_table[SC_25K]);
}


void city_advance (void)
{
	if (flag_test (FLAG_FINAL_MATCH_LIT))
		return;

	lamp_tristate_on (city_lamp);
	if (++cities_reached == NUM_CITIES)
	{
		lamplist_apply (LAMPLIST_CITIES, lamp_off);
		lamplist_apply (LAMPLIST_CITIES, lamp_flash_on);
		flag_on (FLAG_FINAL_MATCH_LIT);
	}
	else
	{
		/* Advance the spinner value by 2500 each time a city
		is collected */
		score_long (city_value);
		city_value_increase ();
		score_add (spinner_value, score_table[SC_2500]);
		city_spinner_reset ();
	}
}

CALLSET_ENTRY (city, sw_spinner_slow)
{
	if (!flag_test (FLAG_FINAL_MATCH_LIT))
	{
		if (city_spins_left > 0)
		{
			if (--city_spins_left == 0)
			{
				city_advance ();
			}
			else
			{
				score_long (spinner_value);
			}
		}
	}
}


CALLSET_ENTRY (city, start_player)
{
	flag_off (FLAG_FINAL_MATCH_LIT);
	cities_reached = 0;
	city_spinner_reset ();
}

CALLSET_ENTRY (city, start_ball)
{
	score_copy (spinner_value, score_table[SC_5K]);
	score_copy (city_value, score_table[SC_200K]);
}

