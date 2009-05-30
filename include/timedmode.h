
#ifndef _TIMEDMODE_H
#define _TIMEDMODE_H


struct timed_mode_ops
{
	/* The constructor, called during timed_mode_begin() */
	void (*init) (void);

	/* The destructor, called when the mode ends. */
	void (*exit) (void);

	/* Called to determine when the timer should be paused */
	bool (*pause) (void);

	/* Called when the mode is finished properly (via a call to
	timed_mode_finish().  The finish hook is executed in addition
	to, and prior to, the exit handler. */
	void (*finish) (void);
	
	/* Called when the mode times out.  This hook is executed
	prior to the exit handler, and happens as soon as the timer
	reaches zero, before the grace period begins. */
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

	/* Pointer to a timer variable, which must be declared separately. */
	U8 *timer;
};


/** The default values for a mode are given by DEFAULT_MODE.
Include this at the top of a mode definition.  Then only the
fields which are different from the default need to be said. */

#define DEFAULT_MODE \
	.init = null_function, \
	.exit = null_function, \
	.pause = null_false_function, \
	.finish = null_function, \
	.timeout = null_function, \
	.deff_starting = 0, \
	.deff_running = 0, \
	.deff_ending = 0, \
	.music = 0, \
	.grace_timer = 2


/** The task that runs the mode timer is passed this structure
to inform it of all the mode parameters.  Currently, this is just
a pointer to the ops, but more could be added here. */

struct timed_mode_task_config
{
	struct timed_mode_ops *ops;
};

/* Timed mode APIs */

void timed_mode_begin (struct timed_mode_ops *ops);
void timed_mode_finish (struct timed_mode_ops *ops);
U8 timed_mode_get_timer (struct timed_mode_ops *ops);
bool timed_mode_running_p (struct timed_mode_ops *ops);
void timed_mode_reset (struct timed_mode_ops *ops, U8 time);
void timed_mode_add (struct timed_mode_ops *ops, U8 time);
void timed_mode_music_refresh (struct timed_mode_ops *ops);
void timed_mode_deff_update (struct timed_mode_ops *ops);

#endif /* _TIMEDMODE_H */
