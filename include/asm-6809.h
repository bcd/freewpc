#ifndef _ASM_6809_H
#define _ASM_6809_H

extern inline void set_stack_pointer (const uint16_t s)
{
	asm ("\tlds %0" :: "L" (s));
}


extern inline void set_direct_page_pointer (const uint8_t dp)
{
	asm ("\ttfr b, dp" :: "d" (dp));
}


#endif /* _ASM_6809_H */
