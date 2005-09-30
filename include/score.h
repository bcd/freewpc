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


extern volatile U8 score_change;

#endif /* _SCORE_H */
