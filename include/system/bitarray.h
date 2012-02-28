/*
 * Copyright 2006-2011 by Brian Dominy <brian@oddchange.com>
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
typedef const U8 *const_bitset;

#if defined(__m6809__) && !defined(CONFIG_PLATFORM_WPC)
#define CONFIG_SINGLE_BIT_SET_ARRAY
extern U8 single_bit_set_array[];
#define single_bit_set(n) single_bit_set_array[n]
#else
#define single_bit_set(n) (1 << (n))
#endif

#define bitarray_const_offset(bits, bitno) (bits[(bitno) / 8])

#define bitarray_const_mask(bits, bitno) single_bit_set ((bitno) % 8)

/* Non-optimized macros for twiddling bits.
 *
 * These work well when the bit number is a constant
 * as the location in the bitarray and the mask can all be 
 * computed at compile time efficiently.
 *
 * These macros will work for non-constants, too, but this should
 * be avoided on the WPC because the code will not be optimal.
 * Under Linux simulation, these macros are always used.
 * WPC has shifter harwdare that can do a better job when the
 * bitno is non-constant. */

#define bitarray_const_set(bits, bitno) \
	(bitarray_const_offset (bits, bitno) |= bitarray_const_mask (bits, bitno))

#define bitarray_const_clear(bits, bitno) \
	(bitarray_const_offset (bits, bitno) &= ~bitarray_const_mask (bits, bitno))

#define bitarray_const_toggle(bits, bitno) \
	(bitarray_const_offset (bits, bitno) ^= bitarray_const_mask (bits, bitno))

#define bitarray_const_test(bits, bitno) \
	(bitarray_const_offset (bits, bitno) & bitarray_const_mask (bits, bitno))


/* WPC shifter macros.  When compiling on a different platform,
 * these macros are never used, but are defined anyway (to zero)
 * just to avoid compile errors. */

#if defined(CONFIG_PLATFORM_WPC) && !defined(CONFIG_NATIVE)

#define __bitarray_constant_p(bitno) __builtin_constant_p (bitno)

#define WPC_BITOP(bs, index, bitop)				\
	({ unsigned char *bs1 = (unsigned char *)bs; \
	   unsigned char v = (unsigned char)index; \
      __asm__ volatile (                     \
		"st%0\t" C_STRING(WPC_SHIFTADDR) "\n"	\
		"\tst%1\t" C_STRING(WPC_SHIFTBIT) "\n" \
		"\tld%0\t" C_STRING(WPC_SHIFTADDR) "\n"\
		"\tld%1\t" C_STRING(WPC_SHIFTBIT) "\n"	\
		bitop												\
		: "+a" (bs1), "+q" (v)             \
	); v; })

#define wpc_setbit(bs, index) \
	WPC_BITOP (bs, index, "\tor%1\t,%0\n\tst%1\t,%0")

#define wpc_clearbit(bs, index) \
	WPC_BITOP (bs, index, "\tcom%1\n\tand%1\t,%0\n\tst%1\t,%0")

#define wpc_togglebit(bs, index) \
	WPC_BITOP (bs, index, "\teor%1\t,%0\n\tst%1\t,%0")

#define wpc_testbit(bs, index) \
	WPC_BITOP (bs, index, "\tand%1\t,%0")

#else /* !CONFIG_PLATFORM_WPC */

#define __bitarray_constant_p(bitno) 1
#define wpc_setbit(bits,bitno) bitarray_const_set(bits,bitno)
#define wpc_clearbit(bits,bitno) bitarray_const_clear(bits,bitno)
#define wpc_togglebit(bits,bitno) bitarray_const_toggle(bits,bitno)
#define wpc_testbit(bits,bitno) bitarray_const_test(bits,bitno)

#endif


/* Generic macros that work on both constant and non-constant
 * bit numbers.  This uses a GCC extension that knows what's a constant
 * and what's not, so you can just use these and the best call
 * will be made. */

#define bitarray_set(bits, bitno) \
	((void)(__bitarray_constant_p (bitno) ? \
	   bitarray_const_set (bits, bitno) : wpc_setbit(bits, bitno)))

#define bitarray_clear(bits, bitno) \
	((void)(__bitarray_constant_p (bitno) ? \
	   bitarray_const_clear (bits, bitno) : wpc_clearbit(bits, bitno)))

#define bitarray_toggle(bits, bitno) \
	((void)(__bitarray_constant_p (bitno) ? \
	   bitarray_const_toggle (bits, bitno) : wpc_togglebit(bits, bitno)))

#define bitarray_test(bits, bitno) \
	(__bitarray_constant_p (bitno) ? \
	   bitarray_const_test (bits, bitno) : wpc_testbit(bits, bitno))

#endif /* _SYS_BITARRAY_H */
