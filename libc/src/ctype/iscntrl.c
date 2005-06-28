#include <inline/ctype.inl>

int iscntrl(unsigned char c)
{
   return __iscntrl(c);
}
