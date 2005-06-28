#include "errno.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "libc/xmonitor.h"
#include "libc/types.h"


int __xmon_dos_rename(int8 drive, const char *old, const char *new)
{
   char *old_entry, *new_entry;

   old_entry = malloc(20);
   if (!old_entry) {
      errno = ENOMEM;
      return -1;
   }

   /* filling with zeros preserves the commentary */
   new_entry = calloc(20, sizeof(char));
   if (!new_entry) {
      errno = ENOMEM;
      return -1;
   }

   memcpy(old_entry, old, NAME_MAX);
   *(old_entry+19) = drive;

   memcpy(new_entry, new, NAME_MAX);
   *(new_entry+19) = drive;

   /* do the call to the extra-monitor routine */
   __asm__ __volatile__ ("ldx %0 \n"
      "	ldy %1  \n"
      "	ldb #15 \n"  /* NAME code: 15 */
      "	jsr 0xEC0C"
      :
      : "m" (old_entry), "m" (new_entry)
      : "b", "d"
   );

   free(new_entry);
   free(old_entry);
   return 0;
}
