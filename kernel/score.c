
#include <freewpc.h>


volatile U8 score_change;

U8 scores[4][4];
U8 *current_score;


void scores_draw_ball (void)
{
	sprintf ("%1iUP", player_up);
	font_render_string (&font_5x5, 4, 26, sprintf_buffer);

	sprintf ("BALL %1i", ball_up);
	font_render_string_right (&font_5x5, 124, 26, sprintf_buffer);
}


void scores_draw_credits (void)
{
	credits_render ();
	font_render_string_center (&font_5x5, 64, 27, sprintf_buffer);
}


void scores_draw_current (void)
{
	sprintf ("%8b", current_score);
	font_render_string_center (&font_5x5, 64, 10, sprintf_buffer);
}


void scores_draw (void)
{
	if (in_game)
		scores_draw_ball ();
	else
		scores_draw_credits ();
	scores_draw_current ();
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
		scores_draw_ball ();
		dmd_copy_low_to_high ();
		scores_draw_current ();
		dmd_show_low ();
		
		/* Restart score effects */

		/* Wait for a score change */
		while (score_change == 0)
		{
			task_sleep (TIME_100MS);
			dmd_show_other ();
		}
	}
}


void score_zero (score_t *s)
{
	memset (s, 0, sizeof (score_t));
}


void score_add (bcd_t *s1, bcd_t *s2, U8 _len)
{
	register bcd_t *bcd1 = s1;
	register bcd_t *bcd2 = s2;
	register bcd_t len = _len;

	bcd1 += len-1;
	bcd2 += len-1;

	asm volatile ("lda\t%0" :: "m" (*bcd1));
	asm volatile ("adda\t%0" :: "m" (*bcd2));
	asm volatile ("daa");
	asm volatile ("sta\t%0" :: "m" (*bcd1));
	bcd1--;
	bcd2--;
  	len--;

	while (len > 0)
	{
	  /* TODO : possible compiler optimization could
		* be done here. (in older implementation)
		* stb ,x; leax -1,x => stb ,-x
		*/
		asm volatile ("lda\t%0" :: "m" (*bcd1));
		asm volatile ("adca\t%0" :: "m" (*bcd2));
		asm volatile ("daa");
		asm volatile ("sta\t%0" :: "m" (*bcd1));
		bcd1--;
		bcd2--;
		len--;
	}
}


void score_add_current (bcd_t *s)
{
	score_add (current_score, s, sizeof (score_t));
	score_change++;
	/* check replay */
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


void scores_reset (void)
{
	score_change = 0;
	memset ((U8 *)scores, 0, sizeof (scores));
	current_score = &scores[0][0];
}


void score_init (void)
{
	scores_reset ();
}


