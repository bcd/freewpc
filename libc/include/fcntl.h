/*
 *	POSIX Standard: 6.5 File Control Operations	<fcntl.h>
 */

#ifndef _FCNTL_H
#define _FCNTL_H

#include <sys/types.h>

#define O_RDONLY  0x0001
#define O_WRONLY  0x0002
#define O_RDWR    0x0004
#define O_APPEND  0x0008  /* should fit in a byte */

#define O_CREAT   0x0010
#define O_EXCL    0x0020
#define O_TRUNC   0x0040

#define S_BIN     0x0000
#define S_EXEC    0x0200
#define S_ASCII   0x01FF

extern int open(const char *filename, int oflag, ...);
extern int creat(const char *filename, mode_t mode);


#endif /* !defined _FCNTL_H */
