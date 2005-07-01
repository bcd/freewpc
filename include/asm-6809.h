#ifndef _ASM_6809_H
#define _ASM_6809_H

#define BITS_PER_BYTE 8
#define BITS_PER_WORD 16

#define CC_CARRY 		0x1
#define CC_OVERFLOW 	0x2
#define CC_ZERO 		0x4
#define CC_NEGATIVE 	0x8
#define CC_IRQ 		0x10
#define CC_HALF 		0x20
#define CC_FIRQ 		0x40
#define CC_E 			0x80

#ifndef __SASM__
extern inline void set_stack_pointer (const uint16_t s)
{
	asm ("\tlds %0" :: "g" (s));
}


extern inline void set_direct_page_pointer (const uint8_t dp)
{
	asm ("\ttfr b, dp" :: "d" (dp));
}

#endif

#endif /* _ASM_6809_H */
