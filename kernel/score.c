
#include <freewpc.h>


volatile U8 score_change;


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

extern inline U8 __add_with_carry (U8 v1, U8 v2)
{
	asm volatile ("adcb\t%1" :: "d" (v1), "g" (v2));
	return v1;
	// return v1 + v2;
}

extern inline void __daa (void)
{
	asm volatile ("daa");
}

void score_add (score_t *s1, score_t *s2)
{
	register bcd_t *bcd1 = (bcd_t *)s1;
	register bcd_t *bcd2 = (bcd_t *)s2;
	register int i = sizeof(score_t);
	bcd1[i] += bcd2[i];
	for (i=sizeof(score_t)-1; i > 0; --i)
	{
		bcd1[i] = __add_with_carry (bcd1[i], bcd2[i]);
	}
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


