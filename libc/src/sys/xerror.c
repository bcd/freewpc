#include "errno.h"
#include "libc/types.h"


int __xmon_translate_err(uint8 err) 
{
   switch (err) {

      case 52:  /* file already open */
         err = EAGAIN;
         break;

      case 64:  /* too many open disk files */
         err = EMFILE;
         break;

      case 65:  /* directory full */
         err = ENOSPC;
         break;

      case 71:  /* no disk */
         err = ENODEV;
         break;

      default:
         err = 127;
   }

   return err;
}
