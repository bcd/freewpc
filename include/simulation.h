
#ifndef _SIMULATION_H
#define _SIMULATION_H


/** The number of IRQs per second. */ 
#define IRQS_PER_SEC 1024

/** The frequency of the realtime thread, in milliseconds */
#define RT_THREAD_FREQ 50

/** The number of IRQs that need to be asserted on every
 * iteration of the realtime thread. */
#define RT_ITERATION_IRQS ((IRQS_PER_SEC * RT_THREAD_FREQ) / 1000)

/** A simulation log class.  All output from the simulator is
categorized into one of the following classes; the output is then
preceded with a class identifier.  This makes the output consumable
by other utilities, which can filter the stream for specific content. */
enum sim_log_class
{
	/** Debug information from the simulator itself */
	SLC_DEBUG,

	/** Text that was rendered to the DMD */
	SLC_TEXT,

	/** Debug information written by the game ROM to the debugger */
	SLC_DEBUG_PORT,

	SLC_LAMPS,

	SLC_SOUNDCALL,
};

#define switch_poll_logical dont_use_switch_poll_logical


void simlog (enum sim_log_class class, const char *format, ...);

void ui_init (void);
void ui_write_debug (const char *format, va_list ap);
void ui_write_solenoid (int, int);
void ui_write_lamp (int, int);
void ui_write_triac (int, int);
void ui_write_switch (int, int);
void ui_write_sound_call (int x);
void ui_exit (void);


#endif /* _SIMULATION_H */
