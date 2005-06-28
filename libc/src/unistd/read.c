#include "errno.h"
#include "fcntl.h"
#include "stdio.h"
#include "libc/dos.h"
#include "libc/xmonitor.h"
#include "sys/mem.h"
#include "sys/types.h"


ssize_t read(int fd, void *buffer, size_t length)
{
   size_t i;

   /* sanity check */
   if ((fd<0) || (fd>OPEN_MAX-1) || (__dos_oflag[fd] & O_WRONLY)) {
      errno = EBADF;
      return -1;
   }

   /* fix global parameters */
   _pokeb(FCBNUM, fd);
   _pokeb(PUTFLG, 0);  /* GET */

   __xmon_errno = 0;

   for (i=0; i<length; i++) {
      /* do the call to the extra-monitor routine */
      __asm__ __volatile__ ("ldx #0 \n"
         "	ldb #07 \n"  /* PUTGET code: 07 */
         "	jsr 0xEC0C"
         :
         :
         : "x", "b", "d"
      );

      /* detect error */
      if (__xmon_errno)
         break;

      *(unsigned char *)buffer++ = __xmon_dos_data;
   }

   return i;
}
