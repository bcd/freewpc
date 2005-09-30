#ifndef _HOOK_H
#define _HOOK_H


/* machine_hooks is a struct of game-specific
 * functions that are called at the appropriately
 * places.  The machine config code should declare
 * an object of this type and define the macro
 * MACHINE_HOOKS to the same name, so that the
 * system can find it.
 *
 * If a particular hook need not be implemented,
 * then it can be left as NULL.
 *
 * All of the game hooks need to be in the same
 * ROM page.
 */
typedef struct machine_hooks
{
	void (*init) (void);
	void (*start_game) (void);
	void (*add_player) (void);
	void (*start_ball) (void);
	void (*end_ball) (void);
	void (*tilt) (void);
	void (*slam_tilt) (void);
	void (*replay) (void);
	void (*high_score) (void);
	void (*grand_champion) (void);
	void (*coin_added) (void);
	void (*credit_added) (void);
	void (*volume_change) (void);
	void (*ball_search) (void);
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

/* If no hooks are defined, then calling a hook is trivial */
#define call_hook(hook)

#endif


#endif /* _HOOK_H */

