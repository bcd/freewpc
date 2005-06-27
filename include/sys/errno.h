#ifndef _SYS_ERRNO_H
#define _SYS_ERRNO_H


#define ERR_NO_FREE_TASKS 			1
#define ERR_TASK_STACK_OVERFLOW 	2
#define ERR_TASK_KILL_CURRENT 	3
#define ERR_TASK_REGISTER_SAVE 	4
#define ERR_NMI 						5
#define ERR_SWI 						6
#define ERR_SWI2 						7
#define ERR_SWI3 						8
#define ERR_IDLE_CANNOT_SLEEP 	9
#define ERR_IDLE_CANNOT_EXIT 		10
#define ERR_INVALID_LAMP_NUM		11
#define ERR_NO_FREE_TIMERS			12
#define ERR_DEFF_QUEUE_FULL		13

#ifndef __SASM__

typedef uint8_t errcode_t;
void fatal (errcode_t errcode) __noreturn__;
void nonfatal (errcode_t errcode);

#endif /* __SASM__ */

#endif /* SYS_ERRNO_H */
