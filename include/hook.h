#ifndef _HOOK_H
#define _HOOK_H



typedef struct machine_hooks
{
	void (*init) (void);
	void (*start_game) (void);
	void (*add_player) (void);
} machine_hooks_t;


#ifdef MACHINE_HOOKS

extern machine_hooks_t MACHINE_HOOKS;

#define call_hook(hook) \
do \
{ \
	if (MACHINE_HOOKS.hook) \
		(MACHINE_HOOKS.hook) (); \
} while (0)
#else
#define call_hook(hook)
#endif


#endif /* _HOOK_H */

