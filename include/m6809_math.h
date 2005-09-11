#ifndef _M6809_MATH_H
#define _M6809_MATH_H

uint16_t div10 (uint8_t val);


#define DIV10(u8, quot, rem) \
do \
{ \
	uint16_t __unused_quot_rem __attribute__ ((unused)) = div10 (u8); \
	asm ("sta %0" :: "m" (quot)); \
	asm ("stb %0" :: "m" (rem)); \
} while (0)


#endif /* _M6809_MATH_H */
