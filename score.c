
#include <freewpc.h>

/** Define the number of digits contained in standard-length scores. */
#define MAX_SCORE_DIGITS 10

/** The standard score type, kept in packed BCD, 2 digits per byte */
typedef bcd_t score_t[(MAX_SCORE_DIGITS + 1)/2];


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

