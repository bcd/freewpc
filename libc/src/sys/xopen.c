#include "errno.h"
#include "stdio.h"
#include "string.h"
#include "libc/monitor.h"
#include "libc/xmonitor.h"
#include "libc/types.h"
#include "sys/mem.h"


unsigned char __xmon_dos_data;


int __xmon_dos_open(int8 drive, const char *name, mode_t mode)
{
   /* fix global parameters */
   _pokeb(DKDRV, drive);
   _pokeb(FILMOD, 0x40); /* direct mode */
   _pokew(RLEN, 1);
   _pokew(BUFFRE, (int)&__xmon_dos_data);
   memcpy((void *)FILNAM, name, NAME_MAX);
   _pokew(FILTYP, mode);

   __xmon_errno = 0;

   /* do the call to the extra-monitor routine */
   __asm__ __volatile__ ("ldb #03 \n"  /* OPEN code: 03 */
      "	jsr 0xEC0C"
      :
      :
      : "b", "d"
   );

   if (__xmon_errno) {
      errno = __xmon_translate_err(__xmon_errno);
      return -1;
   }

   return _peekb(FCBNUM);
}
