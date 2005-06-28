#include "stdlib.h"
#include "string.h"
#include "sys/types.h"


void *calloc(size_t nelem, size_t size)
{
   void *p;

   if (!nelem || !size)
      return NULL;

   p = malloc(nelem * size);
   if (p)
      memset(p, 0, nelem * size);

   return p;
}
