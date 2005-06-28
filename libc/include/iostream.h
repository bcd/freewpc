#ifndef _IOSTREAM_H
#define _IOSTREAM_H

#include <stdio.h>

extern "C++" { 

class ostream
{
public:
   /* constructor */
   ostream() {}

   ostream& operator<<(const char *s) { puts(s); return *this; }
};


ostream cout;

} // extern "C++"

#endif /* !defined _IOSTREAM_H */
