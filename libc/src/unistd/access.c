#include "errno.h"
#include "libc/dos.h"
#include "libc/xmonitor.h"
#include "libc/types.h"


int access(const char *filename, int type) 
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

   return __xmon_dos_access(drive, name, type);
}
