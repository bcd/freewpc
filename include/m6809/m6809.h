/*
 * Copyright 2006, 2007, 2008 by Brian Dominy <brian@oddchange.com>
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

#ifndef _ASM_6809_H
#define _ASM_6809_H

/**
 * \brief Various definitions/macros for the Motorola 6809 MPU.
 */

/* Basic properties of the CPU architecture */
#define BITS_PER_BYTE 8
#define BITS_PER_WORD 16

/* Defines for various bits in the condition code register */
#define CC_CARRY 		0x1
#define CC_OVERFLOW 	0x2
#define CC_ZERO 		0x4
#define CC_NEGATIVE 	0x8
#define CC_IRQ 		0x10
#define CC_HALF 		0x20
#define CC_FIRQ 		0x40
#define CC_E 			0x80

/* Defines for various assembler routines that can be called from C */
__attribute__((noreturn)) void start (void);
U8 far_read8 (const void *address, U8 page);
U16 far_read16 (const void *address, U8 page);
void *far_read_pointer (const void *address, U8 page);
typedef void (*void_function) (void);
void far_indirect_call_handler (void_function address, U8 page);
typedef U8 (*value_function) (void);
U8 far_indirect_call_value_handler (value_function address, U8 page);
void bitmap_blit_asm (U8 *dst, U8 shift);

/* Other externals */

extern void *_far_call_address;
extern U8 _far_call_page;
extern const U8 *bitmap_src;
extern U8 *bitmap_dst;

void slow_memset (U8 *dst, U8 val, U16 len);
void slow_memcpy (U8 *dst, const U8 *src, U16 len);

/* Give an alternate, global name to a function */
#define aka(name) asm (C_STRING(_) name C_STRING(::))

#define far_call_pointer(function, page, arg) \
do { \
	_far_call_address = (void *)function; \
	_far_call_page = page; \
	if (sizeof (arg) == 1) \
		asm (";short arg" :: "q" (arg)); \
	else \
		asm (";long arg" :: "a" (arg)); \
	asm ("jsr\t_far_call_pointer_handler"); \
} while (0);

void *malloc (U8 size);
void free (void *ptr);


extern inline void set_stack_pointer (const U16 s)
{
	asm __volatile__ ("lds\t%0" :: "g" (s) : "d");
}


extern inline U16 get_stack_pointer (void)
{
	U16 result;
	asm __volatile__ ("lea%0\t,s" : "=a" (result));
	return result;
}

extern inline void set_direct_page_pointer (const U8 dp)
{
	asm __volatile__ ("tfr\tb, dp" :: "q" (dp));
}


extern inline void m6809_andcc (const U8 bits)
{
	asm __volatile__ ("andcc\t%0" :: "i" (bits));
}


extern inline void m6809_orcc (const U8 bits)
{
	asm __volatile__ ("orcc\t%0" :: "i" (bits));
}

extern inline void m6809_firq_save_regs (void)
{
	asm __volatile__ ("pshs\td,x");
}

extern inline void m6809_firq_restore_regs (void)
{
	asm __volatile__ ("puls\td,x");
}

/** Optimized memset function.
 * The length n should be a constant.
 * Based on the length, one of the blocks of code will be
 * expanded, whichever is the most optimal.
 * 8-byte aligned sizes will copy 4 words at a time.
 * When 2-byte aligned, copy a word at a time.
 * Otherwise, copy one byte at a time.
 *
 * TODO - for small, odd values of N, no optimization is
 * done and the memset occurs one byte at a time; with small
 * N the loop overhead is significant.  Better to do it
 * inline, completely unrolled.  Try a recursive call to
 * this function...
 */
extern inline void *memset (void *s, U8 c, U16 n)
{
	if (n <= 5)
	{
		register U8 *s1 = (U8 *)s;
		if (n > 4) *s1++ = c;
		if (n > 3) *s1++ = c;
		if (n > 2) *s1++ = c;
		if (n > 1) *s1++ = c;
		*s1 = c;
	}
	else if ((n % 8) == 0)
	{
		register U16 *s1 = (U16 *)s;
		n /= 8;
		while (n > 0)
		{
			*s1++ = ((U16)c << 8) | c;
			*s1++ = ((U16)c << 8) | c;
			*s1++ = ((U16)c << 8) | c;
			*s1++ = ((U16)c << 8) | c;
			n--;
		}
	}
	else if ((n % 2) == 0)
	{
		register U16 *s1 = (U16 *)s;
		n /= 2;
		while (n > 0)
		{
			*s1++ = ((U16)c << 8) | c;
			n--;
		}
	}
	else
	{
		register char *s1 = (char *)s;
		while (n > 0)
		{
			*s1++ = c & 0xFF;
			n--;
		}
	}
	return (s);
}


extern inline void __blockclear16 (void *s1, U16 n)
{
	register U16 *_s1 = (U16 *)s1;

	/* It is tempting to predivide n by 16, and then
	just decrement it inside the loop, but this does
	not actually help any. */
	do
	{
		*_s1++ = 0UL;
		*_s1++ = 0UL;
		*_s1++ = 0UL;
		*_s1++ = 0UL;
		*_s1++ = 0UL;
		*_s1++ = 0UL;
		*_s1++ = 0UL;
		*_s1++ = 0UL;
		n -= 16;
	} while (n > 0);
}


extern inline void *memcpy (void *s1, const void *s2, U16 n)
{
	if ((n == 3) || (n == 5))
	{
		register U8 *_s1 = (U8 *)s1;
		register const U8 *_s2 = (U8 *)s2;
		*_s1 = *_s2;
		s1 = (char *)s1 + 1;
		s2 = (char *)s2 + 1;
		n--;
	}

	if ((n % 2) == 0)
	{
		register U16 *_s1 = (U16 *)s1;
		register const U16 *_s2 = (U16 *)s2;
		n /= 2;
		while (n > 0)
		{
			*_s1++ = *_s2++;
			n--;
		}
	}
	else
	{
		register U8 *_s1 = (U8 *)s1;
		register const U8 *_s2 = (U8 *)s2;
		while (n > 0)
		{
			*_s1++ = *_s2++;
			n--;
		}
	}
	return (s1);
}


extern inline void __blockcopy16 (void *s1, const void *s2, U16 n)
{
	register U16 *_s1 = (U16 *)s1;
	register U16 *_s2 = (U16 *)s2;

	/* It is tempting to predivide n by 16, and then
	just decrement it inside the loop, but this does
	not actually help any. */
	do
	{
		*_s1++ = *_s2++;
		*_s1++ = *_s2++;
		*_s1++ = *_s2++;
		*_s1++ = *_s2++;
		*_s1++ = *_s2++;
		*_s1++ = *_s2++;
		*_s1++ = *_s2++;
		*_s1++ = *_s2++;
		n -= 16;
	} while (n > 0);
}


extern inline bool memcmp (const void *s1, const void *s2, U16 n)
{
	if ((n % 2) == 0)
	{
		register U16 *w1 = (U16 *)s1;
		register U16 *w2 = (U16 *)s2;
		n /= 2;
		do
		{
			if (*w1 != *w2)
				return 1;
			n--;
		} while (n > 0);
	}
	else
	{
		register U8 *b1 = (U8 *)s1;
		register U8 *b2 = (U8 *)s2;
		do
		{
			if (*b1 != *b2)
				return 1;
			n--;
		} while (n > 0);
	}
	return 0;
}


extern inline U16 strlen (const char *s)
{
	U16 len = 0;
	while (*s != '\0')
	{
		len++;
		s++;
	}
	return (len);
}


#endif /* _ASM_6809_H */
