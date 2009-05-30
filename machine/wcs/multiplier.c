
#include <freewpc.h>

U8 city_multiplier;

void multiplier_update (void)
{
	score_multiplier_set (city_multiplier);
}

void city_multiplier_set (U8 m)
{
	city_multiplier = m;
	multiplier_update ();
}

CALLSET_ENTRY (multiplier, start_ball)
{
	city_multiplier = 1;
}
