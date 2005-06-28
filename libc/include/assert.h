/*
 *	ISO C99 Standard: 7.2 Diagnostics	<assert.h>
 */

#ifndef _ASSERT_H
#define _ASSERT_H

#ifdef __cplusplus
extern "C" {
#endif

extern void __assert(const char *assertion, const char *file,
                     unsigned int line, const char *function);

#ifdef __cplusplus
}
#endif

/* void assert (int expression);

   If NDEBUG is defined, do nothing.
   If not, and EXPRESSION is zero, print an error message and abort.  */

#ifdef NDEBUG

#define assert(expr)

#else

#define assert(expr)  (expr) ? 0 : __assert("(expr)", __FILE__, __LINE__, __ASSERT_FUNCTION)

#endif


/* Version 2.4 and later of GCC define a magical variable `__PRETTY_FUNCTION__'
   which contains the name of the function currently being defined.
   This is broken in G++ before version 2.6.
   C9x has a similar variable called __func__, but prefer the GCC one since
   it demangles C++ function names.  */
#if defined __cplusplus
  #define __ASSERT_FUNCTION	__PRETTY_FUNCTION__
#else
  #if defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
    #define __ASSERT_FUNCTION	__func__
  #else
    #define __ASSERT_FUNCTION	((__const char *) 0)
  #endif
# endif


#endif /* !defined _ASSERT_H */
