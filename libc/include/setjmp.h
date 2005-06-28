/*
 *	ISO C99 Standard: 7.13 Nonlocal jumps	<setjmp.h>
 */

#ifndef _SETJMP_H
#define _SETJMP_H

#ifdef __cplusplus
namespace std {
  extern "C" {
#endif

typedef struct __jmp_buf {
  unsigned char __ar, __br;
  unsigned int __xr, __yr, __ur, __sr, __pc;
  unsigned char __dp, __cc;
} jmp_buf[1];


extern void longjmp(jmp_buf env, int val);
extern int setjmp(jmp_buf env);
#define setjmp setjmp   /* required by C++ standard */


#ifdef __cplusplus
  }
}
#endif

#endif /* !defined _SETJMP_H */


#ifdef __cplusplus

using std::setjmp;
using std::longjmp;
using std::jmp_buf;

#endif /* defined __cplusplus */
