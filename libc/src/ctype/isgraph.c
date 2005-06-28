#include <inline/ctype.inl>

int isgraph(unsigned char c)
{
  return __isgraph(c);
}
