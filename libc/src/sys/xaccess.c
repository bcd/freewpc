#include "fcntl.h"
#include "stdio.h"
#include "string.h"
#include "unistd.h"
#include "libc/monitor.h"
#include "libc/xmonitor.h"
#include "libc/types.h"
#include "sys/mem.h"


int __xmon_dos_access(int8 drive, const char *name, int type)
{
   int slot;

   /* fix two global parameters */
   _pokeb(DKDRV, drive);
   memcpy((void *)FILNAM, name, NAME_MAX);

   /* do the call to the extra-monitor routine */
   __asm__ __volatile__ ("ldb #08 \n"  /* DIR0 code: 08 */
      "	jsr 0xEC0C"
      :
      :
      : "b", "d"
   );

   /* test for existence */
   if (_peekb(NAMSEC) == '\0')
      return -1;

   if (type & X_OK) {
      slot = _peekw(NAMSLT);
      return (_peekw(slot+11) == S_EXEC ? 0 : -1);
   }

   return 0;
}
