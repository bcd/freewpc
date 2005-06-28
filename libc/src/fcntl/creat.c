#include "errno.h"
#include "fcntl.h"
#include "unistd.h"
#include "libc/dos.h"
#include "libc/xmonitor.h"
#include "libc/types.h"
#include "sys/types.h"


int creat(const char *filename, mode_t mode)
{
   int fd;
   int8 drive;
   char *name;
   
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
   if (__xmon_dos_access(drive, name, F_OK) == 0)
      __xmon_dos_unlink(drive, name);

   fd = __xmon_dos_open(drive, name, mode);
   if (fd<0)
      return -1;

   __dos_oflag[fd] = O_WRONLY;

   return fd;
}
