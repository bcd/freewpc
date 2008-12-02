
#ifndef _TIMEDMODE_H
#define _TIMEDMODE_H


struct timed_mode_ops
{
	/* Constructor/destructor */
	void (*init) (void);
	void (*exit) (void);

	/* Called to determine when the timer should be paused */
	bool (*pause) (void);

	/* Called when the mode is finished properly */
	void (*finish) (void);
	
	/* Called when the mode times out */
	void (*timeout) (void);

	/* The display effect for starting the mode */
	U8 deff_starting;

	/* The display effect presented while the mode is running */
	U8 deff_running;

	/* The display effect for ending the mode */
	U8 deff_ending;

	/* The music that should be played while the mode is running */
	U8 music;

	/* The priority for the display/music effects */
	U8 prio;

	/* A task group ID that can be used to track the mode */
	task_gid_t gid;

	/* The initial value of the mode timer */
	U8 init_timer;

	/* The length of the grace period */
	U8 grace_timer;

	/* Pointer to a timer variable */
	U8 *timer;
};


struct timed_mode_task_config
{
	struct timed_mode_ops *ops;
};


void timed_mode_begin (struct timed_mode_ops *ops);
void timed_mode_finish (struct timed_mode_ops *ops);
U8 timed_mode_get_timer (struct timed_mode_ops *ops);
bool timed_mode_running_p (struct timed_mode_ops *ops);
void timed_mode_reset (struct timed_mode_ops *ops, U8 time);
void timed_mode_add (struct timed_mode_ops *ops, U8 time);
void timed_mode_music_refresh (struct timed_mode_ops *ops);
void timed_mode_deff_update (struct timed_mode_ops *ops);

#endif /* _TIMEDMODE_H */
