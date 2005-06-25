
#ifndef _SYS_SOL_H
#define _SYS_SOL_H

#include <mach/coil.h>

#ifndef __SASM__
typedef uint8_t solnum_t;

void sol_on (solnum_t sol);
void sol_off (solnum_t sol);
void sol_serve (void);
void sol_init (void);
#endif

#endif /* _SYS_SOL_H */
