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
#define ERR_TASK_KILL_FAILED		14
#define ERR_NOT_IMPLEMENTED_YET	15
#define ERR_UNPRINTABLE_CHAR		16
#define ERR_LEFF_QUEUE_FULL		17
#define ERR_FCFS_LOCKUP				18
#define ERR_REAL_HARDWARE			19
#define ERR_NVRAM_UNLOCKED			20
#define ERR_SOL_CKSUM_ERROR      21


typedef uint8_t errcode_t;
__noreturn__ void fatal (errcode_t errcode);
void nonfatal (errcode_t errcode);

#endif /* SYS_ERRNO_H */
