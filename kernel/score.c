
#include <freewpc.h>


volatile U8 score_change;

U8 scores[4][4];
U8 *current_score;


void scores_draw (void)
{
	sprintf ("%iUP", player_up);
	font_render_string (&font_5x5, 4, 26, sprintf_buffer);

	sprintf ("BALL %i", ball_up);
	font_render_string_right (&font_5x5, 124, 26, sprintf_buffer);

	sprintf ("%8b", current_score);
	font_render_string_center (&font_5x5, 64, 10, sprintf_buffer);
}


void scores_deff (void) __taskentry__
{
	for (;;)
	{
		/* Clear score change flag */
		score_change = 0;

		/* Stop any score effects (i.e. flashing) */

		/* Redraw the scores on the screen */
		dmd_alloc_low_clean ();
		scores_draw ();
		dmd_show_low ();
		
		/* Restart score effects */

		/* Wait for a score change */
		while (score_change == 0)
			task_sleep (TIME_100MS * 2);
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

void score_sub (score_t *s1, score_t *s2)
{
}

void score_mul (score_t *s1, uint8_t multiplier)
{
}

int score_compare (score_t *s1, score_t *s2)
{
	return (0);
}



