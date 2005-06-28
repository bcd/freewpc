/*
 *	POSIX Standard: 2.6 Primitive System Data Types	<sys/types.h>
 */

#ifndef _SYS_TYPES_H
#define _SYS_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
   int quot;
   int rem;
} div_t;

typedef struct {
   long int quot;
   long int rem;
} ldiv_t;

#define __SIZE_T  long unsigned int
typedef __SIZE_T size_t;

typedef unsigned int mode_t;

typedef signed int off_t;

typedef unsigned int ssize_t;

typedef unsigned int fpos_t;

typedef unsigned int clock_t;


#ifdef __cplusplus
}
#endif

#endif /* !defined _SYS_TYPES_H */
