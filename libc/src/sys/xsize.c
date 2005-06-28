#include "libc/xmonitor.h"
#include "sys/mem.h"
#include "sys/types.h"


size_t __xmon_dos_size(int fd)
{
   _pokeb(FCBNUM, fd);
   _pokeb(0x6105, 2);

   /* do the call to the extra-monitor routine */
   __asm__ __volatile__ ("ldb #18 \n"  /* LOF code: 18 */
      "	jsr 0xEC0C"
      :
      :
      : "b", "d"
   );

   if (_peekb(VALTYP) == 4) { /* need conversion from float to int ? */
      __asm__ __volatile__ ("lda #2 \n"
         "	ldb #50 \n"  /* FRCTYP code: 50 */
         "	jsr 0xEC0C"
         :
         :
         : "a", "b", "d"
      );
   }

   return _peekw(FACMO);
}
