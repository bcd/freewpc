
#ifndef _LAMPTIMER_H
#define _LAMPTIMER_H

struct lamptimer_args
{
	lampnum_t lamp;
	U8 secs;
};

__effect__ void lamp_timer_stop (lampnum_t lamp);
__effect__ task_pid_t lamp_timer_find (lampnum_t lamp);
__effect__ void lamp_timer_start (struct lamptimer_args *args);

#endif /* _LAMPTIMER_H */
