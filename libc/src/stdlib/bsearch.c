/* Copyright (C) 1994 DJ Delorie, see COPYING.DJ for details */
#include <stdlib.h>

void *bsearch(const void *key, const void *base, size_t nelem,
              size_t size, int (*compar)(const void *, const void *))
{
  char *base0 = (char *)base;
  int lim, cmpval;
  void *p;

  for (lim = nelem; lim != 0; lim >>= 1)
  {
    p = base0 + (lim >> 1) * size;
    cmpval = (*compar)(key, p);
    if (cmpval == 0)
      return p;
    if (cmpval > 0)
    {                     /* key > p: move right */
      base0 = (char *)p + size;
      lim--;
    }                     /* else move left */
  }

  return NULL;
}
