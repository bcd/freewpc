/* Copyright (C) 2001 DJ Delorie, see COPYING.DJ for details */
/* Copyright (C) 1995 DJ Delorie, see COPYING.DJ for details */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>


FILE *fopen(const char *filename, const char *mode)
{
   int fd, rw, oflags;
   mode_t s_mode;
   char tbchar;
   FILE *f;

   if (!filename || !mode)
      return 0;

   rw = (mode[1] == '+') || (mode[1] && (mode[2] == '+'));

   switch (*mode) {

      case 'a':
         oflags = O_CREAT | (rw ? O_RDWR : O_WRONLY) | O_APPEND;
         break;

      case 'r':
         oflags = rw ? O_RDWR : O_RDONLY;
         break;

      case 'w':
         oflags = O_TRUNC | O_CREAT | (rw ? O_RDWR : O_WRONLY);
         break;

      default:
         return NULL;
   }

   if (mode[1] == '+')
      tbchar = mode[2];
   else
      tbchar = mode[1];

   if (tbchar == 'b')
      s_mode = S_BIN;
   else
      s_mode = S_ASCII;

   f = malloc(sizeof(FILE));
   if (!f) {
      errno = ENOMEM;
      return NULL;
   }

   fd = open(filename, oflags, s_mode);
   if (fd < 0) {
      free(f);
      return NULL;
   }

   f->fd = fd;

   if (rw)
      f->flag = _IORW;
   else if (*mode == 'r')
      f->flag = _IOREAD;
   else
      f->flag = _IOWRT;

   f->cnt = 0;
   f->base = f->ptr = NULL;
   f->bufsiz = 0;

   return f;
}
