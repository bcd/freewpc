/*
 *	ISO C99 Standard: 7.14 Signal handling <signal.h>
 */

#ifndef _SIGNAL_H
#define _SIGNAL_H

#ifdef __cplusplus
extern "C" {
#endif


#define SIGABRT   1
#define SIGFPE    2
#define SIGILL    3
#define SIGINT    4
#define SIGSEGV   5
#define SIGTERM   6

/* type of a signal handler */
typedef void (*__sighandler_t) (int);

/* default signal handler */
extern void __sig_handler(int);

#define __SIG_ERR  1
#define __SIG_IGN  -1

#define SIG_DFL   (&__sig_handler)
#define SIG_ERR   (__sighandler_t)(__SIG_ERR)
#define SIG_IGN   (__sighandler_t)(__SIG_IGN)


int raise(int sig);
__sighandler_t signal(int sig, __sighandler_t func);
  

#ifdef __cplusplus
}
#endif

#endif /* !defined _SIGNAL_H */
