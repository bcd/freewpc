#ifndef _PRIORITY_H
#define _PRIORITY_H

/*
 * Standard priorities for effects (display & lamps).
 * Use the standard priority unless you have a good reason not to.
 */
typedef enum _priority {
	PRI_AMODE,
	PRI_COINS,
	PRI_CREDITS,
	PRI_EGG1,
	PRI_EGG4=PRI_EGG1+3,

	PRI_SCORES,

	PRI_GAME_LOW1,
	PRI_GAME_LOW16=PRI_GAME_LOW1+15,

	PRI_GAME_MODE1,
	PRI_GAME_MODE16=PRI_GAME_MODE1+15,

	PRI_GAME_QUICK1,
	PRI_GAME_QUICK16=PRI_GAME_QUICK1+15,

	PRI_EB,
	PRI_JACKPOT,
	PRI_SUPER_JACKPOT,
	
	PRI_REPLAY,
	PRI_SPECIAL,

	PRI_VOLUME_CHANGE,
	
	PRI_BONUS,
	PRI_TILT_WARNING,
	PRI_TILT,

	PRI_EB_BUYIN,	
	PRI_HSENTRY,
	PRI_MATCH,
	PRI_1COIN_BUYIN,

	PRI_SLAMTILT,
} priority_t;

#endif /* _PRIORITY_H */

