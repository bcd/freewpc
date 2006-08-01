/*
 * Copyright 2006 by Brian Dominy <brian@oddchange.com>
 *
 * This file is part of FreeWPC.
 *
 * FreeWPC is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * FreeWPC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with FreeWPC; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * \file
 * \brief Routines for doing math on scores
 */

#include <freewpc.h>


U8 score_change;
score_t scores[MAX_PLAYERS];
U8 *current_score;
score_t bonus_score;
U8 debug_value;


void scores_draw_ball (void)
{
#if defined(SHOW_DEBUG_VALUE)
	sprintf ("DEBUG %d", debug_value);
#elif defined (CONFIG_TIMED_GAME)
	sprintf ("TIME: 0:%02d", timed_game_timer);
#else
	sprintf ("%1iUP", player_up);
#endif
	font_render_string_center (&font_mono5, 32, 26, sprintf_buffer);

	sprintf ("BALL %1i", ball_up);
	font_render_string_center (&font_mono5, 96, 26, sprintf_buffer);
}


void scores_draw_credits (void)
{
	credits_render ();
	font_render_string_center (&font_mono5, 64, 27, sprintf_buffer);
}


void scores_draw_current (void)
{
	sprintf ("%8b", current_score);
#if 0
	sprintf ("%02X%02X%02X%02X", 
		current_score[0], current_score[1], current_score[2], current_score[3]);
#endif
	font_render_string_center (&font_lucida9, 64, 10, sprintf_buffer);
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
		dmd_alloc_low_high ();
		dmd_clean_page_low ();
		scores_draw_ball ();
		dmd_copy_low_to_high ();
		scores_draw_current ();
		dmd_show_low ();
		
		/* Restart score effects */

		/* Wait for a score change */
		while (score_change == 0)
		{
			task_sleep (ball_in_play ? TIME_100MS * 2 : TIME_100MS);
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
	register U8 len = _len;

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
	if (!in_live_game)
		return;

	score_add (current_score, s, sizeof (score_t));
	score_change++;
	/* TODO : check replay */
}


/* score[sizeof(score_t)-1] = ones
 * score[sizeof(score_t)-2] = hundreds
 * score[sizeof(score_t)-3] = ten thousands
 * score[sizeof(score_t)-4] = millions
 */
void score_add_millions_current (U8 mils)
{
	score_add (current_score + sizeof (score_t) - 4, &mils, sizeof (U8));
}


void score_sub (score_t s1, score_t s2)
{
}

void score_mul (score_t s1, uint8_t multiplier)
{
	register int off = sizeof (score_t);
	register bcd_t *bcd = s1 + off - 1;
	bcd_t overflow = 0;
}


I8 score_compare (score_t s1, score_t s2)
{
	register int len = sizeof (score_t);
	register I8 diff;

	while (len > 0)
	{
		diff = *s1 - *s2;
		if (diff != 0)
			return (diff);
		s1++;
		s2++;
		len--;
	}
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


