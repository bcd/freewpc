/*
 * Raw memory access functions.
 *

You should *always* include this file when using these functions and
compile with optimization enabled.  They don't exist as normal functions
in any library, and they compile down to only a few opcodes when used
this way.

If you don't use optimization, this file becomes prototypes for
mem.c, which generates real functions for these when not optimizing. 

*/

#ifndef _SYS_MEM_H
#define _SYS_MEM_H

#ifdef __cplusplus
extern "C" {
#endif


extern __inline__ void _pokeb(unsigned int addr, unsigned char val)
{
   __asm__ __volatile__ ("st%b0 ,%w1" : : "r" (val), "a" (addr));
}


extern __inline__ void _pokew(unsigned int addr, unsigned int val)
{
   __asm__ __volatile__ ("st%w0 ,%w1" : : "r" (val), "a" (addr));
}


extern __inline__ unsigned char _peekb(unsigned int addr)
{
   unsigned char result;

   __asm__ __volatile__ ("ld%b0, %w1" : "=r" (result) : "a" (addr));

   return result;
}


extern __inline__ unsigned int _peekw(unsigned int addr)
{
   unsigned int result;

   __asm__ __volatile__ ("ld%w0, %w1" : "=r" (result) : "a" (addr));

   return result;
}


#ifdef __cplusplus
}
#endif

#endif /* !defined _SYS_MEM_H */ 
