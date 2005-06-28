#include "errno.h"
#include "stdio.h"
#include "string.h"
#include "libc/dos.h"
#include "libc/types.h"
#include "libc/xmonitor.h"


int rename(const char *old, const char *new)
{
   int8 drive;
   char old_name[NAME_MAX];
   char *new_name;

   if (__xmon_dos_first) {
      if (__xmon_dos_init() != 0)
         return -1;
   }

   if ((drive = __dos_getdrive(old)) < 0) {
      errno = ENODEV;
      return -1;
   }

   /* __dos_getname() is not reentrant */
   memcpy(old_name, __dos_getname(old), NAME_MAX);
   new_name = __dos_getname(new);

   return __xmon_dos_rename(drive, old_name, new_name);
}
