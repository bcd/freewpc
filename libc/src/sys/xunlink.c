#include "stdio.h"
#include "string.h"
#include "libc/monitor.h"
#include "libc/xmonitor.h"
#include "libc/types.h"
#include "sys/mem.h"


int __xmon_dos_unlink(int8 drive, const char *name)
{
   /* fix global parameters */
   _pokeb(DKDRV, drive);
   memcpy((void *)FILNAM, name, NAME_MAX);

   /* do the call to the extra-monitor routine */
   __asm__ __volatile__ ("ldb #14 \n"  /* KILL code: 14 */
      "	jsr 0xEC0C"
      :
      :
      : "b", "d"
   );

   return 0;
}
