#include "libc/xmonitor.h"
#include "sys/mem.h"


int close(int fd)
{
   /* fix global parameters */
   _pokeb(FCBNUM, fd);

   /* do the call to the extra-monitor routine */
   __asm__ __volatile__ ("ldb #06 \n"  /* CLOSE code: 06 */
      "	jsr 0xEC0C"
      :
      :
      : "b", "d"
   );

   return 0;
}
