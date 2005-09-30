#ifndef _SCORE_H
#define _SCORE_H

/** Define the number of digits contained in standard-length scores. */
#ifdef MACHINE_SCORE_DIGITS
#define MAX_SCORE_DIGITS MACHINE_SCORE_DIGITS
#else
#define MAX_SCORE_DIGITS 8
#endif

/** The standard score type, kept in packed BCD, 2 digits per byte */
typedef bcd_t score_t[(MAX_SCORE_DIGITS + 1)/2];

void scores_draw (void);
void scores_deff (void) __taskentry__;
void score_zero (score_t *s);
void score_add (score_t *s1, score_t *s2);
void score_sub (score_t *s1, score_t *s2);
void score_mul (score_t *s1, uint8_t multiplier);
int score_compare (score_t *s1, score_t *s2);

extern volatile U8 score_change;

extern U8 scores[4][4];
extern U8 *current_score;

#endif /* _SCORE_H */
