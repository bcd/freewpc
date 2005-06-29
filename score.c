
#include <freewpc.h>


volatile uint8_t score_change;


void score_deff (void)
{
	for (;;)
	{
		/* Stop any score effects (i.e. flashing) */

		/* Redraw the scores on the screen */
		
		/* Restart score effects */

		/* Clear score change flag */
		score_change = 0;

		/* Wait for a score change */
		while (score_change == 0);
	}
}


void score_zero (score_t *s)
{
	memset (s, 0, sizeof (score_t));
}

score_t *score_add (score_t *s1, score_t *s2)
{
	return (s1);
}

score_t *score_sub (score_t *s1, score_t *s2)
{
	return (s1);
}

score_t *score_mul (score_t *s1, uint8_t multiplier)
{
	return (s1);
}

int score_compare (score_t *s1, score_t *s2)
{
	return (0);
}


