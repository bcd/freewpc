#include "errno.h"
#include "libc/dos.h"
#include "libc/types.h"
#include "libc/xmonitor.h"


int unlink(const char *filename)
{
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

   return __xmon_dos_unlink(drive, name);
}
