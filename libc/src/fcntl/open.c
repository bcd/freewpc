#include "errno.h"
#include "fcntl.h"
#include "stdarg.h"
#include "stdio.h"
#include "unistd.h"
#include "libc/dos.h"
#include "libc/xmonitor.h"
#include "libc/types.h"


uint8 __dos_oflag[OPEN_MAX];


int open(const char *filename, int oflag, ...)
{
   int8 drive;
   va_list ap;
   mode_t mode;
   int fd;
   char *name;
   
   /* test for at least one mandatory flag */
   if ((oflag & (O_RDONLY | O_WRONLY | O_RDWR)) == 0)
      return -1;

   if (__xmon_dos_first) {
      if (__xmon_dos_init() != 0)
         return -1;
   }

   if ((drive = __dos_getdrive(filename)) < 0) {
      errno = ENODEV;
      return -1;
   }

   name = __dos_getname(filename);

   /* test for existing file */
   if (__xmon_dos_access(drive, name, F_OK) == 0) {
      if ((oflag & (O_CREAT | O_EXCL)) == (O_CREAT | O_EXCL)) {
         errno = EEXIST;
         return -1;
      }

      if ((oflag & O_TRUNC) && (oflag & (O_WRONLY | O_RDWR)))
         __xmon_dos_unlink(drive, name);

      mode = S_BIN;  /* TODO: preserve old mode */
   }
   else {
      if ((oflag & O_CREAT) == 0) {
         errno = ENOENT;
         return -1;
      }

      va_start(ap, oflag);
      mode = va_arg(ap, int);
      va_end(ap);
   }

   fd = __xmon_dos_open(drive, name, mode);
   if (fd<0)
      return -1;

   __dos_oflag[fd] = oflag & (O_RDONLY | O_WRONLY | O_RDWR | O_APPEND);

   if (oflag & O_APPEND)
      lseek(fd, 0, SEEK_END);

   return fd;
}
