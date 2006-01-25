#ifndef _ENV_H
#define _ENV_H

/* noreturn is a standard GCC attribute and is always
 * available */
#define __noreturn__ __attribute__((__noreturn__))

/* The remaining attributes are gcc6809 specific and may
 * or may not be available depending on the compiler
 * version used. */

#ifdef HAVE_FASTRAM_ATTRIBUTE
#define __fastram__ __attribute__((section("direct")))
#else
#define __fastram__
#endif

#ifdef HAVE_TASKENTRY_ATTRIBUTE
#define __taskentry__ __attribute__((__taskentry__))
#else
#define __taskentry__
#endif

#define __nvram__ __attribute__((section ("nvram")))

#ifdef CONFIG_MULTIPLAYER
#define __local__ __attribute__((section ("local")))
#else
#define __local__
#endif

#endif /* _ENV_H */
