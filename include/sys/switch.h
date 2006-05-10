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

#ifndef _SYS_SWITCH_H
#define _SYS_SWITCH_H

/** Small integer typedef for a switch number */
typedef U8 switchnum_t;

/** Switch flags */
#define SW_OPTICAL	0x01 /* Switch is active when it is _open_ */
#define SW_EDGE		0x02 /* Switch is handled on any edge */
#define SW_IN_GAME	0x08 /* Only service switch during a game */
#define SW_PLAYFIELD	0x10 /* Declares that switch is 'on the playfield' */
#define SW_NOPLAY	0x20 /* Switch does not mark a ball as 'in play' */
#define SW_IN_TEST	0x40 /* Service switch in test mode; default is no */

/** Switch handler prototype form */
typedef void (*switch_handler_t) (void);

/** A switch descriptor.  Contains all of the static information
 * about a particular switch */
typedef struct
{
	void (*fn) (void);
	U8 fnpage;
	U8 flags;
	U8 lamp;
	sound_code_t sound;
	task_ticks_t active_time;
	task_ticks_t inactive_time;
	int devno;
#if 0
	uint8_t pad[8]; /* Keep this aligned to a power of 2! */
#endif
} switch_info_t;


#define DECLARE_SWITCH_DRIVER(name)	const switch_info_t name =

#define SW_DEVICE_DECL(real_devno)	((real_devno) + 1)

#define SW_HAS_DEVICE(sw)	(sw->devno != 0)

#define SW_GET_DEVICE(sw)	(sw->devno - 1)


#define NUM_PF_SWITCHES 64
#define NUM_DEDICATED_SWITCHES 8
#define NUM_FLIPPER_SWITCHES 8

#define NUM_SWITCHES (NUM_PF_SWITCHES + NUM_DEDICATED_SWITCHES + NUM_FLIPPER_SWITCHES)

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

#if 0
/* Trough switch numbers (game-specific) */
#define NUM_TROUGH_SWITCHES	3
#define SW_TROUGH1				MAKE_SWITCH(1,5)
#define SW_TROUGH2				MAKE_SWITCH(1,6)
#define SW_TROUGH3				MAKE_SWITCH(1,7)
#define SW_TROUGH4				0
#define SW_TROUGH5				0
#define SW_TROUGH6				0
#endif

/* Flipper switches */
#define SW_LR_FLIP_EOS			MAKE_SWITCH(9,1)
#define SW_LR_FLIP_SW			MAKE_SWITCH(9,2)
#define SW_LL_FLIP_EOS			MAKE_SWITCH(9,3)
#define SW_LL_FLIP_SW			MAKE_SWITCH(9,4)
#define SW_UR_FLIP_EOS			MAKE_SWITCH(9,5)
#define SW_UR_FLIP_SW			MAKE_SWITCH(9,6)
#define SW_UL_FLIP_EOS			MAKE_SWITCH(9,7)
#define SW_UL_FLIP_SW			MAKE_SWITCH(9,8)

/* Always closed switch */
#ifndef SW_ALWAYS_CLOSED
#define SW_ALWAYS_CLOSED		MAKE_SWITCH(2,4)
#endif

/* Coin door closed switch */
#ifndef SW_COINDOOR_CLOSED
#define SW_COINDOOR_CLOSED		MAKE_SWITCH(2,2)
#endif

/* Array types. */
#define AR_RAW			0
#define AR_CHANGED 	1
#define AR_PENDING 	2
#define AR_QUEUED 	3
#define AR_RUNNING   4
#define NUM_SWITCH_ARRAYS 	5

extern uint8_t switch_bits[NUM_SWITCH_ARRAYS][SWITCH_BITS_SIZE];

void switch_init (void);
void switch_rtt (void);
void switch_sched (void);
void switch_idle_task (void);
bool switch_poll (const switchnum_t sw);
bool switch_is_opto (const switchnum_t sw);
bool switch_poll_logical (const switchnum_t sw);

#endif /* _SYS_SWITCH_H */
