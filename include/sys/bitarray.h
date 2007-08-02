/*
 * Copyright 2006, 2007 by Brian Dominy <brian@oddchange.com>
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

#ifndef _SYS_BITARRAY_H
#define _SYS_BITARRAY_H

typedef U8 *bitset;

#ifdef CONFIG_PLATFORM_WPC

/** WPC_BITOP is an assembly macro for accessing the WPC shifter
 * hardware. */
#define WPC_BITOP(bs, index, bitop)				\
do {														\
	__asm__ volatile (								\
		"st%0\t" C_STRING(WPC_SHIFTADDR) "\n"	\
		"\tst%1\t" C_STRING(WPC_SHIFTBIT) "\n" \
		"\tld%0\t" C_STRING(WPC_SHIFTADDR) "\n"\
		"\tld%1\t" C_STRING(WPC_SHIFTBIT) "\n"	\
		bitop												\
		: "+a" (bs), "+q" (index)				   \
	);														\
} while (0)


#define __setbit(bs, index) \
	WPC_BITOP (bs, index, "\tor%1\t,%0\n\tst%1\t,%0")

#define __clearbit(bs, index) \
	WPC_BITOP (bs, index, "\tcom%1\n\tand%1\t,%0\n\tst%1\t,%0")

#define __togglebit(bs, index) \
	WPC_BITOP (bs, index, "\teor%1\t,%0\n\tst%1\t,%0")

#define __testbit(bs, index) \
	WPC_BITOP (bs, index, "\tand%1\t,%0")

#else

#define __setbit(bs,index) (bs[index / 8] |= (1 << (index % 8)))
#define __clearbit(bs,index) (bs[index / 8] &= ~(1 << (index % 8)))
#define __togglebit(bs,index) (bs[index / 8] ^= (1 << (index % 8)))
#define __testbit(bs,index) (index = (bs[index / 8] & (1 << (index % 8))))

#endif /* __wpc__ */

#endif /* _SYS_BITARRAY_H */
