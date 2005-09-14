#ifndef _SYS_SWITCH_H
#define _SYS_SWITCH_H

#ifndef __SASM__

/** Small integer typedef for a switch number */
typedef uint8_t switchnum_t;

/** Switch flags */
#define SW_OPTICAL	0x01 /* Switch is active when it is _open_ */
#define SW_EDGE		0x02 /* Switch is handled on any edge */
#define SW_PLAY		0x04 /* Switch marks a ball as 'in play' */

/** Switch handler prototype form */
typedef void (*switch_handler_t) (void);

/** A switch descriptor.  Contains all of the static information
 * about a particular switch */
typedef struct
{
	void (*fn) (void);
	uint8_t flags;
	uint8_t pad[1]; /* Keep this aligned to a power of 2! */
} switch_info_t;

#endif /* __SASM__ */

#define NUM_PF_SWITCHES 64
#define NUM_DEDICATED_SWITCHES 8
#define NUM_SWITCHES (NUM_PF_SWITCHES + NUM_DEDICATED_SWITCHES)
#define SWITCH_BITS_SIZE	(NUM_SWITCHES / 8)

#define SW_COL(x)			((x) >> 3)
#define SW_ROW(x)			((x) & 0x07)
#define SW_ROWMASK(x)	(1 << SW_ROW(x))

#define MAKE_SWITCH(col,row)	((col * 8) + row - 1)
#define MAKE_SW(row,col)  MAKE_SWITCH(row,col)

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

/* Always closed switch */
#ifndef SW_ALWAYS_CLOSED
#define SW_ALWAYS_CLOSED		MAKE_SWITCH(2,4)
#endif

/* Coin door closed switch */
#ifndef SW_COINDOOR_CLOSED
#define SW_COINDOOR_CLOSED		MAKE_SWITCH(2,2)
#endif

#define AR_RAW			0
#define AR_CHANGED 	1
#define AR_PENDING 	2
#define AR_QUEUED 	3
#define NUM_SWITCH_ARRAYS 	4

#ifndef __SASM__
extern uint8_t switch_bits[NUM_SWITCH_ARRAYS][SWITCH_BITS_SIZE];

#if 0
typedef struct switch_props
{
	switch_handler_t handler;
	uint8_t flags;
	uint8_t min_activation_time;
	uint8_t min_deactivation_time;
} switch_props_t;
#endif

void switch_init (void);
void switch_rtt (void);
void switch_sched (void);
void switch_idle_task (void);
bool switch_poll (const switchnum_t sw);
bool switch_is_opto (const switchnum_t sw);
bool switch_poll_logical (const switchnum_t sw);

#endif


#endif /* _SYS_SWITCH_H */
