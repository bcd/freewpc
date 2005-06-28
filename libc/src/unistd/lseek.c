#include "errno.h"
#include "unistd.h"
#include "libc/xmonitor.h"
#include "sys/mem.h"
#include "sys/types.h"


off_t lseek(int fd, off_t offset, int whence)
{
   unsigned int fcb_addr;

   /* fix global parameters */
   _pokeb(FCBNUM, fd);
   _pokeb(PUTFLG, 0);  /* GET */

   fcb_addr = _peekw(FCBPTR);

   switch (whence) {

      case SEEK_SET:
         if (offset == 0) {
            /* in order to return to the beginning of the file,
             * we read the first byte then step back by 1 byte
             */
            __asm__ __volatile__ ("ldx #1 \n"
               "	ldb #07 \n"  /* PUTGET code: 07 */
               "	jsr 0xEC0C"
               :
               :
               : "x", "b", "d"
            );

            _pokew(fcb_addr + fd*FCBLEN + FCB_POS, 1);
            return 0;
         }
         break;

      case SEEK_CUR:
         offset += _peekw(fcb_addr + fd*FCBLEN + FCB_POS) - 1;
         break;

      case SEEK_END:
         offset += __xmon_dos_size(fd);  /* not safe */
         break;

      default:
         errno = EINVAL;
         return (off_t)-1;
   }

   if (offset <= 0)
      return lseek(fd, 0, SEEK_SET);

   /* printf("offset = %d\r\n", offset); */

   /* do the call to the extra-monitor routine */
   __asm__ __volatile__ ("ldx %0 \n"
      "	ldb #07 \n"  /* PUTGET code: 07 */
      "	jsr 0xEC0C"
      :
      : "m" (offset)
      : "x", "b", "d"
   );

   return offset;
}
