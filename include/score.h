#ifndef _SCORE_H
#define _SCORE_H

/** Define the number of digits contained in standard-length scores. */
#ifdef MACHINE_SCORE_DIGITS
#define MAX_SCORE_DIGITS MACHINE_SCORE_DIGITS
#else
#define MAX_SCORE_DIGITS 8
#endif

#define BYTES_PER_SCORE ((MAX_SCORE_DIGITS+1)/2)

/** The standard score type, kept in packed BCD, 2 digits per byte */
typedef bcd_t score_t[BYTES_PER_SCORE];

/** Macros for adding to the CURRENT score; these are shortcuts **/

extern volatile U8 score_change;
extern U8 scores[][BYTES_PER_SCORE];
extern U8 *current_score;

void scores_draw (void);
void scores_draw_ball (void);
void scores_draw_current (void);
void scores_deff (void) __taskentry__;
void score_zero (score_t *s);
void score_add (bcd_t *s1, bcd_t *s2, U8 len);
void score_sub (score_t *s1, score_t *s2);
void score_mul (score_t *s1, uint8_t multiplier);
int score_compare (score_t *s1, score_t *s2);
void scores_reset (void);
void score_init (void);

extern inline void score_add_current (bcd_t *s)
{
	score_add (current_score, s, sizeof (score_t));
	score_change++;
	/* check replay */
}


#endif /* _SCORE_H */
