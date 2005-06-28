#include <stdio.h>

int putc(unsigned char c, FILE *file)
{
   if ((file == stdout) || (file == stderr))
      return putchar(c);

   if (file->flag & _IOSTRG)
      *file->ptr++ = c;

   return 0;
}
