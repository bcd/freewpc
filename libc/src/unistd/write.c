#include "errno.h"
#include "fcntl.h"
#include "stdio.h"
#include "unistd.h"
#include "libc/dos.h"
#include "libc/xmonitor.h"
#include "sys/mem.h"
#include "sys/types.h"


ssize_t write(int fd, const void *buffer, size_t count)
{
   size_t i;

   /* sanity check */
   if ((fd<0) || (fd>OPEN_MAX-1) || (__dos_oflag[fd] & O_RDONLY)) {
      errno = EBADF;
      return -1;
   }

   if (__dos_oflag[fd] & O_APPEND)
      lseek(fd, 0, SEEK_END);

   /* fix global parameters */
   _pokeb(FCBNUM, fd);
   _pokeb(PUTFLG, 0xFF);  /* PUT */

   __xmon_errno = 0;

   for (i=0; i<count; i++) {
      __xmon_dos_data = *(unsigned char *)buffer++;

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
   }

   return i;
}
