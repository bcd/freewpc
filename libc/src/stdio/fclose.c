/* Copyright (C) 1995 DJ Delorie, see COPYING.DJ for details */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int fclose(FILE *f)
{
   int r = -1;

   if (!f)
      return r;

   if (!(f->flag & _IOSTRG)) {
      r = fflush(f);

      if (close(f->fd) < 0)
         r = -1;

      if (f->flag & _IOMYBUF)
         free(f->base);

      free(f);
  }

  return r;
}
