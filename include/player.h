
#ifndef _PLAYER_H
#define _PLAYER_H

typedef struct
{
	U8 lamps[NUM_LAMP_COLS + NUM_VLAMP_COLS];
#ifdef MACHINE_PLAYER_LOCALS
#endif /* MACHINE_PLAYER_LOCALS */
} player_local_t;

void player_start_game (void);
void player_change (void);


#endif /* PLAYER_H */
