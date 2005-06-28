#include <iostream>
#include <cstdlib>
#include <cstring>


class String
{
   friend ostream& operator<<(ostream&, const String&);

public:
   /* constructor */
   String(const char *);

   /* destructor */
   ~String();

private:
   static char null_string;

   char *c_array;
   size_t len;
};


char String::null_string = 0;


ostream& operator<<(ostream& os, const String& s)
{
   os << s.c_array;
   return os;
}


String::String(const char *str)
{
   len = strlen(str);

   if (len>0) {
      c_array = reinterpret_cast<char *>(malloc(len + 1));
      strcpy(c_array, str);
   }
   else {
      c_array = &null_string;
   }
}


String::~String()
{
   if (len>0)
      free(c_array);
}


int main(void)
{
   String hello("Hello world !");

   cout << hello;

   return 0;
}
