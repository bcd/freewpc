
#ifndef _FREEWPC_H
#define _FREEWPC_H

#define offsetof(type, field)		&(((type *)0)->field)

/* Include the standard header files that are needed
 * by most modules */
#include <wpc.h>
#include <version.h>
#ifndef __SASM__
#include <sys/bitarray.h>
#include <sys/errno.h>
#include <sys/task.h>
#endif
#include <sys/sol.h>
#include <sys/lamp.h>
#include <sys/switch.h>
#include <sys/sound.h>
#include <sys/dmd.h>

#include <sys/segment.h>
#ifndef __SASM__
#include <sys/deff.h>
#include <score.h>
#include <coin.h>
#include <game.h>
#include <mach/config.h>
#endif

#endif /* _FREEWPC_H */
