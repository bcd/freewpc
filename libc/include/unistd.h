/*
 *	POSIX Standard: 2.10 Symbolic Constants		<unistd.h>
 */

#ifndef _UNISTD_H
#define _UNISTD_H

#include <sys/types.h>


/* Values for the second argument to access.
   These may be OR'd together.  */
#define R_OK  4   /* Test for read permission    */
#define W_OK  2   /* Test for write permission   */
#define X_OK  1   /* Test for execute permission */
#define F_OK  0   /* Test for existence          */

extern int access(const char *filename, int type);
extern int close(int fd);


/* Values for the WHENCE argument to lseek.  */
#ifndef _STDIO_H         /* <stdio.h> has the same definitions.  */
  #define SEEK_SET  0    /* Seek from beginning of file.  */
  #define SEEK_CUR  1    /* Seek from current position.  */
  #define SEEK_END  2    /* Seek from end of file.  */
#endif

extern off_t   lseek(int fd, off_t offset, int whence);
extern ssize_t read(int fd, void *buffer, size_t length);
extern int     unlink(const char *filename);
extern ssize_t write(int fd, const void *buf, size_t count);

extern int __file_exists(const char *filename);


#endif /* !defined _UNISTD_H */
