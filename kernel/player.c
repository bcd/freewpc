
#include <freewpc.h>

/* Theory of operation:
 *
 * Some state needs to be "per-player" and not global.
 * We will keep all data 'global' so that access can be fast.
 * We will reserve space for each player so that this data can be
 * swapped in/out quickly whenever the current player changes.
 */

/* Array of entries for each player's saved area */
player_local_t player_locals[MAX_PLAYERS];

/* Pointer to the local for the current player */
player_local_t *player_local_current;


void player_start_game (void)
{
}

void player_change (void)
{
}

