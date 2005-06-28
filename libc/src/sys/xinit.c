#include "stdlib.h"
#include "libc/types.h"
#include "libc/xmonitor.h"
#include "sys/mem.h"


uint8 __xmon_first = 1;
volatile uint8 __xmon_errno;

static unsigned int orig_errvec;

static void error_handler(void);
static void __xmon_exit(void);


int __xmon_init(void)
{
   uint8 model;

   /* try to detect whether reset is needed */
   model = _peekb(MODELE);
   if ((model<1) || (model>3)) {
      __asm__ __volatile__ ("clrb \n"  /* RESETC code: 0 */
         "	jsr 0xEC0C"
         :
         :
         : "b", "d"
      );
   }

   /* save original error vector */
   orig_errvec = _peekw(ZPERR);

   /* hook up error vector */
   _pokew(ZPERR, (unsigned int)error_handler);

   /* register clean-up function */
   atexit(__xmon_exit);

   __xmon_first = 0;

   return 0;
}


static void error_handler(void)
{
   __asm__ __volatile__ ("stb ___xmon_errno \n"
      "	lds 0x6175 \n"
      "	puls a,dp,x,y,u,pc"
   );

   /* not reached */
}


static void __xmon_exit(void)
{
   _pokew(ZPERR, orig_errvec);
}
