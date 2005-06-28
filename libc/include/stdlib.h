/*
 *	ISO C99 Standard: 7.20 General utilities	<stdlib.h>
 */

#ifndef _STDLIB_H
#define _STDLIB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

#define EXIT_SUCCESS  0
#define EXIT_FAILURE  1
#define RAND_MAX      32767
#define NULL          0


#define MAX(a,b)  ((a)>(b) ? (a) : (b))
#define MIN(a,b)  ((a)<(b) ? (a) : (b))


extern void   abort(void);
extern int    abs(int i);
extern int    atexit(void (*func)(void));
extern void * bsearch(const void *key, const void *base, size_t nelem,
                      size_t size, int (*compar)(const void *, const void *));
extern void * calloc(size_t nelem, size_t size);
extern div_t  div(int numer, int denom);
extern void   exit(int status);
extern void   free(void *ptr);
extern long   labs(long int i);
extern ldiv_t ldiv(long numer, long denom);
extern void * malloc(size_t size);
extern int    rand(void);
extern void * realloc(void *ptr, size_t size);
extern void   srand(unsigned int seed);


#ifdef __cplusplus
}
#endif

#endif /* !defined _STDLIB_H */
