/*
 *	ISO C99 Standard: 7.23 Date and time	<time.h>
 */

#ifndef _TIME_H
#define _TIME_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

#define CLOCKS_PER_SEC  10


extern clock_t clock(void);


#ifdef __cplusplus
}
#endif

#endif /* !defined _TIME_H */
