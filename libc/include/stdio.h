/*
 *	ISO C99 Standard: 7.19 Input/output	<stdio.h>
 */

#ifndef _STDIO_H
#define _STDIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <sys/types.h>

#define _IOFBF   0x0001
#define _IONBF   0x0002
#define _IOLBF   0x0004
#define _IOREAD  0x0008
#define _IOWRT   0x0010
#define _IORW    0x0020
#define _IOMYBUF 0x0040
#define _IOSTRG  0x0080

#define EOF (-1)
#define NAME_MAX      11  /* 8+3 characters */
#define FILENAME_MAX  11
#define OPEN_MAX      4
#define FOPEN_MAX     4
#define TMP_MAX       25

#define SEEK_SET  0    /* seek from beginning of file */
#define SEEK_CUR  1    /* seek from current position  */
#define SEEK_END  2    /* seek from end of file       */


/* Note that the definitions of these fields are NOT guaranteed!
 * They may change with any release without notice!  The fact that
 * they are here at all is to comply with ANSI specifications.
 */  
typedef struct {
   int fd;
   int flag;
   unsigned int cnt;
   char *ptr;
   char *base;
   size_t bufsiz;
   size_t fillsize;
} FILE;

/* extern FILE __stdin, __stdout, __stderr;
#define stdin   (&__stdin)
#define stdout  (&__stdout)
#define stderr  (&__stderr) */

#define stdin   (FILE *)0
#define stdout  (FILE *)1
#define stderr  (FILE *)2

extern int fclose(FILE *stream);
extern int fflush(FILE *_stream);
extern FILE *fopen(const char *filename, const char *mode);
extern int printf(const char *format, ...) __attribute__ ((format (printf, 1, 2)));
extern int putc(unsigned char c, FILE *file);
extern int putchar(unsigned char c);
extern int puts(const char *s);
extern int remove(const char *filename);
extern int rename(const char *old_filename, const char *new_filename);
extern int sprintf(char *s, const char *format, ...) __attribute__ ((format (printf, 2, 3)));

extern int _doprnt(const char *fmt, va_list args, FILE *fp);


#ifdef __cplusplus
}
#endif

#endif /* !defined _STDIO_H */
