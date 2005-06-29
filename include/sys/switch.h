#ifndef _SYS_SWITCH_H
#define _SYS_SWITCH_H

/** Small integer typedef for a switch number */
#ifndef __SASM__
typedef uint8_t switchnum_t;
#endif

#define NUM_PF_SWITCHES 64
#define NUM_DEDICATED_SWITCHES 8
#define NUM_SWITCHES (NUM_PF_SWITCHES + NUM_DEDICATED_SWITCHES)
#define SWITCH_BITS_SIZE	(NUM_SWITCHES / 8)

#define SW_COL(x)			((x) >> 3)
#define SW_ROW(x)			((x) & 0x07)
#define SW_ROWMASK(x)	(1 << SW_ROW(x))

#define MAKE_SWITCH(row,col)	((row * 8) + col - 1)

/* Coin Door Switch Numbers */
#define SW_LEFT_COIN				0
#define SW_CENTER_COIN			1
#define SW_RIGHT_COIN			2
#define SW_FOURTH_COIN			3
#define SW_ESCAPE					4
#define SW_DOWN					5
#define SW_UP						6
#define SW_ENTER					7

/* Trough switch numbers (game-specific) */
#define NUM_TROUGH_SWITCHES	3
#define SW_TROUGH1				MAKE_SWITCH(1,5)
#define SW_TROUGH2				MAKE_SWITCH(1,6)
#define SW_TROUGH3				MAKE_SWITCH(1,7)
#define SW_TROUGH4				0
#define SW_TROUGH5				0
#define SW_TROUGH6				0

/* Outhole switch */
#define SW_OUTHOLE				MAKE_SWITCH(1,8)

/* Start button */
#define SW_START					MAKE_SWITCH(1,3)

/* Buy in button */
#define SW_BUYIN					MAKE_SWITCH(2,3)

/* Tilt and slam tilt */
#define SW_TILT					MAKE_SWITCH(1,4)
#define SW_SLAM_TILT				MAKE_SWITCH(2,1)

/* Always closed switch */
#define SW_ALWAYS_CLOSED		MAKE_SWITCH(2,4)

/* Coin door closed switch */
#define SW_COINDOOR_CLOSED		MAKE_SWITCH(2,2)

/* Shooter switch */
#define SW_SHOOTER				MAKE_SWITCH(2,7)


#define AR_RAW			0
#define AR_CHANGED 	1
#define AR_PENDING 	2
#define AR_QUEUED 	3
#define NUM_SWITCH_ARRAYS 	4

#ifndef __SASM__
extern uint8_t switch_bits[NUM_SWITCH_ARRAYS][SWITCH_BITS_SIZE];

void switch_init (void);
void switch_sched (uint8_t sw);
void switch_idle_task (void);

#endif


#endif /* _SYS_SWITCH_H */
