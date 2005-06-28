/*
 * DOS: helper functions.
 */

#include "limits.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "libc/dos.h"
#include "libc/types.h"


static int8 current_drive = 0;
static char name[NAME_MAX+1];


int8 __dos_getdrive(const char *filename)
{
   int8 drive;

   /* look for drive delimiter */
   if (filename[1] != ':')
      return current_drive;

   drive = filename[0] - '0';

   if ((drive<0) || (drive>=NDRV))
      return -1;

   return drive;
}


char *__dos_getname(const char *filename)
{
   const char *p;
   int len = 0, i = -1;

   /* skip drive delimiter */
   if (filename[1] == ':')
      filename += 2;

   /* look for extension and find length */
   for (p = filename; *p; p++) {
      if (*p == '.')
         i = len;

      len++;
   }

   /* ' ' is the filling byte */
   memset(name, ' ', NAME_MAX);

   if (i>=0) {
      /* concatenate name+extension */
      memcpy(name, filename, MIN(8, i));
      memcpy(name+8, filename+i+1, MIN(3, len-i-1));
   }
   else {
      memcpy(name, filename, MIN(8, len));
   }

   return name;
}
