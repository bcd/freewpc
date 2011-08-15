/*
 * Copyright 2006, 2007, 2008, 2011 by Brian Dominy <brian@oddchange.com>
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

#include <freewpc.h>

/**
 * \file
 * \brief This module implements a simple random number generator.
 * This generator does not satisfy the properties required for a really
 * good generator, but is adequate for what pinball machines need.
 *
 * It is a linear congruential generator, derived from the relation
 * Xn+1 = (A(Xn) + C) mod M, where A=33, C=1, and M=255.  This is
 * fairly easy to do with shifts and adds.  This produces a sequence that
 * has a full period, i.e. it won't repeat until all 65536 values have been
 * produced.  The most variance is in the upper 8-bits, so that is all that
 * is returned to the caller.
 *
 * To add an element of true randomness, we will periodically drain 1
 * random number from the sequence according to some external, unpredictable
 * event.  These events do not need to be frequent, but should sufficiently
 * vary.
 */


/** The seed for the linear congruential component of the random numbers. */
__permanent__ U16 random_cong_seed;

/** A count of true random events */
U8 random_entropy;


/**
 * Return a new random number from 0-255.
 */
__attribute__((noinline)) U8 random (void)
{
	register U16 r = random_cong_seed * 33 + 1;
	random_cong_seed = r ^ (r >> 1);
	return (r >> 8);
}


/**
 * Return a new random number from 0 to (N-1).
 */
U8
random_scaled (U8 N)
{
	return (random () * (U16)N) >> 8;
}


/**
 * Drain entropy from the sequence every second.
 * The frequency is chosen long because random numbers are not drawn
 * very often.
 */
CALLSET_ENTRY (random, idle_every_second)
{
	random_entropy %= 8;
	while (random_entropy)
	{
		(void)random ();
		random_entropy--;
	}
}


/**
 * Assert a random hardware evnt occurred to stir the random
 * number sequence later.
 */
void random_hw_event (void)
{
	random_entropy++;
}


/**
 * Initialize the random number generator.
 */
CALLSET_ENTRY (random, init)
{
	random_cong_seed = 0x1745;
	random_entropy = 0;
}

