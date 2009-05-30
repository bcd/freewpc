
#include <freewpc.h>

#define NUM_CITIES 9

#define CITY_2X_COMBOS 1
#define CITY_2X_MODE 3
#define CITY_2X_ULTRA 5
#define CITY_EB_LIT 6
#define CITY_2X_JACKPOTS 7
#define CITY_2X_SCORING 9

static const char *city_names[] = {
	"CHICAGO", "DALLAS", "BOSTON",
	"NEW YORK", "ORLANDO", "WASHINGTON D.C.",
	"SAN FRANCISCO", "DETROIT", "LOS ANGELES"
};

static const U8 city_spins_needed[] = {
	12, 16, 20, 24, 28, 32, 36, 40, 44
};

__local__ U8 cities_reached;

__local__ U8 city_spins_left;

score_t spinner_value;

score_t city_value;

#define city_lamp lamplist_index(LAMPLIST_CITIES, cities_reached)


bool city_mode_active (U8 city)
{
	return (city == cities_reached);
}


void city_spinner_reset (void)
{
	city_spins_left = city_spins_needed[cities_reached];
	lamp_tristate_flash (city_lamp);
}


void city_value_increase (void)
{
	score_add (city_value, score_table[SC_100K]);
}


void city_advance (void)
{
	if (flag_test (FLAG_FINAL_MATCH_LIT))
		return;

	score_long (city_value);
	lamp_tristate_on (city_lamp);
	cities_reached++;

	if (cities_reached == NUM_CITIES)
	{
		lamplist_apply (LAMPLIST_CITIES, lamp_off);
		lamplist_apply (LAMPLIST_CITIES, lamp_flash_on);
		flag_on (FLAG_FINAL_MATCH_LIT);
	}
	else
	{
		city_value_increase ();

		/* Advance the spinner value by 2500 each time a city
		is collected.  This resets at the beginning of each
		ball. */
		score_add (spinner_value, score_table[SC_2500]);
		city_spinner_reset ();

		if (cities_reached == CITY_EB_LIT)
		{
			/* light extra ball */
		}
	}
}

CALLSET_ENTRY (city, sw_spinner_slow)
{
	score_long (spinner_value);
	if (!flag_test (FLAG_FINAL_MATCH_LIT))
	{
		if (city_spins_left > 0)
		{
			if (--city_spins_left == 0)
			{
				city_advance ();
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
	score_copy (city_value, score_table[SC_500K]);
}

