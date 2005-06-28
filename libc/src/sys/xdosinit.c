/*
 * Extra-monitor: DOS initialization
 */

#include "errno.h"
#include "stdlib.h"
#include "stdio.h"
#include "libc/dos.h"
#include "libc/xmonitor.h"
#include "libc/types.h"
#include "sys/mem.h"


uint8 __xmon_dos_first = 1;

static unsigned int orig_sectbuf;
static unsigned int orig_fatptr;
static unsigned int orig_fcbptr;

static void __xmon_dos_exit(void);


int __xmon_dos_init(void) 
{
   void *sectbuf = NULL, *fatptr = NULL, *fcbptr = NULL;

   if (__xmon_first)
      __xmon_init();

   /* save original pointers */
   orig_sectbuf = _peekw(SECTBUF);
   orig_fatptr  = _peekw(FATPTR);
   orig_fcbptr  = _peekw(FCBPTR);

   /* allocate new memory areas */
   sectbuf = malloc(SECTLEN);
   if (!sectbuf)
      goto Error;

   fatptr = malloc(NDRV * DSBLEN);
   if (!fatptr)
      goto Error;

   fcbptr = malloc(OPEN_MAX * FCBLEN);
   if (!fcbptr)
      goto Error;

   /* set first two parameters */
   _pokew(SECTBUF, (unsigned int)sectbuf);
   _pokew(FATPTR, (unsigned int)fatptr);

   /* call extra-monitor DOS init routine */
   __asm__ __volatile__ ("ldy %0 \n"
     "	lda %1  \n"
     "	ldx %2  \n"
     "	ldb #02 \n"  /* FCBINI code: 02 */
     "	jsr 0xEC0C"
     :
     : "i" (NDRV), "i" (OPEN_MAX), "m" (fcbptr)
     : "y", "a", "x", "b", "d"
   );

   /* register clean-up function */
   atexit(__xmon_dos_exit);

   __xmon_dos_first = 0;
   return 0;

 Error:
   errno = ENOMEM;

   if (sectbuf)
      free(sectbuf);

   if (fatptr)
      free(fatptr);

   if (fcbptr)
      free(fcbptr);

   return -1;
}


static void __xmon_dos_exit(void)
{
   /* restore first two parameters */
   _pokew(SECTBUF, (unsigned int)orig_sectbuf);
   _pokew(FATPTR, (unsigned int)orig_fatptr);

   /* call extra-monitor DOS init routine with BASIC 512 parameters */
   __asm__ __volatile__ ("ldy %0 \n"
     "	lda %1  \n"
     "	ldx %2  \n"
     "	ldb #02 \n"  /* FCBINI code: 02 */
     "	jsr 0xEC0C"
     :
     : "i" (5), "i" (2), "m" (orig_fcbptr)
     : "y", "a", "x", "b", "d"
   );
}
