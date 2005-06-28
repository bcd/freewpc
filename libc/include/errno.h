/* Copyright (C) 1998 DJ Delorie, see COPYING.DJ for details */
/* Copyright (C) 1995 DJ Delorie, see COPYING.DJ for details */

/*
 *	ISO C99 Standard: 7.5 Errors	<errno.h>
 */

#ifndef _ERRNO_H
#define _ERRNO_H

#ifdef __cplusplus
extern "C" {
#endif


extern int errno;

#define EDOM		1
#define ERANGE		2  
#define E2BIG		3
#define EACCES		4
#define EAGAIN		5
#define EBADF		6
#define EBUSY		7
#define ECHILD		8
#define EDEADLK		9
#define EEXIST		10
#define EFAULT		11
#define EFBIG		12
#define EINTR		13
#define EINVAL		14
#define EIO		15
#define EISDIR		16
#define EMFILE		17
#define EMLINK		18
#define ENAMETOOLONG	19
#define ENFILE		20
#define ENODEV		21
#define ENOENT		22
#define ENOEXEC		23
#define ENOLCK		24
#define ENOMEM		25
#define ENOSPC		26
#define ENOSYS		27
#define ENOTDIR		28
#define ENOTEMPTY	29
#define ENOTTY		30
#define ENXIO		31
#define EPERM		32
#define EPIPE		33
#define EROFS		34
#define ESPIPE		35
#define ESRCH		36
#define EXDEV		37
#define ENMFILE		38
#define ELOOP           39
#define EOVERFLOW	40


extern char *__sys_errlist[];
extern int __sys_nerr;


#ifdef __cplusplus
}
#endif

#endif /* !defined _ERRNO_H */
