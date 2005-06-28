#include "errno.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "libc/dos.h"
#include "libc/monitor.h"
#include "libc/types.h"
#include "sys/mem.h"


int __file_exists(const char *filename) 
{
   char *sectbuf, *name;
   int8 drive, i;
   int j;

   if ((drive = __dos_getdrive(filename)) < 0) {
      errno = ENODEV;
      return 0;
   }

   sectbuf = malloc(SECTLEN);
   if (!sectbuf) {
      errno = ENOMEM;
      return 0;
   }

   name = __dos_getname(filename);

   /* fix parameters */
   _pokeb(DKOPC, 2);  /* read sector */
   _pokeb(DKDRV, drive);
   _pokew(DKTRK, DIR_TRK);
   _pokew(DKBUF, (int)sectbuf);

   /* scan the whole directory */
   for (i = DIR_SEC; i <= NSEC; i++) {
      _pokeb(DKSEC, i);

      /* do the call to the monitor routine */
      __asm__ __volatile__ ("jsr 0xE82A");

      if (_peekb(DKSTA) == 0) {  /* success? */
         for (j = 0; j < SECTLEN; j += SLTLEN)
            if (memcmp(sectbuf+j, name, NAME_MAX) == 0)
               goto Found;
      }
   }

   free(sectbuf);
   return 0;

 Found:
   free(sectbuf);
   return 1;
}
